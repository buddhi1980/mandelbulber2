# Quickstart: Validating Deterministic CPU Rendering

Runnable checks that prove each user story. [plan.md](plan.md) says how the work is built, and [contracts/](contracts/) holds the exact behavior.

## Prerequisites

- Every result comes from a fresh build of the commit being tested. No check may use an existing binary.
- The Linux checks run in the WSL `Ubuntu-24.04` clone at `~/mandelbulber2`, and the Windows checks use the MSVC Release build. The [playbook](../../MIGRATION_PLAYBOOK.md) lists both setups.
- Scratch scripts in these checks import `tests/regression_runner.py` for its isolation and comparison. They stay out of the repository.

Fresh WSL build of the current branch, from the WSL clone:

```bash
git fetch file:///mnt/c/GIT/mandelbulber2 upgrade/deterministic-rendering
git checkout --detach FETCH_HEAD
cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make clean
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
```

The `BIN` variable below is the installed binary:

```bash
BIN=$PWD/mandelbulber2/build/appdir/usr/bin/mandelbulber2
```

## 1. Thread control (User Story 1)

Time one scene for each stage, at one thread and then at four. With `--threads 1`, user plus system time stays within 110% of elapsed time (SC-005).

```bash
EXAMPLES=mandelbulber2/deploy/share/mandelbulber2/examples
OVERRIDES='opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4'
/usr/bin/time -f '%e elapsed, %U user, %S system' $BIN --threads 1 --nogui --format tiff --override "$OVERRIDES" --output /tmp/t1 "$EXAMPLES/IFS_002.fract"
```

| Stage | Scene |
|---|---|
| Render workers | `IFS_001.fract` |
| Screen-space ambient occlusion | `amazing_surf 002.fract` |
| Post-effect depth of field | `IFS_002.fract` |
| HDR blur | `tests/scenes/hdr_blur.fract` |

Repeat with the other headless modes:

| Mode | Command, after `$BIN --threads 1` |
|---|---|
| Keyframe animation | `--nogui --keyframe -s 0 -e 2 "$EXAMPLES/keyframe_anim_mandelbulb.fract"` |
| Flight animation | `--nogui --flight -s 0 -e 2 "$EXAMPLES/flight_anim_menger sponge_3.fract"` |
| Voxel export | `--nogui --voxel slice <scene>` |
| Queue | `--nogui <two scenes>` |
| Test suite | `--test` |
| Benchmark | `--benchmark 1` |

Check `OMP_NUM_THREADS` and a count above the CPU count:

```bash
OMP_NUM_THREADS=8 /usr/bin/time -f '%e elapsed, %U user, %S system' $BIN --threads 1 --nogui --format tiff --output /tmp/t2 "$EXAMPLES/IFS_002.fract"
```

```bash
$BIN --threads 64 --nogui --res 32x24 --format tiff --output /tmp/t3 "$EXAMPLES/IFS_001.fract"
```

Check each refusal (SC-006). Each command prints an error that names `--threads`, exits non-zero, and renders nothing. Run them with no display, so a message box would show up as a failure.

| Arguments | Expected status on Linux |
|---|---|
| `--threads 0 --nogui scene.fract` | 175 |
| `--threads -1 --nogui scene.fract` | 175 |
| `--threads abc --nogui scene.fract` | 175 |
| `--threads 1.5 --nogui scene.fract` | 175 |
| `--threads 99999999999 --nogui scene.fract` | 175 |
| `--nogui scene.fract --threads` | 1 |
| `--threads 4 scene.fract` | 174 |
| `--threads 4 --gpu --nogui scene.fract` | 174 |
| `--threads 4 --gpuall --nogui scene.fract` | 174 |
| `--threads 4 --list` | 174 |
| `--threads 4 --list-formulas` | 174 |
| `--threads 4 --touch scene.fract` | 174 |
| `--threads 4 --help-opencl` | 174 |

Check network rendering by running a server and a client on one computer, with `--threads 2` on the client. The server's log shows 2 workers for that client.

## 2. Evidence (User Story 2)

Follow [contracts/evidence-record.md](contracts/evidence-record.md), then read `evidence.md` alone. It names a cause for every difference (SC-007), and it says whether one-thread renders reproduced before the fix. Confirm no diagnostic change reached the branch:

```bash
git diff --stat master -- mandelbulber2/src/common_math.cpp
```

The only change in that file is the removal of the shared generator.

## 3. Renders reproduce (User Stories 3 and 5)

```bash
python3 tests/regression_runner.py --self-parity
```

It exits 0, reports 487 items passed, and no longer says thread coverage is incomplete. Run it three times in a row (SC-001).

Thread counts beyond those two (SC-002): a scratch script renders every item at 1, 2, 3, 4, 7, and twice the CPU count, then compares each with its one-thread export using `compare_exports`. It reports 0 differences.

Seeds and frames (SC-009): a scratch script renders each of the 6 Monte Carlo scenes twice with each of these overrides added to the harness's own, and compares the results.

| Override | Compared with the default | Compared with itself |
|---|---|---|
| None | - | Identical |
| `render_random_seed=2` | Different | Identical |
| `frame_no=1` | Different | Identical |

Check one real animation too (User Story 6). Render frames 0 and 1 of `keyframe_anim_mandelbulb.fract` twice with `DOF_monte_carlo=true` added to `--override`. Frames 0 and 1 differ from each other, and each frame matches its own second render.

## 4. Exact statistics (User Story 4)

Every item reports `total_iterations` above 0, and the value matches at every thread count (SC-003). A scratch script reads `statistics.txt` from each export of the SC-002 run.

Compare the builds on each side of the statistics work (SC-004). Build the commit before it into a second folder, then:

```bash
python3 tests/regression_runner.py --compare <binary from before the statistics work>
```

The report shows 0 differences in any buffer. Statistics do differ, because total iterations changes from 0 to the real count.

Watch a render with `--stats`, and confirm the values change while it runs. In the GUI, confirm the statistics dock updates during a render (FR-029).

## 5. Nothing else broke (User Story 6)

Harness tests, on both builds:

```bash
python3 tests/test_regression_runner.py
```

Application tests, on fresh qmake and MSVC builds. All pass, including the known-answer test for random values:

```bash
$BIN --test
```

On a fresh MSVC build, `--self-parity` reports 0 differences, although it can't call that a pass on Windows (SC-013).

A one-bit difference seeded in any buffer still fails, which the harness's own tests cover (SC-011).

Noise (SC-010): a scratch script finds the average absolute difference between the final images of two runs on a fresh build of `master`, then between seeds 1 and 2 on this branch, for each of the 6 Monte Carlo scenes. Noise rises by at most 5%. Then a reviewer looks at the final images for patterns.

Time (SC-014): record the time of `--collect` on `master` and on this branch, and the depth of field stage on `IFS_002.fract` at 1920x1080. The Collect step of Cross-Build Check stands in for a gate run, and it stays under 30 minutes.

Search the source (SC-015). This prints nothing, and any call left to the old `Random()` would stop the build:

```bash
grep -rnE 'gRandomSeed|RandomInt' mandelbulber2/src mandelbulber2/qt mandelbulber2/formula
```

## 6. KD-1 closes (User Story 5)

The constitution shows version 1.0.1 and KD-1 closed, and nothing exists under `tests/references/` (SC-016):

```bash
ls tests/references
```
