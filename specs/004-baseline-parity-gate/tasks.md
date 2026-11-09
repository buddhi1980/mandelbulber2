---

description: "Task list for Baseline 1 and the parity gate"
---

# Tasks: Baseline 1 and the Parity Gate

**Input**: Design documents from `specs/004-baseline-parity-gate/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: FR-004 asks for harness tests of the forced path, so T005 writes them before T006 builds it. Every other check is a run whose result goes in `evidence.md`, through [quickstart.md](quickstart.md).

**Organization**: Tasks are grouped by user story, in the plan's order of work. User Story 3's gate steps are written in Phase 5 but pushed in Phase 6, right before Baseline 1's pull request opens, so the gate fails for minutes, not hours (FR-013). The user is the maintainer the spec names.

## Rules for Every Task

- **Where work lands**: Work lands on `deterministic-regression` by direct push. The only other branches are `baseline/1`, `seed/drift-proof`, and `amend/kd-4-kd-5`, plus a later `baseline/<number>` if FR-017 needs one. Each is deleted on GitHub, and locally, once its pull request closes or merges (FR-030).
- **Fresh builds**: Every result comes from a fresh build of the commit being tested. No task may use an existing binary, including the builds already in the WSL clones and in the `C:\GIT\mb2-kd1-*` and `C:\GIT\mb2-kd2-*` clones.
- **WSL**: WSL sees only commits, so each step commits first and then builds there.
- **Commits**: Commit after each task that changes the repository. Stage only the files it changed, plus this file with its finished tasks marked `[X]`.
  - **On side branches**: Commits on `baseline/1`, `seed/drift-proof`, and `amend/kd-4-kd-5` hold only their own change, never this file.
  - **The window**: Between T024's push and T026's merge, nothing lands on `deterministic-regression` (FR-013), so the records of T024 to T026 are committed after the merge.
- **Messages**: Commit messages and pull requests carry no `Co-Authored-By` line, no other co-author statement, and no "Generated with" line.
- **Go-aheads**: Every push, every pull request, the protection change, every branch deletion on GitHub, and every rerun of a workflow need the user's go-ahead. The user merges each pull request, or tells Claude to. Auto-merge is never turned on, with `gh pr merge --auto` or through the app.
- **Waiting on CI**: Wait for a run to finish before reading its result with `gh run view`, and don't poll it in a loop.
- **Gate times**: Record the "Run Parity Gate" step's time of every full gate run the evidence mentions (FR-014, SC-003).
- **A failing push after S4**: If a push run fails once the gate is required, the next commit to land fixes or reverts it, the user chooses which, and references are never regenerated to clear it (FR-021).
- **Scope**: Change nothing outside the plan:
  - No engine source, apart from a fix FR-010 needs, which lands as its own reviewed direct push.
  - No build file, scene list, formula sweep, formula exclusion, render resolution, or render override.
  - No CI file other than the two workflows, and no repository setting other than the protection (FR-022, FR-028, FR-029).
- **Seeds**: Seeds and scratch copies of the harness stay in `~/verify/kd5` or on `seed/drift-proof`, and never reach `deterministic-regression`.
- **No hiding**: No tolerance, exclusion, or skip list may hide a difference (FR-010).
- **References**: Nothing under `tests/references/` reaches `deterministic-regression` except through Baseline 1's pull request.
- **FractalEvaluator**: Never stop or kill FractalEvaluator. Nothing in this feature times anything locally, so every local check may run while it renders.
- **Overrides**: `--override` reads a boolean as a number, so use `=1` and `=0`.

**S0** is the commit T001 records. S1 to S6 are the states of `deterministic-regression` in [data-model.md](data-model.md).

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files or machines, no dependencies)
- **[Story]**: Which user story this task belongs to (for example US1)

## Path Conventions

- Harness: `tests/regression_runner.py`, `tests/test_regression_runner.py`
- CI: `.github/workflows/`
- References: `tests/references/linux-x86_64-gcc/`
- This feature's documents: `specs/004-baseline-parity-gate/`

## Build Environments

- **Checkout**: `C:\GIT\mandelbulber2`, on `deterministic-regression`, which is `/mnt/c/GIT/mandelbulber2` in WSL. `.specify/feature.json` names this feature.
- **WSL**: The `Ubuntu-24.04` distro, with GCC 13.3.0, glibc `2.39-0ubuntu8.9`, Python 3.12.3, Qt 6.4.2, and 32 logical CPUs.
  - **Running scripts**: Use `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/path/to/script.sh`, after stripping carriage returns, because `wsl.exe` expands `$` too early in an inline command.
  - **No installs**: `sudo` needs a password, so only the user can install anything.
- **WSL clones**: `~/mandelbulber2` holds the current builds. `~/mandelbulber2-seed` holds the drift proof's seeded build, and it is the only clone that may hold a patch. `~/mandelbulber2-amend` holds the amendment's build, for T035's local pass. `~/verify/kd5/build_fresh.sh <clone> <ref> [patch]` fetches every branch of `C:\GIT\mandelbulber2` into `refs/remotes/windows/`, creates a clone that is missing, and builds from clean.
- **Windows**: The MSVC Release build goes in a new clone, `C:\GIT\mb2-kd5-<first 9 characters of the commit>`, checked out detached.
  - **Build**: Run `nuget restore mandelbulber2/msvc/mandelbulber2.sln`, then `"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" mandelbulber2/msvc/mandelbulber2.sln /m /p:Configuration=Release /p:Platform=x64`.
  - **OpenCL**: If the binary won't start because `OpenCL.dll` is missing, copy it from `mandelbulber2/msvc/packages/opencl-nuget-msvc-x64.*/build/native/bin/` beside the binary.
  - **Leftovers**: The clone stays until the user clears it.
- **GitHub**: `gh` is at `C:\Program Files\GitHub CLI\gh.exe`, signed in as `Coastrnaut`, for the repository `Coastrnaut/mandelbulber2`.
  - **Pull requests** run Parity Gate, Cross-Build Check, C++ CI Build (Qt6), Azure Pipelines, and CircleCI.
  - **A push to `deterministic-regression`** runs Parity Gate and C++ CI Build (Qt6).
  - **A push that creates a `baseline/` branch** runs Capture Baseline and C++ CI Build (Qt6).
  - **Every branch push**: `github-actions-ci.yml` runs on every branch push and uploads an AppImage prerelease, as it always has, and this feature doesn't change that (FR-029).

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Record the starting point and the repository's settings, and start the evidence

- [ ] T001 From `C:\GIT\mandelbulber2`, confirm the starting point:
  - The branch is `deterministic-regression`, and `git status --short` prints nothing.
  - `tests/references` does not exist, and `.specify/feature.json` names `specs/004-baseline-parity-gate`.
  - `python tests/test_regression_runner.py` passes every test.

  Record `git rev-parse HEAD` as S0, `git rev-parse origin/master`, and the test count, for `specs/004-baseline-parity-gate/evidence.md`
- [ ] T002 Save the repository's settings before the first change (FR-022) into `specs/004-baseline-parity-gate/settings/before/`:
  - `repository.json` from `gh api repos/Coastrnaut/mandelbulber2 --jq 'del(.temp_clone_token)'`, so no token is saved.
  - `protected-branches.json` from `gh api "repos/Coastrnaut/mandelbulber2/branches?protected=true"`.
  - `rulesets.json` from `gh api repos/Coastrnaut/mandelbulber2/rulesets`.
- [ ] T003 [P] In WSL, create `~/verify/kd5/` and copy `~/verify/kd1/build_fresh.sh` into it. Use `~/verify/kd5/build_fresh.sh <clone> <ref> [patch]` for every fresh WSL build below. Every scratch script and collection of this feature stays in `~/verify/kd5/`, and nothing there joins the repository
- [ ] T004 Create `specs/004-baseline-parity-gate/evidence.md` from `specs/004-baseline-parity-gate/contracts/evidence-record.md`, with every section, and fill in the Header:
  - The date, the computer, and the WSL toolchain from Build Environments.
  - S0, `origin/master`, and the test count from T001, and the paths of T002's records.
  - A note that every local result checks output, so it may run while FractalEvaluator renders.

  Commit T002's records, the evidence, and this file with the message `docs: start the Baseline 1 evidence`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: None beyond Setup. User Story 1 changes the harness that every later story runs, so it comes first, and no task here would serve every story.

**Checkpoint**: The starting point and the settings are on record. Nothing else has changed.

---

## Phase 3: User Story 1 - Settle the CPU Math Path (Priority: P1) 🎯 MVP

**Goal**: Every render the harness runs on Linux x86-64 takes glibc's plain math path, a probe confirms it before every gate and capture run, and the reference set records it (FR-001 to FR-005).

**Independent Test**: On this computer, the forced and hidden collections match for all 487 items. A gate run with the forcing broken stops with exit 3 and names which failure it is.

### Tests for User Story 1

- [ ] T005 [US1] Write the tests first, in `tests/test_regression_runner.py`:
  - Add every row of the Tests table in `specs/004-baseline-parity-gate/contracts/math-path.md`. They use the real probe function, with a stand-in child process that prints chosen bits, and patches for Linux, Windows, and a machine that isn't `x86_64`.
  - Change the shared setup for the stand-in renderer, so it replaces the probe with one that passes, and every existing test keeps its meaning. `tests/stub_renderer.py` doesn't change.
  - Run `python tests/test_regression_runner.py` on Windows: the new tests fail, and every existing test passes.

### Implementation for User Story 1

- [ ] T006 [US1] Build the forced path in `tests/regression_runner.py`, as `specs/004-baseline-parity-gate/contracts/math-path.md` says:
  - **Constants**: The forced setting `glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`, the hidden setting exactly as the contract lists it, and the six probe rows with their input, plain, and FMA bits, copied verbatim.
  - **Environment**: `clean_environment()` adds `GLIBC_TUNABLES` with the forced setting on Linux, and adds nothing on other systems.
  - **Probe**: A child `sys.executable` process, with the render environment and a temporary home, prints one result per function. On a mismatch it runs again with the hidden setting, and it runs once with no setting to find the CPU's own path. The CPU's model and its `fma`, `fma4`, and `avx2` flags come from `/proc/cpuinfo`. Each outcome prints the line in the contract's Messages table.
  - **Where it runs**: `run_gate()` and `run_capture()` call the probe after their existing checks and before `build_work_items()`. On a system that isn't Linux, the gate prints `math path: not forced, because this isn't Linux` and goes on. On Linux with a machine that isn't `x86_64`, the gate and capture stop with `setup error: the forced math path needs an x86-64 CPU`.
  - **Settings**: `settings_lines()` gains the fifth line `math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`. `COLLECTION_FIELDS` and `COLLECTION_SETTINGS` don't change.

  `python tests/test_regression_runner.py` on Windows then passes every test
- [ ] T007 [US1] Commit `tests/regression_runner.py`, `tests/test_regression_runner.py`, and this file with the message `test: force glibc's plain math path in every Linux render`. Don't push yet, because FR-004's local cross-check comes first
- [ ] T008 [US1] In WSL, build the commit fresh with `~/verify/kd5/build_fresh.sh ~/mandelbulber2 windows/deterministic-regression`, then run `python3 tests/test_regression_runner.py` in `~/mandelbulber2`. Every test passes. Record both systems' counts in `specs/004-baseline-parity-gate/evidence.md` (SC-010)
- [ ] T009 [US1] In `~/mandelbulber2`, run `python3 tests/regression_runner.py`. It prints the forced math path line, with this computer's CPU, flags, and own path, and then stops with a setup error because no reference set exists, with exit 3. Record the line in `specs/004-baseline-parity-gate/evidence.md` (FR-005, quickstart section 1)
- [ ] T010 [US1] In `~/verify/kd5/`, make three scratch copies of `~/mandelbulber2/tests/regression_runner.py`, each with one change:
  - `runner-unforced.py` leaves the setting out of `clean_environment()`.
  - `runner-misspelled.py` writes the tunable's name as `glibc.cpu.hwcap`.
  - `runner-changed.py` changes the last hexadecimal digit of the `sin` row's plain bits.

  From the root of `~/mandelbulber2`, run each in gate mode. The first two must stop with "the forced math path did not take effect", and the third with "glibc's plain math results changed". Each exits 3 and renders nothing. Record all three in `specs/004-baseline-parity-gate/evidence.md` (FR-002, SC-004)
- [ ] T011 [US1] Collect three times from the root of `~/mandelbulber2`:
  - The committed harness into `~/verify/kd5/collect-forced`.
  - `~/verify/kd5/runner-hidden.py`, a copy whose forced setting is replaced by the hidden setting, into `~/verify/kd5/collect-hidden`.
  - `~/verify/kd5/runner-unforced.py` into `~/verify/kd5/collect-natural`.

  Write `~/verify/kd5/compare_collections.py`. It imports `read_collection`, `read_reference`, and `compare_exports` from `tests/regression_runner.py`, and lists each item and buffer or statistic that differs between two collections. The forced and hidden collections must match for 487 of 487 items (SC-004). List what differs between the natural and forced collections (FR-005), and record both results in `specs/004-baseline-parity-gate/evidence.md`
- [ ] T012 [US1] On Windows, build T007's commit fresh in the new clone `C:\GIT\mb2-kd5-<first 9 characters>`, as Build Environments says, then work in that clone:
  - Run `python tests/test_regression_runner.py`, and run `python tests/regression_runner.py --collect tests/outputs/collection/msvc`.
  - Copy `\\wsl.localhost\Ubuntu-24.04\home\rpanc\verify\kd5\collect-forced` into `tests/outputs/collection/qmake`.
  - Run `python tests/regression_runner.py --cross-check tests/outputs/collection/qmake tests/outputs/collection/msvc`. It must exit 0 with functional agreement.

  Record the result and the count of identical items in `specs/004-baseline-parity-gate/evidence.md` (FR-004, SC-010). T011's forced collection must exist first
- [ ] T013 [US1] Fill in section 2 of `specs/004-baseline-parity-gate/evidence.md` from T008 to T012, and commit it with this file, with the message `docs: record the forced math path locally`

**Checkpoint**: The forced path is proven on this computer, and Linux and MSVC collections still cross-check. Nothing is pushed yet.

---

## Phase 4: User Story 2 - Capture Baseline 1 in CI (Priority: P1)

**Goal**: A push that creates `baseline/1` starts capture on a hosted runner. Its references are checked and committed on that branch (FR-006 to FR-010).

**Independent Test**: The capture run passes, and its artifact holds 26 scene zips, 461 formula zips, `toolchain.txt`, and a `settings.txt` whose fifth line is the `math_path` line, under 104,857,600 bytes.

- [ ] T014 [US2] Create `.github/workflows/capture-baseline.yml` with exactly the listing in `specs/004-baseline-parity-gate/contracts/capture-baseline.md`, and compare the two byte for byte
- [ ] T015 [US2] Commit `.github/workflows/capture-baseline.yml` and this file with the message `ci: add the Capture Baseline workflow`
- [ ] T016 [US2] Ask the user for the go-ahead, then push `deterministic-regression`, which reaches S1. Parity Gate and C++ CI Build (Qt6) must pass on the push. Record their run IDs for section 3 of `specs/004-baseline-parity-gate/evidence.md`
- [ ] T017 [US2] Ask for the go-ahead, then run `git push origin deterministic-regression:refs/heads/baseline/1`. Within 2 minutes, `gh run list --repo Coastrnaut/mandelbulber2 --workflow capture-baseline.yml --branch baseline/1` must show a run whose job is running.
  - **No run**: With the go-ahead, delete `baseline/1` on GitHub with `git push origin --delete baseline/1`. Make a local `baseline/1` at the same commit with one commit from `git commit --allow-empty -m "test: start the Baseline 1 capture"`, push it, and confirm the run (FR-007).
  - Record which start worked, for `specs/004-baseline-parity-gate/evidence.md`.
- [ ] T018 [US2] When the run finishes, its job must have passed in under 350 minutes. Its log must show the forced math path line and "wrote 487 references to tests/references/linux-x86_64-gcc", and Self-Parity must have passed at one thread and at four. Its `Search the Build Files` and `Reject Prohibited Compile Flags` steps must have passed too, so capture's build was checked (User Story 2, scenario 2). Record the CPU line, the CPU's own path, the step times, and the job's time for section 4 of `specs/004-baseline-parity-gate/evidence.md` (FR-005, FR-014, SC-003).
  - **If it failed**: Download `capture-failure-outputs`, find the cause, record it, and stop to tell the user (FR-010).
  - **Repeating**: With the go-ahead, a runner fault may be rerun from the run's page. Any other repeat deletes `baseline/1` on GitHub and pushes it again at the commit to capture.
- [ ] T019 [US2] Download the artifact to a new folder outside the repository with `gh run download <run ID> --repo Coastrnaut/mandelbulber2 -n baseline-references -D C:\GIT\mb2-kd5-baseline-1-artifact`.
  - The folder must hold exactly what the checking table in `specs/004-baseline-parity-gate/contracts/capture-baseline.md` lists, with "math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4" as the fifth line of `settings.txt`.
  - `toolchain.txt` must hold a `compiler` line, a `math_library` line, and a `package` line for the installed `gcc` and `g++` and for `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1` (FR-009).
  - Its total must be under 104,857,600 bytes.
  - Record the counts and the size (FR-008, FR-009, SC-001).
- [ ] T020 [US2] Commit the references on `baseline/1`:
  1. Run `git fetch origin baseline/1` in `C:\GIT\mandelbulber2`, then `git switch -c baseline/1 origin/baseline/1`, or `git switch baseline/1` if T017's fallback already made the branch here.
  2. Copy the artifact's `linux-x86_64-gcc` folder into `tests/references/`, and stage it.
  3. `git diff --cached --stat` must list 489 files, all under `tests/references/linux-x86_64-gcc/`. Commit them with the message `test: add Baseline 1 for linux-x86_64-gcc`.
  4. For every file, the blob ID in `git ls-tree -r HEAD tests/references` must equal `git hash-object --no-filters` of the downloaded copy (FR-015).
  5. Ask for the go-ahead, and push `baseline/1`. The Capture Baseline run for this push must show its job as skipped (FR-007).
  6. Run `git switch deterministic-regression`.
- [ ] T021 [US2] Fill in sections 3 and 4 of `specs/004-baseline-parity-gate/evidence.md` from T016 to T020, and commit it with this file, with the message `docs: record the Baseline 1 capture`. Ask for the go-ahead, and push `deterministic-regression`

**Checkpoint**: `baseline/1` holds Baseline 1, byte-identical to the artifact, and the gate still only checks flags.

---

## Phase 5: User Story 3 - Gate Every Change (Priority: P1)

**Goal**: `parity-gate` runs the harness's tests and the full gate, and uploads the outputs of a failure (FR-011, FR-012).

**Independent Test**: The gate passes on the Baseline 1 pull request in Phase 6, and fails on the drift proof in Phase 8.

- [ ] T022 [US3] Add the three steps to `.github/workflows/parity-gate.yml`, so the file is exactly the listing in `specs/004-baseline-parity-gate/contracts/parity-gate.md`, and compare the two byte for byte. Commit it with this file, with the message `ci: add the gate steps to Parity Gate`. Don't push it, because T024 does that right before the pull request opens (FR-013)

**Checkpoint**: The gate steps are committed and waiting for Baseline 1.

---

## Phase 6: User Story 4 - Land Baseline 1 as a Baseline Change (Priority: P1)

**Goal**: The references reach `deterministic-regression` through one pull request that holds only them, and the gate passes on it and on the push after it (FR-013 to FR-018).

**Independent Test**: The pull request changes only the 489 files, its description holds the nine parts, `parity-gate` passes on 487 of 487 items, and the push run after the merge passes.

- [ ] T023 [US4] Write the pull request's description in `C:\GIT\mb2-kd5-baseline-1-artifact\pr-body.md`, outside the repository. It has the nine parts in `specs/004-baseline-parity-gate/contracts/baseline-change.md`, in order, with no attribution lines. The before-and-after part takes FR-005's comparison from T011. Take the capture run's link, CPU line, and size from T018 and T019, the lines of `toolchain.txt` and the `math_path` line from the artifact, and the blob ID check from T020
- [ ] T024 [US4] When the user can review and merge the pull request in the same sitting, ask for the go-ahead to push the gate steps and open the pull request, then push `deterministic-regression`, which reaches S2. Its Parity Gate run must fail at "Run Parity Gate", as a setup error because no reference set exists. Note the run ID and the push time for section 5 of `specs/004-baseline-parity-gate/evidence.md`, which T027 fills in
- [ ] T025 [US4] Straight away, open the pull request with `gh pr create --repo Coastrnaut/mandelbulber2 --base deterministic-regression --head baseline/1 --title "Add Baseline 1 for linux-x86_64-gcc" --body-file C:\GIT\mb2-kd5-baseline-1-artifact\pr-body.md`.
  - **Files**: `gh pr view <number> --json files` must list only the 489 files under `tests/references/linux-x86_64-gcc/`.
  - **Checks**: `parity-gate` must pass on 487 of 487 items, with the "Run Parity Gate" step under 30 minutes, and the check's name must be `parity-gate`. Cross-Build Check, C++ CI Build (Qt6), Azure Pipelines, and CircleCI must pass.
  - **If `parity-gate` fails**: With the go-ahead, revert T022's commit on `deterministic-regression` by direct push, and close the pull request. Find and record the cause, then repeat from T017, with `baseline/1` deleted on GitHub and pushed again (FR-013).
- [ ] T026 [US4] The user merges the pull request, or tells Claude to, with a merge commit. The push run of `parity-gate` on the merge commit must pass on 487 of 487 items. If it fails, find and record its cause before anything else lands (FR-017). With the go-ahead, delete `baseline/1` on GitHub with `git push origin --delete baseline/1`, and delete it locally. This reaches S3
- [ ] T027 [US4] Fill in section 5 of `specs/004-baseline-parity-gate/evidence.md`:
  - The failing run from T024, and how long the window lasted, from that push to the merge.
  - The pull request, its gate step's time, its job's time, its CPU line, and the check name.
  - Cross-Build Check's result and count of identical items.
  - The merge commit, the passing push run, and the deleted branch.
  - That nothing else landed in the window: `git log --first-parent --format=%H <T022's commit>..<merge commit>` lists only the merge commit (SC-002).

  Commit it with this file, with the message `docs: record Baseline 1`. Don't push yet, because T029 uses this commit

**Checkpoint**: Baseline 1 is merged, which meets KD-4's closing condition. The amendment comes later.

---

## Phase 7: User Story 5 - Require the Gate (Priority: P2)

**Goal**: `parity-gate` is the only required check on `deterministic-regression`, and the user's direct pushes still land (FR-019 to FR-022).

**Independent Test**: The protection reads back as `parity-gate` alone, with `strict` and `enforce_admins` false, and a direct push lands with GitHub's bypass notice.

- [ ] T028 [US5] Ask for the go-ahead, then set the protection:
  - Write `C:\GIT\mb2-kd5-baseline-1-artifact\protection.json` with the JSON in `specs/004-baseline-parity-gate/contracts/parity-gate.md`.
  - Run `gh api -X PUT repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection --input C:\GIT\mb2-kd5-baseline-1-artifact\protection.json`.
  - Read it back with `gh api repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection`. The required checks must be `parity-gate` alone, the name T025 recorded, with `strict` false, `enforce_admins` false, no required reviews, and no restrictions. This reaches S4.

  Record the result in `specs/004-baseline-parity-gate/evidence.md`
- [ ] T029 [US5] Ask for the go-ahead, then push `deterministic-regression`, which carries T027's commit. The push must land, and GitHub must report the rule it bypassed. Save that output. Its Parity Gate push run must pass, and its gate step's time is recorded. Fill in section 6 of `specs/004-baseline-parity-gate/evidence.md`, and commit it with this file, with the message `docs: record the required check` (FR-020, SC-006)

**Checkpoint**: The gate is required, and direct pushes still work.

---

## Phase 8: User Story 6 - Prove the Gate Catches Drift (Priority: P2)

**Goal**: A pull request that changes one constant in `bristorbrot` fails the gate on that formula alone, and it never merges (FR-023).

**Independent Test**: The gate run on the seeded pull request fails on `formula bristorbrot` alone, and the merge state is `BLOCKED`.

- [ ] T030 [P] [US6] Do the drift proof's local check in WSL:
  1. Write `~/verify/kd5/drift.patch`, which changes `2.0 * z.x - z.z` to `2.001 * z.x - z.z` on line 36 of `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`.
  2. Build fresh with `~/verify/kd5/build_fresh.sh ~/mandelbulber2 windows/deterministic-regression` and `~/verify/kd5/build_fresh.sh ~/mandelbulber2-seed windows/deterministic-regression ~/verify/kd5/drift.patch`.
  3. In `~/mandelbulber2`, run `python3 tests/regression_runner.py --compare ~/mandelbulber2-seed/mandelbulber2/build/appdir/usr/bin/mandelbulber2`. It must exit 1, with `formula bristorbrot` the only item that differs.

  Record it in `specs/004-baseline-parity-gate/evidence.md`
- [ ] T031 [US6] Open the drift proof's pull request:
  1. In `C:\GIT\mandelbulber2`, run `git fetch origin` and `git switch -c seed/drift-proof origin/deterministic-regression`, so the branch holds none of the unpushed records, and make the same one-line change in `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`.
  2. Commit it with the message `test: change one constant in bristorbrot to prove the gate`.
  3. Ask for the go-ahead, push the branch, and open a pull request into `deterministic-regression`, titled "Do not merge: drift proof for the parity gate". Its body says it exists to show the gate rejecting drift, which is how it confirms compliance with the constitution, and that it must never merge, with no attribution lines (FR-023). `gh pr view <number> --repo Coastrnaut/mandelbulber2 --json files` must list only that formula file.
  4. Run `git switch deterministic-regression`.
- [ ] T032 [US6] When `parity-gate` finishes on that pull request, it must have failed with exit 1 on `formula bristorbrot` alone, and uploaded `render-regression-snapshots`. `gh pr view <number> --repo Coastrnaut/mandelbulber2 --json mergeStateStatus` must give `BLOCKED`. Other checks may fail too, and nothing waits for them. Record the run, the gate step's time, and the merge state in `specs/004-baseline-parity-gate/evidence.md` (SC-003, SC-005)
- [ ] T033 [US6] With the go-ahead, close the pull request unmerged with `gh pr close <number> --repo Coastrnaut/mandelbulber2`, delete `seed/drift-proof` on GitHub with `git push origin --delete seed/drift-proof`, and delete it locally. Fill in section 7 of `specs/004-baseline-parity-gate/evidence.md`, and commit it with this file, with the message `docs: record the drift proof`

**Checkpoint**: The gate is proven to catch drift, and no seeded change reached `deterministic-regression`.

---

## Phase 9: User Story 7 - Close KD-4 and KD-5 (Priority: P2)

**Goal**: The constitution closes both deviations at version 1.1.1, and the playbook, `CLAUDE.md`, and `README.md` match what was built (FR-024 to FR-027).

**Independent Test**: The constitution's table shows KD-4 and KD-5 closed at 1.1.1. The playbook's Phase 4 listings of the two workflows are byte-identical to the committed files.

- [ ] T034 [US7] Ask for the go-ahead, then push `deterministic-regression` with T029's and T033's commits, so the amendment can link `specs/004-baseline-parity-gate/evidence.md` as it stands on its base
- [ ] T035 [US7] Open the amendment:
  1. In `C:\GIT\mandelbulber2`, run `git fetch origin` and `git switch -c amend/kd-4-kd-5 origin/deterministic-regression`.
  2. In `.specify/memory/constitution.md`, add "**Closed <date>**, see `specs/004-baseline-parity-gate/evidence.md`" to the closing cells of KD-4 and KD-5, as KD-1 to KD-3 have. Set the version to 1.1.1 and the last amended date to the date.
  3. `git diff --stat` must list only the constitution. Commit it with the message `docs: close KD-4 and KD-5`.
  4. Pass the harness locally, as Workflow 2 requires. Build fresh with `~/verify/kd5/build_fresh.sh ~/mandelbulber2-amend windows/amend/kd-4-kd-5` and `~/verify/kd5/build_fresh.sh ~/mandelbulber2 windows/deterministic-regression`. Then, in `~/mandelbulber2`, `python3 tests/regression_runner.py --compare ~/mandelbulber2-amend/mandelbulber2/build/appdir/usr/bin/mandelbulber2` must exit 0.
  5. Ask for the go-ahead, push the branch, and open a pull request into `deterministic-regression`.
  6. Run `git switch deterministic-regression`.

  The pull request's body states the reason, the version change from 1.1.0 to 1.1.1, and the dates. It names the affected plans, those of features 001 to 004, and the affected gates, Workflow 2 and Principles I and II. It links the evidence, confirms the constitution's gates hold, and has no attribution lines (FR-024)
- [ ] T036 [US7] Every check on the amendment's pull request must pass, `parity-gate` included. The user merges it, which reaches S5. With the go-ahead, delete `amend/kd-4-kd-5` on GitHub and locally. Record T035's local compare, the pull request, its gate step's time, and the merge in section 8 of `specs/004-baseline-parity-gate/evidence.md`
- [ ] T037 [P] [US7] Correct `MIGRATION_PLAYBOOK.md` as research R12 lists:
  - Phase 4's steps 1 to 6, with both workflow listings as committed. Phase 3's listing of `parity-gate.yml` stays as Phase 3 built it.
  - Phase 5's steps 1, 4, and 5, which say that work lands on `deterministic-regression` by direct push.

  Then extract Phase 4's two listings from the playbook with a short script, and diff each against `.github/workflows/parity-gate.yml` and `.github/workflows/capture-baseline.yml`. Both must be identical (FR-025, SC-007)
- [ ] T038 [P] [US7] Correct `CLAUDE.md` (FR-026):
  - The auto-merge rule loses "no branch is protected, so no check is required". It says instead that `parity-gate` is the required check on `deterministic-regression`, which the user's direct pushes bypass, and that auto-merge stays off.
  - The CI section adds that a push which creates a `baseline/` branch runs Capture Baseline.
  - The rules against pull requests and side branches name the exceptions FR-026 lists: a Baseline Change, an amendment, and a drift proof, each from a branch deleted once its pull request closes or merges.
- [ ] T039 [P] [US7] Add the playbook appendix's section on continuous integration and regression testing to `README.md`. Make each statement true of what this feature built: the gate runs on pull requests and on pushes to `deterministic-regression`, it is required, and a contributor compares a change with a `deterministic-regression` build before pushing it, not before committing to a feature branch (FR-027)
- [ ] T040 [US7] Commit T036 to T039's changes with this file, with the message `docs: correct the playbook, CLAUDE.md, and README for the parity gate`. Ask for the go-ahead, and push `deterministic-regression`, which reaches S6

**Checkpoint**: KD-4 and KD-5 are closed, and every document matches what was built.

---

## Phase 10: Polish & Cross-Cutting Concerns

**Purpose**: Check the whole feature, and close the evidence

- [ ] T041 Run the final checks in section 9 of `specs/004-baseline-parity-gate/quickstart.md`:
  - **Files**: `git diff --stat <S0> deterministic-regression -- . ':!specs' ':!tests/references'` lists only the two workflows, the two harness files, the constitution, `MIGRATION_PLAYBOOK.md`, `CLAUDE.md`, and `README.md`, plus any fix FR-010 needed (SC-008).
  - **Master**: `git rev-parse origin/master` still equals T001's value.
  - **Branches**: `git ls-remote --heads origin` lists no `baseline/`, `seed/`, or `amend/` branch (SC-009).
  - **Settings**: Save T002's three records again into `specs/004-baseline-parity-gate/settings/after/`, and compare them with `settings/before/`, leaving out timestamps, counts, and commit IDs. Only the protection on `deterministic-regression` may differ: it joins the list of protected branches, and nothing else changes (FR-022, SC-006).
  - **Messages**: `git log <S0>.. --format=%B` holds no attribution line.
  - **Tests**: `python tests/test_regression_runner.py` passes.
- [ ] T042 Fill in sections 8 and 9 of `specs/004-baseline-parity-gate/evidence.md`, with one row for each of SC-001 to SC-010. Commit T041's records, the evidence, and this file with the message `docs: close the Baseline 1 evidence`. Ask for the go-ahead, and push `deterministic-regression`

---

## Requirement Coverage

| Requirement | Tasks |
|---|---|
| FR-001 | T005, T006, T008, T011 |
| FR-002 | T005, T006, T009, T010 |
| FR-003 | T005, T006, T019 |
| FR-004 | T005, T006, T008, T012 |
| FR-005 | T009, T011, T018, T027 |
| FR-006 | T014, T018 |
| FR-007 | T014, T017, T020 |
| FR-008 | T018, T019 |
| FR-009 | T019, T023 |
| FR-010 | T018 |
| FR-011 | T022 |
| FR-012 | T025, T032 |
| FR-013 | T022, T024, T025, T027 |
| FR-014 | T018, T025, T027, T032 |
| FR-015 | T020, T025 |
| FR-016 | T023, T025 |
| FR-017 | T026 |
| FR-018 | T026 |
| FR-019 | T028 |
| FR-020 | T028, T029 |
| FR-021 | Rules for Every Task, T029 |
| FR-022 | T002, T028, T041 |
| FR-023 | T030, T031, T032, T033 |
| FR-024 | T034, T035, T036 |
| FR-025 | T037 |
| FR-026 | T038 |
| FR-027 | T039 |
| FR-028 | T041 |
| FR-029 | T014, T022, T041 |
| FR-030 | T026, T033, T036, T041 |
| SC-001 | T019, T020, T026 |
| SC-002 | T025, T026, T027 |
| SC-003 | T018, T025, T026, T032 |
| SC-004 | T009, T010, T011, T018, T027 |
| SC-005 | T032, T033 |
| SC-006 | T028, T029, T041 |
| SC-007 | T036, T037, T038, T039 |
| SC-008 | T041 |
| SC-009 | T041 |
| SC-010 | T008, T012 |

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies. T002 must come before any change.
- **Foundational (Phase 2)**: Empty.
- **User Story 1 (Phase 3)**: Depends on Setup.
- **User Story 2 (Phase 4)**: Depends on User Story 1. T016 pushes the harness change only after T012's cross-check (FR-004).
- **User Story 3 (Phase 5)**: T022 depends on T020, so the gate steps come after Baseline 1 is captured.
- **User Story 4 (Phase 6)**: Depends on User Stories 2 and 3, and on a sitting in which the user can merge.
- **User Story 5 (Phase 7)**: Depends on User Story 4.
- **User Story 6 (Phase 8)**: T031 to T033 depend on User Story 5, because the merge must show as blocked by the required check. T030 can run earlier, once T026 is done.
- **User Story 7 (Phase 9)**: Depends on User Stories 4, 5, and 6 (FR-024). T037 to T039 come after T036, so the documents can name version 1.1.1.
- **Polish (Phase 10)**: Depends on everything above.

### Within Each Phase

- T005 comes before T006, and T006 before T007. T008 to T011 come after T007, in order, because they share `~/mandelbulber2`. T012 needs T011's forced collection, and T013 comes last.
- T014 comes before T015, T015 before T016, and so on to T021, in order.
- T024, T025, and T026 run in one sitting, in order, and nothing lands on `deterministic-regression` in between.
- T028 comes before T029.
- T031 comes before T032, and T032 before T033.
- T035 comes before T036, and T037 to T039 come before T040.

### Parallel Opportunities

- T003, in WSL, alongside T002.
- T012, on Windows, can start its build as soon as T007 is committed. Its collect and cross-check wait for T011.
- T030, in WSL, alongside T028 and T029.
- T037, T038, and T039, which change different files.
- Any local check may run while FractalEvaluator renders, because none of them times anything.

## Parallel Example: User Story 1

```text
Task: "Run the harness's tests and the probe checks in WSL, in ~/mandelbulber2"
Task: "Build MSVC Release fresh in C:\GIT\mb2-kd5-<commit>, for the cross-check"
```

## Parallel Example: User Story 7

```text
Task: "Correct MIGRATION_PLAYBOOK.md as research R12 lists"
Task: "Correct CLAUDE.md for the required check"
Task: "Add the regression section to README.md"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1.
2. Complete Phase 3.
3. **Stop and validate**: The forced path holds on this computer, and Linux and MSVC still cross-check.

The forced path alone settles KD-5's cause, but nothing records it with references yet, so KD-5 doesn't close until Baseline 1 merges and the amendment follows.

### Incremental Delivery

1. Phase 3 proves the forced path locally.
2. Phase 4 lands it with the capture workflow, and captures Baseline 1.
3. Phases 5 and 6 turn on the gate and merge Baseline 1 in one sitting.
4. Phase 7 makes the gate required.
5. Phase 8 proves it catches drift.
6. Phases 9 and 10 close the deviations and correct the documents.

Stop at each checkpoint. If a check fails, fix it in a new commit on that step before moving on.

### Where the User Is Needed

- **Go-aheads**: Every push, pull request, branch deletion, and the protection change, in T016, T017, T020, T021, T024, T025, T026, T028, T029, T031, T033, T034, T035, T036, T040, and T042.
- **Merges**: T026 and T036, and a sitting in which T024 to T026 can run together.

---

## Notes

- KD-4 and KD-5 stay open until the amendment merges in T036. Do not report them as passing before then.
- References reach `deterministic-regression` only through Baseline 1's pull request.
- `evidence.md` is where every result goes, so a reviewer can read them in one place.
- Phase 5 of the playbook, the upgrade loop, starts after this feature.
