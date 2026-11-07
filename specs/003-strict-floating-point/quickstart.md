# Quickstart: Validating Strict Floating-Point Builds

Runnable checks that prove each user story. [plan.md](plan.md) says how the work is built, and [contracts/](contracts/) holds the exact behavior.

## Prerequisites

- Every result comes from a fresh build of the commit being tested. No check may use an existing binary.
- The Linux checks run in the WSL `Ubuntu-24.04` distro, and the Windows checks use MSVC. The [playbook](../../MIGRATION_PLAYBOOK.md) lists both setups.
- Scratch scripts live in `~/verify/kd2`, starting from copies of KD-1's in `~/verify/kd1`. They stay out of the repository.
- Never stop FractalEvaluator. Output checks can run beside it, but timing can't (section 6).
- `--override` reads a boolean as a number, so use `=1` and `=0`.
- "Final commit" means the last commit that changes what compiles. A CI or document commit doesn't move it.

Fresh WSL builds, from `~/verify/kd2`. `build_fresh.sh` fetches the Windows repository, checks out the ref, applies a patch if given, and builds from clean:

```bash
./build_fresh.sh ~/mandelbulber2-before 8fd49ba59
```

```bash
./build_fresh.sh ~/mandelbulber2 windows/upgrade/strict-floating-point
```

```bash
CXXFLAGS=-march=native ./build_fresh.sh ~/mandelbulber2-native 8fd49ba59
```

```bash
./build_fresh.sh ~/mandelbulber2-diag windows/upgrade/strict-floating-point guard_counts.patch
```

The binaries the sections below use:

```bash
BIN=mandelbulber2/build/appdir/usr/bin/mandelbulber2
BEFORE=$HOME/mandelbulber2-before/$BIN
AFTER=$HOME/mandelbulber2/$BIN
NATIVE=$HOME/mandelbulber2-native/$BIN
DIAG=$HOME/mandelbulber2-diag/$BIN
```

## 1. Strict rules in every build (User Story 1)

Read the gated build's flags, from `~/mandelbulber2`:

```bash
grep -E '^(CFLAGS|CXXFLAGS|LFLAGS|LIBS)[[:space:]]*=' mandelbulber2/build/Makefile
```

They match [build-flags.md](contracts/build-flags.md): `-ffp-contract=off` in `CFLAGS` and `CXXFLAGS`, and no prohibited option anywhere.

The binary no longer needs the vector math library, which only `-ffast-math` brought in:

```bash
readelf -d "$AFTER" | grep -c libmvec
```

It prints 0.

Read the CMake build's flags. From a fresh clone of the final commit:

```bash
cmake -S mandelbulber2/cmake -B /tmp/cmake-kd2 -DCMAKE_BUILD_TYPE=Release
```

```bash
grep CXX_FLAGS /tmp/cmake-kd2/CMakeFiles/mandelbulber2.dir/flags.make
```

It shows `-fopenmp -ffp-contract=off`. The engine has no C sources, so read `CMAKE_C_FLAGS` from `CMakeLists.txt`. Confirm nothing reaches the linker:

```bash
grep -cE 'fast-math|Ofast' /tmp/cmake-kd2/CMakeFiles/mandelbulber2.dir/link.txt
```

It prints 0.

Search the engine for pragmas and attributes (SC-004). This prints only the six `diagnostic` lines in `radiance_hdr.cpp`, which change warnings and not code:

```bash
grep -rnE '#\s*pragma\s+(GCC|clang|float_control|fenv_access|STDC|fp_contract)|__attribute__\s*\(\(\s*optimize' mandelbulber2/src mandelbulber2/qt mandelbulber2/formula
```

## 2. The same instructions on any computer (User Story 2)

This prints nothing:

```bash
grep -nE 'march=|mtune=|mcpu=' mandelbulber2/tools/prepare_for_dev_*.sh mandelbulber2/deploy/linux/install
```

Each `prepare_for_dev_*.sh` script still exports `-msse2`. The script's compile step works with those flags: run its `qmake6` line in a temporary folder with `CXXFLAGS=-msse2`, and read the generated flags. A builder's own `CXXFLAGS` reaches the build in the same way (section 3).

## 3. The flag check (User Story 3)

On the final commit, from the clone's root:

```bash
bash .github/actions/compile-engine/check-flags.sh search
```

It prints the number of files it searched, then `passed`.

```bash
bash .github/actions/compile-engine/check-flags.sh flags mandelbulber2/build/Makefile
```

It prints `passed`.

Run every seed in [flag-check.md](contracts/flag-check.md) with the committed script (SC-002). `~/verify/kd2/seed_check.sh` copies the generated Makefile for each `flags` seed, runs `qmake6` in temporary folders for the environment seeds, and uses a throwaway clone for the `search` seeds. Each row matches its expected exit code and name.

The local checks in sections 4 to 7 come before any push, because a fix would move the final commit and repeat CI. Then, on the pull request, `parity-gate` passes (SC-001). Prove it in GitHub Actions (SC-003), as [research.md](research.md) R8 describes. Pushing needs the user's go-ahead. After both seeded runs, close the draft pull request unmerged, delete the branch, and cancel any other runs it started. Confirm nothing reached `ministic-regression`:

```bash
git log --oneline ministic-regression..origin/upgrade/strict-floating-point
```

No seed commit appears.

A search of the repository (SC-004) finds only what [flag-check.md](contracts/flag-check.md) leaves out, and the history in `deploy/NEWS`:

```bash
git grep -nIE -e '-ffast-math|-Ofast|-funsafe-math-optimizations|-ffinite-math-only|-fassociative-math|-freciprocal-math|-march=native|-mtune=native|-mcpu=native|/fp:fast|FloatingPointModel>Fast' -- . ':!specs' ':!MIGRATION_PLAYBOOK.md' ':!.specify'
```

## 4. Renders reproduce (User Story 4)

From `~/mandelbulber2`:

```bash
python3 tests/regression_runner.py --self-parity
```

It exits 0 and reports 487 items passed. Run it three times in a row (SC-005). A failure here goes to FR-018 and not to the harness.

## 5. What changed in the output (User Story 5)

Collect each build from its own clone, with the harness from its own commit. This doesn't need a quiet computer, because only the timed runs in section 6 do:

```bash
python3 tests/regression_runner.py --collect ~/verify/kd2/collect-after
```

From `~/mandelbulber2`, compare the builds:

```bash
python3 tests/regression_runner.py --compare "$BEFORE"
```

It exits 1, and its report lists each item's differences (FR-019). `--cross-check` can't compare these two collections, because it refuses two commits, so a scratch script imports the harness's comparison functions instead:

```bash
python3 ~/verify/kd2/output_record.py ~/verify/kd2/collect-before ~/verify/kd2/collect-after
```

It writes `output-record.tsv`, as [evidence-record.md](contracts/evidence-record.md) defines, prints how many items are identical, names any final image that holds one value on one side only, and ranks the swept formulas.

Prove the diagnostic build leaves renders alone:

```bash
python3 tests/regression_runner.py --compare "$DIAG"
```

It exits 0 on all 487 items. Then `~/verify/kd2/guard_counts.py` renders every item with `$DIAG` and writes the Guards table.

`~/verify/kd2/review_images.py` writes the before, after, and difference images for the 26 scenes and the 20 formulas. A person reviews them, and the evidence names each visible change and its cause (SC-006).

## 6. What it costs (User Story 6)

Before timing, wait until `pgrep -f FractalEvaluator` has been empty for a minute. During each run, sample it every 5 seconds and log the time from `/proc/uptime`, as KD-1's `run_quiet.sh` does. Repeat any run that overlaps. A check made only at the start isn't enough, and the harness prints its result lines at the end, so check in the background.

| Run | Command, from each build's own clone |
|---|---|
| All threads, before build first | `timed_collect.py <dir>` on before, then on after |
| All threads, after build first | `timed_collect.py <dir>` on after, then on before |
| One CPU | `taskset -c 4 python3 ~/verify/kd2/timed_collect.py <dir>`, on both builds |
| Host-tuned | `timed_collect.py <dir>` on `$NATIVE` at all threads |

`~/verify/kd2/timed_collect.py` runs the harness's `--collect`, wrapping `render_item` so one run gives the whole time, and scenes and formulas apart. It reads the monotonic clock, because the WSL wall clock steps back by about 0.85 seconds every 32 seconds. The evidence records every time (SC-007).

On the pull request, read the Collect step of Cross-Build Check's `linux-qmake` job. It stays under 30 minutes (SC-008). It took 4.4 minutes on the last KD-1 pull request.

## 7. Everything else still works (User Story 7)

Application tests, on fresh qmake and MSVC builds:

```bash
env -i HOME="$(mktemp -d)" PATH="$PATH" LC_ALL=C.UTF-8 QTEST_FUNCTION_TIMEOUT=1800000 "$AFTER" --test
```

Every test passes (SC-009). On Windows, set `QT_FORCE_STDERR_LOGGING=1`, and run it from `build-win/Release/x64`.

Build each configuration in [research.md](research.md) R14 from a fresh tree (SC-010):

| Build | Command |
|---|---|
| qmake without OpenCL | `qmake6 ../qmake/mandelbulber.pro CONFIG+=release`, then `make` |
| CMake | `cmake --build /tmp/cmake-kd2 -j"$(nproc)"` |
| MSVC Release | The playbook's `nuget restore` and `MSBuild.exe` lines |
| MSVC Debug | The same lines with `/p:Configuration=Debug`. Its `cl` lines hold `/fp:precise` |
| macOS | CircleCI on the pull request |

Build MSVC Debug on `8fd49ba59` first. If it fails there, it doesn't compile today, and the evidence says so.

Fresh MSVC Release builds of `8fd49ba59` and of the final commit each run `--collect`. `output_record.py` on those two collections finds all 487 items identical, because neither the Release flags nor the sources changed.

Then cross-check the qmake and MSVC collections of the final commit, as the playbook's Local Development section shows, and record the `identical` line (SC-009). Cross-Build Check passes on the pull request.

Confirm what must not change (SC-011). These print nothing, apart from anything FR-018 required:

```bash
git diff --stat 8fd49ba59 -- mandelbulber2/src mandelbulber2/qt mandelbulber2/formula tests
```

```bash
git diff --stat 8fd49ba59 -- .github/workflows/cross-build-check.yml .github/workflows/github-actions-ci.yml .github/workflows/codeql.yml .github/actions/build-linux .github/actions/build-windows .appveyor.yml azure-pipelines.yml .circleci
```

## 8. KD-2 and KD-3 close (User Story 8)

The constitution shows version 1.0.2, with KD-2 and KD-3 closed and KD-4 open, and nothing exists under `tests/references/` (SC-012):

```bash
grep -nE '^\| KD-|^\*\*Version' .specify/memory/constitution.md
```

```bash
ls tests/references
```

The playbook's listings match the committed action and workflow, and its Phase 4 has the math path step (FR-029).
