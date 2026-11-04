# Feature Specification: Regression Harness

**Feature Branch**: `upgrade/regression-harness`

**Created**: 2026-11-04

**Status**: Draft

**Input**: User description: "Create a specification for a headless differential testing engine. It reads a version-controlled scene list from tests/scenes.txt, renders each .fract file with the mandelbulber2 CPU engine using --nogui, and compares decoded 32-bit float TIFF samples against per-target reference data with exact bitwise equality and no tolerance. It needs three modes: self-parity, reference capture, and gate. It must fail on an empty scene list, a missing scene, a failed settings load, or a missing output file, and must not rely on the exit code, because the application exits 0 after a failed load or a failed save. It must also record final render statistics from cStatistics (total iterations, raymarch count, missed DE, and the iteration and step-count histograms) and diff them exactly, excluding time-based values. No deterministic export of these statistics exists, so the spec must define one. All written image channels must be compared, and the spec must decide whether to enable the z-buffer and normal channels as well."

## Clarifications

### Session 2026-11-04

- Q: The 32-bit TIFF color channel is effectively 16-bit, EXR color skips post effects, and no file format saves the post-effect float image. What should the gate compare for color? → A: A raw buffer export. The application writes its engine buffers directly, which replaces TIFF as the compared data.
- Q: Monte Carlo scenes take too long at their authored sample counts for a small CI machine. How should the harness keep them affordable? → A: One fixed sample cap, applied by the harness to every scene.
- Q: Only formulas used by listed scenes get checked, and a 25-scene suite reaches about 76 of the 462 formulas. What should the gate cover? → A: The Standard Scene Suite plus a low-resolution formula sweep that renders every formula once.
- Q: References come from CI, so a local run on a different toolchain could report drift when nothing changed. What should happen? → A: Add a fourth mode that compares two builds on the same machine. The gate keeps its behavior on a toolchain mismatch.
- Q: Failed scene loads are detected by matching error text, which is translatable and could be reworded. Should the verification export be written only after a successful load and render? → A: Yes. The export's presence proves success, and text checks stay as a second layer.

## User Scenarios & Testing *(mandatory)*

Terms such as Standard Scene Suite, Reference Target, Self-Parity, and Baseline Change are defined in the project constitution. KD numbers refer to its Known Deviations.

### User Story 1 - Never Pass What Was Not Verified (Priority: P1)

An engine developer or CI job runs the harness and can trust a passing result. The application can exit successfully after failing to load a scene, failing to load a texture, or failing to save an image, so the harness decides each outcome from evidence it checks itself.

**Why this priority**: A harness that can pass falsely is worse than no harness, because it gives false confidence during refactors. Every other story depends on this one.

**Independent Test**: Run the harness against deliberately broken inputs, one at a time, and confirm that each run fails and names the cause. This needs no references and no reproducible renders.

**Acceptance Scenarios**:

1. **Given** a scene list with no scene entries, **When** the harness runs in any mode, **Then** it fails and reports that the list is empty.
2. **Given** a listed scene file that does not exist, **When** the harness runs, **Then** it fails before rendering and names the missing file.
3. **Given** a listed file that is not a valid settings file, **When** it is rendered, **Then** the application writes no verification export and the harness fails that scene, even though the application exited 0 and saved an image.
4. **Given** a scene with a texture that cannot be loaded, or that is found only by the application's alternate-location search, **When** it is rendered, **Then** the harness fails that scene.
5. **Given** a render that writes no verification export, or an incomplete one, **When** it finishes, **Then** the harness fails that scene, even though the application reported success.
6. **Given** a render that exits with a non-zero status or exceeds the time limit, **When** it ends, **Then** the harness fails that scene.

---

### User Story 2 - Prove a Build Reproduces Its Own Output (Priority: P1)

An engine developer checks whether a build produces identical output every time. This is how the Determinism Foundation work is measured, and it must pass before any references are accepted.

**Why this priority**: It is useful as soon as it exists. It reports KD-1 on today's build, and it is the only check available for Phase 3, because no references exist until after that work is done.

**Independent Test**: Run self-parity on today's build and confirm it reports differences. Run it against a stand-in renderer with fixed output and confirm that it passes.

**Acceptance Scenarios**:

1. **Given** a build whose renders differ between runs, **When** self-parity runs, **Then** it fails and reports, for each scene and buffer, how many samples differ.
2. **Given** a build whose renders are identical, **When** self-parity runs at two thread counts, **Then** it passes.
3. **Given** two thread counts are available, **When** self-parity runs, **Then** every scene renders three consecutive times at each thread count, one of them 1, and every result is compared with the first.
4. **Given** the application has no working headless thread control, **When** self-parity runs, **Then** it still renders and compares at the available setting, but it does not report a pass, and it states that thread coverage is incomplete.
5. **Given** a scene whose final image changes when the normal buffer and the verification export are switched off, **When** self-parity runs, **Then** it fails that scene.

---

### User Story 3 - Detect Drift Against References (Priority: P2)

A CI job or engine developer confirms that a change did not alter any output for any scene in the Standard Scene Suite on a Reference Target.

**Why this priority**: This is the protection the constitution requires for refactors. It cannot run for real until Baseline 1 exists, but it can be fully exercised with a stand-in renderer.

**Independent Test**: With a stand-in renderer and prepared references, confirm that identical output passes and that one flipped bit in one sample fails.

**Acceptance Scenarios**:

1. **Given** references for the target and identical output, **When** the gate runs, **Then** it passes.
2. **Given** one bit differs in one sample of any buffer, **When** the gate runs, **Then** it fails and names the scene, the buffer, and the number of differing samples.
3. **Given** a difference that disappears when rounded to 16-bit, **When** the gate runs, **Then** it still fails.
4. **Given** a listed scene with no reference, or a reference with no listed scene, **When** the gate runs, **Then** it fails and names that scene.
5. **Given** an export that contains a buffer with no reference, **When** the gate runs, **Then** it fails and names the buffer.
6. **Given** no reference set exists for the selected target, **When** the gate runs, **Then** it fails and does not report a pass for zero comparisons.
7. **Given** a buffer whose dimensions or sample type differ from the reference, **When** the gate runs, **Then** it fails and reports the layout difference.
8. **Given** drift in a formula that no listed scene uses, **When** the gate runs, **Then** the formula sweep fails and names the formula.
9. **Given** a formula the application offers with no sweep reference, **When** the gate runs, **Then** it fails and names the formula.

---

### User Story 4 - Capture References for a Baseline Change (Priority: P2)

A maintainer performing a Baseline Change produces a complete new reference set for a Reference Target, from a build proven to reproduce its own output.

**Why this priority**: The gate has nothing to compare against without references, and a bad capture would make every later comparison meaningless.

**Independent Test**: Use a stable stand-in renderer and confirm that capture writes a complete set. Use an unstable one and confirm that it writes nothing.

**Acceptance Scenarios**:

1. **Given** a build that passes Self-Parity at two thread counts, **When** capture runs, **Then** it writes a complete reference set for the target: every buffer, every statistics record, and the toolchain identity.
2. **Given** a build that fails Self-Parity, **When** capture runs, **Then** it fails and leaves any existing references unchanged.
3. **Given** no working headless thread control, **When** capture runs, **Then** it refuses before rendering and explains why.
4. **Given** a failure part way through the scene list, **When** capture stops, **Then** existing references are unchanged and no partial set remains.
5. **Given** a platform where a render cannot be isolated from per-user state, **When** capture runs, **Then** it refuses.

---

### User Story 5 - Catch Computation Drift With Statistics (Priority: P3)

An engine developer wants drift in the computation caught even when no pixel changes. A change in iteration or raymarch counts can reveal a behavior change that the fixed test resolution does not show.

**Why this priority**: Buffer comparison provides value by itself, and statistics add a second, independent signal.

**Independent Test**: Render one scene with the export requested and confirm that the record holds all five statistics. Change one count in a copy of the record and confirm that the comparison fails.

**Acceptance Scenarios**:

1. **Given** the export is requested, **When** a render finishes, **Then** a record holds total iterations, raymarch count, missed DE count, and the full iteration and step-count histograms, all as exact whole numbers.
2. **Given** two records that differ by 1 in any value, **When** they are compared, **Then** the comparison fails and names the statistic and both values.
3. **Given** two identical renders, **When** their records are compared, **Then** the records match exactly, because they contain no time, rate, process, host, or path values.
4. **Given** the export is not requested, **When** a render runs, **Then** the application behaves as it does today and writes no export.
5. **Given** a missing or malformed record, **When** the harness checks a scene, **Then** it fails that scene.

---

### User Story 6 - Check a Change Locally Before Pushing (Priority: P3)

An engine developer compares their branch against master on their own machine, before pushing, using any toolchain. References come from CI and may not match a local compiler, so this check compares two local builds with each other instead.

**Why this priority**: It shortens the refactoring loop in Phase 5, but the CI gate remains the authority, and this mode gives clean results only after Phase 3.

**Independent Test**: Run compare mode with two stand-in renderers. Identical output passes, and output with one flipped bit fails. No references are needed.

**Acceptance Scenarios**:

1. **Given** two builds that produce identical output, **When** compare mode runs, **Then** it passes.
2. **Given** two builds that differ in one sample of any buffer, or by 1 in any statistic, **When** compare mode runs, **Then** it fails and names the scene or formula and what differs.
3. **Given** no references exist, **When** compare mode runs, **Then** it still runs, and it neither reads nor changes any reference.
4. **Given** a render that fails on either build, **When** compare mode runs, **Then** it fails and names the build and the cause.
5. **Given** any compare result, **When** the report is shown, **Then** it states that this is a local check and does not replace the gate.

---

### Edge Cases

- **Comment-only scene list**: It is treated as empty and fails.
- **Same file name twice**: Two listed scenes with the same file name fail the run, because their outputs and references would collide.
- **Stale outputs**: Output from an earlier run is removed before each render, so it can never satisfy a check.
- **URL or absolute-path assets**: A listed scene that references an asset this way fails the run. 54 of the 791 example scenes do, and such assets depend on the network or on one machine's folders.
- **NaN and negative zero**: Samples are compared by their bits. A NaN matches only the same bit pattern, and positive zero does not match negative zero.
- **Interrupted capture**: Existing references survive, and no partial set is left behind.
- **Scene removed from the list**: The gate fails on the leftover reference, so coverage cannot shrink without a Baseline Change.
- **Toolchain mismatch**: The report shows when the toolchain differs from the one recorded with the references. The comparison result still decides the verdict, so a gate run on a different toolchain can report drift when nothing changed. Compare mode is the local check for that case.
- **Asset found by the alternate-location search**: This counts as a failure even if the render completes, because the search looks outside the repository, including the user's home folder.

## Requirements *(mandatory)*

### Functional Requirements

**Scene list**

- **FR-001**: The harness MUST read the Standard Scene Suite from `tests/scenes.txt`, a version-controlled file with one scene path per line, relative to the repository root. Blank lines and comment lines MUST be ignored.
- **FR-002**: The harness MUST fail before rendering if the list has no scenes, a listed scene is missing, two scenes share a file name, or a scene references an asset by URL or absolute path.

**Rendering**

- **FR-003**: Every render MUST run headlessly through the application's `--nogui` mode on the CPU engine, at one fixed resolution and one fixed Monte Carlo sample cap, both the same in all modes and for all scenes.
- **FR-004**: Every render MUST be isolated from per-user settings, caches, and the host locale.
- **FR-005**: On a platform where FR-004 cannot be met, the harness MAY run for debugging, but it MUST label the results as not gate-valid and MUST refuse to capture.
- **FR-006**: The harness MUST remove earlier output for a render before starting it.
- **FR-007**: Each render MUST have a time limit, and exceeding it MUST fail that scene.

**Failure detection**

- **FR-008**: The harness MUST NOT treat a zero exit status as success. A scene succeeds only if the exit status is zero, the application reported no settings load failure, no asset load failure, and no alternate-location asset search, and the verification export exists, is complete, and is valid.
- **FR-009**: Any failed check in FR-008 MUST fail the run and identify the scene and the cause.

**Engine buffers**

- **FR-010**: Each gated render MUST produce a verification export containing five engine buffers at the engine's own precision: the raw render, the post-effect image, the final image, depth, and normal.
- **FR-011**: The harness MUST compare every buffer in the export. A missing buffer or an unexpected buffer MUST fail the scene.
- **FR-012**: Self-parity MUST include one render of each scene with the normal buffer and the verification export switched off, and the final image it saves MUST match the final-image buffer of the gated renders.

**Comparison**

- **FR-013**: Buffers MUST be compared bit for bit, with no tolerance, rounding, or down-conversion. File metadata and compression MUST NOT affect the result.
- **FR-014**: A difference in buffer dimensions, sample count, or sample type MUST fail the comparison.

**Modes**

- **FR-015**: The harness MUST provide gate, self-parity, capture, and compare modes, one per run, with gate as the default.
- **FR-016**: Self-parity MUST perform three consecutive renders of each scene at each of at least two thread counts, one of them 1, and compare every buffer and statistics record with the first.
- **FR-017**: Without a working headless thread control, self-parity MUST NOT report a pass and MUST state that thread coverage is incomplete.
- **FR-018**: Capture MUST pass self-parity in the same run before it writes anything, and MUST replace the target's reference set completely or not at all.
- **FR-019**: The gate MUST render each scene once and compare all of its buffers and its statistics against the selected target's references. It MUST fail on a missing reference set, a listed scene with no reference, and a reference with no listed scene.
- **FR-020**: Gate and self-parity modes MUST NOT modify references.

**References**

- **FR-021**: References MUST be stored per Reference Target, and each run MUST state which target it used.
- **FR-022**: A reference set MUST contain every buffer and statistics record for every listed scene, plus the compiler and math library versions of the build that produced it.
- **FR-023**: The gate MUST report a toolchain difference from the recorded versions, without changing its verdict.

**Verification export**

- **FR-024**: The application MUST offer an opt-in verification export that writes the engine buffers and a statistics record to a requested location after a still render. It MUST write the export only when the settings loaded and the render reported success.
- **FR-025**: The statistics record MUST contain total iterations, raymarch count, missed DE count, and every bucket of the iteration and step-count histograms, including the overflow bucket.
- **FR-026**: Buffers MUST hold the engine's exact values, with no scaling, normalization, gamma, or quantization added by the export. Every statistics value MUST be an exact whole number in a fixed order, and the export MUST use a versioned format. It MUST NOT contain time-based values, rates, process IDs, host names, file paths, or locale-dependent formatting.
- **FR-027**: The export MUST only read values the renderer has already produced. It MUST NOT add work to the render path or change any rendered output, and without the request the application MUST behave exactly as it does today.
- **FR-028**: Statistics MUST be compared exactly in every mode. A mismatch MUST name the statistic and both values.

**Reporting and scope**

- **FR-029**: The report MUST give a verdict for each scene and buffer, the number of differing samples for each failed comparison, and an overall result. It MUST distinguish setup errors from parity failures.
- **FR-030**: The harness MUST return a success status only when every check passed, and MUST keep the outputs of failed comparisons for inspection, outside version control.
- **FR-031**: Apart from the verification export and a read-only formula listing, this feature MUST NOT change engine code. It MUST NOT create or commit references, because renders do not reproduce yet (KD-1).

**Formula sweep**

- **FR-032**: The gate MUST include a formula sweep: one render of every formula the application offers, from one fixed base scene in the repository, at one fixed low resolution that is the same in all modes.
- **FR-033**: Sweep renders MUST meet every requirement that applies to scene renders, in every mode, including isolation, failure detection, buffer and statistics comparison, and reference storage.
- **FR-034**: A formula with no sweep reference, or a sweep reference for a formula the application no longer offers, MUST fail the gate. Any formula excluded from the sweep MUST be listed with its reason in a version-controlled file.

**Compare mode**

- **FR-035**: Compare mode MUST render every listed scene and sweep formula with two supplied builds on the same machine, and compare their buffers and statistics exactly, with the same fixed settings as the gate.
- **FR-036**: Compare mode MUST apply the same isolation and failure detection as the gate, and a failed render on either build MUST fail the run and name that build.
- **FR-037**: Compare mode MUST NOT read or modify references, and its report MUST state that it is a local check that does not replace the gate.

### Decisions

**Compared data: export the engine's buffers directly.**

- **Why**: The application's image formats cannot deliver the full precision the constitution requires. Every color sample in a TIFF saved at `color_quality=2` sits on a 1/65536 grid, so it is 16-bit data stored as float. EXR color holds true floats but comes from before SSAO, depth of field, HDR blur, and tone mapping. No format saves the float image after those effects, and saved z-buffers are rescaled per image.
- **Buffers**: The raw render is the float image before post effects. The post-effect image is the float image after SSAO, depth of field, and HDR blur. The final image is the 16-bit result of brightness, contrast, and gamma. Depth is the true hit distance. Normal is the surface direction.
- **Cost**: One render produces all five, with no new library. Uncompressed, they take 46 bytes per pixel, about 3.5 MB per scene at 320x240.

**Extra buffers: include depth and normal alongside color.**

- **Why**: Color is the shaded end result, so clipping, fog, and dark regions can hide a change in geometry. Depth and normal record hit distance and surface orientation directly, and they show a developer whether drift began in geometry or in shading.
- **Risk**: The gated configuration is not the default one. Depth is always computed, and the normal buffer only transforms a vector the shader has already produced, so the final image is not expected to change. FR-012 confirms this rather than assuming it.
- **Not included**: Alpha, specular, diffuse, world position, world normal, shadow, global illumination, and not-denoised. Adding one later is a Baseline Change.

**Statistics: part of the same export.**

- **Why**: Existing outputs cannot be diffed exactly. Log lines start with a process ID and elapsed time, and `--stats` rounds to six significant digits (for example `3.16706e+08`). `--logfilepath` also wrote no file in testing.

**Failure detection: treat the export as proof of success.**

- **Why**: The application ignores its own settings load and render results, and it reports failures as translatable text that could be reworded. An export written only after success turns a missed text match into a failed scene rather than a false pass. Text checks remain because texture problems do not affect those results.

**Coverage: add a formula sweep to the gate.**

- **Why**: Phase 5 refactors the formula code, and only formulas that get rendered are protected. The 791 example scenes reach 315 of the 462 formulas, and the best 25 scenes reach about 76.
- **Cost**: All 462 formulas rendered in 34 seconds on 32 threads, or an estimated 2 to 5 minutes on a CI runner, plus about 16 MB of references before compression.

### Key Entities *(include if feature involves data)*

- **Scene List**: The version-controlled Standard Scene Suite, an ordered list of `.fract` files.
- **Formula Sweep**: One low-resolution render of every formula the application offers, from one fixed base scene, compared like any other render.
- **Gated Render**: One headless CPU render of one scene, at the fixed resolution and sample cap, in an isolated environment. It produces one verification export.
- **Verification Export**: The opt-in output of a successful render: five engine buffers and a statistics record, in a versioned format.
- **Engine Buffer**: One of the raw render, post-effect image, final image, depth, or normal, for one scene.
- **Statistics Record**: The final exact counts for one render, with no time-based values.
- **Reference Set**: All engine buffers and statistics records for one Reference Target, plus the toolchain identity.
- **Toolchain Identity**: The compiler and math library versions of the build that produced a reference set.
- **Run Report**: The per-scene and per-buffer verdicts and the overall result of one run.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Every failure condition in this specification, seeded one at a time, produces a failing run that names the cause, with no false passes.
- **SC-002**: A single flipped bit in any one sample of any buffer, and a change of 1 in any statistic, is detected in 100% of trials.
- **SC-003**: On today's build, self-parity reports non-reproducible output for every scene tested, matching KD-1. After Phase 3, it passes with no changes to the harness other than enabling the second thread count.
- **SC-004**: A full gate run, including the formula sweep, finishes in under 30 minutes in the automated build environment.
- **SC-005**: From the report alone, a developer can identify the failing scene, buffer or statistic, and size of the difference without rerunning the harness.
- **SC-006**: A failed or interrupted capture leaves existing references byte-for-byte unchanged in 100% of trials.
- **SC-007**: After Phase 3, the final image matches exactly with the normal buffer and the verification export on or off, for every scene in the suite.
- **SC-008**: One target's stored reference set, including the formula sweep, stays under 100 MB.
- **SC-009**: The gate covers 100% of the formulas the application offers, apart from any listed exclusions.
- **SC-010**: After Phase 3, compare mode reports zero differences between two builds of the same source on one machine, and it reports a deliberate change to one formula against that formula.

## Assumptions

**Dependencies on Phase 3 (Determinism Foundation)**

- **Statistics will not match until the counters are fixed.** All render threads update one shared statistics object with no synchronization. On 2026-11-04, three identical renders of `IFS_001.fract` reported about 3.17e8, 2.70e8, and 2.43e8 total iterations. That spread suggests lost updates, though the cause is not proven. This feature exports the values as collected (FR-027). Making them exact belongs to KD-1, because Self-Parity includes statistics and KD-1 closes only when Self-Parity passes.
- **Two thread counts need a new control.** `limit_CPU_cores` showed no effect under `--nogui`, so self-parity cannot pass until Phase 3 adds a working control (FR-017).
- **FR-012 and SC-007 cannot be confirmed until renders reproduce.**
- **Compare mode needs reproducible builds.** Until Phase 3, every render differs from the last, so compare mode will report differences even between two builds of the same source.

**Scope**

- This feature covers still renders on the CPU engine. OpenCL, animation, voxel export, and network rendering are out of scope.
- Engine determinism fixes, floating-point flag changes, CI jobs, and Baseline 1 are out of scope. They belong to Phases 3 and 4 of the playbook.
- Selecting the full Standard Scene Suite is an implementation task. The constitution requires coverage of Monte Carlo depth of field, global illumination, and ambient occlusion, and the top-level examples include 22, 9, and 77 such scenes respectively.

**Environment and defaults**

- The first Reference Target is the Linux x86-64 GCC build on GitHub-hosted `ubuntu-24.04` runners. Windows runs are for debugging only, because the application ignores a redirected home folder there.
- The fixed resolution is 320x240, which keeps runs short while still exercising each render path.
- The fixed Monte Carlo cap is a maximum of 8 samples per pixel with a minimum of 4, so both the sampling loop and its early-stop check still run. Images are noisier than authored, but they are still exact. Changing the cap is a Baseline Change.
- The formula sweep renders at 32x24. It has not been measured in CI.
- References are stored as full buffers in the repository.
- Application behavior described here was observed on a Windows Release build from July 2026 and read from the current source. It has not yet been confirmed on a Linux build.
