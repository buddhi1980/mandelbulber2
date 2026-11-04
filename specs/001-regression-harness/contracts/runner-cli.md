# Contract: Regression Runner Command Line

```text
python3 tests/regression_runner.py [--self-parity | --capture | --compare BINARY]
```

Run it from the repository root.

## Modes

| Command | Purpose | Writes references |
|---|---|---|
| (no mode) | Gate: compare renders against references | No |
| `--self-parity` | Require repeated renders to match each other | No |
| `--capture` | Pass Self-Parity, then write a new reference set. Runs only in CI | Yes |
| `--compare BINARY` | Compare the default binary with a second binary on this machine | No |

Exactly one mode runs at a time. Capture refuses to run unless the `CI` environment variable is `true`, which GitHub Actions always sets.

## Thread Counts

Self-Parity uses 1 thread and the machine's CPU count, or 2 threads on a single-CPU machine. All-thread and neutral renders run one item at a time. Single-thread renders then run for several items at once, one worker per CPU, and each item's own runs stay consecutive.

## Environment

| Variable | Default | Meaning |
|---|---|---|
| `MANDELBULBER_BINARY` | `mandelbulber2/build/appdir/usr/bin/mandelbulber2` | Binary under test |
| `PARITY_TARGET` | `linux-x86_64-gcc` | Reference Target |

## Exit Codes

| Code | Meaning |
|---|---|
| 0 | Every check passed |
| 1 | Parity failure: drift, non-reproducible output, a difference between builds, or a missing or leftover reference |
| 2 | Invalid command line |
| 3 | Setup or render failure, a Reference Target mismatch, or a result that cannot count as a pass |

Code 3 covers a missing binary, an empty scene list, a missing scene, a failed render, an invalid export, a missing reference set, changed render settings, a Reference Target mismatch, incomplete thread coverage, and a gate or self-parity run that is not gate-valid. Capture also exits 3 when it refuses to run, or when the new set is incomplete, over the size limit, or mixes toolchains.

If a run has both parity failures and a code 3 condition that did not stop it, it exits 1, and the report lists both. For example, self-parity on today's build reports differences and incomplete thread coverage, and exits 1. With a toolchain difference, the gate reports each comparison difference as a Reference Target mismatch instead of a parity failure, so those alone exit 3. A missing or leftover reference stays a parity failure, because it does not depend on the toolchain. An unexpected error in the harness exits 3, so exit 1 only ever means a parity failure.

## Report

Output goes to the console and to `tests/outputs/report.txt`.

```text
mode: gate
target: linux-x86_64-gcc
toolchain: gcc 13.3.0, glibc 2.39
[+] scene IFS_001: exact match
[-] scene difs_tree raw_render: 214566 of 230400 samples differ
[-] scene difs_tree statistics: total_iterations 316706000 vs 316706012
[+] formula mandelbulb: exact match
result: parity failure (1 of 487 items failed)
```

**Rules**

- Every line names the item, and every failure names the buffer or statistic.
- In gate mode, a toolchain difference with matching output appears as a note and the run passes. With differing output, the result is `reference target mismatch`.
- Compare mode prints both toolchains and states that it is a local check that does not replace the gate.
- On platforms without isolation, the report states that the run is not gate-valid. Compare mode can still pass there.

## Files

| Path | Purpose |
|---|---|
| `tests/scenes.txt` | Standard Scene Suite, one path per line. Blank lines and lines starting with `#` are ignored |
| `tests/formula_sweep_base.fract` | Base scene for the formula sweep |
| `tests/formula_exclusions.txt` | One excluded formula per line, as `name reason`. Blank lines and lines starting with `#` are ignored |
| `tests/outputs/` | Working output. Failed items are kept here, and passing items are removed |
| `tests/references/<target>/` | [Reference set](reference-set.md) |
