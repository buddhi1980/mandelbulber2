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
import sys
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


class SetupError(Exception):
    """The run cannot go ahead or cannot be trusted."""


class RenderError(Exception):
    """One item failed to render or to validate."""


def binary_path():
    """The binary under test, read from the environment at call time."""
    default = "build-win/Release/x64/mandelbulber2.exe" if os.name == "nt" else "mandelbulber2/build/appdir/usr/bin/mandelbulber2"
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
