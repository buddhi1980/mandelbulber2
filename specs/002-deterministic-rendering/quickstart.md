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

| Stage | Scene | Turned off with | Raised with |
|---|---|---|---|
| Render workers | `IFS_001.fract` | - | - |
| Screen-space ambient occlusion | `amazing_surf 002.fract` | `ambient_occlusion_enabled=false` | `ambient_occlusion_quality` |
| Post-effect depth of field | `IFS_002.fract` | `DOF_enabled=false` | `DOF_radius` |
| HDR blur | `tests/scenes/hdr_blur.fract` | `hdr_blur_enabled=false` | `hdr_blur_radius` |

Each scene must spend most of its one-thread time in its stage (SC-005), because a stage under about 9% of a run could ignore `--threads` and still stay within 110%. Time each post effect's scene at `--threads 1` with the stage on and with it off, using `--override`. Its share is the difference divided by the time with it on. If the share is 50% or less, raise the setting in the table until it is above 50%, and keep that setting for every later run of that scene in this section. `amazing_surf 002.fract` also has post-effect depth of field, so add `DOF_enabled=false` to every run of it.

Repeat with the other headless modes:

```bash
VOXEL='voxel_samples_x=100#voxel_samples_y=100#voxel_samples_z=100'
```

| Mode | Command, after `$BIN --threads 1` |
|---|---|
| Keyframe animation | `--nogui --keyframe -s 0 -e 2 --output "$(mktemp -d)/" "$EXAMPLES/keyframe_anim_mandelbulb.fract"` |
| Flight animation | `--nogui --flight -s 0 -e 2 --output "$(mktemp -d)/" "$EXAMPLES/flight_anim_menger sponge_3.fract"` |
| Voxel export | `--nogui --voxel slice --override "$VOXEL" "$EXAMPLES/voxel_export_with_Bristorbrot4D.fract"` |
| Mesh export | `--nogui --voxel ply --override "$VOXEL" "$EXAMPLES/voxel_export_with_Bristorbrot4D.fract"` |
| Queue | `--nogui "$EXAMPLES/IFS_001.fract" "$EXAMPLES/Makin3D-Julia_001.fract"` |
| Test suite | `--test` |
| Benchmark | `--benchmark 1` |

Each animation run gets a new output folder, because a rerun into the same folder stops to ask about deleting frames. The path ends in `/`, because the application joins the frame's file name straight onto it. Neither queue scene has an OpenMP stage, so `limit_CPU_cores` governs all of their work.

The queue never exits, because it waits for more work after its last item, so `/usr/bin/time` can't measure it. Run it through a scratch script that gives it a new temporary `HOME`, samples its CPU time from `/proc/<pid>/stat` once a second, and stops it when `Queue is empty` appears in its output. At `--threads 1`, no sample is above 1.1 CPUs (SC-005).

Set `QTEST_FUNCTION_TIMEOUT=1800000` for the test suite, as CI does. QtTest stops a function after 5 minutes by default, and one function renders all 791 examples.

Check `OMP_NUM_THREADS` and a count above the CPU count:

```bash
OMP_NUM_THREADS=8 /usr/bin/time -f '%e elapsed, %U user, %S system' $BIN --threads 1 --nogui --format tiff --output /tmp/t2 "$EXAMPLES/IFS_002.fract"
```

```bash
$BIN --threads 64 --nogui --res 32x24 --format tiff --output /tmp/t3 "$EXAMPLES/IFS_001.fract"
```

Check each refusal (SC-006). Each command prints an error that names `--threads`, exits non-zero, and renders nothing. Run them with no display, so a message box would show up as a failure. `scene.fract` stands for `$EXAMPLES/IFS_001.fract`.

| Arguments | Expected status on Linux |
|---|---|
| `--threads 0 --nogui scene.fract` | 175 |
| `--threads -1 --nogui scene.fract` | 175 |
| `--threads abc --nogui scene.fract` | 175 |
| `--threads 1.5 --nogui scene.fract` | 175 |
| `--threads 99999999999 --nogui scene.fract` | 175 |
| `--threads "" --nogui scene.fract` | 175 |
| `--threads=0 --nogui scene.fract` | 175 |
| `--nogui scene.fract --threads` | 1 |
| `--threads 4 scene.fract` | 174 |
| `--threads 4 --gpu --nogui scene.fract` | 174 |
| `--threads 4 --gpuall --nogui scene.fract` | 174 |
| `--threads 4 --list` | 174 |
| `--threads 4 --list-formulas` | 174 |
| `--threads 4 --touch scene.fract` | 174 |
| `--threads 4 --help-input` | 174 |
| `--threads 4 --help-examples` | 174 |
| `--threads 4 --help-opencl` | 174 |

Check network rendering by running a server and a client on one computer, with `--threads 2` on the client. The server's log shows 2 workers for that client.

Then use `--threads 1` on both, with a render that lasts at least 30 seconds. The client never exits either, so sample it the same way as the queue, until the server exits. No sample is above 1.1 CPUs (SC-005). Start the client within 5 seconds of the server each time, because the server only waits that long for clients.

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
| `render_random_seed=1` | Identical | Identical |
| `render_random_seed=2` | Different | Identical |
| `frame_no=1` | Different | Identical |

Check one real animation too (User Story 6). Make a scratch copy of `keyframe_anim_mandelbulb.fract` in which every row of `[keyframes]` repeats the values of keyframe 0, so frames 0 and 1 differ only in their frame number. Its own frames can't show this, because the fractal power and light change between them. Render frames 0 and 1 twice with `--keyframe -s 0 -e 2`, `--format tiff`, and `DOF_monte_carlo=true` added to `--override`. Give each render a new empty `--output` folder whose path ends in `/`, for the reasons in section 1. Decode the images with the harness's `read_tiff`. Frames 0 and 1 differ from each other, and each frame matches its own second render.

Compare two builds of one source (SC-010 of `specs/001-regression-harness`). Build the final commit a second time in another folder, then run `--compare` with that binary. It exits 0 on all 487 items. Change one formula that no suite scene uses in a third build of the same commit, and run `--compare` again. It exits 1, and the report names only that formula.

Check two-pass stereo. Render `amazing_surf 002.fract` with `stereo_enabled=true#stereo_mode=2` added to the harness's overrides, three times at one thread and three times at all threads. Red-cyan stereo with screen-space ambient occlusion takes two passes. All six renders match in every buffer and statistic.

## 4. Exact statistics (User Story 4)

Every item reports `total_iterations` above 0, and the value matches at every thread count (SC-003). A scratch script reads `statistics.txt` from each export of the SC-002 run.

Compare the builds on each side of the statistics work (SC-004). Build the commit before it into a second folder, then:

```bash
python3 tests/regression_runner.py --compare <binary from before the statistics work>
```

The report shows 0 differences in any buffer. Statistics do differ, because total iterations changes from 0 to the real count.

Watch a render with `--stats`, and confirm the values change while it runs. In the GUI, confirm the statistics dock updates during a render (FR-029).

Check for colliding updates (FR-027). In a throwaway clone, build with `QMAKE_CXXFLAGS+=-fsanitize=thread QMAKE_LFLAGS+=-fsanitize=thread` added to the `qmake6` line, then render `IFS_001.fract` and `chromatic_aberration.fract` at the harness's resolution with `--threads 4`. No report names `cStatistics`, `cHistogram`, or `cPositionRandom`. Libgomp isn't built for ThreadSanitizer, so reports from inside OpenMP regions can be false, and `cScheduler`'s unlocked reads of line owners are expected. This build never leaves the clone.

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

The constitution shows version 1.0.1, KD-1 closed, and the seed clarification, and nothing exists under `tests/references/` (SC-016):

```bash
ls tests/references
```
