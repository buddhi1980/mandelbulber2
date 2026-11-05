#!/usr/bin/env python3
"""Headless differential testing engine for the Mandelbulber CPU renderer.

Modes, one per run:

  (no mode)                     Gate: compare renders against references.
  --self-parity                 Require repeated renders to match each other.
  --capture                     Pass Self-Parity, then write a new reference set.
  --compare BINARY              Compare the default binary with a second binary.
  --collect DIR                 Render every item once and write a collection.
  --cross-check FIRST SECOND    Compare two collections from their files alone.

See specs/001-regression-harness/contracts/runner-cli.md.
"""

import argparse
import os
import subprocess
import sys
import threading
from pathlib import Path

EXIT_PASSED = 0
EXIT_PARITY = 1
EXIT_SETUP = 3

SCENE_LIST = Path("tests/scenes.txt")
FORMULA_BASE = Path("tests/formula_sweep_base.fract")
FORMULA_EXCLUSIONS = Path("tests/formula_exclusions.txt")
REF_ROOT = Path("tests/references")
OUT_DIR = Path("tests/outputs")
SCENE_RESOLUTION = "320x240"
FORMULA_RESOLUTION = "32x24"
RENDER_OVERRIDES = "opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4"
SCENE_TIMEOUT = 1800
FORMULA_TIMEOUT = 120
SELF_PARITY_RUNS = 3
MAX_REFERENCE_BYTES = 100 * 1024 * 1024
EXPORT_VERSION = 1

# Playbook Phase 3 adds a working headless thread control and sets this to its option name.
THREAD_OPTION = None

# Variables a render may inherit. LD_LIBRARY_PATH stays out, because it can select another math library.
ALLOWED_VARIABLES = ("PATH", "SYSTEMROOT", "TEMP", "TMP", "TMPDIR")

# The console shows this many statistic differences per item and comparison. A histogram has about
# 1,000 values, and report.txt keeps them all.
CONSOLE_STATISTIC_LIMIT = 5

PARITY = "parity failure"
MISMATCH = "reference target mismatch"
SETUP = "setup error"


class SetupError(Exception):
    """The run cannot go ahead or cannot be trusted."""


class RenderError(Exception):
    """One item failed to render or to validate."""


class Report:
    """One run's results. Every line goes to tests/outputs/report.txt and to the console."""

    def __init__(self, mode, environment, target=None):
        self.mode = mode
        self.environment = environment
        self.target = target
        self.gate_validity = None
        self.toolchain_note = None
        self.item_results = {}
        self.overall_result = None
        self.item_count = 0
        self.parity_failures = set()
        self.setup_failures = set()
        self.mismatches = set()
        self.run_problems = []
        self.lock = threading.Lock()
        OUT_DIR.mkdir(parents=True, exist_ok=True)
        self.file = open(OUT_DIR / "report.txt", "w", encoding="utf-8", newline="\n")
        self.line(f"mode: {mode}")
        self.line(f"environment: {environment}")
        if target:
            self.line(f"target: {target}")

    def line(self, text, console=True):
        """Writes one line to the report file, and to the console unless told not to."""
        with self.lock:
            self.file.write(text + "\n")
            self.file.flush()
            if console:
                print(text, flush=True)

    def note(self, text):
        self.line(f"note: {text}")

    def set_gate_validity(self, valid, reason=""):
        self.gate_validity = valid
        self.line("gate validity: valid" if valid else f"gate validity: not gate-valid ({reason})")

    def set_toolchain_note(self, text):
        self.toolchain_note = text
        self.note(text)

    def passed(self, label, text="exact match"):
        """Records an item that passed, unless it has already failed."""
        with self.lock:
            if label in self.item_results:
                return
            self.item_results[label] = "passed"
        self.line(f"[+] {label}: {text}")

    def failed(self, label, text, category=PARITY):
        """Records one failure of one item."""
        with self.lock:
            self.item_results[label] = "failed"
            self.failures(category).add(label)
        self.line(f"[-] {label}: {text}")

    def failures(self, category):
        return {PARITY: self.parity_failures, MISMATCH: self.mismatches, SETUP: self.setup_failures}[category]

    def differences(self, label, differences, category=PARITY, context=""):
        """Reports one comparison, capping its statistic lines on the console only."""
        if not differences:
            return
        with self.lock:
            self.item_results[label] = "failed"
            self.failures(category).add(label)
        suffix = f" ({context})" if context else ""
        statistic_lines = 0
        for difference in differences:
            if difference.name == "statistics":
                statistic_lines += 1
            console = difference.name != "statistics" or statistic_lines <= CONSOLE_STATISTIC_LIMIT
            self.line(f"[-] {label} {difference.name}: {difference.text}{suffix}", console)
        if statistic_lines > CONSOLE_STATISTIC_LIMIT:
            rest = statistic_lines - CONSOLE_STATISTIC_LIMIT
            with self.lock:
                print(f"[-] {label} statistics: {rest} more differ{suffix}, see {OUT_DIR / 'report.txt'}", flush=True)

    def recorded(self, label, text):
        """Writes a difference that never changes the result."""
        self.line(f"[~] {label}: {text}")

    def cannot_pass(self, reason):
        """Records something that stops the run from passing, whatever its results are."""
        with self.lock:
            self.run_problems.append(reason)
        self.note(reason)

    def setup_error(self, cause):
        """Records a setup failure that stopped the run."""
        with self.lock:
            self.run_problems.append(cause)
        self.line(f"setup error: {cause}")

    def exit_code(self):
        """A parity failure wins over every other condition, as the runner contract says."""
        if self.parity_failures:
            return EXIT_PARITY
        if self.mismatches or self.setup_failures or self.run_problems:
            return EXIT_SETUP
        if "passed" not in self.item_results.values():
            return EXIT_SETUP
        return EXIT_PASSED

    def result_name(self):
        code = self.exit_code()
        if code == EXIT_PARITY:
            return "functional disagreement" if self.mode == "cross-check" else PARITY
        if code == EXIT_PASSED:
            return "functional agreement" if self.mode == "cross-check" else "passed"
        return MISMATCH if self.mismatches else SETUP

    def finish(self):
        """Writes the overall result, closes the report, and returns the exit code."""
        failed = sum(1 for result in self.item_results.values() if result == "failed")
        total = self.item_count or len(self.item_results)
        self.overall_result = self.result_name()
        detail = f" ({failed} of {total} items failed)" if total else ""
        self.line(f"result: {self.overall_result}{detail}")
        code = self.exit_code()
        self.close()
        return code

    def close(self):
        with self.lock:
            self.file.close()


def clean_environment(home):
    """Builds a render's whole environment, so nothing from the host can affect it (FR-004)."""
    environment = {name: os.environ[name] for name in ALLOWED_VARIABLES if name in os.environ}
    environment.update(HOME=str(home), LC_ALL="C.UTF-8", LANG="C.UTF-8")
    return environment


def gate_valid():
    """Whether renders can be isolated here. The application ignores HOME on Windows (FR-005)."""
    return os.name != "nt"


def binary_path():
    """The binary under test, read from the environment at call time."""
    default = "mandelbulber2/build/appdir/usr/bin/mandelbulber2"
    if os.name == "nt":
        default = "build-win/Release/x64/mandelbulber2.exe"
    return Path(os.environ.get("MANDELBULBER_BINARY", default))


def parity_target():
    """The Reference Target, read from the environment at call time."""
    default = "windows-x86_64-msvc" if os.name == "nt" else "linux-x86_64-gcc"
    return os.environ.get("PARITY_TARGET", default)


def build_parser():
    parser = argparse.ArgumentParser(description="Regression harness for the Mandelbulber CPU renderer.")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--self-parity", action="store_true", help="require repeated renders to match each other")
    group.add_argument("--capture", action="store_true", help="pass Self-Parity, then write a new reference set")
    group.add_argument("--compare", metavar="BINARY", help="compare the default binary with a second binary")
    group.add_argument("--collect", metavar="DIR", help="render every item once and write a collection")
    group.add_argument("--cross-check", nargs=2, metavar=("FIRST", "SECOND"), help="compare two collections")
    return parser


def run_mode(options):
    raise SetupError("this mode is not implemented yet")


def main(argv=None):
    options = build_parser().parse_args(argv)
    try:
        return run_mode(options)
    except SetupError as error:
        print(f"setup error: {error}", flush=True)
        return EXIT_SETUP
    except Exception as error:  # An unexpected error must never look like a parity failure.
        print(f"unexpected harness error: {type(error).__name__}: {error}", flush=True)
        return EXIT_SETUP


if __name__ == "__main__":
    sys.exit(main())
