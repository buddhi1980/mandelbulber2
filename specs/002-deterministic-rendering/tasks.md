---

description: "Task list for deterministic CPU rendering"
---

# Tasks: Deterministic CPU Rendering

**Input**: Design documents from `specs/002-deterministic-rendering/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: Included where the spec asks for them: the known-answer test (SC-012) and the harness's own tests (FR-031, SC-011). Every other check runs a fresh build through [quickstart.md](quickstart.md).

**Organization**: Tasks are grouped by user story, in the plan's order of work. That order is fixed, because each story needs the one before it. User Stories 5 and 6 check the finished work, and closing KD-1 comes after both, in the last phase.

## Rules for Every Task

- Every result comes from a fresh build of the commit being tested. No task may use an existing binary, including the one already installed in the WSL clone.
- WSL can only see commits, so each step commits first and then builds. A fix that a check calls for is a new commit on top of that step, before the next step starts.
- Each step of the plan gets its own commit, so SC-004 can compare builds on either side of the statistics work. T014, T029, T047, T051, T061, and T088 are those commits. Every commit must compile. Stage only the files the step changed.
- Each commit also stages this file, with its finished tasks marked `[X]`. A task after T029 that only records a result in `evidence.md` commits it with a short `docs:` message, because the project commits after each task that changes the repository. T066 finds the final engine commit with a path filter, so these commits don't move it. T090 and T091 come after the last commit, so they stay unmarked in the branch, because another commit would restart the pull request's checks.
- Commit messages and the pull request carry no `Co-Authored-By` line, no other co-author statement, and no "Generated with" line. Pushing needs the user's go-ahead.
- Change nothing outside the plan: no compile flags, no workflows, no GUI control for the seed, and no change to formulas, shading math, or default settings (FR-036, FR-038).
- Diagnostic changes stay in a throwaway clone and never reach the repository (FR-011).
- Scratch scripts stay out of the repository, in `~/verify/kd1/` in WSL. They import `tests/regression_runner.py` for its isolation and comparison, and they do not rewrite either.
- No tolerance, exclusion, or skip list may hide a difference (FR-023). The 5% noise limit is a one-time quality check, not a parity comparison.
- Do not create anything under `tests/references/` (FR-033).
- `--override` reads a boolean as a number, so use `=1` and `=0`. `=true` reads as 0, which silently turns the setting off.
- New C++ must compile as C++17, and as C++20 in the MSVC project. It follows `.clang-format`, which uses tabs.
- Line numbers below come from commit `a41ffadb5`, and they move as the work goes on, so each task also names the function or the code.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (for example US1)

## Path Conventions

- Application: `mandelbulber2/src/` and `mandelbulber2/qt/`
- Harness: `tests/` at the repository root
- This feature's documents: `specs/002-deterministic-rendering/`

## Build Environments

- **WSL**: the `Ubuntu-24.04` distro, with GCC 13.3.0, Qt 6.4.2, and 32 logical CPUs. The main clone is `~/mandelbulber2`. `~/mandelbulber2-before`, `~/mandelbulber2-base`, and `~/mandelbulber2-diag` hold builds to compare with, and only `~/mandelbulber2-diag` may hold diagnostic changes. Use `wsl -d Ubuntu-24.04`, because the default distro has no Qt. `valgrind` and `cmake` are not installed yet, and `sudo` needs a password, so only the user can install them. The wall clock steps back by about 0.85 seconds every 32 seconds, so every time check in WSL uses `~/verify/kd1/cpu_rate.py`, which reads the monotonic clock. A ThreadSanitizer binary only starts under `setarch "$(uname -m)" -R`.
- **Windows**: the MSVC Release build from the playbook's Local Development section, in a new clone at `C:\GIT\mb2-kd1-<first 9 characters of the commit>`. Every Windows check on that build runs inside the clone. A new clone has no `build-win`. The checkout's own `build-win` holds generated files from older source, and the project compiles every `.cpp` file in that folder, so using it would need a permanent delete of 17 GB, which only the user may approve. The clones stay until the user clears them. A Windows run can't pass `--self-parity`, because the application ignores a redirected home folder there.
- **Base branch**: `ministic-regression` is the base for this feature and every later regression feature. Until this feature merges, it holds the commit this feature started from. `master` follows upstream, so it has no harness, and no check compares with it.
- **GitHub Actions**: T091 needs the branch pushed. No workflow changes, and no workflow runs Self-Parity until playbook Phase 4.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Confirm the starting point, and make fresh builds repeatable

- [X] T001 Confirm the starting point from the repository root: the branch is `upgrade/deterministic-rendering`, `git status --short` prints nothing, `tests/references` does not exist, and `python3 tests/test_regression_runner.py` passes all 223 tests
- [X] T002 In WSL, write `~/verify/kd1/build_fresh.sh <clone> <ref> [patch] [qmake arguments]`. It clones `/mnt/c/GIT/mandelbulber2` if `<clone>` is missing, fetches every branch from `file:///mnt/c/GIT/mandelbulber2` into `refs/remotes/windows/`, discards any earlier patch with `git reset --hard` and `git clean -fd`, checks out `<ref>` detached, and applies `[patch]` unless it is `-`. It then creates `mandelbulber2/build` if it is missing and runs the build steps from `specs/002-deterministic-rendering/quickstart.md`, including `qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release` with any extra arguments, `make clean`, `make -j$(nproc)`, and `make INSTALL_ROOT=appdir install`. It prints the commit and the binary's path. Every fresh WSL build below uses it

---

## Phase 2: Foundational (Blocking Prerequisites)

No tasks. User Story 1 is the foundation: the spec's order of work needs the thread control before the evidence, and the evidence before the random values, so the stories run in order and not in parallel.

---

## Phase 3: User Story 1 - Choose the Thread Count for a Headless Render (Priority: P1) 🎯 MVP

**Goal**: `--threads N` sets the thread count of every parallel stage of a headless CPU render (plan step 1).

**Independent Test**: Render one scene for each of screen-space ambient occlusion, post-effect depth of field, and HDR blur at one thread, then at four. The first uses about one CPU's worth of time, and the second uses more.

### Implementation for User Story 1

- [X] T003 [US1] In `mandelbulber2/src/system_data.hpp`, add `int threadsFromCommandLine = 0;` below `numberOfThreads` in `sSystem`, and declare `int GetOpenMpThreads() const;`. Define it in `mandelbulber2/src/system_data.cpp`, beside the other `sSystem` methods: it returns `threadsFromCommandLine` if that is above 0, and otherwise `omp_get_max_threads()`, or 1 in a build without OpenMP. Include `<omp.h>` under `#ifdef _OPENMP`, as `voxel_export.cpp` does. The fallback matters: using `numberOfThreads` there would make `limit_CPU_cores` start limiting OpenMP regions in the GUI, which FR-004 forbids
- [X] T004 [US1] In `mandelbulber2/src/command_line_interface.hpp`, add `cliErrorThreadsInvalid = -81` and `cliErrorThreadsNotAllowed = -82` to `cliErrors`, after `cliErrorVerificationExportMode`. Declare the private methods `void handleThreads(const QString &threadsText);` and `static void removeThreadsOption(QStringList &arguments);`
- [X] T005 [US1] In the `cCommandLineInterface` constructor in `mandelbulber2/src/command_line_interface.cpp`, add `threadsOption` with the name `threads`, the value name `N`, and the help text from `specs/002-deterministic-rendering/contracts/application-cli.md`, and add it to the parser (FR-001, FR-006). At the very end of the constructor, after the lines that set `cliData.nogui`, call `handleThreads(parser.value(threadsOption))` if `parser.isSet(threadsOption)`
- [X] T006 [US1] Implement `cCommandLineInterface::handleThreads` in `mandelbulber2/src/command_line_interface.cpp`, in this order (FR-005, research R2). First, refuse with `cliErrorThreadsInvalid` unless `threadsText.toInt(&ok)` succeeds with a value of at least 1. Second, refuse with `cliErrorThreadsNotAllowed` if `cliData.gpu`, `cliData.gpuAll`, `cliData.listParameters`, `cliData.listFormulas`, `cliData.touch`, `cliData.showInputHelp`, `cliData.showExampleHelp`, or `cliData.showOpenCLHelp` is set. Third, refuse the same way unless the run is headless, which means `cliData.nogui` (it already covers `--queue`, `--test`, and `--benchmark`), `cliData.keyframe`, `cliData.flight`, `cliData.voxel`, `cliData.server`, `cliData.host != ""`, more than one positional argument, or one positional argument that is a `.fractlist` or a folder, the same test as `handleArgs`. Last, set `systemData.numberOfThreads` and `systemData.threadsFromCommandLine` to the value. Each refusal calls `cErrorMessage::showMessage` with text that names `--threads`, then `parser.showHelp` with its status, the pattern `ReadCLI` uses for `--verification-export`. This runs before `QApplication` exists, so no message box can open
- [X] T007 [US1] Implement `cCommandLineInterface::removeThreadsOption` in `mandelbulber2/src/command_line_interface.cpp`. It removes `--threads` and the argument after it, and any argument that starts with `--threads=`. Call it on `arguments` in `runTestCasesAndExit` and `runBenchmarksAndExit`, before `QTest::qExec`, because QTest stops on options it does not know
- [X] T008 [P] [US1] In `cQueue::RenderQueue` in `mandelbulber2/src/queue.cpp:573`, set `systemData.numberOfThreads` from `limit_CPU_cores` only when `systemData.threadsFromCommandLine` is 0, so `--threads` takes precedence (FR-004)
- [X] T009 [US1] Add `num_threads(systemData.GetOpenMpThreads())` to all 7 OpenMP regions: `mandelbulber2/src/dof.cpp:101`, `mandelbulber2/src/dof.cpp:284`, `mandelbulber2/src/post_effect_hdr_blur.cpp:78`, `mandelbulber2/src/voxel_export.cpp:239`, `mandelbulber2/src/marchingcubes.cpp:267`, `mandelbulber2/src/audio_track.cpp:302`, and `mandelbulber2/src/opencl_engine_render_dof.cpp:177` (FR-002). Keep each region's `schedule`. Put the `voxel_export.cpp` pragma on one line, and move its comment to the line above. Include `system_data.hpp` in `voxel_export.cpp`, `audio_track.cpp`, and `opencl_engine_render_dof.cpp`. The `dof.cpp:284` region needs the clause for now, because T022 measures it, and T050 removes it
- [X] T010 [US1] Where per-thread buffers are sized, change `omp_get_max_threads()` to `systemData.GetOpenMpThreads()` in `mandelbulber2/src/voxel_export.cpp:228` and `mandelbulber2/src/marchingcubes.cpp:107`, so a thread index always fits its buffer when the count is above the default (FR-003)
- [X] T011 [P] [US1] In `tests/regression_runner.py:55`, set `THREAD_OPTION = "--threads"`, and replace the comment above it with one that says it names the application's thread option. Make no other change to the file (FR-031)
- [X] T012 [US1] In `tests/test_regression_runner.py`, keep the case with no thread control covered by patching `THREAD_OPTION` to `None` in the 4 tests that relied on the old default: `ThreadCoverageTests.test_identical_output_exits_three`, `ThreadCoverageTests.test_differing_output_exits_one_and_reports_both`, `ThreadCoverageTests.test_thread_settings_without_a_control`, and `StatisticsComparisonTests.test_console_shows_five_and_counts_the_rest`. `test_no_thread_control_refuses` already patches it. Add `test_thread_control_is_enabled`, which checks that `regression_runner.THREAD_OPTION` is `"--threads"` (FR-031)
- [X] T013 [US1] Run `python3 tests/test_regression_runner.py`, and confirm all 224 tests pass. Confirm `grep -rnE '^\s*#pragma omp parallel' mandelbulber2/src | grep -v num_threads` prints nothing
- [X] T014 [US1] Commit step 1 with the message `feat: add --threads for headless CPU renders`, staging the files changed in T003 to T012 under `mandelbulber2/src/` and `tests/`

### Checks for User Story 1

- [X] T015 [US1] Build T014's commit with `build_fresh.sh ~/mandelbulber2 windows/upgrade/deterministic-rendering`. Confirm `--help` describes `--threads` with the text in `specs/002-deterministic-rendering/contracts/application-cli.md` (FR-006), and that `--threads 4 --help` and `--threads 4 --version` still exit 0 (FR-005)
- [X] T016 [US1] Write `~/verify/kd1/cpu_rate.py`, the timing script section 1 of `specs/002-deterministic-rendering/quickstart.md` describes, and use it for both tables in that section, one row for each parallel stage and then each headless mode, at `--threads 1` and `--threads 4`. Confirm each post effect adds at least 2 seconds of CPU time at one thread, by timing its row again with the stage turned off. At one thread, no sample is above 1.1 CPUs (SC-005, FR-001, FR-002). At four threads, no sample is above 4.4 CPUs, and the highest sample on each stage row is above 1.1 CPUs. Keep the numbers for T025
- [X] T017 [US1] Run the two remaining checks at the end of section 1 of `specs/002-deterministic-rendering/quickstart.md`: `OMP_NUM_THREADS=8` with `--threads 1` still has no sample above 1.1 CPUs (FR-002), and `--threads 64` renders at the harness's formula resolution, above the CPU count, with no crash (FR-003). Repeat `--threads 64` with `--voxel slice` and with `--voxel ply`, which use the buffers from T010. Keep the numbers for T025
- [X] T018 [US1] Run every row of the refusal table in section 1 of `specs/002-deterministic-rendering/quickstart.md` through `env -u DISPLAY -u WAYLAND_DISPLAY`, because WSL has a display and a message box must not be able to open. Each row exits with the listed status, prints an error that names `--threads`, and writes no image (SC-006, FR-005). Keep the results for T025
- [X] T019 [US1] Confirm the defaults in `specs/002-deterministic-rendering/contracts/application-cli.md` (FR-004). With no `--threads`, `IFS_001.fract` peaks above 90% of the logical CPU count in `cpu_rate.py`. The queue from section 1 of `specs/002-deterministic-rendering/quickstart.md`, with `limit_CPU_cores=2` set through `--override`, peaks between 1.8 and 2.2 CPUs in `cpu_rate.py`, and the same queue with `--threads 3` peaks between 2.7 and 3.3. Keep the numbers for T025
- [X] T020 [US1] Run the network check in section 1 of `specs/002-deterministic-rendering/quickstart.md`: a server and a client on one computer, each with its own temporary `HOME`. With `--threads 2` on the client, the server's log shows 2 workers for that client (FR-001). With `--threads 1` on both, the client shows one thread named `RenderWorker` while it renders, and never more (SC-005). Keep the results for T025

**Checkpoint**: Every parallel stage obeys `--threads`, every refusal works, and the behavior without the option has not changed. KD-1 is still open, so `--self-parity` now fails on real differences.

---

## Phase 4: User Story 2 - Confirm the Cause Before Replacing Anything (Priority: P1)

**Goal**: A written record says why renders differ, before anything is replaced (plan step 2). [contracts/evidence-record.md](contracts/evidence-record.md) defines it.

**Independent Test**: Read `evidence.md` alone. For each tested scene, it says whether one-thread renders reproduce, and which buffers and statistics still differ once random values no longer depend on thread order.

- [X] T021 [US2] Write `~/verify/kd1/evidence.py`, run from a clone's root with that clone's `tests/regression_runner.py` on the path. It takes a binary, a label, a list of thread counts, a run count, and a scene list that defaults to the 13 scenes in `specs/002-deterministic-rendering/contracts/evidence-record.md`. It finds each scene in `build_work_items(binary)`, renders it with `render_item(item, binary, folder, ("--threads", str(count)))`, reads each result with `read_export`, and compares the first run with each later run, and the first run at each thread count with the first run at the first count, using `compare_exports`. It records every differing buffer and statistic, and the lowest and highest value of each statistic, in `~/verify/kd1/results/<label>.json`. It stops on a failed render. `--self-parity` can't do this, because it compares single-thread runs with the first all-thread run (research R10)
- [X] T022 [US2] With T015's binary, run `evidence.py` at 1 thread and at all threads, 3 renders of each, with the label `thread-control` (FR-007, FR-008)
- [X] T023 [US2] Write `~/verify/kd1/constant_values.patch`, which makes `Random(max)` in `mandelbulber2/src/common_math.cpp` return `max / 2`. Build it with `build_fresh.sh ~/mandelbulber2-diag <T014's commit> ~/verify/kd1/constant_values.patch`, then run `evidence.py` at 1 thread and at all threads, 3 renders of each, with the label `constant-values` (FR-007, FR-011)
- [X] T024 [US2] Write `~/verify/kd1/one_thread_blend.patch`, which holds the change from `constant_values.patch` and also removes the depth of field phase II pragma from `mandelbulber2/src/dof.cpp`. Build it the same way in `~/mandelbulber2-diag`, then run `evidence.py` at all threads, 3 renders of each, with the label `constant-values-one-thread-blend` (FR-007, FR-011)
- [X] T025 [US2] Create `specs/002-deterministic-rendering/evidence.md` from `specs/002-deterministic-rendering/contracts/evidence-record.md`, with the header, builds, scenes, runs, and one Results row per scene, build, and thread count, including the spread of each statistic (FR-008, FR-009). Describe the local changes in words. Add the results of T016 to T020 to the Success criteria table
- [X] T026 [US2] In `specs/002-deterministic-rendering/evidence.md`, write one Causes entry per cause found, with the code's file and line, the scenes that show it, the buffers and statistics it reaches, the comparison that isolates it, and the requirement that removes it (FR-009, SC-007). Say whether one-thread renders reproduced before the fix, and whether the 30 percent figure in the request repeats. The spec's Assumptions list the candidates, and the record states only what was measured
- [X] T027 [US2] If one-thread renders differ, or a difference in `specs/002-deterministic-rendering/evidence.md` has no cause, find the cause before going on (FR-010, FR-023). A ThreadSanitizer build, made as T065 makes it, lists data races, and `valgrind` finds reads of uninitialized values (research R10). Add any new work to this file as new tasks. The shared seed is still replaced, because Principle II forbids it either way
- [X] T028 [US2] Confirm the diagnostic changes stayed in `~/mandelbulber2-diag`: in the repository, `git status --short` shows nothing under `mandelbulber2/` or `tests/`, and `git diff --stat master -- mandelbulber2/src/common_math.cpp` prints nothing (FR-011)
- [X] T029 [US2] Commit step 2 with the message `docs: record why CPU renders differ`, staging `specs/002-deterministic-rendering/evidence.md`

**Checkpoint**: Every difference has a named cause. Nothing about how random values are made has changed yet.

---

## Phase 5: User Story 3 - Render the Same Values at Any Thread Count (Priority: P1)

**Goal**: Every engine buffer reproduces at any thread count (plan steps 3 and 4). [contracts/random-values.md](contracts/random-values.md) is the contract for the generator.

**Independent Test**: Render one Monte Carlo scene and one post-effect depth of field scene several times, at one thread and at all threads, and compare every buffer.

### Generator and its test

- [X] T030 [US3] Create `mandelbulber2/src/position_random.hpp` with the class `cPositionRandom`, following the algorithm in `specs/002-deterministic-rendering/contracts/random-values.md` exactly. Use the repository's license header with `Authors: Mandelbulber Team`, the include guard `MANDELBULBER2_SRC_POSITION_RANDOM_HPP_`, and `<cstdint>` as the only include. Members: `enum enumStage { stageRender = 1, stageSSAO = 2, stageDOF = 3, stageOutsideRender = 4 };`, a default constructor that leaves the key and state at 0, `cPositionRandom(int seed, int frame, int eye, int stage)`, `void SetPosition(int a, int b, int c)`, and `int Random(int max)`. Every function is inline, because `Random` sits on the hottest path. All arithmetic uses `std::uint64_t`, each input converts to `std::uint32_t` and then widens, and the divisor is `std::uint64_t(max) + 1` so it can't overflow an `int`. Nothing else feeds the state (FR-012, FR-014, FR-017, FR-018)
- [X] T031 [US3] Write `~/verify/kd1/known_answers.cpp`, which checks all 36 values in the Known answers table of `specs/002-deterministic-rendering/contracts/random-values.md`. Compile it with `g++ -std=c++17 -Wall -Wextra -O2 -I/mnt/c/GIT/mandelbulber2/mandelbulber2/src`, and confirm every value matches before any caller changes
- [X] T032 [US3] Add `void testPositionRandom() const;` to `private slots:` in `mandelbulber2/src/test.hpp`, and define it in `mandelbulber2/src/test.cpp`, including `position_random.hpp`. It returns at once in benchmark mode, as `netrender` does. For each of the 9 rows of Known answers, it builds the generator, sets the position, and uses `QCOMPARE` on four draws with `max` of 1000, 1000, 65536, and 2147483646. It also sets one position twice, and confirms the sequence restarts. This is the new test of FR-018 (SC-012)

### Random values (plan step 3)

- [X] T033 [P] [US3] In `mandelbulber2/src/initparameters.cpp`, add `par->addParam("render_random_seed", 1, 0, 2147483647, morphNone, paramStandard);` after `clouds_random_seed`. The default is 1, the range is 0 to 2147483647, and there is no GUI control (FR-016)
- [X] T034 [P] [US3] In `mandelbulber2/src/fractparams.hpp`, add `int renderRandomSeed;` beside `frameNo`. In `mandelbulber2/src/fractparams.cpp`, read it with `container->Get<int>("render_random_seed")`
- [X] T035 [P] [US3] In `mandelbulber2/src/stereo.h:89`, make `GetForcedEye()` `const`, because `cSSAOWorker` holds a `const sRenderData *`
- [X] T036 [US3] In `mandelbulber2/src/render_worker.hpp`, include `position_random.hpp` and add `mutable cPositionRandom random;` beside `objectStack`, with a comment that it is per-thread state used from `const` shaders. In `cRenderWorker::doWork` in `mandelbulber2/src/render_worker.cpp`, before the main loop, assign it `cPositionRandom(params->renderRandomSeed, params->frameNo, int(data->stereo.GetForcedEye()) + 1, cPositionRandom::stageRender)`. The eye expression gives 0 with no forced eye, 1 for left, and 2 for right. Do this in `doWork` and not the constructor, because the OpenCL path also builds a `cRenderWorker` and never renders with it. As the first statement of the `repeat` loop, call `random.SetPosition(xs, ys, repeat)`. Change all 12 `Random(` calls in the file to `random.Random(`, keeping each `max` (FR-012, FR-013, FR-017)
- [X] T037 [US3] Change every `Random(` call to `random.Random(`, keeping each `max`, in `mandelbulber2/src/shader_ambient_occlusion.cpp` (1 call), `mandelbulber2/src/shader_aux_shadow.cpp` (4), `mandelbulber2/src/shader_global_illumination.cpp` (3), `mandelbulber2/src/shader_specular_highlight.cpp` (1), and `mandelbulber2/src/shader_volumetric.cpp` (1). Each is a `cRenderWorker` method, so each draw takes the next value of the sample's sequence (FR-013, FR-017)
- [X] T038 [US3] In `mandelbulber2/src/light.h`, forward declare `class cPositionRandom;`, and add a last parameter `cPositionRandom *random` to `CalculateLightVector` and `CalculateBeam`. In `mandelbulber2/src/light.cpp`, include `position_random.hpp`, pass it through, and use `random->Random(10000)`. Pass `&random` from the 4 callers: `mandelbulber2/src/render_worker.cpp:1740`, `mandelbulber2/src/shader_light_shading.cpp:47`, and `mandelbulber2/src/shader_volumetric.cpp:409` and `:592` (FR-013)
- [X] T039 [US3] In `cSSAOWorker::doWork` in `mandelbulber2/src/ssao_worker.cpp`, include `position_random.hpp` and build a local `cPositionRandom random(params->renderRandomSeed, params->frameNo, int(data->stereo.GetForcedEye()) + 1, cPositionRandom::stageSSAO);` before the `y` loop. As the first statement of the `x` loop, call `random.SetPosition(x, y, 0)`. Change the 2 `Random(` calls to `random.Random(` (FR-012, FR-013)
- [X] T040 [US3] In `mandelbulber2/src/dof.hpp`, include `position_random.hpp`, and add a last parameter `cPositionRandom random`, passed by value, to `cPostRenderingDOF::Render`. In `mandelbulber2/src/dof.cpp`, call `random.SetPosition(pass, int(i), 0)` as the first statement of the shuffle's `for i` loop, and change its 2 `Random(` calls to `random.Random(`, keeping the `int(randomStep)` cast (FR-012, FR-013)
- [X] T041 [US3] Pass a generator with stage `stageDOF` to all 5 `dof.Render` calls. In `cRenderer::RenderDOF` in `mandelbulber2/src/render_image.cpp` (lines 328, 333, and 339), build one at the top with the seed, frame, and eye expression from T036. Both regions of a left-right stereo render start from it, and that is fine, because each region is its own shuffle and the values only set blend order. In `cRenderJob::RefreshPostEffects` in `mandelbulber2/src/render_job.cpp:1325`, use `params.renderRandomSeed`, `params.frameNo`, and eye 0. In `cOpenClEngineRenderDOF::RenderDOF` in `mandelbulber2/src/opencl_engine_render_dof.cpp:120`, use `paramRender->renderRandomSeed`, `paramRender->frameNo`, and eye 0
- [X] T042 [US3] In the parallel shuffle loop in `mandelbulber2/src/opencl_engine_render_dof.cpp:177`, give each loop entry its own generator with stage `stageDOF`, built from `paramRender->renderRandomSeed`, `paramRender->frameNo`, and eye 0, call `SetPosition(pass, int(i), 0)` on it, and change the 2 `Random(` calls to use it (FR-015). The loop stays parallel, because OpenCL is not gated
- [X] T043 [P] [US3] In `cInterface::ResetView` in `mandelbulber2/src/interface.cpp:1089`, build `cPositionRandom random(1, 0, 0, cPositionRandom::stageOutsideRender);` and call `random.SetPosition(0, 0, 0)` before the `for i` loop, then change the 3 `Random(` calls to `random.Random(` (FR-015)
- [X] T044 [P] [US3] In `cGradientEditWidget::pressedButtonRandomColors` in `mandelbulber2/qt/gradient_edit_widget.cpp:617`, add the time-seeded local `cRandom` that `pressedButtonRandomColorsAndPositions` already uses, and use it for the 3 calls. In `pressedButtonRandomColorsAndPositions`, change the remaining `Random(10000)` at line 634 to `random.Random(10000)` (FR-015)
- [X] T045 [P] [US3] In `mandelbulber2/qt/thumbnail_widget.h`, include `src/position_random.hpp` and add a private `cPositionRandom timerRandom;`. In `cThumbnailWidget::Init` in `mandelbulber2/qt/thumbnail_widget.cpp`, after `instanceIndex` is set, assign it `cPositionRandom(1, 0, 0, cPositionRandom::stageOutsideRender)` and call `timerRandom.SetPosition(instanceIndex, 0, 0)`. Change the 5 `Random(` calls to `timerRandom.Random(` (FR-015)
- [X] T046 [US3] Remove `gRandomSeed`, `RandomInt()`, and `Random()`, with the comment above them, from `mandelbulber2/src/common_math.h` and `mandelbulber2/src/common_math.cpp`. Any call left behind stops the build, which is how FR-013 and FR-015 are checked. `cRandom` stays, because random lights and the GUI use it with their own seeds (FR-020)
- [X] T047 [US3] Commit step 3 with the message `feat: derive random values from the seed and position`, staging the files changed in T030 and T032 to T046 under `mandelbulber2/src/` and `mandelbulber2/qt/`
- [X] T048 [US3] Build T047's commit with `build_fresh.sh ~/mandelbulber2 windows/upgrade/deterministic-rendering`. From the clone's root, run `env -i HOME="$(mktemp -d)" PATH="$PATH" LC_ALL=C.UTF-8 mandelbulber2/build/appdir/usr/bin/mandelbulber2 --test`. Every test passes, including `testPositionRandom`. The SC-015 search in section 5 of `specs/002-deterministic-rendering/quickstart.md` prints nothing
- [X] T049 [US3] Run `evidence.py` at 1 thread and at all threads, 3 renders of each, with the label `random-values`. Among the 13 scenes, only scenes with post-effect depth of field may still differ in a buffer, and statistics may still differ. Any other buffer difference is a new cause: find it and fix it before going on (FR-023). Add the Results rows to `specs/002-deterministic-rendering/evidence.md`

### Order-dependent stages (plan step 4)

- [X] T050 [US3] In `cPostRenderingDOF::Render` in `mandelbulber2/src/dof.cpp`, remove the `#pragma omp parallel for` line above the phase II `for j` loop. Change nothing else, so the blend runs in `temp_sort` order, which is the order a one-thread run uses today (FR-022, research R6). Phase I keeps its region
- [X] T051 [US3] Commit step 4 with the message `fix: blend depth of field in one fixed order`, staging `mandelbulber2/src/dof.cpp`
- [X] T052 [US3] Build T051's commit fresh in `~/mandelbulber2`, then run `evidence.py` on all 26 scenes at 1 thread and at all threads, 3 renders of each, with the label `one-thread-blend`. No buffer differs in any scene, in any run, or between 1 thread and all threads (FR-012, FR-014, FR-022). Statistics may still differ until User Story 4
- [X] T053 [US3] In `specs/002-deterministic-rendering/evidence.md`, add the Results rows, then write the Order audit: one entry each for `dof.cpp:101`, the phase II loop, and `post_effect_hdr_blur.cpp:78`, with what each reads and writes, its verdict, the scene that reaches it, the proof from T052 for `IFS_002.fract` and `tests/scenes/hdr_blur.fract`, and its result (FR-021, SC-008). Add a note that `audio_track.cpp:302` is outside the gate, because no suite scene has audio tracks. Commit it with the message `docs: record the order audit`

**Checkpoint**: Every buffer of every scene reproduces at one thread and at all threads. Statistics do not yet, so `--self-parity` still fails.

---

## Phase 6: User Story 4 - Report Exact Statistics (Priority: P1)

**Goal**: A finished render reports the same statistics at every thread count, and no buffer changes (plan step 5). [data-model.md](data-model.md) has the rules.

**Independent Test**: Render one scene at one thread and at all threads, and compare its statistics record line by line.

- [X] T054 [P] [US4] In `mandelbulber2/src/statistics.h` and `mandelbulber2/src/statistics.cpp`, add `void Add(const cStatistics &other);`. It sums `totalNumberOfIterations`, `numberOfRaymarchings`, `missedDE`, `numberOfRenderedPixels`, `totalNumberOfDOFRepeats`, and `totalNoise`, and it merges both histograms bin by bin with `AddWeighted` from 0 through `GetSize()`, because the last bin is the overflow bin. It leaves `time`, `maskedPixels`, and `tilesDone` alone. Make `Reset` clear `totalNoise` too
- [X] T055 [P] [US4] In `mandelbulber2/src/render_data.hpp`, include `<QMutex>` and add `QMutex statisticsMutex;` below `statistics` in `sRenderData`
- [X] T056 [P] [US4] In `cScheduler::InitFirstLine` in `mandelbulber2/src/scheduler.cpp`, take `mutex`, set `linePendingThreadId[firstLine] = threadId` only if it is 0, and unlock. A thread whose first line is taken already gives up at its first pixel and asks `NextLine` for a free line, so nothing else changes, and each line has one owner (FR-025)
- [X] T057 [US4] In `mandelbulber2/src/render_worker.hpp`, include `statistics.h` and add `mutable cStatistics statistics;` beside `random`. In `cRenderWorker::doWork` in `mandelbulber2/src/render_worker.cpp`, before the main loop, call `Resize` on its two histograms with the sizes of the ones in `data->statistics`. Change the 10 uses of `data->statistics` at lines 478, 479, 523, 803, 804, 810, 918, 919, 954, and 961 to `statistics`. After the `xs` loop, still inside the `ys` loop: if `lastLineWasBroken` is false, take `data->statisticsMutex` and call `data->statistics.Add(statistics)`. Then call `statistics.Reset()` either way (FR-025, FR-027, FR-029)
- [X] T058 [US4] Change `data->statistics` to `statistics` in `mandelbulber2/src/shader_aux_shadow.cpp:111`, `mandelbulber2/src/shader_calculate_normals.cpp` (7 uses), `mandelbulber2/src/shader_ambient_occlusion.cpp:75`, and `mandelbulber2/src/shader_fast_ambient_occlusion.cpp:59`. A search for `data->statistics` in `render_worker.cpp` and `shader_*.cpp` then finds only the lines from T057 (FR-027)
- [X] T059 [US4] In `mandelbulber2/src/render_image.cpp`, add one file-local helper that copies `data->statistics` under `data->statisticsMutex`, and use its result for the `updateStatistics` signals at lines 167, 440, and 546. `data->statistics.time` stays as it is, because only `cRenderer` writes it and `Add` does not touch it. The `updateStatistics` in `render_job.cpp:582` needs no change, because no worker exists yet at that point (FR-027, FR-029)
- [X] T060 [P] [US4] In `sDistanceOut` in `mandelbulber2/src/calculate_distance.hpp`, change the two fields to `int iters = 0;` and `int totalIters = 0;`. In `CalculateDistanceFromObjectsTree` in `mandelbulber2/src/calculate_distance.cpp`, add `out->totalIters += nodeOut.totalIters;` after `leafIters = nodeOut.iters;` in the fractal and hybrid node cases (lines 399 and 475). Nothing else reads `totalIters`, so that change can't reach a buffer. The `iters` default can reach one only on the limit box exit, the one case outside the suite in FR-028 (FR-024, FR-026, FR-028)
- [X] T061 [US4] Commit step 5 with the message `fix: make render statistics exact`, staging the files changed in T054 to T060 under `mandelbulber2/src/`
- [X] T062 [US4] Build T061's commit fresh in `~/mandelbulber2`, and T053's commit fresh in `~/mandelbulber2-before`. From `~/mandelbulber2`, run `python3 tests/regression_runner.py --compare ~/mandelbulber2-before/mandelbulber2/build/appdir/usr/bin/mandelbulber2`. The report shows 0 differences in any buffer across all items (SC-004, FR-028). It still exits 1, because total iterations changes from 0 to the real count. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T063 [US4] Run `evidence.py` on all 26 scenes at 1 thread and at all threads, 3 renders of each, with the label `exact-statistics`. No buffer or statistic differs, and `total_iterations` is above 0 in every scene (FR-024, FR-026). Add the Results rows to `specs/002-deterministic-rendering/evidence.md`
- [ ] T064 [US4] Render `IFS_001.fract` at 1920x1080 with `--stats`, and confirm the printed statistics change while it runs (FR-029). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T065 [US4] Build T061's commit with `build_fresh.sh ~/mandelbulber2-diag <T061's commit> - CONFIG+=force_debug_info CONFIG+=nostrip 'QMAKE_CXXFLAGS+=-fsanitize=thread' 'QMAKE_LFLAGS+=-fsanitize=thread'`, and run the check for colliding updates in section 4 of `specs/002-deterministic-rendering/quickstart.md`, under `setarch "$(uname -m)" -R`. No ThreadSanitizer report names `cStatistics`, `cHistogram`, or `cPositionRandom` (FR-027). Record the result in `specs/002-deterministic-rendering/evidence.md`

**Checkpoint**: Buffers and statistics both reproduce on the scenes, and the statistics work changed no buffer.

---

## Phase 7: User Story 5 - Pass Self-Parity and Close KD-1 (Priority: P1)

**Goal**: `--self-parity` passes on a fresh build, which is the measured condition for closing KD-1.

**Independent Test**: Build the branch fresh on Linux and run `--self-parity`.

- [ ] T066 [US5] Find the final engine commit with `git log -1 --format=%H -- mandelbulber2 tests`, and build it fresh in `~/mandelbulber2`. Every check in Phases 7 and 8 uses this commit. If a check calls for an engine change, commit the fix, record it under Other dependence in `specs/002-deterministic-rendering/evidence.md` (FR-023), repeat T063 to T065 on it, and start again from this task
- [ ] T067 [US5] In `~/mandelbulber2`, run `python3 tests/regression_runner.py --self-parity` three times in a row. Each run exits 0, reports 487 items passed, and says nothing about thread coverage (SC-001, FR-032). Run it in the background, and keep the three reports. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T068 [US5] Write `~/verify/kd1/thread_sweep.py`, which renders every item at 1, 2, 3, 4, and 7 threads and at twice the CPU count, compares each export with the one-thread export using `compare_exports`, and reads `total_iterations` from each. It reports 0 differences (SC-002, FR-024, FR-025), and `total_iterations` is above 0 on every item and the same at every count (SC-003, FR-026). The formula sweep has 24 lines, so twice the CPU count also covers more threads than image lines. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T069 [P] [US5] Write `~/verify/kd1/seeds.py`, which renders each of the 6 Monte Carlo scenes twice with each of these added to the harness's overrides: nothing, `render_random_seed=1`, `render_random_seed=2`, and `frame_no=1`. Each render matches its own second render. `render_random_seed=1` matches the render with nothing added, so a scene that names no seed uses the default. `render_random_seed=2` and `frame_no=1` each differ from it (SC-009, FR-016). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T070 [P] [US5] Run the two-pass stereo check in section 3 of `specs/002-deterministic-rendering/quickstart.md`: all six renders match in every buffer and statistic, and their raymarch count is above the plain render's (FR-012). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T071 [US5] Run `python3 tests/test_regression_runner.py` in WSL and on Windows. All tests pass, including the seeded one-bit difference in every buffer (SC-011) and the capture tests, which run with `--threads` as the option and write only to temporary folders. With `test_thread_control_is_enabled`, they show thread coverage no longer stops `--capture`. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T072 [US5] Confirm SC-010 of `specs/001-regression-harness`, which User Story 5 says this feature makes true. Build T066's commit fresh in `~/mandelbulber2-before`, then from `~/mandelbulber2` run `python3 tests/regression_runner.py --compare ~/mandelbulber2-before/mandelbulber2/build/appdir/usr/bin/mandelbulber2`. It exits 0 on all 487 items. Write `~/verify/kd1/one_formula.patch`, which changes `.9` to `.91` on the `z.x` line of `FormulaCode` in `mandelbulber2/formula/definition/fractal_ides.cpp`, a formula no suite scene uses. Build it with `build_fresh.sh ~/mandelbulber2-diag <T066's commit> ~/verify/kd1/one_formula.patch`, and run the same command with that binary. It exits 1, and the report names only `ides`. Record both results in `specs/002-deterministic-rendering/evidence.md`

**Checkpoint**: Self-Parity passes on all 487 items. KD-1's closing condition is met, and the closing edit waits for Phase 9.

---

## Phase 8: User Story 6 - Keep Everything Else Working (Priority: P2)

**Goal**: No loss for anyone else: images look as they did, and every build, test, and mode that works today still works.

**Independent Test**: Compare Monte Carlo renders from before and after this change, and run the application's own tests and Cross-Build Check.

- [ ] T073 [US6] On T066's build, run `--test` with the `env -i` command from T048. Every test passes (SC-012, FR-018, FR-037). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T074 [US6] Build T066's commit fresh with MSVC, from the playbook's Local Development section, in the new clone that Build Environments describes: clone the repository, check out the commit detached, run `nuget restore`, then build Release x64 with MSBuild. Run `--test` from `build-win/Release/x64` with `QT_FORCE_STDERR_LOGGING=1`, then `python tests/test_regression_runner.py`. All tests pass, and `testPositionRandom` gives the same answers as qmake (SC-012, FR-018, FR-037). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T075 [US6] On T074's build, run `python tests/regression_runner.py --self-parity`. It reports 0 differences, although it can't call that a pass on Windows (SC-013). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T076 [US6] Run Cross-Build Check locally, from section 4 of the playbook's Local Development section, with T074's clone in place of the Windows checkout: `--collect` on the T066 and T074 builds, then `--cross-check` on both collections. It passes. Any difference it records between the builds is recorded and is not drift. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T077 [US6] Confirm the other builds compile from fresh, on `windows/ministic-regression` and on T066's commit: `mandelbulber2/qmake/mandelbulber.pro`, which has no OpenCL, and `mandelbulber2/cmake/CMakeLists.txt`. Build them in `~/mandelbulber2-diag`, never in `~/mandelbulber2`, because later checks use the T066 binary there. Follow `build_fresh.sh` by hand, because it only builds `mandelbulber-opencl.pro`. Ask the user to run `sudo apt-get install -y cmake` in WSL first. If a build fails on `ministic-regression` too, record that, and this feature only has to fail the same way (FR-037). Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T078 [P] [US6] Build `windows/ministic-regression` fresh in `~/mandelbulber2-base`. Write `~/verify/kd1/noise.py`, which finds the average absolute difference between the `final_image` buffers of two renders of each of the 6 Monte Carlo scenes: two runs of the `ministic-regression` build with no thread arguments, because that branch has no `--threads` yet, then seeds 1 and 2 on T066's build at all threads. Noise rises by at most 5% on every scene. Note any drop above 5% with its reason, for the pull request (SC-010, FR-019). Record the numbers under Measurements in `specs/002-deterministic-rendering/evidence.md`
- [ ] T079 [US6] Using the renders from T078, write a before image, an after image, and a noise image for each of the 6 Monte Carlo scenes to `~/verify/kd1/patterns/` as PNG files. The noise image shows the difference between seeds 1 and 2, amplified so any structure can be seen. Look at each one for lines, grids, or repeated shapes, then show them to the user, because SC-010 needs a reviewer. Record the outcome in `specs/002-deterministic-rendering/evidence.md` (FR-019)
- [ ] T080 [US6] With nothing else running, use `cpu_rate.py` to record the time of `--collect` on the `ministic-regression` and T066 builds, each run from its own clone, so each build runs with the harness from its own commit. Record the depth of field stage too, on `IFS_002.fract` at 1920x1080 at 1, 4, 16, and 32 threads. Take the stage time as the render's time minus the same render with `DOF_enabled=0`, and set threads with `OMP_NUM_THREADS` on `ministic-regression` and `--threads` on T066's build (FR-039, SC-014). Record both under Measurements in `specs/002-deterministic-rendering/evidence.md`
- [X] T081 [P] [US6] Run the animation check in section 3 of `specs/002-deterministic-rendering/quickstart.md`: frames 0 and 1 of `keyframe_anim_mandelbulb.fract` differ from each other, and each frame matches its own second render. Record the result in `specs/002-deterministic-rendering/evidence.md`
- [ ] T082 [US6] Check OpenCL on T074's build. If `--help-opencl` lists a device, render `IFS_002.fract` with `--gpu`, which selects the first device, and `--override opencl_mode=3`, the full engine. Confirm it finishes, and that the image differs from the same render with `DOF_enabled=0`. If it lists none, the compile checks are all this feature can show, so record that as Not covered in `specs/002-deterministic-rendering/evidence.md`
- [ ] T083 [US6] Check the GUI on T074's build. This needs a person at the screen, or computer use if the user allows it. Confirm the statistics dock updates during a render (FR-029), `limit_CPU_cores` in Preferences still limits the render workers (FR-004), and the view reset, both random buttons in the gradient editor, and thumbnails all still work (FR-015). Record the result in `specs/002-deterministic-rendering/evidence.md`, or Not covered if nobody could run it
- [ ] T084 [US6] Repeat T016 to T020 on T066's build, so the thread control and network rendering are checked on the final code (SC-005, SC-006, FR-001 to FR-005). Record the results in `specs/002-deterministic-rendering/evidence.md`
- [X] T092 [US6] T081 found that the last frame of a `--nogui --keyframe` render can go missing or be cut off, so commit a fix, as T066 says. In `mandelbulber2/src/animation_keyframes.cpp`, `RenderKeyframes` keeps a list of the threads that save its frames, and waits for them after the last frame and when a render stops. Keep processing events during the wait, because a saving thread only stops when the rendering thread handles its `quit()` call. Record it under Other dependence in `specs/002-deterministic-rendering/evidence.md`, then repeat T063 to T065 and start again from T066 (FR-023, FR-040)
- [X] T093 [US6] Build T092's commit and the commit before it fresh, and run `~/verify/kd1/last_frame_test.py` on each: 20 two-frame renders on a quiet computer, then 10 with every CPU busy. The build before the fix loses or cuts the last frame, and T092's build exits 0 with both frames whole in every run. `~/verify/kd1/frames_match.py` shows frames 0 and 1 are identical on both builds, so the fix changes no image (SC-017). Record the result in `specs/002-deterministic-rendering/evidence.md`

**Checkpoint**: Quality, builds, tests, and other modes all hold on the final engine commit.

---

## Phase 9: Polish & Cross-Cutting Concerns: Closing KD-1

**Purpose**: Finish the record, close KD-1, and open the one pull request (plan step 6). This comes last because closing depends on Phases 7 and 8.

- [ ] T085 Finish `specs/002-deterministic-rendering/evidence.md`: complete Measurements, Other dependence, and the Success criteria table, with anything not covered named plainly. Read it alone and confirm every difference has a cause (SC-007), and every audited region has a verdict and a scene (SC-008)
- [ ] T086 In `.specify/memory/constitution.md`, mark KD-1 closed in the Known Deviations table, with the date and a pointer to `specs/002-deterministic-rendering/evidence.md`. In Principle II, add this sentence after the seed rule's first sentence: "A seed setting that a scene file does not name takes its default value, which counts as fixed in the scene file." Set Version to 1.0.1 and Last Amended to the closing date. Leave Ratified alone. Both changes are one PATCH amendment (FR-034, SC-016)
- [ ] T087 In `MIGRATION_PLAYBOOK.md`, correct what this feature makes out of date (FR-034): line 222, which says to enable the second thread count; line 224, which says to fill in `total_iterations`; and line 370, which removes KD-1 through KD-4, so it closes only what is still open by then. Search the file for `KD-1`, `thread control`, and `total_iterations`, and correct anything else that is now wrong. Because `master` now follows upstream and has no harness, name `ministic-regression` wherever the playbook means the branch regression work builds on: the push triggers of both Parity Gate workflows (lines 270 and 303), the branch protection step (line 362), and the local comparison steps (lines 399 to 420, 453, and 460). Say once, in the paragraph at line 399, that `ministic-regression` is the base for regression features and `master` follows upstream
- [ ] T088 Commit step 6 with the message `docs: close KD-1`, staging `specs/002-deterministic-rendering/evidence.md`, `.specify/memory/constitution.md`, and `MIGRATION_PLAYBOOK.md`
- [ ] T089 Check the whole branch against `ministic-regression`. `git diff --stat ministic-regression...HEAD -- .github mandelbulber2/qmake mandelbulber2/cmake mandelbulber2/msvc mandelbulber2/formula` prints nothing (FR-036, FR-038). `mandelbulber2/src/verification_export.cpp`, `mandelbulber2/src/lights.cpp`, `mandelbulber2/src/material.cpp`, `mandelbulber2/src/random.cpp`, and the Perlin noise files are unchanged (FR-020, FR-030). In `tests/regression_runner.py`, only `THREAD_OPTION` and the comment above it changed (FR-031). In `mandelbulber2/src/initparameters.cpp`, only the `render_random_seed` line was added (FR-036). In `mandelbulber2/src/common_math.cpp`, only the shared generator was removed (FR-011). The SC-015 search prints nothing. `tests/references` does not exist (FR-033, SC-016). No commit message on the branch has an attribution line
- [ ] T090 Commit this file with T089 marked. Ask the user for the go-ahead to push, then push `upgrade/deterministic-rendering` and open one pull request against `ministic-regression` with `gh pr create --repo Coastrnaut/mandelbulber2 --base ministic-regression`. The body states the expected effect on output in the words of FR-035, adds that the last frame of a headless keyframe render is now whole (FR-040), explains any noise drop above 5% (SC-010), links `specs/002-deterministic-rendering/evidence.md`, and gives what the amendment needs: the reason for both changes, the version change from 1.0.0 to 1.0.1, the dates, and the affected plans and gates. It confirms the constitution's gates hold, lists anything not covered, and has no attribution lines
- [ ] T091 Confirm `.github/workflows/cross-build-check.yml` and the workflows that pass today pass on the pull request, and that its Collect step stays under 30 minutes (SC-012, SC-014, FR-038). CodeQL only runs for `master`, so it does not run on this pull request, and T090's body lists it as not covered. A maintainer approves the amendment before it merges. Do not enable auto-merge

---

## Requirement Coverage

| Requirement | Tasks |
|---|---|
| FR-001 | T005, T006, T016, T020, T084 |
| FR-002 | T009, T016, T017, T084 |
| FR-003 | T006, T010, T017, T084 |
| FR-004 | T003, T008, T019, T083, T084 |
| FR-005 | T006, T015, T018, T084 |
| FR-006 | T005, T015 |
| FR-007 | T021, T022, T023, T024 |
| FR-008 | T021, T022, T025 |
| FR-009 | T025, T026 |
| FR-010 | T027 |
| FR-011 | T023, T024, T028, T089 |
| FR-012 | T030, T036, T039, T040, T052, T070 |
| FR-013 | T036, T037, T038, T039, T040, T046 |
| FR-014 | T030, T052 |
| FR-015 | T042, T043, T044, T045, T046, T083 |
| FR-016 | T033, T034, T069 |
| FR-017 | T030, T036, T037 |
| FR-018 | T030, T031, T032, T073, T074 |
| FR-019 | T078, T079 |
| FR-020 | T046, T089 |
| FR-021 | T053 |
| FR-022 | T050, T052 |
| FR-023 | T027, T049, T066 |
| FR-024 | T054, T060, T063, T068 |
| FR-025 | T056, T057, T068 |
| FR-026 | T060, T063, T068 |
| FR-027 | T055, T057, T058, T059, T065 |
| FR-028 | T060, T062 |
| FR-029 | T057, T059, T064, T083 |
| FR-030 | T089 |
| FR-031 | T011, T012, T013, T089 |
| FR-032 | T067 |
| FR-033 | T089 |
| FR-034 | T086, T087 |
| FR-035 | T090 |
| FR-036 | T089 |
| FR-037 | T073, T074, T077 |
| FR-038 | T089, T091 |
| FR-039 | T080 |
| FR-040 | T092, T093 |
| SC-001 | T067 |
| SC-002 | T068 |
| SC-003 | T068 |
| SC-004 | T062 |
| SC-005 | T016, T020, T084 |
| SC-006 | T018, T084 |
| SC-007 | T026, T085 |
| SC-008 | T053, T085 |
| SC-009 | T069 |
| SC-010 | T078, T079, T090 |
| SC-011 | T071 |
| SC-012 | T032, T073, T074, T091 |
| SC-013 | T075 |
| SC-014 | T080, T091 |
| SC-015 | T048, T089 |
| SC-016 | T086, T089 |
| SC-017 | T093 |

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies.
- **User Story 1 (Phase 3)**: Depends on Setup.
- **User Story 2 (Phase 4)**: Depends on User Story 1, because the evidence needs one-thread renders.
- **User Story 3 (Phase 5)**: Depends on User Story 2, because the cause must be recorded before the seed is replaced (FR-007).
- **User Story 4 (Phase 6)**: Depends on User Story 3, because FR-028 can only be proved once renders reproduce.
- **User Story 5 (Phase 7)**: Depends on User Stories 1, 3, and 4, because Self-Parity needs all of them.
- **User Story 6 (Phase 8)**: Depends on the final engine commit from T066. It can run alongside User Story 5, apart from T080 and T084, which need a quiet computer.
- **Closing (Phase 9)**: Depends on everything above. T086 must not run until T067 has passed.

### Within Each Phase

- T003 comes before T004 to T010, because they use its function.
- T004 comes before T005 to T007.
- T030 comes before T031, T032, and every generator caller. T035 comes before T036, T039, and T041.
- T036 comes before T037 and T038, because they use the worker's member.
- T040 comes before T041, because the callers need the new parameter.
- T046 comes after T036 to T045, because any call left stops the build.
- T054 and T055 come before T057, and T057 comes before T058 and T059.
- Each commit task comes after every task in its step, and each step's checks come after its commit.

### Parallel Opportunities

There is little to run in parallel, because the steps are ordered and most of them change one call path.

- T008 and T011, once T003 is done
- T033, T034, and T035
- T043, T044, and T045
- T054, T055, T056, and T060
- T069, T070, T078, and T081, which are checks on one build, as long as nothing is being timed

## Parallel Example: Callers Outside the Render

```text
Task: "Give cInterface::ResetView a local generator in mandelbulber2/src/interface.cpp"
Task: "Give the gradient editor a local cRandom in mandelbulber2/qt/gradient_edit_widget.cpp"
Task: "Give each thumbnail widget a generator in mandelbulber2/qt/thumbnail_widget.cpp"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1.
2. Complete Phase 3.
3. **Stop and validate**: `--threads` works in every mode, and every refusal works.

The thread control is useful alone, but KD-1 stays open, so nothing merges at this point. All six steps land in one pull request.

### Incremental Delivery

1. Step 1 gives a working thread control.
2. Step 2 gives the record of causes, which can redirect the rest of the work.
3. Steps 3 and 4 make the buffers reproduce.
4. Step 5 makes the statistics reproduce, and proves it changed no buffer.
5. Phases 7 and 8 check the final commit.
6. Step 6 closes KD-1 and opens the pull request.

Stop at each checkpoint. If a check fails, fix it in a new commit on that step before moving on.

---

## Notes

- This feature closes KD-1 only. KD-2 and KD-3 belong to `upgrade/strict-floating-point`, and KD-4 belongs to Baseline 1. Do not report any of them as passing.
- This feature creates no references, even though it makes `--capture` possible.
- `evidence.md` is where every result goes, so a reviewer can read them in one place.
