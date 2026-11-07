---

description: "Task list for strict floating-point builds"
---

# Tasks: Strict Floating-Point Builds

**Input**: Design documents from `specs/003-strict-floating-point/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: The spec asks for seeded failures (FR-015), and those are the check's tests. They run once and go in the evidence, so no test file joins the repository. Every other check runs a fresh build through [quickstart.md](quickstart.md).

**Organization**: Tasks are grouped by user story, in the plan's order of work. Local checks come before the first push, because a fix would move the final commit and repeat every CI run. So User Story 3's GitHub Actions proof comes in Phase 10, after the local checks, and closing comes last.

## Rules for Every Task

- Every result comes from a fresh build of the commit being tested. No task may use an existing binary, including the ones already in the WSL clones and in `C:\GIT\mb2-kd1-*`.
- WSL can only see commits, so each step commits first and then builds. A fix that a check calls for is a new commit on top of that step, before the next step starts.
- T010, T017, T022, and T059 are the commits that make the change, and each must compile. Stage only the files the step changed.
- Each commit also stages this file, with its finished tasks marked `[X]`. A task that only records a result in `evidence.md` commits it with a short `docs:` message, because the project commits after each task that changes the repository. T027 finds the final commit with a path filter, so these commits don't move it. T061 and T062 come after the last commit, so they stay unmarked in the branch, because another commit would restart the pull request's checks.
- Commit messages and the pull request carry no `Co-Authored-By` line, no other co-author statement, and no "Generated with" line.
- Pushing needs the user's go-ahead each time: T050, T052, and T061.
- Change nothing outside the plan: no engine source unless FR-018 requires it, no harness change, no existing workflow or CI file, no option kept from `-ffast-math`, no OpenCL build option, no branch protection, and no references (FR-016, FR-030, FR-031, FR-032, FR-034).
- Diagnostic changes and seeds stay in throwaway clones or on a throwaway branch, and never reach this branch (FR-015, FR-020).
- Scratch scripts stay out of the repository, in `~/verify/kd2/` in WSL. They import `tests/regression_runner.py` for its isolation and comparison, and they do not rewrite it.
- No tolerance, exclusion, or skip list may hide a difference (FR-018). The 20 formulas in FR-021 limit a review, and they are not a parity comparison.
- Do not create anything under `tests/references/` (FR-034).
- Never stop or kill FractalEvaluator. It is the user's work, and it renders in bursts with no warning. Only timing waits for it. If it won't go quiet, tell the user.
- `--override` reads a boolean as a number, so use `=1` and `=0`. `=true` reads as 0, which silently turns the setting off.
- Line numbers below come from commit `8fd49ba59`, so each task also names the code.

**Final commit** means the last commit that changes what compiles, which T027 finds. CI files and documents can't change a binary, so they don't move it.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (for example US1)

## Path Conventions

- Build definitions: `mandelbulber2/qmake/`, `mandelbulber2/cmake/`, `mandelbulber2/msvc/`
- Scripts: `mandelbulber2/tools/`, `mandelbulber2/deploy/linux/`
- CI: `.github/`
- This feature's documents: `specs/003-strict-floating-point/`

## Build Environments

- **Checkout**: This branch is checked out in the worktree `C:\GIT\mandelbulber2\.claude\worktrees\elastic-goldwasser-6934fe`, or `/mnt/c/GIT/mandelbulber2/.claude/worktrees/elastic-goldwasser-6934fe` in WSL. `C:\GIT\mandelbulber2` holds another branch, and both share one repository, so a commit in the worktree is fetchable from `file:///mnt/c/GIT/mandelbulber2`. `.specify/feature.json` is per-checkout, and it points at this feature only in the worktree.
- **WSL**: the `Ubuntu-24.04` distro, with GCC 13.3.0, glibc 2.39, Qt 6.4.2, CMake 3.28.3, and 32 logical CPUs. Run scripts with `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/path/to/script.sh`, after stripping carriage returns, because `wsl.exe` expands `$` too early in an inline command. The default distro has no Qt. Clang is not installed. `sudo` needs a password, so only the user can install anything. The wall clock steps back by about 0.85 seconds every 32 seconds, so every time check uses the monotonic clock.
- **WSL clones**: `~/mandelbulber2` holds the after build, `~/mandelbulber2-before` the before build, `~/mandelbulber2-native` the host-tuned build, and `~/mandelbulber2-diag` the diagnostic builds, one at a time. Only `~/mandelbulber2-diag` may hold a patch. `build_fresh.sh` creates a clone that is missing.
- **Windows**: the MSVC builds from the playbook's Local Development section, each in a new clone of `C:\GIT\mandelbulber2`, checked out detached at the commit: `C:\GIT\mb2-kd2-<first 9 characters>` for Release, and `C:\GIT\mb2-kd2-debug-<first 9 characters>` for Debug. A new clone has no `build-win`, and the project compiles every `.cpp` file in that folder. Every Windows check runs inside its clone. MSBuild is `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe`. If the binary won't start because `OpenCL.dll` is missing, copy it from `mandelbulber2/msvc/packages/opencl-nuget-msvc-x64.*/build/native/bin/` beside the binary. The clones stay until the user clears them. A Windows run can't pass `--self-parity`, because the application ignores a redirected home folder there.
- **Base branch**: `ministic-regression` at `8fd49ba59` is the base. `master` follows upstream, has no harness, and no check compares with it.
- **GitHub**: `gh` is at `C:\Program Files\GitHub CLI\gh.exe`, signed in as `Coastrnaut`. The repository is `Coastrnaut/mandelbulber2`. Do not call `gh pr merge --auto` or turn on auto-merge. Pull requests here run the two GitHub workflows, Azure Pipelines, and CircleCI. AppVeyor doesn't run here. `github-actions-ci.yml` runs on every branch and replaces the `continuous` release.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Confirm the starting point, and prepare the scratch folder and the evidence record

- [ ] T001 Confirm the starting point from the worktree's root: the branch is `upgrade/strict-floating-point`, `git status --short` prints nothing, `git rev-parse ministic-regression` starts with `8fd49ba59`, `tests/references` does not exist, `.specify/feature.json` names `specs/003-strict-floating-point`, and `python tests/test_regression_runner.py` passes all 224 tests
- [ ] T002 In WSL, copy `build_fresh.sh` and `noise.py` from `~/verify/kd1/` to `~/verify/kd2/`. `~/verify/kd2/prototype/` already holds `check-flags.sh`, `seed_flags.sh`, and `seed_search.sh` from planning, and `~/verify/kd2/results/` holds this feature's logs. Use `build_fresh.sh <clone> <ref> [patch] [qmake arguments]` for every fresh WSL build below. It fetches every branch into `refs/remotes/windows/`
- [ ] T003 Create `specs/003-strict-floating-point/evidence.md` from `specs/003-strict-floating-point/contracts/evidence-record.md`, with every section and the Header filled in: the date, the computer, the environment, the toolchain, the thread counts 1 and 32, and the CPU's model and whether it has FMA and AVX2, read from `/proc/cpuinfo` in WSL. Commit it with the message `docs: start the KD-2 and KD-3 evidence`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Build the before state, and find out whether MSVC Debug compiles today (plan step 1)

**⚠️ CRITICAL**: Later tasks compare against these builds. They don't depend on any change in this feature, so they can run at any time, but User Story 7 can't read its before column until T006 finishes.

- [ ] T004 [P] Build the before state in WSL with `~/verify/kd2/build_fresh.sh ~/mandelbulber2-before 8fd49ba59`. Confirm `CXXFLAGS` in `mandelbulber2/build/Makefile` holds `-ffast-math`, and `readelf -d mandelbulber2/build/appdir/usr/bin/mandelbulber2 | grep -c libmvec` prints 1. Record the build in the Builds table of `specs/003-strict-floating-point/evidence.md`
- [ ] T005 [P] Build MSVC Release x64 fresh on `8fd49ba59` in the new clone `C:\GIT\mb2-kd2-8fd49ba59`: clone the repository, check out the commit detached, run `nuget restore mandelbulber2/msvc/mandelbulber2.sln`, then build with MSBuild, `/m /p:Configuration=Release /p:Platform=x64`. From that clone, run `python tests/regression_runner.py --collect tests/outputs/collection/msvc`. Keep this collection for T036 and T044, and record the build in `specs/003-strict-floating-point/evidence.md`
- [ ] T006 [P] Build MSVC Debug x64 fresh on `8fd49ba59` in the new clone `C:\GIT\mb2-kd2-debug-8fd49ba59`, the same way with `/p:Configuration=Debug`. Nothing has shown that this build compiles today (research R14). Record the result in Other builds in `specs/003-strict-floating-point/evidence.md`. If it fails, record the first error: this build doesn't compile today, FR-033 doesn't bind it, and T047 only reads the setting in the project file

**Checkpoint**: The before builds exist, and the Debug question has an answer. Nothing in the branch has changed yet.

---

## Phase 3: User Story 1 - Use Strict Rules in Every Build (Priority: P1) 🎯 MVP

**Goal**: No build lets the compiler change a floating-point result (plan step 2). [contracts/build-flags.md](contracts/build-flags.md) is the contract.

**Independent Test**: Generate the build files for each build system, and read the flags they pass to the compiler and the linker.

### Implementation for User Story 1

- [ ] T007 [US1] In `mandelbulber2/qmake/common.pri`, remove `-ffast-math` from lines 127 to 130, keeping every other option: the `unix:!macx` and `macx:!m1` lines become `QMAKE_CXXFLAGS += -fopenmp`, the commented-out `#macx:m1` line becomes `QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp`, and the `macx:m1` line becomes `QMAKE_CXXFLAGS += -Xpreprocessor`. Directly under the `# compiler build flags` comment, above those lines, add the `gcc` block from `specs/003-strict-floating-point/contracts/build-flags.md`, which adds `-ffp-contract=off` to `QMAKE_CFLAGS` and `QMAKE_CXXFLAGS`. It must stay below the environment import and above the `macx:m1` line, because that line's bare `-Xpreprocessor` takes the next option as its own (FR-001, FR-004)
- [ ] T008 [P] [US1] In `mandelbulber2/cmake/CMakeLists.txt`, inside `ELSEIF(NOT USING_INTEL)`, change line 63 to `set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -ffp-contract=off")`, and add `set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffp-contract=off")` below it. Leave the Windows and Intel paths alone (FR-002, FR-004)
- [ ] T009 [P] [US1] In `mandelbulber2/msvc/mandelbulber2.vcxproj:97`, in the `Debug|x64` block, change `<FloatingPointModel>Fast</FloatingPointModel>` to `<FloatingPointModel>Precise</FloatingPointModel>`. Keep the line endings. Change nothing else: Debug keeps its `AdvancedVectorExtensions2` setting, Release and AnalysisRelease keep the compiler's default, and `/fp:contract` stays unset (FR-003)
- [ ] T010 [US1] Commit with the message `build: remove -ffast-math and set -ffp-contract=off`, staging the files from T007 to T009 and this file, so no commit has a half-strict build

### Checks for User Story 1

- [ ] T011 [US1] Build T010's commit with `~/verify/kd2/build_fresh.sh ~/mandelbulber2 windows/upgrade/strict-floating-point`. Run the first two checks in section 1 of `specs/003-strict-floating-point/quickstart.md`: `CFLAGS` and `CXXFLAGS` in `mandelbulber2/build/Makefile` match the expected lines in `specs/003-strict-floating-point/contracts/build-flags.md`, no prohibited option is on the `CFLAGS`, `CXXFLAGS`, `LFLAGS`, or `LIBS` lines, and `readelf -d` finds no `libmvec`. Keep the lines for T026 (FR-001, FR-004)
- [ ] T012 [US1] In WSL, from `~/mandelbulber2`, run `cmake -S mandelbulber2/cmake -B /tmp/cmake-kd2 -DCMAKE_BUILD_TYPE=Release`. `CXX_FLAGS` in `/tmp/cmake-kd2/CMakeFiles/mandelbulber2.dir/flags.make` holds `-fopenmp -ffp-contract=off`, and `link.txt` beside it holds neither `-ffast-math` nor `-Ofast`. Read `CMAKE_C_FLAGS` from the file, because the engine has no C sources. Remove `/tmp/cmake-kd2`, and keep the result for T026 (FR-002, FR-004)
- [ ] T013 [P] [US1] Run the pragma search in section 1 of `specs/003-strict-floating-point/quickstart.md`. It prints only the six `diagnostic` lines in `mandelbulber2/src/radiance_hdr.cpp`. Keep the result for T026 (FR-005, SC-004)

**Checkpoint**: Every build definition follows Principle III. Nothing yet stops a flag coming back, and KD-2 stays open.

---

## Phase 4: User Story 2 - Build the Same Instructions on Any Computer (Priority: P1)

**Goal**: No setup or install script tunes a build to its computer (plan step 2).

**Independent Test**: Read the flags each script exports, then generate build files with those flags.

### Implementation for User Story 2

- [ ] T014 [P] [US2] In `mandelbulber2/tools/prepare_for_dev_ubuntu.sh:12`, change `export CXXFLAGS="-march=native -msse2"` to `export CXXFLAGS="-msse2"`. `-msse2` stays, because it "names part of the plain x86-64 target and does not depend on the build computer" (FR-006)
- [ ] T015 [P] [US2] Make the same change in `mandelbulber2/tools/prepare_for_dev_debian_testing.sh:12` (FR-006)
- [ ] T016 [P] [US2] In `mandelbulber2/deploy/linux/install:13`, delete the line `export CXXFLAGS="-march=native"`. Leave the rest of the script alone (FR-007)
- [ ] T017 [US2] Commit with the message `build: remove -march=native from the setup scripts`, staging the files from T014 to T016 and this file

### Checks for User Story 2

- [ ] T018 [US2] Run the check in section 2 of `specs/003-strict-floating-point/quickstart.md`: the search prints nothing, and `sh -n` passes on both `prepare_for_dev_*.sh` scripts and `bash -n` on `install`. In a new empty folder in WSL, run `CXXFLAGS=-msse2 qmake6 ~/mandelbulber2/mandelbulber2/qmake/mandelbulber-opencl.pro CONFIG+=release`, and confirm `CXXFLAGS` in the generated Makefile holds `-msse2` and no host-tuned option. Repeat with `CXXFLAGS=-DKD2_OWN_FLAG`, and confirm a builder's own flag still reaches the build. The compile path of `install` needs a packaged layout and `sudo`, so record it as Not covered, with its one-line diff as the proof (FR-006, FR-007, FR-008). Keep the results for T026

**Checkpoint**: Two people running one script get the same instruction set. KD-3 stays open until the check rejects a host-tuned flag.

---

## Phase 5: User Story 3 - Reject a Prohibited Option in CI, Local Proof (Priority: P1)

**Goal**: One script searches the build files and reads the gated build's real flags, and seeded failures prove it on this computer (plan steps 3 and 4). [contracts/flag-check.md](contracts/flag-check.md) and [contracts/parity-gate.md](contracts/parity-gate.md) are the contracts.

**Independent Test**: Seed each prohibited option into a gated build's flags, one at a time, and confirm that the check fails and names it. Then seed one into each kind of build file, and confirm that the search fails and names the file.

### Implementation for User Story 3

- [ ] T019 [US3] Create `.github/actions/compile-engine/check-flags.sh` from `specs/003-strict-floating-point/contracts/flag-check.md`, starting from `~/verify/kd2/prototype/check-flags.sh`, which already passed every seed, and removing its prototype comment. It has two commands, `search` and `flags <Makefile>`, and exit codes 0, 1, and 2. One `OPTIONS` array holds the 11 prohibited options, and it is the only list (FR-012). "An option matches as a whole option", so use `(^|[^[:alnum:]_-])` before it and `([^[:alnum:]_-]|$)` after it. `flags` reads the `CC`, `CXX`, `LINK`, `CFLAGS`, `CXXFLAGS`, `LFLAGS`, and `LIBS` lines (FR-011), fails on any `-ffp-contract` value other than `off`, and fails if `-ffp-contract=off` is missing from `CFLAGS` or `CXXFLAGS` (FR-013). `search` uses `git grep -nIE` over the pathspecs the contract lists, and it rejects every `-ffp-contract` value other than `off`, so `-ffp-contract=fast-honor-pragmas` can't pass (FR-035). "An untracked file is not searched", and "A comment counts". A `git grep` exit of 1 means no match, and any other exit above 0 stops with exit 2, as does an empty file list. Every failure is listed and not only the first, so do not use `set -e`. Print the messages the contract gives (FR-014). The script holds no Windows line endings
- [ ] T020 [P] [US3] Create `.github/actions/compile-engine/action.yml`, a composite action named `Compile Engine`, with a `description` that says it searches the build files, builds and installs the release binary, then rejects prohibited compile flags, and with these steps in order, each named as in `specs/003-strict-floating-point/contracts/parity-gate.md`: `Search the Build Files`, with `shell: bash` and `run: bash "$GITHUB_ACTION_PATH/check-flags.sh" search`, then `Build and Install to a Local Tree`, with `uses: ./.github/actions/build-linux`, then `Reject Prohibited Compile Flags`, with `shell: bash` and `run: bash "$GITHUB_ACTION_PATH/check-flags.sh" flags mandelbulber2/build/Makefile` (FR-009, FR-035)
- [ ] T021 [P] [US3] Create `.github/workflows/parity-gate.yml`, named `Parity Gate`. It triggers on `pull_request:` with no branch filter and on `push:` to `ministic-regression`, and sets `permissions:` to `contents: read` at the top level, because this repository's default workflow token is `write`. Its one job, `parity-gate`, runs on `ubuntu-24.04` with `timeout-minutes: 120`. Its steps are `actions/checkout@v7`, then `./.github/actions/compile-engine`. It uses no secrets, and adds nothing else (FR-009, FR-010)
- [ ] T022 [US3] Commit with the message `ci: add parity-gate with a build-file search and flag check`, staging the files from T019 to T021 and this file. Confirm `git ls-files --eol .github/actions/compile-engine/check-flags.sh` shows `i/lf`, and that `git status --short` shows no other change

### Checks for User Story 3

- [ ] T023 [US3] Build T022's commit with `~/verify/kd2/build_fresh.sh ~/mandelbulber2 windows/upgrade/strict-floating-point`. From that clone, run both commands in section 3 of `specs/003-strict-floating-point/quickstart.md`. `search` prints the number of files it searched, and both print `passed` and exit 0 (FR-011, FR-012, FR-013)
- [ ] T024 [US3] Write `~/verify/kd2/seed_flags.sh` from `~/verify/kd2/prototype/seed_flags.sh`, so it runs the committed script from `~/mandelbulber2/.github/actions/compile-engine/check-flags.sh`, on copies of T023's Makefile. Run the three `flags` tables and the Cannot run table in `specs/003-strict-floating-point/contracts/flag-check.md`, including the 4 environment variables through real `qmake6` runs in temporary folders. Every row gives its expected exit code and name (SC-002, FR-008, FR-011, FR-012, FR-013, FR-014, FR-015). Keep the output in `~/verify/kd2/results/seed_flags.txt`
- [ ] T025 [US3] Write `~/verify/kd2/seed_search.sh` from `~/verify/kd2/prototype/seed_search.sh`, so it works in a throwaway clone of the branch under `/tmp` and uses the committed script. First confirm the search on a throwaway clone at `8fd49ba59`, with the committed script copied in, reports exactly the 9 lines the contract names. Then run every row of the `search` table in `specs/003-strict-floating-point/contracts/flag-check.md`, restoring each file between rows (SC-002, FR-012, FR-014, FR-015, FR-035). Keep the output in `~/verify/kd2/results/seed_search.txt`
- [ ] T026 [US3] Fill in the Flag check section of `specs/003-strict-floating-point/evidence.md` from T024 and T025, and add the results of T011 to T013 and T018 to the Success criteria table. Commit with the message `docs: record the flag check seeds`

**Checkpoint**: The check works on this computer, and every option and rule has a seed that proves it. It hasn't run in GitHub Actions yet, so KD-2 and KD-3 stay open.

---

## Phase 6: User Story 4 - Keep Renders Reproducible (Priority: P1)

**Goal**: `--self-parity` still passes with strict rules.

**Independent Test**: Build the final commit fresh on Linux and run `--self-parity`.

- [ ] T027 [US4] Find the final commit with `git log -1 --format=%H -- mandelbulber2/qmake mandelbulber2/cmake mandelbulber2/msvc mandelbulber2/src mandelbulber2/qt mandelbulber2/formula`, and build it fresh in `~/mandelbulber2`, detached at that commit. Every check in Phases 6 to 9 uses this commit, and its first 9 characters name the Windows clones. If a check calls for an engine change, commit the fix with a `fix:` message, record it under Failures found in `specs/003-strict-floating-point/evidence.md` with the failure, its cause, and the commit (FR-018), and start again from this task
- [ ] T028 [US4] In `~/mandelbulber2`, run `python3 tests/regression_runner.py --self-parity` three times in a row, in the background, and keep the three reports in `~/verify/kd2/results/`. Each run exits 0 and reports 487 items passed, with every buffer and statistic matching at 1 thread and at all 32 (SC-005, FR-017). This doesn't need a quiet computer. A failure goes to T027's rule and not to the harness (FR-018). Record the runs in `specs/003-strict-floating-point/evidence.md`, and commit with the message `docs: record Self-Parity on the strict build`

**Checkpoint**: Renders reproduce on the strict build, so the output comparison means something.

---

## Phase 7: User Story 5 - Know What Changed in the Output (Priority: P1)

**Goal**: A record shows how output changed, and each visible change has a known cause (plan step 5). [contracts/evidence-record.md](contracts/evidence-record.md) defines it.

**Independent Test**: Read the record alone. For each item, it says what changed and by how much, and for each reviewed image it says whether the change is visible and why.

- [ ] T029 [US5] Collect both builds, each from its own clone with the harness from its own commit: `python3 tests/regression_runner.py --collect ~/verify/kd2/collect-before` from `~/mandelbulber2-before`, and `--collect ~/verify/kd2/collect-after` from `~/mandelbulber2`. Both exit 0 with 487 items, and neither reports uncommitted changes. This doesn't need a quiet computer
- [ ] T030 [US5] From `~/mandelbulber2`, run `python3 tests/regression_runner.py --compare ~/mandelbulber2-before/mandelbulber2/build/appdir/usr/bin/mandelbulber2`, and copy `tests/outputs/report.txt` to `~/verify/kd2/results/compare-before-after.txt`. It exits 1, because output is expected to change (FR-019)
- [ ] T031 [US5] Write `~/verify/kd2/output_record.py <first collection> <second collection> [record path]`, run from a clone's root with `tests/` on the path. `--cross-check` refuses two commits, so it uses the harness's `read_collection`, `read_reference`, `compare_exports`, and `holds_one_value` directly. It prints how many items are identical in every buffer and statistic, how many differ in each buffer and each statistic, and any final image that holds one value on one side only. With a record path, it writes the columns the `output-record.tsv` section of `specs/003-strict-floating-point/contracts/evidence-record.md` lists, one row per item. It ranks the swept formulas by `final_image_average` and writes the top 20 to `~/verify/kd2/results/top-formulas.txt`. Run it on T029's collections, with the record path `specs/003-strict-floating-point/output-record.tsv` in the worktree. An item whose final image holds one value on the after side only is a failure under T027's rule (FR-018, FR-019, SC-006)
- [ ] T032 [US5] Write `~/verify/kd2/guard_counts.patch` on the final commit (research R11). It adds `mandelbulber2/src/guard_counts.hpp`, with two arrays of `std::atomic<long long>`, infinite and NaN, for 4 sites, and an inline `CountGuard(int site, bool infinite)`. A static object's destructor writes one line per site to `/tmp/kd2-guards/<pid>.txt`, even with no hits, so a missing file shows a broken count. `exit` and a return from `main` both run it. Inside each `if` block, before the value changes, call it with `std::isinf` of any component: site 0 at `mandelbulber2/src/compute_fractal.cpp:316`, site 1 at `mandelbulber2/src/compute_fractal.cpp:500`, site 2 at `mandelbulber2/src/render_worker.cpp:776`, and site 3 at `mandelbulber2/src/shader_calculate_normals.cpp:120`. Site 2's test also fires on `point == lastPoint`, so count it only when `point.IsNotANumber()` is true. Build it with `~/verify/kd2/build_fresh.sh ~/mandelbulber2-diag <final commit> ~/verify/kd2/guard_counts.patch`. From `~/mandelbulber2`, `--compare` with that binary exits 0 on all 487 items, or its counts mean nothing (FR-020)
- [ ] T033 [US5] Write `~/verify/kd2/guard_counts.py`, run from `~/mandelbulber2-diag` with `tests/` on the path. For each item in `build_work_items(binary)`, it empties `/tmp/kd2-guards`, renders with `render_item`, checks it with `check_render_output`, and reads the one file written, stopping if there is none. Render one item at a time. Write `~/verify/kd2/results/guards.tsv`, one row per item and site, with infinite and NaN hits, and whether the item's scene turns on `iteration_threshold_mode` or `interior_mode`. A hit at site 0 with an infinite value is an orbit the escape test used to catch. It changes the distance estimate, and the point counts as inside only with one of those modes on (`mandelbulber2/src/calculate_distance.cpp:607` and `:623`). Confirm no diagnostic change reached the branch: `git status --short` in the worktree shows nothing under `mandelbulber2/` (FR-020, SC-006)
- [ ] T034 [US5] Write `~/verify/kd2/guards_off.patch` on the final commit. In `mandelbulber2/src/algebra.hpp`, it makes both `IsNotANumber()` methods return `false`, which is what `-ffast-math` does today. Build it with `~/verify/kd2/build_fresh.sh ~/mandelbulber2-diag <final commit> ~/verify/kd2/guards_off.patch`. From `~/mandelbulber2`, run `python3 tests/regression_runner.py --compare ~/mandelbulber2-diag/mandelbulber2/build/appdir/usr/bin/mandelbulber2`, and copy `tests/outputs/report.txt` to `~/verify/kd2/results/compare-guards-off.txt`. It exits 1 if the guards change anything, and 0 if they change nothing. Every difference it lists is the guards' own effect, and an item it doesn't list changed only through strict arithmetic. Then, from `~/mandelbulber2-diag`, run `python3 tests/regression_runner.py --collect ~/verify/kd2/collect-guards-off`, because `--compare` deletes each render once it has compared it, and T036 needs the images. The collection notes the patch as an uncommitted change, which is expected (FR-020, SC-006)
- [ ] T035 [US5] Write `~/verify/kd2/no_contract_off.patch` on the final commit, which removes the `gcc` block from `mandelbulber2/qmake/common.pri`. Build it the same way, and run the same `--compare`, copying the report to `~/verify/kd2/results/compare-no-contract-off.txt`. It exits 0 on all 487 items, which proves the statement in FR-022 that `-ffp-contract=off` has no effect on the gated build. If it exits 1, correct that statement for T050 and T061, and record why (FR-022)
- [ ] T036 [US5] Write `~/verify/kd2/review_images.py` from `~/verify/kd2/noise.py`'s `write_png`. For each of the 26 scenes and the 20 formulas from T031, it writes one PNG to `~/verify/kd2/review/`, with four panels side by side: before, after, the amplified difference, and the guards' area, which marks where T029's after collection and T034's guards-off collection differ. It enlarges the 32x24 formula images by a whole number so they can be seen. Where a shape appears or vanishes, add the before image from T005's collection as a fifth panel, read from `/mnt/c/GIT/mb2-kd2-8fd49ba59/tests/outputs/collection/msvc`. Look at each one, then ask the user to review them, because SC-006 needs a reviewer, and wait for the answer. A visible change is one that can be seen in the before and after panels at normal brightness, and the amplified panel only shows where to look. Name each visible change's cause from the guards' area, with T034's comparison as the proof: a guard if the change falls inside that area, and otherwise strict arithmetic. A visible change with a named cause is accepted once it is recorded (FR-023). One with no named cause blocks the feature, so find the cause first, and a failure goes to T027's rule (FR-018, FR-021, SC-006)
- [ ] T037 [US5] Fill in the Output section of `specs/003-strict-floating-point/evidence.md`: the Summary, one row per scene, one row per formula from T031 with the rule that chose them, the Guards table with totals from T033, the items T034 names, T035's result, the review's verdicts, and Failures found, or the word none. Commit it with `specs/003-strict-floating-point/output-record.tsv`, with the message `docs: record what changed in the output`

**Checkpoint**: Each item's change is on record, the guards' reach is known, and a person has reviewed the images.

---

## Phase 8: User Story 6 - Know What It Costs (Priority: P2)

**Goal**: The record shows what render time this change costs (plan step 5).

**Independent Test**: Time the suite on the before and after builds on a quiet computer, and read the difference.

- [ ] T038 [US6] Build the host-tuned before state with `CXXFLAGS=-march=native ~/verify/kd2/build_fresh.sh ~/mandelbulber2-native 8fd49ba59`, because qmake imports that variable. Confirm `CXXFLAGS` in its `mandelbulber2/build/Makefile` holds `-march=native`. Record the build in `specs/003-strict-floating-point/evidence.md` (FR-027)
- [ ] T039 [US6] Write `~/verify/kd2/timed_collect.py <folder>`, run from a clone's root with `tests/` on the path. It imports `regression_runner`, wraps `render_item` to add up `time.monotonic()` for scenes and for formulas, and runs `main(["--collect", folder])`. It prints and appends to `~/verify/kd2/results/timing.tsv` the build, the whole time, the scene time, the formula time, and the exit status. Write `~/verify/kd2/run_quiet.sh` from `~/verify/kd1/run_quiet.sh`. It waits until `pgrep -f 'FractalEvaluator|bin/mandelbulber2'` has been empty for 60 seconds. During each run, a background loop samples `pgrep -f FractalEvaluator` every 5 seconds and logs the time from `/proc/uptime` to `~/verify/kd2/results/not_quiet.log`, so each run can be matched to any overlap (FR-025)
- [ ] T040 [US6] Run in the background, in this order, through `run_quiet.sh`: `timed_collect.py` on the before build, then the after build, then the after build, then the before build, each from its own clone. Then `taskset -c 4 python3 ~/verify/kd2/timed_collect.py <folder>` on each. Then `timed_collect.py` on `~/mandelbulber2-native`. Use new folders under `~/verify/kd2/`, and check `not_quiet.log` throughout, because `--collect` prints its results at the end. Run nothing else from this feature during these runs, because the process check can't see a compile. Repeat any run that overlaps another render, and do not record it. If FractalEvaluator won't go quiet, tell the user and wait, and never stop it (FR-024, FR-025, FR-027, SC-007)
- [ ] T041 [US6] Fill in the Time tables in `specs/003-strict-floating-point/evidence.md`, with the note on how quiet each run was, and commit with the message `docs: record the render times`

**Checkpoint**: The cost is on record, with no slowdown limit beyond the gate's 30 minutes.

---

## Phase 9: User Story 7 - Keep Everything Else Working, Local Checks (Priority: P2)

**Goal**: Every build, test, and mode that works today still works, on this computer (plan step 5).

**Independent Test**: Build each configuration, then run the application's own tests and Cross-Build Check.

- [ ] T042 [P] [US7] On T027's build, from `~/mandelbulber2`, run `env -i HOME="$(mktemp -d)" PATH="$PATH" LC_ALL=C.UTF-8 QTEST_FUNCTION_TIMEOUT=1800000 mandelbulber2/build/appdir/usr/bin/mandelbulber2 --test`, and `python3 tests/test_regression_runner.py`. Every test passes (SC-009, FR-033). Record the result in `specs/003-strict-floating-point/evidence.md`
- [ ] T043 [US7] Build MSVC Release x64 fresh on the final commit in the new clone `C:\GIT\mb2-kd2-<first 9 characters>`, as T005 does. Run `--test` from `build-win/Release/x64` with `QT_FORCE_STDERR_LOGGING=1` and `QTEST_FUNCTION_TIMEOUT=1800000`, then `python tests/test_regression_runner.py`, then `python tests/regression_runner.py --collect tests/outputs/collection/msvc`. Every test passes, and the collection reports no uncommitted changes (SC-009, FR-033). Record the result in `specs/003-strict-floating-point/evidence.md`
- [ ] T044 [US7] In WSL, run `output_record.py` on T005's and T043's collections through `/mnt/c`, with no record path. It finds all 487 items identical, because neither the Release flags nor the sources changed, which proves the statement in FR-022. If FR-018 changed engine code, it may differ, and the record says why. Record the result under MSVC and cross-build in `specs/003-strict-floating-point/evidence.md`
- [ ] T045 [US7] Copy `~/verify/kd2/collect-after` to `tests/outputs/collection/qmake` in T043's clone. From that clone, run `python tests/regression_runner.py --cross-check tests/outputs/collection/qmake tests/outputs/collection/msvc`. Both come from the final commit, so it runs. It exits 0, and its `identical:` line goes under MSVC and cross-build in `specs/003-strict-floating-point/evidence.md` (SC-009). A difference it records isn't drift, because the two builds are different targets
- [ ] T046 [P] [US7] Confirm the other builds compile from fresh on `8fd49ba59` and on the final commit, by copying `~/verify/kd1/run_t077.sh` to `~/verify/kd2/` and changing its first ref from `windows/ministic-regression` to `8fd49ba59`. It builds in `~/mandelbulber2-diag`, never in `~/mandelbulber2`, and it discards any patch there, so run it after T035: `mandelbulber2/qmake/mandelbulber.pro`, which has no OpenCL, and `mandelbulber2/cmake/CMakeLists.txt`. No `mandelbulber.pro` compile line has `USE_OPENCL`. If a build fails on `8fd49ba59` too, this feature only has to fail the same way (SC-010, FR-033). Record both under Other builds in `specs/003-strict-floating-point/evidence.md`
- [ ] T047 [US7] Build MSVC Debug x64 fresh on the final commit in the new clone `C:\GIT\mb2-kd2-debug-<first 9 characters>`, as T006 does. It compiles, and its `cl` lines hold `/fp:precise` and not `/fp:fast` (SC-010, FR-003, FR-033). If T006 failed, it only has to fail the same way, and the `FloatingPointModel` line must read `Precise`. Record it under Other builds in `specs/003-strict-floating-point/evidence.md`
- [ ] T048 [US7] Check OpenCL on T043's build. If `--help-opencl` lists a device, render `IFS_002.fract` with `--gpu` and `--override opencl_mode=3`, confirm it finishes, and that the image differs from the same render with `DOF_enabled=0`. WSL has no OpenCL device, so record the Linux host code as Not covered. OpenCL build options stayed as they were (FR-032). Record the result in `specs/003-strict-floating-point/evidence.md`
- [ ] T049 [US7] Commit the records from T042 to T048 with the message `docs: record the other builds and tests`

**Checkpoint**: All local checks pass on the final commit. This is the point to ask for the first push.

---

## Phase 10: Pull Request and GitHub Actions Proof

**Purpose**: The proof that needs GitHub: User Story 3's closing conditions, User Story 6's gate time, and User Story 7's CI checks (plan step 6). [research.md](research.md) R8 describes the seeded runs.

- [ ] T050 [US3] Ask the user for the go-ahead to push. Then push `upgrade/strict-floating-point`, and open one draft pull request against `ministic-regression`: `gh pr create --draft --repo Coastrnaut/mandelbulber2 --base ministic-regression`. Its body states the expected effect on output in the words of FR-022, then what was measured, from `specs/003-strict-floating-point/evidence.md`, and it has no attribution lines. Bind it with the app's pull request tools. Auto-fix stays the user's choice to turn on themselves, and auto-merge stays off
- [ ] T051 [US3] Read the `parity-gate` run on the pull request. It passes, on a fresh gated build. `Search the Build Files` passes before the build starts, and `Reject Prohibited Compile Flags` prints `passed` after it. The workflow shows read-only access (SC-001, FR-009, FR-010). Add the run to the GitHub Actions table in `specs/003-strict-floating-point/evidence.md`. KD-2's closing condition is now met
- [ ] T052 [US3] Ask the user for the go-ahead to push a throwaway branch, open a draft pull request into `upgrade/strict-floating-point`, and delete both afterward. Then, in a new temporary worktree: `git worktree add -b seed/flag-check <temporary folder> upgrade/strict-floating-point`.
  1. Commit `-ffast-math` on the CMake line, push, and open the draft pull request with `--base upgrade/strict-floating-point`, a title that says not to merge, and no attribution lines. Cancel the `C++ CI Build (Qt6)` run on that branch at once with `gh run cancel`, because that workflow replaces the `continuous` release, and cancel the other GitHub Actions runs the branch starts apart from `parity-gate`. Azure Pipelines and CircleCI run too, `gh` can't cancel them, and they do no harm. `Search the Build Files` fails and names the file, and the build never starts.
  2. Revert that seed, add a job-level `env:` with `CXXFLAGS: -march=${{ 'native' }}` to `.github/workflows/parity-gate.yml`, which the search can't match, commit, and push. Cancel the same runs. The search passes, the build runs, and `Reject Prohibited Compile Flags` fails with `-march=native in CXXFLAGS`. This is KD-3's closing condition on a real gated build.
  3. Close the pull request unmerged, delete `seed/flag-check` on GitHub and locally, and remove the worktree. Confirm with `git branch -a` that nothing named `seed` remains, and with the check in section 3 of `specs/003-strict-floating-point/quickstart.md` that no seed reached the branch.

  Add both runs to the GitHub Actions table in `specs/003-strict-floating-point/evidence.md` (SC-003, FR-015, FR-035)
- [ ] T053 [US6] Read the Collect step of the `linux-qmake` job in Cross-Build Check on the pull request, from `gh api repos/Coastrnaut/mandelbulber2/actions/runs/<run>/jobs`. It stays under 30 minutes, which stands in for a gate run, because no references exist (SC-008, FR-026). Add it to the Time section of `specs/003-strict-floating-point/evidence.md`
- [ ] T054 [US7] Read the other checks on the pull request with `gh pr checks`: Cross-Build Check with `linux-qmake`, `windows-msvc`, and `cross-check`, `C++ CI Build (Qt6)`, Azure Pipelines, and CircleCI's `build-and-pack`. All pass. CircleCI is the only Clang build, so confirm from its log, if it can be read, that compile lines hold `-ffp-contract=off`. Record the `identical:` line from `cross-check` and any row not covered, including `mandelbulber-opencl-m1.pro`, which nothing builds, under Other builds in `specs/003-strict-floating-point/evidence.md` (SC-009, SC-010, FR-004, FR-033)
- [ ] T055 Commit the records from T051 to T054 with the message `docs: record the GitHub Actions proof`. Don't push yet, because T059 follows and each push restarts the checks

**Checkpoint**: `parity-gate` passes, and it has rejected a host-tuned flag on a real gated build. Both closing conditions are met, and the closing edits follow.

---

## Phase 11: User Story 8 - Close KD-2 and KD-3 (Priority: P2)

**Goal**: The constitution shows both deviations closed, and the playbook agrees with what was built (plan step 6).

**Independent Test**: Read the constitution's table and the evidence it cites.

- [ ] T056 [US8] Finish `specs/003-strict-floating-point/evidence.md`: complete the Success criteria table with one row per criterion, with anything not covered named plainly. Read it alone, and confirm every visible change has a cause (SC-006), and that no recorded time overlaps another render (SC-007)
- [ ] T057 [US8] In `.specify/memory/constitution.md`, end the "Closed when" cell of the KD-2 and KD-3 rows the way the KD-1 row ends: the bold words "Closed" and the closing date, then ", see", then `specs/003-strict-floating-point/evidence.md` in code format. Add the KD-5 row from research R18 to the same table. Set Version to 1.1.0 and Last Amended to the closing date, and leave Ratified alone. KD-4 and KD-5 stay open. This needs T051 and T052 first (FR-028, SC-012)
- [ ] T058 [US8] In `MIGRATION_PLAYBOOK.md`, make the corrections in research R16 (FR-029, SC-012):
  - Line 87: change "Use Release, because Debug uses `/fp:fast` (KD-2)." to "Use Release, because that is the binary the harness finds."
  - Line 207: change "It is not the parity gate, which arrives in Phase 4 as its own workflow." to "It is not the parity gate, whose workflow arrives in Phase 3 and whose gate steps arrive in Phase 4."
  - Line 233: name the `IsNotANumber()` guards in `compute_fractal.cpp`, `render_worker.cpp`, and `shader_calculate_normals.cpp`, drop `math_utilities.h`, because `CheckNAN` has no callers, and say the guards catch infinity too, so an orbit that overflows keeps its last finite value, as it already does on MSVC Release.
  - Line 235: say that a script beside the action holds the list of options, searches the tracked build files before the build, and then reads the gated build's compile and link flags.
  - Lines 237 to 259: replace the listing with the committed `.github/actions/compile-engine/action.yml`, and point to `check-flags.sh` and `specs/003-strict-floating-point/contracts/flag-check.md`.
  - Lines 263 to 279 and 296 to 323: add `permissions:` with `contents: read` to both `parity-gate.yml` listings.
  - Before line 352: add a new step, `### 2. Settle the CPU Math Path`, and renumber the later Phase 4 steps. It says glibc picks its math code path from the CPU at run time, gives the measured result from the spec's Assumptions, and says that before capture the gate's handling must be decided and recorded with the references, that finishing this step closes KD-5, and that a Reference Target that names CPU features is a constitution amendment.
  - Line 370: close KD-4 and KD-5, and say KD-1 closed in version 1.0.1, and KD-2 and KD-3 closed in version 1.1.0.

  Then search the file for `/fp:fast`, `math_utilities`, `KD-2`, and `KD-3`, and correct anything else that is now wrong. The `/speckit-specify` prompt on line 230 stays, as a record of the request
- [ ] T059 [US8] Commit with the message `docs: close KD-2 and KD-3`, staging `specs/003-strict-floating-point/evidence.md`, `.specify/memory/constitution.md`, `MIGRATION_PLAYBOOK.md`, and this file

**Checkpoint**: KD-2 and KD-3 are closed on the branch, and KD-5 is on record. KD-4 and KD-5 stay open for Phase 4.

---

## Phase 12: Polish & Cross-Cutting Concerns

**Purpose**: Check the whole branch, then make the pull request ready

- [ ] T060 Check the whole branch against `8fd49ba59` (SC-011):
  - The two `git diff --stat` commands in section 7 of `specs/003-strict-floating-point/quickstart.md` print nothing, apart from anything FR-018 required (FR-016, FR-030, FR-034).
  - `git diff 8fd49ba59 -- mandelbulber2/qmake mandelbulber2/cmake mandelbulber2/msvc mandelbulber2/tools mandelbulber2/deploy/linux/install` shows only what `specs/003-strict-floating-point/contracts/build-flags.md` lists, and no `-fno-math-errno` or other new option (FR-001, FR-002, FR-003, FR-031).
  - OpenCL's build options live in `mandelbulber2/src/opencl_engine.cpp`, which the first `git diff` covers, so they did not change (FR-032).
  - The SC-004 search in section 3 of `specs/003-strict-floating-point/quickstart.md` finds only the check's own files, `mandelbulber2/tools/DOE/NaNtester.sh`, and the history in `mandelbulber2/deploy/NEWS`.
  - No guard changed (FR-023). `tests/references` does not exist (FR-034, SC-012). No commit message on the branch has an attribution line.
- [ ] T061 Commit this file with T060 marked. Ask the user for the go-ahead to push, then push and mark the pull request ready with `gh pr ready`. Update its body to state the expected effect on output in the words of FR-022, what was measured, the render cost, and each visible change's cause. Link `specs/003-strict-floating-point/evidence.md`, and give what the amendment needs: the reason for closing two deviations and adding KD-5, the version change from 1.0.1 to 1.1.0, the dates, and the affected plans and gates. Confirm the constitution's gates hold, list anything not covered, name KD-5 and the Phase 4 step that settles it, and add no attribution lines
- [ ] T062 Confirm `parity-gate`, Cross-Build Check, and the other checks pass on the final push (SC-001, SC-009, SC-010). A maintainer approves the amendment before it merges. Do not enable auto-merge. `parity-gate` isn't a required check, and CodeQL only runs for `master`, so T061's body lists it as not covered. After a maintainer merges, read the repository's Actions list, and confirm `parity-gate` started for the push to `ministic-regression` (FR-010). The evidence can't hold that, because it comes after the last commit, so report it to the user

---

## Requirement Coverage

| Requirement | Tasks |
|---|---|
| FR-001 | T007, T011, T060 |
| FR-002 | T008, T012, T060 |
| FR-003 | T009, T047, T060 |
| FR-004 | T007, T008, T011, T012, T054 |
| FR-005 | T013 |
| FR-006 | T014, T015, T018 |
| FR-007 | T016, T018 |
| FR-008 | T018, T024 |
| FR-009 | T020, T021, T051 |
| FR-010 | T021, T051, T062 |
| FR-011 | T019, T023, T024 |
| FR-012 | T019, T023, T024, T025 |
| FR-013 | T019, T023, T024 |
| FR-014 | T019, T024, T025 |
| FR-015 | T024, T025, T052 |
| FR-016 | T060 |
| FR-017 | T028 |
| FR-018 | T027, T028, T031, T036, T044, T060 |
| FR-019 | T030, T031 |
| FR-020 | T032, T033, T034 |
| FR-021 | T036 |
| FR-022 | T035, T044, T050, T061 |
| FR-023 | T036, T060 |
| FR-024 | T040 |
| FR-025 | T039, T040 |
| FR-026 | T053 |
| FR-027 | T038, T040 |
| FR-028 | T057 |
| FR-029 | T058 |
| FR-030 | T060 |
| FR-031 | T060 |
| FR-032 | T048, T060 |
| FR-033 | T006, T042, T043, T046, T047, T054 |
| FR-034 | T060 |
| FR-035 | T019, T020, T025, T052 |
| SC-001 | T051, T062 |
| SC-002 | T024, T025 |
| SC-003 | T052 |
| SC-004 | T013, T060 |
| SC-005 | T028 |
| SC-006 | T031, T033, T034, T036, T056 |
| SC-007 | T040, T056 |
| SC-008 | T053 |
| SC-009 | T042, T043, T045, T054, T062 |
| SC-010 | T046, T047, T054, T062 |
| SC-011 | T060 |
| SC-012 | T057, T058, T060 |

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies.
- **Foundational (Phase 2)**: Depends on Setup. T004 to T006 use commit `8fd49ba59`, so they can run at any time.
- **User Story 1 (Phase 3)**: Depends on Setup.
- **User Story 2 (Phase 4)**: Depends on Setup. It doesn't depend on User Story 1.
- **User Story 3, local proof (Phase 5)**: Depends on User Stories 1 and 2, because `search` must pass on the branch.
- **User Story 4 (Phase 6)**: Depends on User Story 1.
- **User Story 5 (Phase 7)**: Depends on User Story 4 and T004. T036 needs T005's collection.
- **User Story 6 (Phase 8)**: Depends on T004 and T027. It needs a quiet computer. Phase 9 can go on while T040 waits for one, but nothing from this feature runs during a timing run. Phase 10 needs T041, so the first push waits for it.
- **User Story 7 (Phase 9)**: Depends on T027. T044 needs T005 and T043. T045 needs T029 and T043. T046 needs T035, because it reuses `~/mandelbulber2-diag`.
- **Pull request (Phase 10)**: Depends on every local check, and on the user's go-ahead.
- **User Story 8 (Phase 11)**: Depends on Phase 10. T057 must not run until T051 and T052 have passed.
- **Polish (Phase 12)**: Depends on everything above.

### Within Each Phase

- T007 to T009 come before T010, and T010 before T011 to T013.
- T014 to T016 come before T017, and T017 before T018.
- T019 to T021 come before T022, and T022 before T023 to T025.
- T029 comes before T031, and T031 before T036. T032 comes before T033, T033 before T034, and T034 before T035, because they share `~/mandelbulber2-diag`.
- T038 and T039 come before T040.
- T043 comes before T044, T045, and T048.
- T050 comes before T051 to T054.
- Each commit task comes after every task in its step, and each step's checks come after its commit.

### Parallel Opportunities

- T004, T005, and T006, which use separate clones
- T008 and T009, alongside T007
- T014, T015, and T016
- T020 and T021
- T013, once T010 is done
- T042 and T046, as long as nothing is being timed
- Any output or build check can run while FractalEvaluator runs, but T040 can't

## Parallel Example: The Before State

```text
Task: "Build the before state in WSL in ~/mandelbulber2-before"
Task: "Build MSVC Release fresh on 8fd49ba59 in C:\GIT\mb2-kd2-8fd49ba59"
Task: "Build MSVC Debug fresh on 8fd49ba59 in C:\GIT\mb2-kd2-debug-8fd49ba59"
```

## Parallel Example: User Story 2

```text
Task: "Remove -march=native from mandelbulber2/tools/prepare_for_dev_ubuntu.sh"
Task: "Remove -march=native from mandelbulber2/tools/prepare_for_dev_debian_testing.sh"
Task: "Delete the export line from mandelbulber2/deploy/linux/install"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phases 1 and 2.
2. Complete Phase 3.
3. **Stop and validate**: Every build definition follows Principle III.

The strict flags are the heart of KD-2, but nothing stops them from being undone yet, and neither deviation closes, so nothing merges at this point. All of it lands in one pull request.

### Incremental Delivery

1. Phases 3 and 4 make every build strict and independent of its computer.
2. Phase 5 proves the check on this computer.
3. Phase 6 shows renders still reproduce.
4. Phase 7 shows what changed, and a person reviews it.
5. Phase 8 records the cost, whenever the computer is quiet.
6. Phase 9 checks everything else.
7. Phase 10 proves the check in GitHub Actions, which meets both closing conditions.
8. Phases 11 and 12 close the deviations and make the pull request ready.

Stop at each checkpoint. If a check fails, fix it in a new commit on that step before moving on.

### Where the User Is Needed

- A go-ahead before each push: T050, T052, and T061.
- The image review in T036.
- FractalEvaluator going quiet for T040.

---

## Notes

- This feature closes KD-2 and KD-3 only. KD-4 belongs to Baseline 1. Do not report it as passing.
- This feature records KD-5 and doesn't close it, because glibc picks its math path from the CPU, and Phase 4 settles that. Do not report it as passing.
- This feature creates no references. Baseline 1 follows it.
- `evidence.md` is where every result goes, so a reviewer can read them in one place.
