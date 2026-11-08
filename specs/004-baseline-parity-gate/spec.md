# Feature Specification: Baseline 1 and the Parity Gate

**Feature Branch**: None. This work lands on `deterministic-regression` itself, and the only other branches are the ones FR-030 lists.

**Created**: 2026-11-08

**Status**: Draft

**Input**: User description: "Start playbook Phase 4: Baseline 1 and the GitHub Actions parity gate. Add the gate steps to .github/workflows/parity-gate.yml (test the harness, run the full gate, and upload failed outputs) and add a capture workflow that builds through the compile-engine action and runs tests/regression_runner.py --capture. Settle the CPU math path (KD-5) before capture: decide how the gate handles glibc choosing its math code path from the CPU's features, and record the decision with the references. Capture Baseline 1 in CI from a build that passes Self-Parity, record the exact compiler and math library versions with it, and land the references as a Baseline Change pull request that contains only reference files. Make parity-gate a required check on deterministic-regression, prove that the gate catches drift with a seeded formula change on a throwaway branch that is never merged, and amend the constitution to close KD-4 and KD-5. Work directly on deterministic-regression, and use pull requests only where the constitution requires them: the Baseline Change, the drift proof, and the amendment. The repository's default branch is master, which cannot hold harness work, so the capture workflow must be startable without changing master."

## Clarifications

### Session 2026-11-08

- Q: How should the gate handle the CPU math path (KD-5)? → A: Force one path. On Linux, the harness turns off glibc's FMA, FMA4, and AVX2 math paths for every render, and checks on each run that this took effect, so every x86-64 runner takes the same path. The Reference Target's definition stays as it is, so closing KD-5 is a PATCH amendment.
- Q: How should capture start while `master` is the default branch? → A: With a push of a branch made for it, such as `baseline/1`, which the Baseline 1 pull request also comes from. Later pushes to it don't capture again, and no repository setting changes.

## User Scenarios & Testing *(mandatory)*

Terms such as Standard Scene Suite, Reference Target, First Reference Target, Self-Parity, and Baseline Change are defined in the project constitution. KD numbers refer to its Known Deviations. The harness modes come from `specs/001-regression-harness`, and the `compile-engine` action and the `parity-gate` job come from `specs/003-strict-floating-point`. An item is one scene or one swept formula. There are 487 today: 26 scenes and 461 formulas.

- The **gated build** is the qmake release build of `mandelbulber-opencl.pro` on Linux x86-64 with GCC, which `compile-engine` makes. It belongs to the First Reference Target, whose references go in `tests/references/linux-x86_64-gcc/`.
- The **math path** is the code that glibc's math library picks, from the features of the CPU it runs on, for functions such as `sin`, `exp`, and `pow` (KD-5). The **forced path** is glibc's plain x86-64 math path, which FR-001 makes every Linux render take.
- A **direct push** is a push straight to `deterministic-regression`, with no pull request.
- The **maintainer** is the repository owner, `Coastrnaut`.

### User Story 1 - Settle the CPU Math Path (Priority: P1)

A maintainer knows that a gated result can't change with the runner's CPU, because every render the harness runs on Linux takes the forced path.

**Why this priority**: This is KD-5, and capture can't start until it is settled. One binary gives different `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `exp`, `log`, and `pow` results on CPUs whose features send glibc down different paths. Hosted runners don't all have the same CPU, so without this, a later run could report drift that no commit caused.

**Independent Test**: On this computer, render the suite on the forced path, then again with every optional x86-64 feature hidden from glibc, and confirm that nothing changes. Then stop the forcing from taking effect, and confirm that the gate and capture refuse to run.

**Acceptance Scenarios**:

1. **Given** any x86-64 CPU, **When** the harness renders on Linux, **Then** glibc takes the forced path, whatever the CPU offers.
2. **Given** a run on this computer with every optional x86-64 feature also hidden from glibc, **When** it renders the suite, **Then** every item's results match the forced run's exactly.
3. **Given** the forced path doesn't take effect, **When** the gate or capture runs, **Then** it stops as a setup error before it compares or writes anything.
4. **Given** Baseline 1, **When** a reader looks at its references, **Then** they record that they were made on the forced path, and the gate refuses a set made without it.
5. **Given** the harness's own tests, **When** they run, **Then** they cover the forcing, its check, and its record with the stand-in renderer.

---

### User Story 2 - Capture Baseline 1 in CI (Priority: P1)

A maintainer pushes a capture branch and gets a complete reference set from a hosted runner, made by a build that passes Self-Parity, with the toolchain that made it.

**Why this priority**: The gate has nothing to compare against until references exist, and the constitution requires them to come from a CI build that passes Self-Parity.

**Independent Test**: Push a capture branch, download what capture uploads, and check it against the harness's rules: one reference per item, the toolchain, settings, and math path records, and nothing else.

**Acceptance Scenarios**:

1. **Given** the capture workflow, **When** the maintainer pushes a branch made for it, such as `baseline/1`, **Then** capture runs on that commit on `ubuntu-24.04`, with no change to `master` or to the repository's settings.
2. **Given** a build file or the gated build holds a prohibited option, **When** capture starts, **Then** it stops before it renders, as `parity-gate` does.
3. **Given** the build passes Self-Parity at one thread and at all of the runner's threads, **When** capture finishes, **Then** its artifact holds a reference for every item and the toolchain, settings, and math path records, and nothing else.
4. **Given** the build fails Self-Parity, **When** capture runs, **Then** it uploads no reference set and the job fails.
5. **Given** a reference set over 100 MB, **When** capture runs, **Then** it refuses.
6. **Given** capture has finished, **When** another commit lands on its branch, such as the one that adds the references, **Then** capture doesn't run again.

---

### User Story 3 - Gate Every Change (Priority: P1)

A contributor opens a pull request, or the maintainer pushes, and `parity-gate` renders every item on a fresh build and fails if any buffer or statistic differs from Baseline 1.

**Why this priority**: This is the protection the constitution builds toward. After Baseline 1, every pull request must pass the gate before it merges.

**Independent Test**: Read the gate run on the Baseline 1 pull request, which must pass, and the one on the drift proof, which must fail.

**Acceptance Scenarios**:

1. **Given** references are present, **When** `parity-gate` runs, **Then** it searches the build files, builds, checks the flags, runs the harness's own tests, and then runs the gate on all 487 items.
2. **Given** every buffer and statistic matches its reference exactly, **When** the gate finishes, **Then** the job passes.
3. **Given** any difference, **When** the gate finishes, **Then** the job fails, names each item and each buffer or statistic that differs, and uploads the failed outputs, kept for 7 days.
4. **Given** no references, **When** the gate runs, **Then** it fails as a setup error and never passes.
5. **Given** the runner's toolchain differs from the one recorded with the references, **When** the gate runs, **Then** it says so, and if output differs too, it fails as a reference target mismatch.
6. **Given** a full gate run on the hosted 4-vCPU runner, **When** it finishes, **Then** the gate step took under 30 minutes.

---

### User Story 4 - Land Baseline 1 as a Baseline Change (Priority: P1)

A maintainer reviews one pull request that holds only reference files, sees that a CI build passing Self-Parity made them, and merges it.

**Why this priority**: KD-4 closes when Baseline 1 is merged, and the constitution lets references arrive only through a Baseline Change.

**Independent Test**: Read the pull request: its files, its description, and its checks.

**Acceptance Scenarios**:

1. **Given** the capture artifact, **When** it is committed, **Then** the pull request changes only files under `tests/references/linux-x86_64-gcc/`, and each is byte-identical to the artifact's copy.
2. **Given** the pull request, **When** `parity-gate` runs on it, **Then** it passes on all 487 items.
3. **Given** its description, **When** the maintainer reads it, **Then** it says that no earlier references exist, which replaces the usual before-and-after evidence, links the capture run whose Self-Parity passed, and names the toolchain and the math path.
4. **Given** the merge, **When** the push run of `parity-gate` on `deterministic-regression` finishes, **Then** it passes.
5. **Given** the gate steps land right before the pull request opens, **When** their push run fails for lack of references, **Then** that failure is expected and recorded, and nothing else lands until Baseline 1 merges.

---

### User Story 5 - Require the Gate (Priority: P2)

A contributor's pull request can't merge while `parity-gate` fails or hasn't run, and the maintainer can still push straight to `deterministic-regression`.

**Why this priority**: A CI job doesn't block a merge by itself. The maintainer works on `deterministic-regression` directly, so the rule has to keep those pushes working.

**Independent Test**: Read the branch's protection, look at the drift proof's merge box, and make one direct push.

**Acceptance Scenarios**:

1. **Given** a pull request whose `parity-gate` fails or hasn't reported, **When** anyone who can't bypass the rule tries to merge it, **Then** GitHub blocks the merge.
2. **Given** a direct push by the maintainer, **When** it lands, **Then** GitHub accepts it, and the push run of `parity-gate` checks it.
3. **Given** a direct push whose gate run fails, **When** the failure shows, **Then** it is treated as a defect in that change, and fixed or reverted before anything else lands.
4. **Given** the protection, **When** a reader checks the repository's settings, **Then** nothing else changed: no other required check, no required review, and no auto-merge.

---

### User Story 6 - Prove the Gate Catches Drift (Priority: P2)

A maintainer sees the gate fail on a change that alters one formula's output and nothing else.

**Why this priority**: A gate that only ever passes proves nothing, and the playbook makes this proof part of Phase 4.

**Independent Test**: Read the gate run on the seeded pull request.

**Acceptance Scenarios**:

1. **Given** one constant changed in a formula that the sweep renders and no scene uses, **When** `parity-gate` runs on its pull request, **Then** it fails, names that formula, and no other item fails.
2. **Given** that failure, **When** a reader opens the pull request, **Then** its merge shows as blocked, and the failed outputs were uploaded.
3. **Given** the proof is recorded, **When** it is done, **Then** the pull request is closed unmerged and its branch deleted.

---

### User Story 7 - Close KD-4 and KD-5 (Priority: P2)

A maintainer reads the constitution and sees both deviations closed, with the evidence behind them, and the playbook agrees with what was built.

**Why this priority**: Engine refactors can't begin until Baseline 1 exists, and plans must not report an open deviation as passing.

**Independent Test**: Read the constitution's table, the playbook's Phase 4, and the evidence they cite.

**Acceptance Scenarios**:

1. **Given** Baseline 1 is merged, the gate is required, and the drift proof is recorded, **When** the amendment merges, **Then** the constitution shows KD-4 and KD-5 closed, at version 1.1.1.
2. **Given** the playbook, **When** a reader checks Phase 4, **Then** it matches what was built, including how capture starts and both workflow listings.
3. **Given** `CLAUDE.md` and `README.md`, **When** a reader checks them, **Then** `CLAUDE.md` no longer says that no branch is protected, and `README.md` explains the gate to contributors.

### Edge Cases

- **The runner image changes between capture and the pull request**: GitHub typically updates `ubuntu-24.04` weekly. The gate reports a toolchain difference, and if output changes too, it fails as a reference target mismatch. Before Baseline 1 merges, that means capturing again.
- **The runner's CPU changes**: Hosted runners don't promise one CPU model, so capture and the gate can run on different ones. User Story 1 makes that harmless, because every render takes the forced path.
- **A new glibc**: An update can add a CPU-chosen path that the forcing doesn't turn off. The toolchain record changes with it, so the gate reports a reference target mismatch, not drift, and the forcing must be checked again before the next Baseline Change.
- **Capture and the gate build separately**: The gate builds its own binary from the same source, so a pass also shows that a fresh build reproduces the references. The gate steps change only a workflow file, so they can't change the binary.
- **Line endings**: A zip is binary, but `toolchain.txt` and `settings.txt` are text, and a line-ending conversion would make the gate refuse the set. Every file must reach the repository as the artifact holds it, whichever checkout commits it.
- **The red window**: Between the gate steps' push and Baseline 1's merge, every push to `deterministic-regression` fails the gate. FR-013 keeps that window short and empty.
- **A wrong required check name**: A required check that never reports blocks every pull request. The protection must name the check that `parity-gate` reports.
- **The maintainer's bypass**: GitHub lets the maintainer's direct pushes through by default, and it also offers the maintainer a way to merge a pull request whose gate failed. The drift proof must never use it.
- **The drift proof's formula**: It must be in the sweep, not in `tests/formula_exclusions.txt`, and no scene in `tests/scenes.txt` may use it, so exactly one item can fail.
- **An interrupted capture**: The harness writes a whole set or nothing, so a failed or cancelled run leaves no usable set, and the job fails.
- **The artifact's lifetime**: An artifact expires after its retention period, so the references are committed soon after capture.
- **Other checks on these pull requests**: Cross-Build Check, C++ CI Build (Qt6), Azure Pipelines, and CircleCI also run on each pull request. They stay as they are and are not required.

## Requirements *(mandatory)*

### Functional Requirements

**The CPU math path (KD-5)**

- **FR-001**: Before capture, every render the harness runs on Linux x86-64 MUST take the forced path. The harness turns off glibc's CPU-chosen math paths, such as its FMA, FMA4, and AVX2 paths, so the runner's CPU can't change a gated result.
- **FR-002**: The harness MUST confirm on every gate and capture run that the forced path took effect, and MUST stop with a setup error, never a pass, when it didn't.
- **FR-003**: The reference set MUST record that it was made on the forced path, and the gate MUST refuse a set made without it.
- **FR-004**: The harness's own tests MUST cover FR-001 to FR-003 with the stand-in renderer, and every test that passes today MUST still pass. Collect and cross-check MUST keep working on Linux and on Windows, so Cross-Build Check keeps passing. A push to `deterministic-regression` doesn't run Cross-Build Check, so a local collect and cross-check MUST show this before the harness change lands.
- **FR-005**: The evidence MUST record, for the capture run and for the Baseline 1 gate run, the runner's CPU model and the math features glibc saw. It MUST also say, from renders of a fresh build on this computer, which items give different results on glibc's FMA path and on the forced path, so a reader knows what forcing changes.

**Capture**

- **FR-006**: The repository MUST gain `.github/workflows/capture-baseline.yml`, whose job builds through the existing `compile-engine` action on `ubuntu-24.04`, runs `tests/regression_runner.py --capture`, and uploads `tests/references` as an artifact. It MUST fail when no reference set was written. It MUST have read-only repository access and use no secrets.
- **FR-007**: Capture MUST start when the maintainer pushes a branch made for it, named `baseline/<number>`, and only then. Another commit on that branch, such as the one that adds the references, MUST NOT capture again. Nothing about `master` or the repository's settings may change for capture to start.
- **FR-008**: Capture MUST keep the harness's rules: it runs only in CI, passes Self-Parity at one thread and at all of the runner's threads before it writes anything, writes a whole set or nothing, and stays under 100 MB.
- **FR-009**: The reference set MUST record the exact compiler and math library versions and the toolchain packages, as the constitution requires of Baseline 1. The harness's `toolchain.txt` already holds them.
- **FR-010**: If capture fails Self-Parity in CI, the cause MUST be found and recorded. An engine fix MUST land as its own reviewed change before capture repeats, and tolerances, exclusions, and skip lists MUST NOT hide the failure.

**The gate**

- **FR-011**: `.github/workflows/parity-gate.yml` MUST gain three steps after `compile-engine`: the harness's own tests, the full gate run, and an upload of `tests/outputs` when the job fails, kept for 7 days. Its triggers, access, runner, and time limit MUST stay as they are.
- **FR-012**: The gate MUST compare every buffer and every statistic of all 487 items exactly against Baseline 1, as the harness does, with no tolerance.
- **FR-013**: The gate steps MUST land on `deterministic-regression` after Baseline 1 is captured and right before its pull request opens, so the pull request's run uses them. The push run that fails for lack of references is expected, and it MUST be recorded once Baseline 1 merges. Nothing else may land on `deterministic-regression` in between.
- **FR-014**: A full gate run MUST finish in under 30 minutes on the hosted 4-vCPU runner, as SC-004 of `specs/001-regression-harness` requires. The evidence MUST record the time of the gate step and of the whole job.

**Baseline 1**

- **FR-015**: Baseline 1 MUST land as a Baseline Change: one pull request into `deterministic-regression`, from a branch made for it, that holds only the files capture wrote, under `tests/references/linux-x86_64-gcc/`, each byte-identical to the artifact's copy.
- **FR-016**: Its description MUST say why output is being recorded and that no earlier references exist, which replaces the before-and-after evidence. It MUST link the capture run whose Self-Parity passed, name the toolchain and the math path, and hold no attribution lines.
- **FR-017**: Its `parity-gate` run MUST pass on all 487 items, and so MUST the push run after it merges.
- **FR-018**: The maintainer MUST approve it by merging it. Auto-merge MUST NOT be used.

**Requiring the gate**

- **FR-019**: After Baseline 1 merges, `parity-gate` MUST become a required status check on `deterministic-regression`, under the name the job reports.
- **FR-020**: The maintainer's direct pushes MUST still land. Pull request merges, and pushes from anyone who can't bypass the rule, MUST need a passing gate.
- **FR-021**: A direct push whose gate run fails MUST be treated as a defect in that change, as Workflow 3 requires: it is fixed or reverted before anything else lands, and references are never regenerated to clear it.
- **FR-022**: No other repository setting may change: no other required check, no required review, no auto-merge, and no other protected branch. The default branch stays `master`.

**Proving the gate**

- **FR-023**: After the gate is required, a pull request into `deterministic-regression` from a throwaway branch MUST change one constant in one formula that the sweep renders and no scene uses. Its `parity-gate` run MUST fail on that formula alone and upload its outputs, and its merge MUST show as blocked. The pull request MUST then be closed unmerged and its branch deleted. No seeded change may be merged.

**Closing KD-4 and KD-5**

- **FR-024**: After FR-017, FR-019, and FR-023 are recorded, an amendment MUST close KD-4 and KD-5 in the constitution, citing the evidence. It is a PATCH, version 1.1.1, as the playbook says, because forcing the path leaves the Reference Target's definition as it is. It MUST state the reason, update the dates, and name the affected plans and gates. It lands as its own pull request that edits only the constitution, and a maintainer approves it.
- **FR-025**: Statements in `MIGRATION_PLAYBOOK.md` that this feature makes out of date MUST be corrected. These include how capture starts, the two workflow listings, which must match the committed files byte for byte, the math path step, the order of the gate steps and Baseline 1, and how direct pushes meet the required gate.
- **FR-026**: `CLAUDE.md` MUST no longer say that no branch is protected and no check is required.
- **FR-027**: Once the gate is required, `README.md` MUST gain the section on continuous integration and regression testing from the playbook's appendix, corrected to match what was built.

**Boundaries**

- **FR-028**: This feature MUST NOT change the engine, its build files, the scene list, the formula sweep, the formula exclusions, the render resolutions, or the render overrides. Apart from FR-001 to FR-004, it MUST NOT change the harness.
- **FR-029**: Apart from `parity-gate.yml` and the new capture workflow, no CI file may change: the other three workflows, the `build-linux`, `build-windows`, and `compile-engine` actions, `.appveyor.yml`, `azure-pipelines.yml`, `.circleci/config.yml`, and `.travis.yml`.
- **FR-030**: Work MUST land on `deterministic-regression` directly. The only other branches are the Baseline 1 branch, which also starts capture, the drift proof's branch, and the amendment's branch. Each MUST be deleted on GitHub once its pull request closes or merges.

### Decisions

**Every Linux render takes glibc's plain x86-64 math path.**

- **Why**: KD-5 closes when the path is settled and recorded. Forcing one path makes the runner's CPU irrelevant, so every x86-64 runner gives the same results, and the Reference Target's definition stays as it is, so the amendment is the PATCH the playbook expects.
- **Not chosen**: Recording the capture runner's path and refusing a runner that takes another. It keeps renders as users get them, but a runner on another path blocks merges, and naming CPU features in the Reference Target is a MINOR amendment. Recording the CPU only in the evidence, which leaves a false drift report possible and doesn't close KD-5.
- **Cost**: Gated renders use glibc's plain math, which may be a little slower, and their output can differ from what the same binary gives outside the harness on a modern CPU. FR-005 and FR-014 measure both.

**Capture starts with a push of its branch.**

- **Why**: GitHub starts a workflow by hand only from the default branch, which is `master`, and `master` can't hold harness work. A push of `baseline/<number>` needs no repository setting, and the Baseline Change's pull request comes from the same branch.
- **Not chosen**: Making `deterministic-regression` the default branch, a repository setting that also moves new pull requests, clones, the front page, and CodeQL's weekly scan. A tag, which stays in the repository unless it is deleted.

**Work on `deterministic-regression`, with pull requests only where they are needed.**

- **Why**: The maintainer works on `deterministic-regression` directly. The constitution requires a pull request for a Baseline Change and for an amendment. The gate runs only on pull requests and on pushes to `deterministic-regression`, so the drift proof needs a pull request that never merges.
- **Not chosen**: A pull request for every change, which the maintainer no longer wants. Putting the drift proof on `deterministic-regression` and reverting it, which puts a deliberate fault in its history and fails the gate for every push in between.

**The required gate lets the maintainer's pushes through.**

- **Why**: GitHub doesn't apply branch protection to admins by default, so direct pushes keep working. The gate still blocks every pull request merge, the drift proof's included, and its push run still checks every direct push.
- **Not chosen**: Applying the rule to admins too, which refuses every direct push, documents included, and forces a pull request for each.
- **Cost**: A direct push lands before the gate checks it, so drift shows after the fact. FR-021 makes that a defect to fix at once.

**The gate steps land last, right before Baseline 1.**

- **Why**: The gate fails until references exist. Landing its steps after capture keeps `deterministic-regression` red for minutes, not for the hours capture takes.
- **Not chosen**: Landing them first, which leaves every push red through the capture. Putting them in the Baseline 1 pull request, which may hold only reference files.

**The amendment edits only the constitution.**

- **Why**: Governance requires an amendment to be a pull request. The playbook, `CLAUDE.md`, and `README.md` are ordinary documents, so their corrections land directly.
- **Not chosen**: Putting every document in the amendment's pull request, which turns ordinary commits into a review.

### Key Entities *(include if feature involves data)*

- **Reference Set**: `tests/references/linux-x86_64-gcc/`, with one zip per item, `toolchain.txt`, `settings.txt`, and the math path record.
- **Capture Run**: One run of the capture workflow, started by a push of its branch, and the artifact it uploads.
- **Gate Run**: One `parity-gate` job that runs the full gate.
- **Math Path**: The code glibc's math library picks from the CPU's features. The harness forces the plain x86-64 path.
- **Toolchain Record**: The compiler, math library, and package versions kept with a reference set.
- **Drift Proof**: The seeded pull request that must fail the gate and never merge.
- **Required Check**: The branch protection rule that makes a pull request wait for `parity-gate`.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Baseline 1 is merged, with one reference for each of the 487 items and the toolchain, settings, and math path records, under 100 MB. Every file is byte-identical to the artifact of a CI capture run whose Self-Parity passed at one thread and at all of the runner's threads.
- **SC-002**: The Baseline 1 pull request changes only files under `tests/references/`, and its description holds everything FR-016 lists. Its gate run passes on 487 of 487 items, and so does the push run after its merge. No other commit lands between the gate steps' push and that merge.
- **SC-003**: A full gate run's gate step finishes in under 30 minutes on the hosted 4-vCPU runner, and the capture run finishes within its time limit. Both times are recorded.
- **SC-004**: On this computer, renders with every optional x86-64 feature hidden from glibc match the forced renders exactly for all 487 items, and a run whose forced path doesn't take effect never passes. The evidence records the CPU model and math features of the capture run and the Baseline 1 gate run, and which items differ between glibc's FMA path and the forced path.
- **SC-005**: The drift proof fails `parity-gate` on exactly 1 of 487 items, the seeded formula, its failed outputs are uploaded, and its merge shows as blocked. It is closed unmerged, and no seeded change reaches `deterministic-regression`.
- **SC-006**: `parity-gate` is the only required check on `deterministic-regression`, and a direct push by the maintainer after it becomes required still lands. No other repository setting changed, and no pull request used auto-merge.
- **SC-007**: The constitution shows KD-4 and KD-5 closed at the version FR-024 gives. The playbook's Phase 4 matches what was built, with both workflow listings byte-identical to the committed files. `CLAUDE.md` no longer says no branch is protected, and `README.md` holds the regression section.
- **SC-008**: Apart from the two workflows, the harness changes FR-001 to FR-004 allow, the reference set, the constitution, `MIGRATION_PLAYBOOK.md`, `CLAUDE.md`, `README.md`, and `specs/`, no file changes, and `master` doesn't change. Both workflows have read-only repository access and use no secrets, every harness test passes, and Cross-Build Check passes on the Baseline 1 pull request.
- **SC-009**: Every branch this feature pushes to GitHub is deleted once its pull request closes or merges.

## Assumptions

**Read on 2026-11-08**

- **Manual starts need the default branch.** GitHub's documentation says a workflow must be on the default branch before `workflow_dispatch` can start it, and that a run can then pick another branch's version. The default branch is `master`, which follows upstream and holds only `codeql.yml` and `github-actions-ci.yml`. So the playbook's "Run workflow" step can't work today, and the `workflow_dispatch` trigger that `cross-build-check.yml` already declares can't be used either.
- **Admins bypass branch protection by default.** GitHub's documentation says a branch protection rule doesn't apply to people with admin permissions unless "Do not allow bypassing the above settings" is on, and that required checks must pass before changes reach a protected branch. The maintainer owns the repository.
- **The repository** is a public fork with auto-merge off and no protected branch.
- **Parity Gate passes today.** Its runs on `b0458852e` and `bc38921a7` passed. A full build in `parity-gate` took 8 minutes 13 seconds in the second seeded run of `specs/003-strict-floating-point`.

**Found by reading the harness on 2026-11-08**

- **Capture** runs only when `CI` is `true`, needs a working thread control, and refuses on a platform where renders can't be isolated. Its Self-Parity renders each item 7 times: three times at all threads, once as a neutral render, and three times at one thread, where several items run at once, one per CPU. It writes the whole set or nothing, and refuses a set over 100 MB. The playbook expects about 2 hours.
- **The reference set** holds one zip per item, `toolchain.txt`, and `settings.txt`. `toolchain.txt` lists the compiler, the math library, and the installed versions of `gcc`, `g++`, `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1`. `settings.txt` lists the export format, both resolutions, and the render overrides, and the gate refuses a set whose settings differ.
- **The gate** renders each item once and compares it exactly. A missing reference set is a setup error. When the toolchain differs from the record, the gate says so, and a difference in output then counts as a reference target mismatch, exit 3, instead of a parity failure, exit 1. The Collect step, which also renders each item once, took 5 minutes 34 seconds and 10 minutes 28 seconds on two 4-vCPU runners in `specs/003-strict-floating-point`.
- **Renders get a clean environment**: `PATH`, `SYSTEMROOT`, `TEMP`, `TMP`, and `TMPDIR`, plus `HOME`, `LC_ALL`, and `LANG`. So a `GLIBC_TUNABLES` setting in CI never reaches a render today, and choosing a math path from the harness is a harness change.
- **The math path test** in `specs/003-strict-floating-point` used `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4,-AVX2_Usable,-FMA_Usable` to turn off glibc's FMA and AVX2 paths. Nine functions gave different results, and `sqrt` gave the same.

**Dependencies**

- `specs/001-regression-harness`, `specs/002-deterministic-rendering`, and `specs/003-strict-floating-point` are merged, and KD-1, KD-2, and KD-3 closed in version 1.1.0.
- The constitution requires a Baseline Change to be its own pull request, holding only references, made by a CI build that passes Self-Parity and approved by a maintainer. It requires every amendment to be a pull request that a maintainer approves. After Baseline 1, every pull request must pass the gate before it merges.
- GitHub doesn't let a pull request's author approve it, and the maintainer opens these pull requests, so the maintainer's merge is the approval, as it was for pull request 7.
- **Order of work**: FR-001 to FR-007 land first, then capture, then the gate steps and Baseline 1 (FR-013), then the required check, the drift proof, the amendment, and the documents.

**Scope**

- The gated scope stays the CPU engine on the First Reference Target. MSVC gets no references, and Cross-Build Check stays outside the gate, as FR-054 of `specs/001-regression-harness` requires.
- This feature treats glibc's math library as the only code chosen by the CPU at run time that reaches a gated render, as `specs/003-strict-floating-point` found. The build no longer uses the vector math library. The evidence records each runner's CPU, so a difference from anywhere else would show.
- Phase 5, the upgrade loop, starts after this feature.

**Environment and defaults**

- Every push needs the maintainer's go-ahead, and so does each repository setting this feature changes.
- Every local measurement comes from a fresh build of the commit being tested. No task may rely on an existing binary.
- Capture refuses outside CI, so Baseline 1 can only come from a hosted runner.
- The local measurement in FR-005 runs in the WSL `Ubuntu-24.04` distro on the Intel Core i9-14900K, whose CPU offers FMA and AVX2. It checks output, not time, so it can run while FractalEvaluator renders. The times this feature records come from CI.
