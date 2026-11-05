#!/usr/bin/env python3
"""Stand-in for the mandelbulber2 binary, used by tests/test_regression_runner.py.

It implements specs/001-regression-harness/contracts/application-cli.md and renders nothing.
Its mode comes from stub_mode.txt in its working directory, because the harness's clean
environment removes unknown variables. Without that file the mode is "same". Every call is
appended to stub_calls.jsonl in the same folder.

A mode written as only:<item>:<mode> applies to one scene name or formula ID, and every other
item behaves as "same". The modes are:

  same                     Fixed output for each item.
  flip-bit:<buffer>        One bit differs in one sample of the buffer.
  flip-mantissa:<buffer>   The lowest mantissa bit differs in one sample of a float buffer.
  stat-plus-one:<names>    Adds 1 to each statistic in a comma-separated list.
  unstable                 Every buffer differs on every call, using a counter file.
  unstable:<buffer>        The buffer differs on every call.
  unstable-stat:<names>    Each statistic in a comma-separated list differs on every call.
  no-export                Writes nothing to the export folder.
  no-end                   The manifest has no end line.
  bad-version              The manifest has an unknown format version.
  wrong-size:<buffer>      The buffer file is one byte short.
  wrong-type:<buffer>      The manifest names the wrong sample type for the buffer.
  missing-buffer:<buffer>  The buffer is left out of the manifest and the folder.
  extra-buffer             The export has an unexpected buffer.
  unlisted-buffer          The export has an unexpected buffer file that the manifest leaves out.
  bad-statistics           A statistic is not a whole number.
  no-statistics            The statistics file is missing.
  bad-settings-text        Prints the failed load message and writes no export.
  settings-line-error      Prints a settings line error and still writes an export.
  texture-error            Prints a texture failure and still writes an export.
  alternate-lookup         Prints the alternate asset search and still writes an export.
  system-data-dir          Prints the system data folder fallback and still writes an export.
  print:<text>             Prints the text and otherwise behaves as "same".
  exit-3                   Exits with status 3 and writes nothing.
  hang                     Sleeps 30 seconds and then exits.
  neutral-differs          The neutral image differs from the final image buffer.
  no-neutral-image         The neutral render saves no image.
  resize                   The output is one pixel wider than requested.
  other-toolchain          The manifest names another compiler.
  other-toolchain-drift    The manifest names another compiler, and one buffer differs.
  extra-formula            The formula list has one more entry.
  missing-formula          The formula list has one entry fewer.
  renumber-formulas        Each formula has a different numeric ID.
  no-list-end              The formula list has no end line.
  no-list-start            The formula list has no first line.
  empty-list               The formula list has no entries.
  list-exit-3              --list-formulas exits with status 3.
  list-hang                --list-formulas sleeps 30 seconds and then exits.
  blank                    Every final_image sample holds one value.
  blank-color              Every final_image pixel holds one color, whose R, G, and B differ.
  other-version            --version prints a different version.
  no-version               --version prints no version line.
  other-histogram-size     The iteration histogram has one more bucket.
"""

import hashlib
import json
import os
import struct
import sys
import time
import zlib
from contextlib import contextmanager
from pathlib import Path

VERSION = "Mandelbulber 2.35-test"
COMPILER = "gcc 13.3.0"
MATH_LIBRARY = "glibc 2.39"
OTHER_COMPILER = "gcc 99.1.0"
BUFFERS = (
    ("raw_render", "f32", 3),
    ("post_effect", "f32", 3),
    ("final_image", "u16", 3),
    ("alpha", "u16", 1),
    ("depth", "f32", 1),
    ("normal", "f32", 3),
    ("normal_world", "f32", 3),
)
SAMPLE_BYTES = {"f32": 4, "u16": 2}
FORMULAS = ((10000, "custom"), (2, "mandelbulb"), (3, "mandelbulbPower2"), (19, "aexion"))
EXTRA_FORMULA = (777, "stubExtra")
RENUMBER_OFFSET = 1000
ITERATION_BUCKETS = 50
STEP_BUCKETS = 1000
OPTIONS_WITH_VALUE = {"--threads", "--res", "--format", "--override", "--output", "--verification-export"}


@contextmanager
def folder_lock():
    """Serializes the calls that share this folder, because renders can run at the same time."""
    while True:
        try:
            handle = os.open("stub.lock", os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            break
        except OSError:
            time.sleep(0.001)
    try:
        yield
    finally:
        os.close(handle)
        os.remove("stub.lock")


def next_count():
    """Returns a number no earlier call returned."""
    path = Path("stub_counter.txt")
    with folder_lock():
        count = int(path.read_text(encoding="utf-8")) + 1 if path.exists() else 1
        path.write_text(str(count), encoding="utf-8")
    return count


def record_call(arguments):
    output = arguments[arguments.index("--output") + 1] if "--output" in arguments else None
    call = {
        "args": arguments,
        "env": dict(os.environ),
        "cwd": os.getcwd(),
        "output_folder_exists": output is not None and os.path.isdir(os.path.dirname(output)),
    }
    line = json.dumps(call)
    with folder_lock():
        with open("stub_calls.jsonl", "a", encoding="utf-8") as file:
            file.write(line + "\n")


def read_mode():
    path = Path("stub_mode.txt")
    return path.read_text(encoding="utf-8").strip() if path.exists() else "same"


def parse_arguments(arguments):
    options = {"positional": []}
    remaining = iter(arguments)
    for argument in remaining:
        if argument in OPTIONS_WITH_VALUE:
            options[argument] = next(remaining)
        elif argument.startswith("--"):
            options[argument] = True
        else:
            options["positional"].append(argument)
    return options


def formula_table(mode):
    offset = RENUMBER_OFFSET if mode == "renumber-formulas" else 0
    table = [(number + offset, name) for number, name in FORMULAS]
    if mode == "extra-formula":
        table.append(EXTRA_FORMULA)
    if mode == "missing-formula":
        table.pop()
    return table


def print_formulas(mode):
    if mode == "list-exit-3":
        return 3
    if mode == "list-hang":
        time.sleep(30)
    print("cannot move folder /home/settings to /home/mandelbulber/settings!")
    if mode != "no-list-start":
        print("mandelbulber-formula-list 1")
    if mode != "empty-list":
        for number, name in formula_table(mode):
            print(f"{number}\t{name}")
    if mode != "no-list-end":
        print("end")
    return 0


def print_version(mode):
    print("cannot move folder /home/settings to /home/mandelbulber/settings!")
    if mode != "no-version":
        print("Mandelbulber 2.35-other" if mode == "other-version" else VERSION)


def identify_item(options, global_mode):
    """Returns the identity a mode can name, and the name its fixed data comes from."""
    for part in options.get("--override", "").split("#"):
        if part.startswith("fractal1_formula="):
            number = part.partition("=")[2]
            names = dict(formula_table(global_mode))
            return number, names.get(int(number), number)
    stem = Path(options["positional"][-1]).stem
    return stem, stem


def item_mode(mode, identity):
    if mode.startswith("only:"):
        target, _, inner = mode[len("only:"):].partition(":")
        return inner if target == identity else "same"
    return mode


def split_mode(mode):
    name, _, argument = mode.partition(":")
    return name, argument


def fixed_bytes(seed, size):
    digest = hashlib.sha256(seed.encode("utf-8")).digest()
    return bytearray((digest * (size // len(digest) + 1))[:size])


def build_buffers(seed, width, height, mode, count):
    """Returns the fixed sample data for one render, with the mode's change applied."""
    name, argument = split_mode(mode)
    buffers = {}
    for buffer, sample, channels in BUFFERS:
        data = fixed_bytes(f"{seed}/{buffer}", width * height * channels * SAMPLE_BYTES[sample])
        if name == "flip-bit" and argument == buffer:
            data[len(data) // 2] ^= 0x10
        if name == "flip-mantissa" and argument == buffer:
            data[0] ^= 0x01
        if name == "unstable" and argument in ("", buffer):
            data[:4] = struct.pack("<I", count)
        if name == "other-toolchain-drift" and buffer == "raw_render":
            data[0] ^= 0x01
        if name == "blank" and buffer == "final_image":
            data = bytearray(b"\x34\x12" * (len(data) // 2))
        if name == "blank-color" and buffer == "final_image":
            data = bytearray(struct.pack("<3H", 0x1000, 0x2000, 0x3000) * (len(data) // 6))
        buffers[buffer] = data
    return buffers


def build_statistics(seed, mode, count):
    """Returns the statistics record as a list of name and value pairs."""
    name, argument = split_mode(mode)
    iteration_buckets = ITERATION_BUCKETS + 1 if name == "other-histogram-size" else ITERATION_BUCKETS
    names = ["total_iterations", "raymarch_count", "missed_de"]
    for histogram, buckets in (("iteration_histogram", iteration_buckets), ("step_histogram", STEP_BUCKETS)):
        names.append(f"{histogram}.size")
        names += [f"{histogram}.{bucket}" for bucket in range(buckets + 1)]
    values = {}
    for key in names:
        if key.endswith(".size"):
            values[key] = iteration_buckets if key.startswith("iteration") else STEP_BUCKETS
        else:
            digest = hashlib.sha256(f"{seed}/{key}".encode("utf-8")).digest()
            values[key] = int.from_bytes(digest[:4], "little") % 100000
    for key in argument.split(","):
        if name == "stat-plus-one":
            values[key] += 1
        if name == "unstable-stat":
            values[key] += count
    return list(values.items())


def write_export(directory, width, height, buffers, statistics, mode):
    name, argument = split_mode(mode)
    directory.mkdir(parents=True, exist_ok=True)
    version = 99 if name == "bad-version" else 1
    compiler = OTHER_COMPILER if name.startswith("other-toolchain") else COMPILER
    manifest = [
        f"mandelbulber-verification-export {version}",
        f"width {width}",
        f"height {height}",
        f"compiler {compiler}",
        f"math_library {MATH_LIBRARY}",
    ]
    for buffer, sample, channels in BUFFERS:
        if name == "missing-buffer" and argument == buffer:
            continue
        data = bytes(buffers[buffer])
        if name == "wrong-size" and argument == buffer:
            data = data[:-1]
        if name == "wrong-type" and argument == buffer:
            sample = "u16" if sample == "f32" else "f32"
        (directory / f"{buffer}.bin").write_bytes(data)
        manifest.append(f"buffer {buffer} {sample} {channels}")
    if name == "extra-buffer":
        (directory / "specular.bin").write_bytes(bytes(width * height * 12))
        manifest.append("buffer specular f32 3")
    if name == "unlisted-buffer":
        (directory / "specular.bin").write_bytes(bytes(width * height * 12))
    if name != "no-statistics":
        text = "".join(f"{key} {value}\n" for key, value in statistics)
        if name == "bad-statistics":
            text = text.replace("missed_de", "missed_de 1.5e8\nignored", 1)
        (directory / "statistics.txt").write_bytes(text.encode("utf-8"))
    manifest.append("statistics statistics.txt")
    if name != "no-end":
        manifest.append("end")
    (directory / "manifest.txt").write_bytes(("\n".join(manifest) + "\n").encode("utf-8"))


def write_tiff(path, width, height, samples):
    """Writes a classic little-endian 16-bit RGB TIFF with one deflate strip."""
    strip = zlib.compress(bytes(samples))
    entry_count = 10
    bits_offset = 8 + 2 + entry_count * 12 + 4
    strip_offset = bits_offset + 6
    entries = (
        (256, 4, 1, width),
        (257, 4, 1, height),
        (258, 3, 3, bits_offset),
        (259, 3, 1, 8),
        (262, 3, 1, 2),
        (273, 4, 1, strip_offset),
        (277, 3, 1, 3),
        (278, 4, 1, height),
        (279, 4, 1, len(strip)),
        (284, 3, 1, 1),
    )
    content = b"II" + struct.pack("<HI", 42, 8) + struct.pack("<H", entry_count)
    for tag, kind, count, value in entries:
        content += struct.pack("<HHI", tag, kind, count) + struct.pack("<I", value)
    content += struct.pack("<I", 0) + struct.pack("<3H", 16, 16, 16) + strip
    Path(path).write_bytes(content)


def render(options, global_mode):
    identity, seed = identify_item(options, global_mode)
    mode = item_mode(global_mode, identity)
    name, argument = split_mode(mode)
    export = options.get("--verification-export")

    if name == "exit-3":
        return 3
    if name == "hang":
        time.sleep(30)
        return 0
    if name == "print":
        print(argument)
    messages = {
        "bad-settings-text": "Error: It's not valid Mandelbulber settings file. No header",
        "settings-line-error": "Error in settings file. Line: 5 (bad_parameter 1;)",
        "texture-error": "Can't load texture: missing.png",
        "alternate-lookup": "Looking for the file at /home/textures/missing.png",
        "system-data-dir": "Trying to use /usr/share/mandelbulber2 as the data folder",
    }
    if name in messages:
        print(messages[name])
    if name in ("bad-settings-text", "no-export"):
        return 0

    width, height = (int(part) for part in options["--res"].split("x"))
    if name == "resize":
        width += 1
    count = next_count() if name in ("unstable", "unstable-stat") else 0
    buffers = build_buffers(seed, width, height, mode, count)
    if export:
        statistics = build_statistics(seed, mode, count)
        write_export(Path(export), width, height, buffers, statistics, mode)
    elif name != "no-neutral-image":
        samples = bytearray(buffers["final_image"])
        if name == "neutral-differs":
            samples[0] ^= 0x01
        write_tiff(options["--output"] + ".tiff", width, height, samples)
    print(f"Image saved to: {options.get('--output')}.tiff")
    return 0


def main(arguments):
    record_call(arguments)
    options = parse_arguments(arguments)
    mode = read_mode()
    if "--list-formulas" in options:
        return print_formulas(mode)
    if "--version" in options:
        print_version(mode)
        return 0
    return render(options, mode)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
