# Contract: The Forced Math Path

How the harness, `tests/regression_runner.py`, forces glibc's plain x86-64 math path, confirms it, and records it, for FR-001 to FR-004. [research.md](../research.md) R1 to R4 give the reasons.

## The setting

```text
GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4
```

- One constant in the harness holds this value, and both the environment and the settings line use it.
- `clean_environment()` adds it on Linux, so every render and every query the harness starts gets it, in every mode. On Windows the environment doesn't change.
- Nothing from the caller's environment reaches a render, as before, so a `GLIBC_TUNABLES` set in CI or in a shell can't add to or replace this value.

## The probe

On Linux, gate and capture run it once, after their existing checks and before they build the work list, so a failure stops them before any render. Self-Parity, compare, and collect don't run it.

1. Start a child process, the harness's own Python interpreter, with the render environment from `clean_environment()` and a temporary home.
2. The child computes each function below at its input, given as bits, and prints each result's bits.
3. Compare each result with the plain result. If one differs, run the child again with the hidden setting below in place of the forced one, to tell the two failures apart.

| Function | Input | Plain result | FMA result |
|---|---|---|---|
| `sin` | `c023f95182a9930c` | `3fe10e8ae531c9f9` | `3fe10e8ae531c9f8` |
| `cos` | `c023a35a858793de` | `bfed8b7b0b14170a` | `bfed8b7b0b14170b` |
| `exp` | `c033f504816f0069` | `3e227b3381e9a676` | `3e227b3381e9a677` |
| `log` | `3ff2e042927c0e01` | `3fc528a95565bf3c` | `3fc528a95565bf3b` |
| `atan` | `c02bb0cb295e9e1b` | `bff7faa774776c93` | `bff7faa774776c94` |
| `asin` | `bfef0b04ab606b7b` | `bff535bf4d1dfedb` | `bff535bf4d1dfedc` |

Bits are big-endian hexadecimal, from glibc `2.39-0ubuntu8.9`.

4. Run the same child once more without the setting, to find the CPU's own path: `fma` if every result is the FMA one, `plain` if every one is plain, and `unknown` otherwise.
5. Read the CPU's model name and whether its flags include `fma`, `fma4`, and `avx2`, from `/proc/cpuinfo`.

The hidden setting hides the optional x86-64 features below, and SC-004 uses it too:

```text
GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX,-AVX2,-FMA,-FMA4,-SSE3,-SSSE3,-SSE4_1,-SSE4_2,-POPCNT,-LZCNT,-MOVBE,-BMI1,-BMI2,-F16C,-AVX512F,-AVX512CD,-AVX512BW,-AVX512DQ,-AVX512VL,-ERMS,-XSAVE,-XSAVEC
```

## Messages

| Case | Line | Exit |
|---|---|---|
| Forced | `math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: <model name>, flags fma avx2` | Run goes on |
| Missed a path | `setup error: the forced math path did not take effect: sin gave 3fe10e8ae531c9f8, and with every optional feature hidden it gives 3fe10e8ae531c9f9` | 3 |
| Plain results changed | `setup error: glibc's plain math results changed: sin gives <bits> with the forcing and with every optional feature hidden, and the probe expects 3fe10e8ae531c9f9` | 3 |
| Can't run | `setup error: cannot run the math path probe: <cause>` | 3 |
| Not x86-64 | `setup error: the forced math path needs an x86-64 CPU` | 3 |
| Not Linux | `math path: not forced, because this isn't Linux` | The gate goes on for debugging, and can't pass |

The forced line goes to the console and to `tests/outputs/report.txt`, so the evidence can quote the CPU of every CI run. A failure names the first function that differs.

## The record

`settings_lines()` gains a fifth line:

```text
math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4
```

- Capture writes it into `settings.txt`, so Baseline 1 holds it.
- The gate's existing check refuses a set whose lines differ: "render settings differ from the reference set: math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4". A set without the line is refused before anything renders.
- Collections don't change. `COLLECTION_FIELDS` and `COLLECTION_SETTINGS` keep their names and values, so cross-check reads Windows and Linux collections as before.

## What doesn't change

- The render resolutions and overrides, and the export format (FR-028).
- Compare, collect, and cross-check, apart from Linux renders taking the forced path.
- Exit codes: a probe failure is a setup error, like any other.

## Tests

`tests/test_regression_runner.py` gains these, and they pass on Linux and on Windows:

| Test | Shows |
|---|---|
| The environment on Linux | `clean_environment()` holds the setting, and a caller's `GLIBC_TUNABLES` doesn't reach it |
| The environment on Windows | `clean_environment()` holds no `GLIBC_TUNABLES` |
| A forced probe | A child that prints the plain results passes, and the report names the CPU's own path |
| A forcing that missed a path | A child that prints an FMA result under the forcing and the plain one when hidden fails as a setup error that says the forcing missed a path and names the function |
| Changed plain results | A child that prints the same unexpected result under the forcing and when hidden fails as a setup error that says glibc's plain results changed |
| A probe that can't run | A child that exits non-zero or prints the wrong number of lines fails as a setup error |
| The CPU's own path | All FMA gives `fma`, all plain gives `plain`, and a mix gives `unknown` |
| The gate stops first | With a failing probe, the gate exits 3 and the stand-in renderer never runs |
| Capture stops first | With a failing probe, capture exits 3, renders nothing, and leaves the reference set as it was |
| The gate on Windows | With no probe, the gate prints the not-forced line, runs for debugging, and can't pass |
| Another CPU architecture | On Linux with a machine that isn't `x86_64`, the gate and capture exit 3 before any render |
| The settings line | `settings.txt` from capture holds the `math_path` line |
| Old and other sets | The gate refuses a set whose `settings.txt` lacks the line or holds another setting |
| Collections | A collection's listing has the same fields as before, and cross-check of two collections still passes |

The shared setup for the stand-in renderer replaces the probe with one that passes, so every existing test runs as before on both systems. The tests in this table use the real probe function with a stand-in child.
