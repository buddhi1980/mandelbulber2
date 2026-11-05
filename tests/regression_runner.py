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
import contextlib
import functools
import operator
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import threading
import zipfile
import zlib
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, replace
from pathlib import Path
from typing import Optional

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

# Scene files must not reach outside the repository, and nebula mode changes the render path.
ASSET_PATTERN = re.compile(r'^[A-Za-z0-9_]*(file|texture)[A-Za-z0-9_]* +"?(https?://|/|[A-Za-z]:[/\\])', re.MULTILINE)
NEBULA_PATTERN = re.compile(r"^nebula_mode true", re.MULTILINE)

# The application reports these only as text, and it still exits 0 (FR-008).
FAILURE_MARKERS = (
    "not valid Mandelbulber settings file",
    "No materials defined",
    "Error in settings file",
    "Too many errors in settings file",
    "Can't load texture",
    "Looking for the file at",
    "Trying to use /usr/share/mandelbulber2",
)

SAMPLE_BYTES = {"f32": 4, "u16": 2}
EXPECTED_BUFFERS = (
    ("raw_render", "f32", 3),
    ("post_effect", "f32", 3),
    ("final_image", "u16", 3),
    ("alpha", "u16", 1),
    ("depth", "f32", 1),
    ("normal", "f32", 3),
    ("normal_world", "f32", 3),
)
EXPORT_HEADER = "mandelbulber-verification-export"
FORMULA_LIST_HEADER = "mandelbulber-formula-list 1"
HISTOGRAMS = ("iteration_histogram", "step_histogram")
PLAIN_STATISTICS = ("total_iterations", "raymarch_count", "missed_de")
SAMPLE_CODES = {"f32": "I", "u16": "H"}
NOT_ISOLATED_REASON = "the application ignores a redirected home folder on this platform"

# A fixed timestamp keeps a recapture of unchanged data byte-identical.
REFERENCE_TIMESTAMP = (1980, 1, 1, 0, 0, 0)
REFERENCE_ENTRIES = ("manifest.txt", "statistics.txt") + tuple(f"{name}.planes" for name, _, _ in EXPECTED_BUFFERS)
TOOLCHAIN_PACKAGES = ("gcc-[0-9]*", "g++-[0-9]*", "libc6", "libstdc++6", "libgcc-s1", "libgomp1")


class SetupError(Exception):
    """The run cannot go ahead or cannot be trusted."""


class RenderError(Exception):
    """One item failed to render or to validate."""


@dataclass(frozen=True)
class WorkItem:
    """One thing the harness renders and checks."""

    kind: str
    name: str
    source: Path
    formula_id: Optional[int]
    resolution: str
    time_limit: int

    @property
    def label(self):
        return f"{self.kind} {self.name}"


@dataclass
class Export:
    """A validated verification export, read into memory."""

    width: int
    height: int
    compiler: str
    math_library: str
    buffers: dict
    statistics: dict
    manifest_bytes: bytes
    statistics_bytes: bytes


@dataclass
class ReferenceSet:
    """The folder of one target's references, and the toolchain that made them."""

    folder: Path
    toolchain: list


@dataclass(frozen=True)
class Difference:
    """One difference from a comparison. The name is a buffer, statistics, or layout."""

    name: str
    text: str


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
        self.toolchain_shown = False
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

    def show_toolchain(self, export):
        """Prints the toolchain of the build under test, once."""
        with self.lock:
            if self.toolchain_shown:
                return
            self.toolchain_shown = True
        self.line(f"toolchain: {export.compiler}, {export.math_library}")

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


def detect_environment():
    """Works out where this run happens, in the order the data model gives (FR-040)."""
    if os.environ.get("GITHUB_ACTIONS") == "true":
        return "github-actions"
    if os.name == "nt":
        return "windows"
    if "WSL_DISTRO_NAME" in os.environ:
        return "wsl"
    return "linux"


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


@contextlib.contextmanager
def temporary_folder():
    """Yields a temporary folder whose removal cannot fail.

    A timed-out launcher's child can still hold files open on Windows, and on Python 3.10 the
    ignore_cleanup_errors option of TemporaryDirectory recurses until it fails there.
    """
    path = tempfile.mkdtemp(prefix="mandelbulber-harness-")
    try:
        yield path
    finally:
        shutil.rmtree(path, ignore_errors=True)


def remove_tree(path):
    """Removes a folder if it exists, and fails rather than leave stale output behind (FR-006)."""
    if path.exists():
        shutil.rmtree(path, ignore_errors=True)
    if path.exists():
        raise RenderError(f"cannot remove earlier output in {path}")


def load_scenes():
    """Reads the Standard Scene Suite, and checks every scene before anything renders (FR-002)."""
    if not SCENE_LIST.is_file():
        raise SetupError(f"scene list not found: {SCENE_LIST}")
    lines = [line.strip() for line in SCENE_LIST.read_text(encoding="utf-8").splitlines()]
    paths = [Path(line) for line in lines if line and not line.startswith("#")]
    if not paths:
        raise SetupError(f"{SCENE_LIST} lists no scenes")
    names = set()
    for path in paths:
        if not path.is_file():
            raise SetupError(f"scene not found: {path}")
        if path.stem in names:
            raise SetupError(f"two scenes have the same file name: {path.stem}")
        names.add(path.stem)
        text = path.read_text(encoding="utf-8", errors="replace")
        asset = ASSET_PATTERN.search(text)
        if asset:
            line = text[asset.start() : text.find("\n", asset.start())].strip()
            raise SetupError(f"{path}: asset referenced by URL or absolute path: {line}")
        if NEBULA_PATTERN.search(text):
            raise SetupError(f"{path}: scene enables nebula mode")
    return paths


def run_query(binary, option):
    """Runs the binary with one query option in a temporary home, and returns its output."""
    binary = Path(binary)
    if not binary.is_file():
        raise SetupError(f"binary not found: {binary}")
    binary = binary.resolve()
    with temporary_folder() as home:
        # A file rather than a pipe, because a timed-out launcher's child can keep a pipe open.
        log_path = Path(home) / "query.log"
        try:
            with open(log_path, "wb") as log:
                result = subprocess.run(
                    [str(binary), option],
                    cwd=binary.parent,
                    env=clean_environment(home),
                    stdin=subprocess.DEVNULL,
                    stdout=log,
                    stderr=subprocess.STDOUT,
                    timeout=FORMULA_TIMEOUT,
                )
        except subprocess.TimeoutExpired:
            raise SetupError(f"{binary} {option} timed out after {FORMULA_TIMEOUT} seconds") from None
        except OSError as error:
            raise SetupError(f"cannot start {binary}: {error}") from None
        text = log_path.read_bytes().decode("utf-8", errors="replace")
    if result.returncode != 0:
        raise SetupError(f"{binary} {option} exited with status {result.returncode}")
    return text


def load_formulas(binary):
    """Asks the binary which formulas it offers, and returns their IDs by internal name."""
    lines = run_query(binary, "--list-formulas").splitlines()
    if FORMULA_LIST_HEADER not in lines:
        raise SetupError(f"{binary} --list-formulas printed no {FORMULA_LIST_HEADER!r} line")
    start = lines.index(FORMULA_LIST_HEADER) + 1
    if "end" not in lines[start:]:
        raise SetupError(f"{binary} --list-formulas printed no end line")
    formulas = {}
    for line in lines[start : lines.index("end", start)]:
        number, separator, name = line.partition("\t")
        if not separator or not number.isdigit() or not name:
            raise SetupError(f"{binary} --list-formulas printed a bad line: {line!r}")
        if name in formulas:
            raise SetupError(f"{binary} --list-formulas printed {name} twice")
        formulas[name] = int(number)
    if not formulas:
        raise SetupError(f"{binary} --list-formulas printed no formulas")
    return formulas


def load_exclusions():
    """Reads the formulas left out of the sweep, as a dictionary of name and reason (FR-034)."""
    if not FORMULA_EXCLUSIONS.is_file():
        raise SetupError(f"exclusions file not found: {FORMULA_EXCLUSIONS}")
    exclusions = {}
    for line in FORMULA_EXCLUSIONS.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        name, _, reason = line.partition(" ")
        if not reason.strip():
            raise SetupError(f"{FORMULA_EXCLUSIONS}: exclusion of {name} has no reason")
        exclusions[name] = reason.strip()
    return exclusions


def build_work_items(binary):
    """Builds the list of everything to render: the scenes, then the formula sweep."""
    scenes = load_scenes()
    formulas = load_formulas(binary)
    exclusions = load_exclusions()
    unknown = sorted(set(exclusions) - set(formulas))
    if unknown:
        raise SetupError(f"{FORMULA_EXCLUSIONS}: the binary does not offer {', '.join(unknown)}")
    if not FORMULA_BASE.is_file():
        raise SetupError(f"formula sweep base scene not found: {FORMULA_BASE}")
    items = [WorkItem("scene", path.stem, path, None, SCENE_RESOLUTION, SCENE_TIMEOUT) for path in scenes]
    for name, number in formulas.items():
        if name not in exclusions:
            items.append(WorkItem("formula", name, FORMULA_BASE, number, FORMULA_RESOLUTION, FORMULA_TIMEOUT))
    return items


def render_overrides(item, neutral):
    """The one --override value for a render. A neutral render uses the default buffers (FR-012)."""
    overrides = RENDER_OVERRIDES.split("#")
    if neutral:
        overrides.remove("normal_enabled=1")
        overrides.append("color_quality=1")
    if item.formula_id is not None:
        overrides.append(f"fractal1_formula={item.formula_id}")
    return "#".join(overrides)


def render_command(item, binary, output_dir, thread_args, neutral):
    """Builds the command from the application contract, with absolute paths throughout."""
    output_dir = output_dir.resolve()
    command = [str(binary.resolve()), *thread_args, "--nogui", "--res", item.resolution, "--format", "tiff"]
    command += ["--override", render_overrides(item, neutral), "--output", str(output_dir / "image")]
    if not neutral:
        command += ["--verification-export", str(output_dir / "export")]
    command.append(str(item.source.resolve()))
    return command


def render_item(item, binary, output_dir, thread_args=(), neutral=False):
    """Runs one isolated render, and returns its exit status and the path of its log."""
    binary = Path(binary)
    remove_tree(output_dir)
    # The application reports success even when its output folder is missing.
    output_dir.mkdir(parents=True)
    log_path = output_dir / "render.log"
    command = render_command(item, binary, output_dir, thread_args, neutral)
    with temporary_folder() as home:
        try:
            with open(log_path, "wb") as log:
                result = subprocess.run(
                    command,
                    cwd=binary.resolve().parent,
                    env=clean_environment(home),
                    stdin=subprocess.DEVNULL,
                    stdout=log,
                    stderr=subprocess.STDOUT,
                    timeout=item.time_limit,
                )
        except subprocess.TimeoutExpired:
            raise RenderError(f"timed out after {item.time_limit} seconds, see {log_path}") from None
        except OSError as error:
            raise SetupError(f"cannot start {binary}: {error}") from None
    return result.returncode, log_path


def check_render_output(returncode, log_path):
    """Fails on a non-zero exit and on any failure message. A zero exit alone proves nothing."""
    if returncode != 0:
        raise RenderError(f"exit status {returncode}, see {log_path}")
    text = log_path.read_bytes().decode("utf-8", errors="replace")
    for marker in FAILURE_MARKERS:
        if marker in text:
            raise RenderError(f"{marker}, see {log_path}")


def parse_manifest(data, source):
    """Parses the bytes of a manifest, and returns its fields. Source names it in failures."""
    lines = data.decode("utf-8", errors="replace").split("\n")
    if lines[-2:] != ["end", ""]:
        raise RenderError(f"{source} has no end line, so the export is incomplete")
    lines = lines[:-2]
    header = lines[0].split(" ") if lines else []
    if len(header) != 2 or header[0] != EXPORT_HEADER:
        raise RenderError(f"{source} does not start with {EXPORT_HEADER}")
    if header[1] != str(EXPORT_VERSION):
        raise RenderError(f"{source} has unknown format version {header[1]}")
    fields = {"buffer": []}
    for line in lines[1:]:
        name, _, value = line.partition(" ")
        if name == "buffer":
            fields["buffer"].append(tuple(value.split(" ")))
        else:
            fields[name] = value
    for name in ("width", "height", "compiler", "math_library", "statistics"):
        if name not in fields:
            raise RenderError(f"{source} has no {name} line")
    if not fields["width"].isdigit() or not fields["height"].isdigit():
        raise RenderError(f"{source} has a bad image size")
    check_buffer_list(source, fields["buffer"])
    return fields


def check_buffer_list(path, listed):
    """Requires exactly the seven expected buffers, with their expected types (FR-011)."""
    expected = {name: (name, sample, str(channels)) for name, sample, channels in EXPECTED_BUFFERS}
    names = [entry[0] for entry in listed]
    for name in names:
        if name not in expected:
            raise RenderError(f"{path} lists unexpected buffer {name}")
    for name in expected:
        if name not in names:
            raise RenderError(f"{path} has a missing buffer {name}: missing buffer {name}")
    for entry in listed:
        if entry != expected[entry[0]]:
            raise RenderError(f"{path} lists buffer {entry[0]} as {' '.join(entry[1:])}")


def parse_statistics(text):
    """Parses a statistics record, and requires the contract's exact names, order, and values."""
    lines = text.split("\n")
    if lines[-1] != "":
        raise RenderError("the statistics record does not end with a line break")
    values = []
    for line in lines[:-1]:
        name, separator, value = line.partition(" ")
        if not separator or not re.fullmatch(r"[0-9]+", value):
            raise RenderError(f"statistics line is not a name and a whole number: {line!r}")
        values.append((name, int(value)))

    statistics = {}
    position = 0

    def take(expected):
        nonlocal position
        if position >= len(values):
            raise RenderError(f"the statistics record is incomplete: {expected} is missing")
        name, value = values[position]
        if name != expected:
            if re.fullmatch(r"(total_iterations|raymarch_count|missed_de|(iteration|step)_histogram\.(size|[0-9]+))", name):
                raise RenderError(f"the statistics record has {name} where {expected} belongs")
            raise RenderError(f"the statistics record has unknown statistic {name}")
        statistics[name] = value
        position += 1
        return value

    for name in PLAIN_STATISTICS:
        take(name)
    for histogram in HISTOGRAMS:
        size = take(f"{histogram}.size")
        for bucket in range(size + 1):
            take(f"{histogram}.{bucket}")
    if position != len(values):
        raise RenderError(f"the statistics record has unknown statistic {values[position][0]}")
    return statistics


def read_export(directory):
    """Reads and validates one export. Anything other than a complete export fails (FR-008)."""
    directory = Path(directory)
    manifest_path = directory / "manifest.txt"
    if not manifest_path.is_file():
        raise RenderError(f"no manifest in {directory}, so the export is incomplete")
    manifest_bytes = manifest_path.read_bytes()
    fields = parse_manifest(manifest_bytes, manifest_path)
    width = int(fields["width"])
    height = int(fields["height"])
    buffers = {}
    for name, sample, channels in EXPECTED_BUFFERS:
        path = directory / f"{name}.bin"
        if not path.is_file():
            raise RenderError(f"buffer file {name}.bin is missing from {directory}")
        data = path.read_bytes()
        expected = width * height * channels * SAMPLE_BYTES[sample]
        if len(data) != expected:
            raise RenderError(f"buffer {name} holds {len(data)} bytes, expected {expected}")
        buffers[name] = data
    statistics_path = directory / fields["statistics"]
    if not statistics_path.is_file():
        raise RenderError(f"no statistics record in {directory}")
    statistics_bytes = statistics_path.read_bytes()
    statistics = parse_statistics(statistics_bytes.decode("utf-8", errors="replace"))
    return Export(
        width, height, fields["compiler"], fields["math_library"], buffers, statistics, manifest_bytes, statistics_bytes
    )


def read_tiff(path):
    """Decodes a deflate TIFF and returns its layout and raw sample bytes, without conversion."""
    with open(path, "rb") as file:
        data = file.read()
    order = {b"II": "<", b"MM": ">"}.get(data[:2])
    if order is None or struct.unpack(order + "H", data[2:4])[0] != 42:
        raise RenderError(f"{path}: not a classic TIFF")
    ifd = struct.unpack(order + "I", data[4:8])[0]
    tags = {}
    for index in range(struct.unpack(order + "H", data[ifd : ifd + 2])[0]):
        entry = ifd + 2 + index * 12
        tag, kind, count = struct.unpack(order + "HHI", data[entry : entry + 8])
        code = {3: "H", 4: "I"}.get(kind)
        if code is None:
            continue
        size = count * struct.calcsize(code)
        offset = entry + 8
        if size > 4:
            offset = struct.unpack(order + "I", data[offset : offset + 4])[0]
        tags[tag] = struct.unpack(f"{order}{count}{code}", data[offset : offset + size])
    if tags.get(259) not in ((8,), (32946,)) or tags.get(317, (1,)) != (1,):
        raise RenderError(f"{path}: unsupported TIFF compression or predictor")
    width, height = tags[256][0], tags[257][0]
    bits, samples_per_pixel = tags[258], tags[277][0]
    strips = zip(tags[273], tags[279])
    samples = b"".join(zlib.decompress(data[start : start + size]) for start, size in strips)
    if len(samples) != width * height * samples_per_pixel * bits[0] // 8:
        raise RenderError(f"{path}: decoded size does not match the image layout")
    return (width, height, bits, samples_per_pixel, tags.get(339), order), samples


def render_and_validate(item, binary, output_dir, report, thread_args=(), neutral=False, context=""):
    """Renders one item and checks its evidence.

    Returns the export, or for a neutral render the path of its image, or None after recording
    the failure, so the run can go on to the remaining items. A SetupError stops the run.
    """
    prefix = f"{context}: " if context else ""
    try:
        returncode, log_path = render_item(item, binary, output_dir, thread_args, neutral)
        check_render_output(returncode, log_path)
        if not neutral:
            return read_export(output_dir / "export")
        image = output_dir / "image.tiff"
        if not image.is_file():
            raise RenderError(f"the neutral render saved no image, see {log_path}")
        try:
            read_tiff(image)
        except (KeyError, IndexError, struct.error, zlib.error) as error:
            raise RenderError(f"{image} cannot be decoded: {error}") from None
        return image
    except RenderError as error:
        report.failed(item.label, f"{prefix}{error}", SETUP)
        return None


def count_differing_samples(first, second, sample):
    """Counts differing samples, comparing the bits of each sample rather than its value."""
    code = SAMPLE_CODES[sample]
    return sum(map(operator.ne, memoryview(first).cast(code), memoryview(second).cast(code)))


def compare_statistics(first, second):
    """Compares two statistics records exactly, naming each statistic and both values (FR-028)."""
    names = list(first) + [name for name in second if name not in first]
    differences = []
    for name in names:
        if first.get(name) != second.get(name):
            one, other = first.get(name, "missing"), second.get(name, "missing")
            differences.append(Difference("statistics", f"{name} {one} vs {other}"))
    return differences


def compare_exports(first, second):
    """Compares two exports exactly, and returns a list of differences (FR-013, FR-014).

    The compiler and math library lines identify a build, so they are never compared.
    """
    differences = []
    if (first.width, first.height) != (second.width, second.height):
        differences.append(Difference("layout", f"{first.width}x{first.height} vs {second.width}x{second.height}"))
    else:
        for name, sample, _ in EXPECTED_BUFFERS:
            if first.buffers[name] != second.buffers[name]:
                count = count_differing_samples(first.buffers[name], second.buffers[name], sample)
                total = len(first.buffers[name]) // SAMPLE_BYTES[sample]
                differences.append(Difference(name, f"{count} of {total} samples differ"))
    return differences + compare_statistics(first.statistics, second.statistics)


def compare_neutral(export, tiff_path):
    """Compares a saved 16-bit image with the final image buffer (FR-012)."""
    (width, height, bits, channels, _, order), samples = read_tiff(tiff_path)
    if (width, height) != (export.width, export.height) or channels != 3 or set(bits) != {16} or order != "<":
        layout = f"{width}x{height}, {channels} channels of {bits[0]} bits"
        return [Difference("layout", f"{layout} vs {export.width}x{export.height}, 3 channels of 16 bits")]
    if samples == export.buffers["final_image"]:
        return []
    count = count_differing_samples(export.buffers["final_image"], samples, "u16")
    return [Difference("final_image", f"{count} of {len(samples) // 2} samples differ")]


def thread_settings():
    """Returns the thread arguments of each render series, and whether coverage is complete (FR-017)."""
    if THREAD_OPTION is None:
        return [()], False
    cpus = os.cpu_count() or 1
    return [(THREAD_OPTION, "1"), (THREAD_OPTION, str(cpus if cpus > 1 else 2))], True


def item_folder(item):
    return OUT_DIR / "renders" / f"{item.kind}s" / item.name


def render_series(item, binary, thread_args, report, first=None):
    """Renders one item SELF_PARITY_RUNS times, comparing each run with its first export (FR-016).

    Returns that first export, or None if the item never produced one.
    """
    count = thread_args[1] if thread_args else "default"
    for run in range(1, SELF_PARITY_RUNS + 1):
        context = f"run {run} with {count} threads"
        output_dir = item_folder(item) / f"threads-{count}-run-{run}"
        export = render_and_validate(item, binary, output_dir, report, thread_args, context=context)
        if export is None:
            break
        if first is None:
            first = export
        else:
            report.differences(item.label, compare_exports(first, export), context=context)
    return first


def check_self_parity(items, binary, report):
    """Runs Self-Parity, and returns each item's first export by label.

    All-thread and neutral renders run one item at a time. Single-thread renders then run for
    several items at once, one worker per CPU, and each item's own runs stay consecutive.
    """
    settings, _ = thread_settings()
    all_threads, single_thread = settings[-1], settings[:-1]
    firsts = {}
    for item in items:
        first = render_series(item, binary, all_threads, report)
        firsts[item.label] = first
        if first is None:
            continue
        report.show_toolchain(first)
        context = "neutral render"
        output_dir = item_folder(item) / "neutral"
        image = render_and_validate(item, binary, output_dir, report, all_threads, neutral=True, context=context)
        if image is not None:
            report.differences(item.label, compare_neutral(first, image), context=context)
    if single_thread:
        remaining = [item for item in items if firsts[item.label] is not None]
        with ThreadPoolExecutor(max_workers=os.cpu_count() or 1) as pool:
            series = [
                pool.submit(render_series, item, binary, single_thread[0], report, firsts[item.label])
                for item in remaining
            ]
            for future in series:
                future.result()
    return firsts


def finish_item(item, report, text="exact match"):
    """Gives an item that did not fail its verdict, and removes its outputs (FR-030)."""
    report.passed(item.label, text)
    if report.item_results[item.label] == "passed":
        shutil.rmtree(item_folder(item), ignore_errors=True)


def split_planes(data, size):
    """Splits samples into byte planes: all first bytes, then all second bytes, and so on."""
    return b"".join(data[plane::size] for plane in range(size))


def join_planes(data, size):
    """Reverses split_planes."""
    joined = bytearray(len(data))
    count = len(data) // size
    for plane in range(size):
        joined[plane::size] = data[plane * count : (plane + 1) * count]
    return bytes(joined)


def write_reference(path, export):
    """Writes one render as a zip. Writing the same data twice gives the same bytes."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    entries = {"manifest.txt": export.manifest_bytes, "statistics.txt": export.statistics_bytes}
    for name, sample, _ in EXPECTED_BUFFERS:
        entries[f"{name}.planes"] = split_planes(export.buffers[name], SAMPLE_BYTES[sample])
    with zipfile.ZipFile(path, "w") as archive:
        for name in REFERENCE_ENTRIES:
            info = zipfile.ZipInfo(name, date_time=REFERENCE_TIMESTAMP)
            info.create_system = 3  # Otherwise the bytes depend on the platform that wrote them.
            archive.writestr(info, entries[name], compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


def read_reference(path):
    """Reads one zip back into an Export. Reading an entry verifies its checksum."""
    try:
        with zipfile.ZipFile(path) as archive:
            if archive.namelist() != list(REFERENCE_ENTRIES):
                raise RenderError("it does not hold the expected entries")
            contents = {name: archive.read(name) for name in REFERENCE_ENTRIES}
        fields = parse_manifest(contents["manifest.txt"], path)
        width = int(fields["width"])
        height = int(fields["height"])
        buffers = {}
        for name, sample, channels in EXPECTED_BUFFERS:
            buffers[name] = join_planes(contents[f"{name}.planes"], SAMPLE_BYTES[sample])
            if len(buffers[name]) != width * height * channels * SAMPLE_BYTES[sample]:
                raise RenderError(f"buffer {name} has the wrong size")
        statistics = parse_statistics(contents["statistics.txt"].decode("utf-8"))
    except (OSError, EOFError, UnicodeDecodeError, zipfile.BadZipFile, zlib.error, RenderError) as error:
        raise SetupError(f"cannot read {path}: {error}") from None
    return Export(
        width,
        height,
        fields["compiler"],
        fields["math_library"],
        buffers,
        statistics,
        contents["manifest.txt"],
        contents["statistics.txt"],
    )


@functools.lru_cache(maxsize=None)
def package_lines():
    """Reports the installed toolchain packages, where the platform can say (FR-022)."""
    command = ["dpkg-query", "-W", "-f", "${Package} ${Version}\n", *TOOLCHAIN_PACKAGES]
    try:
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, encoding="utf-8")
    except OSError:
        return []
    lines = set()
    for line in result.stdout.splitlines():
        name, _, version = line.partition(" ")
        if version.strip():
            lines.add(f"package {name} {version.strip()}")
    return sorted(lines)


def toolchain_identity(export):
    """The lines of toolchain.txt for the build that made an export."""
    return [f"compiler {export.compiler}", f"math_library {export.math_library}"] + package_lines()


def settings_lines():
    """The lines of settings.txt: the render settings a reference set depends on."""
    return [
        f"export_format {EXPORT_VERSION}",
        f"scene_resolution {SCENE_RESOLUTION}",
        f"formula_resolution {FORMULA_RESOLUTION}",
        f"overrides {RENDER_OVERRIDES}",
    ]


def load_reference_set(target):
    """Finds a target's references, and refuses a set made with other render settings."""
    folder = REF_ROOT / target
    if not folder.is_dir():
        raise SetupError(f"no reference set for {target}: {folder} does not exist")
    toolchain_path = folder / "toolchain.txt"
    settings_path = folder / "settings.txt"
    for required in (toolchain_path, settings_path):
        if not required.is_file():
            raise SetupError(f"the reference set for {target} is incomplete: {required} is missing")
    recorded = settings_path.read_text(encoding="utf-8").splitlines()
    changed = [line for line in settings_lines() if line not in recorded]
    if changed or len(recorded) != len(settings_lines()):
        raise SetupError(f"render settings differ from the reference set: {', '.join(changed) or settings_path}")
    return ReferenceSet(folder, toolchain_path.read_text(encoding="utf-8").splitlines())


def list_references(folder):
    """Returns the path of every reference in a folder, by item label."""
    references = {}
    for kind in ("scene", "formula"):
        for path in sorted((folder / f"{kind}s").glob("*.zip")):
            references[f"{kind} {path.stem}"] = path
    return references


def compare_against_references(items, binary, reference_set, report):
    """Renders each item once and compares it with its reference (FR-019, FR-034)."""
    references = list_references(reference_set.folder)
    leftovers = sorted(set(references) - {item.label for item in items})
    report.item_count = len(items) + len(leftovers)
    differing_lines = set()
    for item in items:
        if item.label not in references:
            report.failed(item.label, "no reference for this item")
            continue
        export = render_and_validate(item, binary, item_folder(item) / "gate", report)
        if export is None:
            continue
        report.show_toolchain(export)
        identity = toolchain_identity(export)
        category = PARITY
        if identity != reference_set.toolchain:
            differing_lines.update(set(identity) ^ set(reference_set.toolchain))
            category = MISMATCH
        expected = read_reference(references[item.label])
        report.differences(item.label, compare_exports(expected, export), category)
        finish_item(item, report)
    for label in leftovers:
        report.failed(label, "reference has no work item")
    if differing_lines:
        report.set_toolchain_note(f"the toolchain differs from the reference set: {', '.join(sorted(differing_lines))}")


def finish_items(items, report):
    """Gives every item that did not fail its verdict, and removes the outputs of passing items."""
    for item in items:
        finish_item(item, report)


def require_gate_validity(report):
    """States whether renders are isolated, and blocks a pass if they are not (FR-005)."""
    valid = gate_valid()
    report.set_gate_validity(valid, NOT_ISOLATED_REASON)
    if not valid:
        report.cannot_pass("this run is not gate-valid, so it cannot pass")


def run_self_parity(report):
    require_gate_validity(report)
    if not thread_settings()[1]:
        report.cannot_pass("thread coverage is incomplete, because there is no working headless thread control yet")
    binary = binary_path()
    items = build_work_items(binary)
    report.item_count = len(items)
    check_self_parity(items, binary, report)
    finish_items(items, report)


def run_gate(report):
    """The default mode. It reads references and never writes them (FR-020)."""
    require_gate_validity(report)
    binary = binary_path()
    items = build_work_items(binary)
    reference_set = load_reference_set(parity_target())
    compare_against_references(items, binary, reference_set, report)


def write_lines(path, lines):
    """Writes a text file with LF line endings on every platform."""
    Path(path).write_bytes(("\n".join(lines) + "\n").encode("utf-8"))


def folder_size(folder):
    return sum(file.stat().st_size for file in folder.rglob("*") if file.is_file())


def stage_references(items, exports):
    """Builds a whole reference set under tests/outputs, and returns its folder."""
    staging = OUT_DIR / "staging"
    shutil.rmtree(staging, ignore_errors=True)
    identities = {tuple(toolchain_identity(exports[item.label])) for item in items}
    if len(identities) != 1:
        raise SetupError("the renders came from more than one toolchain")
    for item in items:
        write_reference(staging / f"{item.kind}s" / f"{item.name}.zip", exports[item.label])
    write_lines(staging / "toolchain.txt", list(identities.pop()))
    write_lines(staging / "settings.txt", settings_lines())
    return staging


def promote_references(staging, target, items):
    """Replaces a target's reference set whole, or not at all (FR-018)."""
    if set(list_references(staging)) != {item.label for item in items}:
        raise SetupError("the new reference set is incomplete")
    size = folder_size(staging)
    if size >= MAX_REFERENCE_BYTES:
        raise SetupError(f"the new reference set is {size} bytes, over the {MAX_REFERENCE_BYTES} byte limit")
    final = REF_ROOT / target
    old = OUT_DIR / "replaced-references"
    shutil.rmtree(old, ignore_errors=True)
    final.parent.mkdir(parents=True, exist_ok=True)
    had_old_set = final.exists()
    if had_old_set:
        shutil.move(str(final), str(old))
    try:
        shutil.move(str(staging), str(final))
    except OSError as error:
        if had_old_set:
            shutil.move(str(old), str(final))
        raise SetupError(f"cannot move the new reference set into place: {error}") from None
    shutil.rmtree(old, ignore_errors=True)


def run_capture(report):
    """Passes Self-Parity in this run, then replaces the target's references (FR-018, FR-038)."""
    if os.environ.get("CI") != "true":
        raise SetupError("capture refuses to run outside CI, because references must come from a CI build")
    if not thread_settings()[1]:
        raise SetupError("capture refuses to run, because thread coverage is incomplete")
    report.set_gate_validity(gate_valid(), NOT_ISOLATED_REASON)
    if not report.gate_validity:
        raise SetupError("capture refuses to run, because this run is not gate-valid")
    binary = binary_path()
    items = build_work_items(binary)
    report.item_count = len(items)
    exports = check_self_parity(items, binary, report)
    finish_items(items, report)
    if report.exit_code() != EXIT_PASSED:
        report.note("capture wrote nothing, because Self-Parity did not pass")
        return
    target = parity_target()
    promote_references(stage_references(items, exports), target, items)
    report.note(f"wrote {len(items)} references to {REF_ROOT / target}")


def compare_builds(items, first_binary, second_binary, second_formulas, report):
    """Renders every item with both binaries, and compares the exports exactly (FR-035, FR-036).

    Each binary renders with its own formula ID, because IDs can differ between builds.
    """
    toolchains_shown = False
    for item in items:
        second_item = item
        if item.kind == "formula":
            second_item = replace(item, formula_id=second_formulas[item.name])
        folder = item_folder(item)
        first = render_and_validate(item, first_binary, folder / "first", report, context="first build")
        second = render_and_validate(second_item, second_binary, folder / "second", report, context="second build")
        if first is None or second is None:
            continue
        if not toolchains_shown:
            toolchains_shown = True
            report.line(f"first toolchain: {first.compiler}, {first.math_library}")
            report.line(f"second toolchain: {second.compiler}, {second.math_library}")
        report.differences(item.label, compare_exports(first, second))
        finish_item(item, report)


def run_compare(options, report):
    """Compares two builds on this machine. It never reads references (FR-037)."""
    report.set_gate_validity(gate_valid(), NOT_ISOLATED_REASON)
    report.note("this is a local check, and it does not replace the gate")
    first = binary_path()
    second = Path(options.compare)
    items = build_work_items(first)
    exclusions = load_exclusions()
    first_names = set(load_formulas(first)) - set(exclusions)
    second_formulas = load_formulas(second)
    second_names = set(second_formulas) - set(exclusions)
    only_first = sorted(first_names - second_names)
    only_second = sorted(second_names - first_names)
    shared = [item for item in items if item.kind == "scene" or item.name in second_names]
    report.item_count = len(shared) + len(only_first) + len(only_second)
    for name in only_first:
        report.failed(f"formula {name}", "offered by the first build only")
    for name in only_second:
        report.failed(f"formula {name}", "offered by the second build only")
    compare_builds(shared, first, second, second_formulas, report)


def build_parser():
    parser = argparse.ArgumentParser(description="Regression harness for the Mandelbulber CPU renderer.")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--self-parity", action="store_true", help="require repeated renders to match each other")
    group.add_argument("--capture", action="store_true", help="pass Self-Parity, then write a new reference set")
    group.add_argument("--compare", metavar="BINARY", help="compare the default binary with a second binary")
    group.add_argument("--collect", metavar="DIR", help="render every item once and write a collection")
    group.add_argument("--cross-check", nargs=2, metavar=("FIRST", "SECOND"), help="compare two collections")
    return parser


def mode_name(options):
    if options.self_parity:
        return "self-parity"
    if options.capture:
        return "capture"
    if options.compare:
        return "compare"
    if options.collect:
        return "collect"
    if options.cross_check:
        return "cross-check"
    return "gate"


def run_mode(options, report):
    if options.self_parity:
        run_self_parity(report)
    elif options.capture:
        run_capture(report)
    elif options.compare:
        run_compare(options, report)
    elif options.collect or options.cross_check:
        raise SetupError("this mode is not implemented yet")
    else:
        run_gate(report)


def stop_run(report, cause):
    """Ends a run that could not finish. Without a report, the cause goes to the console."""
    if report is None:
        print(f"setup error: {cause}", flush=True)
        return EXIT_SETUP
    report.setup_error(cause)
    return report.finish()


def main(argv=None):
    options = build_parser().parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(errors="replace")  # A log line must not stop a run on a narrow console.
    mode = mode_name(options)
    target = None if mode in ("collect", "cross-check") else parity_target()
    report = None
    try:
        report = Report(mode, detect_environment(), target)
        run_mode(options, report)
        return report.finish()
    except SetupError as error:
        return stop_run(report, str(error))
    except Exception as error:  # An unexpected error must never look like a parity failure.
        return stop_run(report, f"unexpected harness error: {type(error).__name__}: {error}")
    finally:
        if report is not None:
            report.close()


if __name__ == "__main__":
    sys.exit(main())
