# Feature Specification: Deterministic CPU Rendering

**Feature Branch**: `upgrade/deterministic-rendering`

**Created**: 2026-11-05

**Status**: Draft

**Input**: User description: "Close KD-1 from the constitution by making CPU renders reproducible. First confirm the cause, because it is not yet proven: render threads share the unsynchronized global gRandomSeed in common_math.cpp, and Random() runs on every raymarch step. Replace that shared state so each random value derives only from a seed fixed in the scene or harness and from the pixel and sample position. Cover every CPU caller of Random(): render_worker, dof, light, ssao_worker, shader_ambient_occlusion, shader_aux_shadow, shader_global_illumination, shader_specular_highlight, and shader_volumetric. Audit the OpenMP regions in dof and post_effect_hdr_blur for order-dependent results. Make the render statistics exact: all render threads update one shared statistics object with no synchronization, and identical renders reported total iterations that differed by about 30 percent. Total iterations, raymarch count, missed DE, and both histograms must give the same values at every thread count, without changing rendered output. Give --nogui a working thread-count control that also governs those OpenMP regions, because limit_CPU_cores is only applied by the GUI startup path and the queue. Success means --self-parity passes for all engine buffers and statistics at one thread and at all available threads."

## Clarifications

### Session 2026-11-06

- Q: Should each animation frame get its own noise? → A: Yes. The frame number joins each random value's position, so noise changes between frames as it does today, rendering one frame again gives the same pixels, and a still render counts as frame 0.
- Q: Where should the render seed live? → A: In one scene setting with a fixed default. No scene or harness edit is needed, plain and gated renders use the same seed, and the GUI does not change.
- Q: What limit should the noise quality check use? → A: Noise may rise by at most 5%. A drop passes, but a drop above 5% needs a written reason in the pull request.
- Q: Where should `--threads` apply? → A: In every headless CPU render mode, where it beats `limit_CPU_cores` in the queue. It is refused in the GUI, with `--gpu` or `--gpuall`, and with options that render nothing.
- Q: How much of its parallel speed must the depth of field fix keep? → A: None for now. The blend runs on one thread, and a parallel version with the same order can follow Baseline 1 as an ordinary change.

## User Scenarios & Testing *(mandatory)*

Terms such as Standard Scene Suite, Reference Target, Self-Parity, and Baseline Change are defined in the project constitution. KD numbers refer to its Known Deviations. The harness modes, the formula sweep, and the verification export come from `specs/001-regression-harness`. An item is one scene or one swept formula. There are 487 today: 26 scenes and 461 formulas.

### User Story 1 - Choose the Thread Count for a Headless Render (Priority: P1)

A developer starts a headless render with a chosen number of threads, and every parallel stage of that render uses that number.

**Why this priority**: The rest of this feature needs it. The cause cannot be confirmed without one-thread renders, and Self-Parity needs two thread counts. Today `limit_CPU_cores` has no effect under `--nogui`, because only the GUI startup path and the queue apply it, and nothing in the engine sets the OpenMP thread count.

**Independent Test**: Render a scene that reaches screen-space ambient occlusion, post-effect depth of field, and HDR blur at one thread, then at four. The first uses about one CPU's worth of time, and the second uses more.

**Acceptance Scenarios**:

1. **Given** `--nogui --threads 1`, **When** a scene renders, **Then** the render workers, screen-space ambient occlusion, post-effect depth of field, and HDR blur each run on one thread.
2. **Given** `--nogui --threads 4`, **When** the same scene renders, **Then** each of those stages uses up to 4 threads, whatever the computer's CPU count.
3. **Given** no thread option, **When** a scene renders, **Then** it uses every logical CPU, as it does today.
4. **Given** a thread count of 0, a negative number, or text, **When** the application starts, **Then** it exits with a non-zero status before rendering and names the option.
5. **Given** the option is used in the GUI, with `--gpu` or `--gpuall`, or with an option that renders nothing, **When** the application starts, **Then** it refuses the option and does not ignore it.
6. **Given** the GUI, **When** a user sets `limit_CPU_cores` in preferences, **Then** it works as it does today.
7. **Given** any other headless CPU render mode, such as an animation or the queue, **When** it runs with `--threads 2`, **Then** it uses 2 threads, whatever `limit_CPU_cores` says.

---

### User Story 2 - Confirm the Cause Before Replacing Anything (Priority: P1)

A developer gathers evidence for why renders differ before changing how random values are made.

**Why this priority**: KD-1 records the shared seed as the leading suspect and says the cause is not proven. Reading the code on 2026-11-05 found two more candidates that no seed can explain (see Assumptions), so a fix aimed only at the seed could leave renders still differing.

**Independent Test**: Read the evidence record alone. For each tested scene, it says whether one-thread renders reproduce, and which buffers and statistics still differ once random values no longer depend on thread order.

**Acceptance Scenarios**:

1. **Given** a fresh build with only the thread control added, **When** each tested scene renders three times at one thread, **Then** the evidence records whether every buffer and statistic matched.
2. **Given** a diagnostic build whose random values do not depend on thread order, such as one that holds them constant, **When** the same scenes render at all threads, **Then** the evidence records which buffers and statistics still differ.
3. **Given** the finished evidence, **When** a reviewer reads it, **Then** it names each cause found, the scenes that show it, and what it affects.
4. **Given** evidence that contradicts the suspected cause, such as one-thread renders that differ, **When** the work continues, **Then** the cause found joins the work, and the shared seed is still replaced, because Principle II forbids it either way.
5. **Given** the diagnostic build, **When** this feature merges, **Then** none of its diagnostic changes are in the repository.

---

### User Story 3 - Render the Same Values at Any Thread Count (Priority: P1)

A developer renders a scene any number of times, at any thread count, and gets the same value in every engine buffer.

**Why this priority**: This is KD-1 and Principle II. Until renders reproduce, a parity failure cannot be told apart from noise, and no baseline can be accepted.

**Independent Test**: Render one Monte Carlo scene and one post-effect depth of field scene several times, at one thread and at all threads, and compare every buffer.

**Acceptance Scenarios**:

1. **Given** a scene, **When** it renders twice at one thread count, **Then** every value in every buffer matches.
2. **Given** a scene, **When** it renders at one thread and at all threads, **Then** every value in every buffer matches.
3. **Given** a Monte Carlo scene whose pixels stop sampling early, **When** it renders at different thread counts, **Then** each pixel stops at the same sample.
4. **Given** a scene with post-effect depth of field, **When** it renders at different thread counts, **Then** the post-effect image matches, even where blurred pixels overlap.
5. **Given** a scene on any other stochastic path, such as global illumination, soft shadows, multi-ray or random screen-space ambient occlusion, volumetric fog, or a beam light, **When** it renders at different thread counts, **Then** every buffer matches.
6. **Given** a scene that names a seed, **When** it renders with another seed, **Then** its noise differs, and each seed reproduces its own result.
7. **Given** a scene that names no seed, **When** it renders, **Then** it uses the fixed default, with no edit to the scene file.

---

### User Story 4 - Report Exact Statistics (Priority: P1)

A developer reads the same statistics for a finished render at any thread count.

**Why this priority**: Self-Parity includes statistics, so KD-1 cannot close without them. All render threads update one shared statistics object with no synchronization, and identical renders report different counts.

**Independent Test**: Render one scene at one thread and at all threads, and compare its statistics record line by line.

**Acceptance Scenarios**:

1. **Given** a finished render, **When** it ran at any thread count, **Then** total iterations, raymarch count, missed DE, the iteration histogram, and the step histogram hold the same values.
2. **Given** any item, **When** it finishes, **Then** total iterations reports the real count and not 0.
3. **Given** two threads reach the same line, **When** the render finishes, **Then** each pixel counted once.
4. **Given** more threads than image lines, **When** the render finishes, **Then** the statistics match the one-thread render.
5. **Given** two fresh builds, one before the statistics work and one after it, **When** compare mode runs, **Then** no buffer differs.
6. **Given** a render in progress, **When** a user watches the GUI's statistics dock or `--stats`, **Then** the values keep updating.

---

### User Story 5 - Pass Self-Parity and Close KD-1 (Priority: P1)

A developer runs `--self-parity` on a fresh build, and it passes.

**Why this priority**: This is the feature's definition of success, and the constitution closes KD-1 on it. It also makes SC-003, SC-007, and SC-010 of `specs/001-regression-harness` come true.

**Independent Test**: Build the branch fresh on Linux and run `--self-parity`.

**Acceptance Scenarios**:

1. **Given** a fresh Linux build, **When** `--self-parity` runs, **Then** every item passes for every buffer and statistic at one thread and at all available threads, and it exits 0.
2. **Given** the harness's thread setting is enabled, **When** `--self-parity` runs, **Then** it no longer says thread coverage is incomplete.
3. **Given** the harness's thread setting is enabled, **When** `--capture` runs, **Then** thread coverage no longer stops it, and this feature still creates no references.
4. **Given** a seeded difference in one sample of one buffer, **When** the harness compares it, **Then** it still fails, so passing did not come from a weaker comparison.
5. **Given** Self-Parity passes, **When** this feature merges, **Then** the constitution shows KD-1 closed.

---

### User Story 6 - Keep Everything Else Working (Priority: P2)

A user or developer sees no loss from this change: images look as they did, and every build, test, and mode that works today still works.

**Why this priority**: Reproducible output has no value if it brings visible patterns or breaks another mode. The random values sit on the render's hottest path, so a poor replacement could cost quality or speed.

**Independent Test**: Compare Monte Carlo renders from before and after this change, and run the application's own tests and Cross-Build Check.

**Acceptance Scenarios**:

1. **Given** the Monte Carlo scenes, **When** they render before and after this change, **Then** noise rises by no more than SC-010 allows, and no new pattern appears.
2. **Given** fresh qmake and MSVC builds, **When** the application's `--test` suite runs, **Then** it passes on both.
3. **Given** a pull request, **When** Cross-Build Check runs, **Then** it passes.
4. **Given** the OpenCL build, **When** it compiles, **Then** OpenCL rendering and its depth of field work as they do today.
5. **Given** render time on the Standard Scene Suite, **When** it is measured before and after, **Then** the cost is recorded.
6. **Given** a Monte Carlo scene in an animation, **When** two frames render, **Then** their noise differs, and rendering either frame again gives the same pixels.

### Edge Cases

- **More threads than image lines**: The formula sweep renders 24 lines, so on a 32-thread computer every thread starts on the first line. Its buffers and statistics must still match the one-thread render.
- **A line reached by two threads**: A thread that starts late can take over a line another thread has begun or finished. Each pixel must still count once.
- **More threads than CPUs**: `--threads` accepts any count of at least 1, because the harness asks for 2 threads on a one-CPU computer.
- **Optional outputs**: Turning the normal buffer or the verification export on or off must not shift any random value. The harness's neutral render checks this.
- **Two-pass stereo**: Statistics still describe the second pass only, the limit `specs/001-regression-harness` documents, and they must still be exact.
- **Animation**: Each frame gets its own noise from its frame number, as it does today, and rendering one frame twice gives the same pixels. Animation stays out of the gate.
- **A render cut short**: A render stopped by a time limit or a stop request depends on time, so it is not covered. Gated renders set no time limit.
- **GUI and network rendering**: Progressive rendering and network clients hand out pixels differently from a headless render. Neither is gated, and both must keep working.
- **Two builds**: Random values match on every build, but pixels can still differ, because compilers, math libraries, and the shuffle behind Perlin noise differ. That difference is recorded and is not drift.

## Requirements *(mandatory)*

### Functional Requirements

**Thread control**

- **FR-001**: Every headless CPU render mode MUST accept `--threads N`, with N a whole number of at least 1 given as the next argument. These modes are still renders, keyframe and flight animations, the queue, voxel export, network rendering, `--test`, and `--benchmark`.
- **FR-002**: `--threads` MUST set the thread count of every parallel stage a CPU render can reach: the render workers, screen-space ambient occlusion, the OpenMP regions in post-effect depth of field and HDR blur, and the OpenMP regions in voxel export. `OMP_NUM_THREADS` in the environment MUST NOT override it.
- **FR-003**: `--threads` MUST accept and use counts above the number of logical CPUs.
- **FR-004**: Without `--threads`, a render MUST use every logical CPU, as it does today. `limit_CPU_cores` MUST keep working as it does today in the GUI and the queue, and in the queue `--threads` MUST take precedence over it.
- **FR-005**: An invalid count MUST stop the application before rendering, with a non-zero exit status and a message that names the option. In the GUI, with `--gpu` or `--gpuall`, and with options that render nothing, such as `--list` and `--touch`, the application MUST refuse the option the same way and MUST NOT ignore it.
- **FR-006**: `--help` MUST describe `--threads`.

**Confirming the cause**

- **FR-007**: Before the shared seed is replaced, the work MUST record evidence from fresh builds of two things: whether one-thread renders reproduce today, and what still differs at all threads once random values do not depend on thread order.
- **FR-008**: The evidence MUST cover both scenes named in KD-1 and one scene for each stochastic path in the Standard Scene Suite, and it MUST compare every buffer and statistic.
- **FR-009**: The evidence MUST name each cause found, the scenes that show it, and what it affects, and it MUST state the measured spread of each statistic.
- **FR-010**: If the evidence contradicts the suspected cause, the cause found MUST join this work. The shared seed MUST still be replaced, because Principle II forbids random values that depend on shared mutable state.
- **FR-011**: Diagnostic changes used for the evidence MUST NOT be merged.

**Random values**

- **FR-012**: Every random value in a gated CPU render MUST derive only from the render seed and that value's position in the render. In the render workers, the position is the pixel, the sample within that pixel, and the value's place in that sample's sequence. In a post effect, it is the effect's own position, such as the pixel and ray for screen-space ambient occlusion, or the pass and entry for depth of field. In an animation, the frame number is part of the position, and a still render counts as frame 0.
- **FR-013**: FR-012 MUST cover all 27 CPU calls to `Random()`, in `render_worker`, `dof`, `light`, `ssao_worker`, `shader_ambient_occlusion`, `shader_aux_shadow`, `shader_global_illumination`, `shader_specular_highlight`, and `shader_volumetric`.
- **FR-014**: Thread identity, thread count, line order, wall-clock time, process IDs, and memory addresses MUST NOT feed any random value.
- **FR-015**: The shared `gRandomSeed` MUST be removed, so no code can reach it. The two callers outside the gated path, OpenCL depth of field and the GUI's view reset in `interface.cpp`, MUST keep working with their own local state.
- **FR-016**: The render seed MUST be a scene setting with one fixed default. A scene that names no seed MUST use that default, with no edit to the scene file and no new harness override.
- **FR-017**: Each caller MUST keep the range of values it uses today, so shading math does not change.
- **FR-018**: The same seed and position MUST give the same random value on every build, so a difference between builds never starts in the random values.
- **FR-019**: Random values MUST stay free of visible patterns. Neighboring pixels and samples MUST NOT share their noise.
- **FR-020**: Random lights, clouds, and material coloring already have their own seeds, and they MUST stay as they are.

**Order-dependent stages**

- **FR-021**: The work MUST audit each OpenMP region a CPU render can reach, and it MUST record for each one whether its result depends on thread order, with a suite scene that reaches it. There are three today: two in `dof` and one in `post_effect_hdr_blur`.
- **FR-022**: A region whose result depends on order MUST give the same result at every thread count, and two threads MUST NOT update one pixel at the same time. The blend in post-effect depth of field MUST do this by running on one thread, in one fixed order. A region whose result does not depend on order MUST stay parallel.
- **FR-023**: Any other dependence on thread count, scheduling, or elapsed time that Self-Parity exposes MUST be found, fixed, and recorded. Tolerances, exclusions, and skip lists MUST NOT be used to hide a difference.

**Statistics**

- **FR-024**: For a finished render, total iterations, raymarch count, missed DE, the iteration histogram, and the step histogram MUST hold the same values at every thread count.
- **FR-025**: Each pixel MUST count exactly once in those values, however lines reach threads, including a line that one thread abandons or a second thread renders again.
- **FR-026**: Total iterations MUST report the real count on the current CPU path, where it reads 0 today. Playbook Phase 3 places this fix in this change.
- **FR-027**: Two threads MUST NOT update one statistics counter at the same time. This includes rendered pixels, Monte Carlo repeats, and noise, which the verification export does not record.
- **FR-028**: The statistics work MUST NOT change any buffer.
- **FR-029**: The GUI's statistics dock and `--stats` MUST keep updating during a render. Only the values of a finished render need to be exact.
- **FR-030**: The verification export's format MUST NOT change.

**Harness and closing KD-1**

- **FR-031**: The harness's thread setting MUST be set to `--threads`. That MUST be the only change to how the harness behaves, and its own tests MUST keep covering the case with no thread control.
- **FR-032**: Self-Parity MUST pass for every item, every buffer, and every statistic, at one thread and at all available threads, on a fresh Linux build.
- **FR-033**: This feature MUST NOT create or commit references. Baseline 1 belongs to playbook Phase 4.
- **FR-034**: Closing KD-1 MUST mark it closed in the constitution as a PATCH amendment, version 1.0.1, stating the reason, as Governance requires. Statements in `MIGRATION_PLAYBOOK.md` that this feature makes out of date MUST be corrected.

**Boundaries**

- **FR-035**: The pull request MUST state the expected effect on output, as the constitution requires of every change before Baseline 1: every scene's pixels change, because raymarch step jitter applies to every scene; images should look the same apart from noise; and total iterations changes from 0 to the real count.
- **FR-036**: Apart from the thread control, the random values, the order-dependent stages, the statistics, and anything FR-023 exposes, this feature MUST NOT change engine behavior. Formulas, shading math, default settings, and compile flags stay as they are.
- **FR-037**: Every build that compiles the engine today MUST still compile, with and without OpenCL, and the application's `--test` suite MUST keep passing on the qmake and MSVC builds.
- **FR-038**: This feature MUST NOT add or change a workflow. Cross-Build Check and the workflows that pass today MUST keep passing.
- **FR-039**: CPU render time on the Standard Scene Suite MUST be measured before and after this change and recorded with it, and a full gate run MUST stay within SC-004 of `specs/001-regression-harness`. The time of the depth of field stage on `IFS_002.fract` at 1920x1080 MUST be recorded the same way.

### Decisions

**Thread control: a new `--threads` option, not `limit_CPU_cores`.**

- **Why**: `limit_CPU_cores` is an application setting whose maximum is the CPU count, so it cannot give the two thread counts the harness asks for on a one-CPU computer. The harness's stand-in renderer and tests already use `--threads`, and the harness sends it as an option followed by a value.
- **Not chosen**: Applying `limit_CPU_cores` under `--nogui`. That leaves the maximum in place, and a saved per-user setting would start changing headless renders.

**Seed: one scene setting with a fixed default.**

- **Why**: The constitution requires a seed fixed in the scene file or the harness. A scene setting meets that the way `clouds_random_seed` and `random_lights_random_seed` do, existing scenes load with no edit, and a plain `--nogui` render gives the same pixels as a gated one. Where a scene names no seed, the default is part of the binary, which Principle II already counts as an input.
- **Not chosen**: A seed that only the harness passes. It needs a harness change that SC-003 of `specs/001-regression-harness` rules out, and ungated renders would use a different seed from gated ones.

**Animation: the frame number is part of each value's position.**

- **Why**: Noise changes from frame to frame today, and FR-036 keeps other behavior as it is. Pinning the noise in place over a moving image would be a visible change for animators. The frame number comes from the animation and not from time, so one frame still reproduces exactly, and a still render is unaffected.
- **Not chosen**: The same seed for every frame, which is the request's literal wording. Noise would stay still, and only a hand-edited settings file could change that, because the GUI adds a setting to an animation through its control and the seed has none.

**Depth of field blend: one thread for now.**

- **Why**: One thread gives the fixed order with the smallest engine change before Baseline 1. A parallel version with the same order gives identical pixels, so it can follow as an ordinary change that the gate proves.
- **Effect**: The measured times are under Assumptions. With the scene's own blur, the stage gets faster on the 32-thread computer measured. At 4 to 16 threads it gets slower, most of all with a large blur.
- **Not chosen**: Keeping the blend parallel in this change, which adds new engine code to the stage this change makes exact.

### Key Entities *(include if feature involves data)*

- **Thread Count**: The number of threads a render uses at every parallel stage, set by `--threads` or defaulting to every logical CPU.
- **Render Seed**: The fixed number every random value in a render derives from. It is a scene setting with one default.
- **Random Position**: What identifies one random value within a render: the pixel, sample, and place in sequence, or a post effect's own position, along with the frame number in an animation.
- **Cause Evidence**: The record of what differed between renders before the fix, and why, per tested scene.
- **Order Audit**: One verdict per OpenMP region on whether its result depends on thread order, with the scene that reaches it.
- **Render Statistics**: The exact values of a finished render: total iterations, raymarch count, missed DE, and the two histograms.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: `--self-parity` passes for all 487 items, across all seven buffers and all statistics, at one thread and at all available threads, on a fresh Linux build, in three full runs in a row.
- **SC-002**: At 2, 3, 4, and 7 threads, and at twice the CPU count, every item's buffers and statistics match its one-thread render.
- **SC-003**: Every item reports total iterations above 0, and the value matches at every thread count.
- **SC-004**: Compare mode reports 0 buffer differences across all items between fresh builds from before and after the statistics work.
- **SC-005**: With `--threads 1`, CPU time stays within 110% of elapsed time on a scene that spends most of its time in each parallel stage, and in each headless CPU render mode.
- **SC-006**: Each invalid `--threads` value, and each use of the option where FR-005 refuses it, fails before rendering in 100% of trials.
- **SC-007**: The evidence names a cause for every difference seen before the fix, and none is left unexplained.
- **SC-008**: Every audited region has a recorded verdict and a suite scene that reaches it.
- **SC-009**: On all 6 Monte Carlo scenes, two different seeds give different output, two different frame numbers give different output, and each seed and frame number reproduces its own output exactly.
- **SC-010**: On all 6 Monte Carlo scenes, noise rises by at most 5% from its level before this change, the pull request explains any drop above 5%, and a reviewer finds no new pattern in the final images. Noise is the average absolute difference between the final images of two renders of a scene: two runs before this change, and two seeds after it.
- **SC-011**: The harness's own tests pass, and a one-bit difference seeded in any buffer still fails.
- **SC-012**: All 8 `--test` tests pass on fresh qmake and MSVC builds, and Cross-Build Check passes.
- **SC-013**: A `--self-parity` run on a fresh MSVC build reports 0 differences, although the harness cannot call it a pass there.
- **SC-014**: The render time of the Standard Scene Suite and the depth of field time of `IFS_002.fract` at 1920x1080 are recorded before and after, and a full gate run stays under 30 minutes on a 4-vCPU CI runner.
- **SC-015**: A search of the source finds no remaining use of the shared seed.
- **SC-016**: The constitution shows KD-1 closed at version 1.0.1, and no file exists under `tests/references/`.

## Assumptions

**Found by reading the code on 2026-11-05**

None of these has been measured. User Story 2 and the audit must confirm or correct them.

- **Post-effect depth of field blends in thread order.** The OpenMP region in phase II blends overlapping discs into the shared image. A blend gives a different result in a different order, and two threads can update one pixel at once, so the result depends on scheduling. Any scene with post-effect depth of field reaches it, including `IFS_002.fract` in the suite.
- **The other two regions look safe.** Phase I of depth of field and HDR blur each read pixels that do not change during the region and write only their own pixel, so neither is expected to depend on order.
- **Two threads can render one line.** Each thread claims its start line with no lock and no check. A thread that starts late takes that line from whoever holds it, even if it is finished, so the line's pixels render and count twice. When threads outnumber lines, each thread's start line is the first line. Statistics can therefore differ even with no lost updates.
- **The 30 percent figure cannot be repeated on the current source.** The request says identical renders reported total iterations about 30 percent apart. That figure entered the playbook on 2026-11-04, probably from the July 2026 build used for most measurements that day. On builds with the verification export, total iterations read 0 for the three scenes measured that day. The spread measured was in raymarch counts: for `IFS_001.fract`, a 76,800-pixel image, 76,675 and 76,733 on the WSL build and 76,469 on the MSVC build. FR-009 records the real spread.

**Dependencies**

- `specs/001-regression-harness` is merged, and this feature uses its modes, its Standard Scene Suite, and its verification export.
- **Order of work.** The thread control comes first, because the evidence needs it. The evidence comes before the seed is replaced. FR-028 needs renders that reproduce, so the statistics work follows the random values and the order-dependent stages.
- KD-2 and KD-3 stay open. Self-Parity compares one binary with itself, so compile flags do not block it. `upgrade/strict-floating-point` follows as a separate change.
- Changed output is not drift here, because no references exist yet (KD-4). Baseline 1 follows both changes.

**Scope**

- The gated scope is still renders on the CPU engine. Animation, voxel export, network rendering, and OpenCL stay outside it.
- OpenCL kernels keep seeding with `std::rand()`, because the constitution does not gate OpenCL.
- Nothing in this feature requires a GUI control for the seed.
- `parity-gate` and the CI flag check belong to the floating-point change and Phase 4, not this feature.

**Environment and defaults**

- Every measurement comes from a fresh build of the commit being tested. No task may rely on an existing binary.
- Self-Parity can only report a pass on Linux, because the application ignores a redirected home folder on Windows. The local proof is a fresh qmake build in the WSL `Ubuntu-24.04` distro on 32 threads.
- No workflow runs Self-Parity until Phase 4, so hosted runners do not prove it here. SC-002 includes 4 threads, the count those runners have.
- A gate cannot run until references exist, so the Collect step of Cross-Build Check, which renders the same items, stands in for the gate time in SC-014. It took 3.8 to 6.1 minutes before this change.
- Moving the depth of field blend to one thread changes that stage's speed. On 2026-11-06, a fresh WSL build timed the whole stage on `IFS_002.fract` at 1920x1080, with `OMP_NUM_THREADS` standing in for the missing thread control. At the scene's own blur radius of 19.72, it took 2.9 seconds on 1 thread, 1.0 on 4, 0.8 on 16, and 24.8 on all 32. At radius 100, it took 34.3, 10.2, 4.3, and 25.0. The one-thread times are an upper limit for this fix, because phase I stays parallel. Nothing explains the 32-thread times yet, and this feature does not set out to fix them. FR-039 records the result.
- The 5% limit in SC-010 is a one-time quality check of this change. It is not a parity comparison, so the constitution's ban on tolerances does not apply to it. On 2026-11-06, a fresh WSL build at 32 threads rendered each Monte Carlo scene 6 times, and the highest of the 15 pairs measured 0.6% to 1.4% above the lowest, so 5% does not fail at random.
- The default seed value is chosen during planning.
