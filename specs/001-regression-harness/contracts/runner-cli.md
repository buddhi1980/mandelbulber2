# Contract: Regression Runner Command Line

```text
python3 tests/regression_runner.py [--self-parity | --capture | --compare BINARY | --collect DIR | --cross-check FIRST SECOND]
```

Run it from the repository root. On Windows, use `python` in place of `python3`. The command line is otherwise the same in every environment.

## Modes

| Command | Purpose | Writes references |
|---|---|---|
| (no mode) | Gate: compare renders against references | No |
| `--self-parity` | Require repeated renders to match each other | No |
| `--capture` | Pass Self-Parity, then write a new reference set. Runs only in CI | Yes |
| `--compare BINARY` | Compare the default binary with a second binary on this machine | No |
| `--collect DIR` | Render every work item once with the default binary, and write a [collection](collection.md) to `DIR` | No |
| `--cross-check FIRST SECOND` | Compare two collections from their files alone. Starts no build | No |

Exactly one mode runs at a time. Capture refuses to run unless the `CI` environment variable is `true`, which GitHub Actions always sets.

## Thread Counts

Self-Parity uses 1 thread and the machine's CPU count, or 2 threads on a single-CPU machine. All-thread and neutral renders run one item at a time. Single-thread renders then run for several items at once, one worker per CPU, and each item's own runs stay consecutive.

## Environment

| Variable | Default | Meaning |
|---|---|---|
| `MANDELBULBER_BINARY` | `mandelbulber2/build/appdir/usr/bin/mandelbulber2` on Linux, `build-win/Release/x64/mandelbulber2.exe` on Windows | Binary under test |
| `PARITY_TARGET` | `linux-x86_64-gcc` on Linux, `windows-x86_64-msvc` on Windows | Reference Target. No references exist for Windows, so a gate run there reports a missing reference set |

Every render runs with the binary's own folder as its working directory, because the Windows build finds its data there.

## Exit Codes

| Code | Meaning |
|---|---|
| 0 | Every check passed |
| 1 | Parity failure: drift, non-reproducible output, a difference between builds, or a missing or leftover reference. In a cross-check, a functional disagreement |
| 2 | Invalid command line |
| 3 | Setup or render failure, a Reference Target mismatch, or a result that cannot count as a pass |

Code 3 covers a missing binary, an empty scene list, a missing scene, a failed render, an invalid export, a missing reference set, changed render settings, a Reference Target mismatch, incomplete thread coverage, and a gate or self-parity run that is not gate-valid. Capture also exits 3 when it refuses to run, or when the new set is incomplete, over the size limit, or mixes toolchains. Collect mode exits 3 when any item failed, after it writes the complete collection, and when it refuses its folder. The cross-check exits 3 for a missing or incomplete collection, differing source commits or render settings, or uncommitted changes.

If a run has both parity failures and a code 3 condition that did not stop it, it exits 1, and the report lists both. For example, self-parity on today's build reports differences and incomplete thread coverage, and exits 1. With a toolchain difference, the gate reports each comparison difference as a Reference Target mismatch instead of a parity failure, so those alone exit 3. A missing or leftover reference stays a parity failure, because it does not depend on the toolchain. An unexpected error in the harness exits 3, so exit 1 only ever means that a comparison failed. Differences a cross-check records never change its exit code.

## Report

Output goes to the console and to `tests/outputs/report.txt`. Every line goes to the file. The console shows at most the first 5 statistic differences per item and comparison, followed by a count of the rest, because a histogram has about 1,000 values and renders do not reproduce yet.

```text
mode: gate
environment: github-actions
target: linux-x86_64-gcc
toolchain: gcc 13.3.0, glibc 2.39
[+] scene IFS_001: exact match
[-] scene difs_tree raw_render: 214566 of 230400 samples differ
[-] scene difs_tree statistics: total_iterations 316706000 vs 316706012
[+] formula mandelbulb: exact match
result: parity failure (1 of 487 items failed)
```

A cross-check report uses `[~]` for a recorded difference, which never fails the run:

```text
mode: cross-check
environment: wsl
first: wsl, isolated, gcc 13.3.0, glibc 2.39
second: windows, not isolated, msvc 195136231, unknown
source: 012345678 on both
[+] scene IFS_001: both builds agree
[~] scene IFS_001 raw_render: 230400 of 230400 samples differ
[~] scene IFS_001 statistics: 412 of 1290 values differ, first total_iterations 316706000 vs 270312000
[-] formula mandelbulbPower2: offered by first only
identical: 0 of 487 items matched exactly in every buffer and statistic
result: functional disagreement (1 of 487 items failed)
note: this check does not replace the gate, and a difference between builds is not drift
```

**Rules**

- Every report names the environment (FR-040).
- Every line names the item, and every failure names the buffer or statistic.
- In gate mode, a toolchain difference with matching output appears as a note and the run passes. With differing output, the result is `reference target mismatch`.
- Compare mode prints both toolchains and states that it is a local check that does not replace the gate.
- On platforms without isolation, the report states that the run is not gate-valid. Compare mode can still pass there, and collect mode can still complete.
- A collect report states that the run does not replace the gate, and it notes a binary older than the commit.
- A cross-check report names the build in every failure, and it states that the check does not replace the gate. It claims equality only for items that matched exactly in every buffer and statistic. It adds a note if both sides used the same compiler, and if either binary was older than its commit.
- A cross-check report has one `[~]` line per differing buffer and one per item's statistics. Every differing statistic is in `tests/outputs/cross-check-details.txt`, which is described in [collection.md](collection.md).

## Files

| Path | Purpose |
|---|---|
| `tests/scenes.txt` | Standard Scene Suite, one path per line. Blank lines and lines starting with `#` are ignored |
| `tests/formula_sweep_base.fract` | Base scene for the formula sweep |
| `tests/formula_exclusions.txt` | One excluded formula per line, as `name reason`. Blank lines and lines starting with `#` are ignored |
| `tests/outputs/` | Working output. Failed items are kept here, and passing items are removed |
| `tests/references/<target>/` | [Reference set](reference-set.md) |
| `tests/outputs/collection/` | The usual place for collections, which are not version-controlled |
| `tests/outputs/cross-check-details.txt` | Every statistic that differed in the last cross-check, as described in [collection.md](collection.md) |
