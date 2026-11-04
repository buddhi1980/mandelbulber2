# Feature Specification: Regression Harness

**Feature Branch**: `upgrade/regression-harness`

**Created**: 2026-11-04

**Status**: Draft

**Input**: User description: "Create a specification for a headless differential testing engine. It reads a version-controlled scene list from tests/scenes.txt, renders each .fract file with the mandelbulber2 CPU engine using --nogui, and compares decoded 32-bit float TIFF samples against per-target reference data with exact bitwise equality and no tolerance. It needs three modes: self-parity, reference capture, and gate. It must fail on an empty scene list, a missing scene, a failed settings load, or a missing output file, and must not rely on the exit code, because the application exits 0 after a failed load or a failed save. It must also record final render statistics from cStatistics (total iterations, raymarch count, missed DE, and the iteration and step-count histograms) and diff them exactly, excluding time-based values. No deterministic export of these statistics exists, so the spec must define one. All written image channels must be compared, and the spec must decide whether to enable the z-buffer and normal channels as well."

## User Scenarios & Testing *(mandatory)*

Terms such as Standard Scene Suite, Reference Target, Self-Parity, and Baseline Change are defined in the project constitution. KD numbers refer to its Known Deviations.

### User Story 1 - Never Pass What Was Not Verified (Priority: P1)

An engine developer or CI job runs the harness and can trust a passing result. The application can exit successfully after failing to load a scene, failing to load a texture, or failing to save an image, so the harness decides each outcome from evidence it checks itself.

**Why this priority**: A harness that can pass falsely is worse than no harness, because it gives false confidence during refactors. Every other story depends on this one.

**Independent Test**: Run the harness against deliberately broken inputs, one at a time, and confirm that each run fails and names the cause. This needs no references and no reproducible renders.

**Acceptance Scenarios**:

1. **Given** a scene list with no scene entries, **When** the harness runs in any mode, **Then** it fails and reports that the list is empty.
2. **Given** a listed scene file that does not exist, **When** the harness runs, **Then** it fails before rendering and names the missing file.
3. **Given** a listed file that is not a valid settings file, **When** it is rendered, **Then** the harness fails that scene, even though the application exited 0 and saved an image.
4. **Given** a scene with a texture that cannot be loaded, or that is found only by the application's alternate-location search, **When** it is rendered, **Then** the harness fails that scene.
5. **Given** a render that writes no image for an expected channel, **When** it finishes, **Then** the harness fails that scene, even though the application reported the image as saved.
6. **Given** a render that exits with a non-zero status or exceeds the time limit, **When** it ends, **Then** the harness fails that scene.

---

### User Story 2 - Prove a Build Reproduces Its Own Output (Priority: P1)

An engine developer checks whether a build produces identical output every time. This is how the Determinism Foundation work is measured, and it must pass before any references are accepted.

**Why this priority**: It is useful as soon as it exists. It reports KD-1 on today's build, and it is the only check available for Phase 3, because no references exist until after that work is done.

**Independent Test**: Run self-parity on today's build and confirm it reports differences. Run it against a stand-in renderer with fixed output and confirm that it passes.

**Acceptance Scenarios**:

1. **Given** a build whose renders differ between runs, **When** self-parity runs, **Then** it fails and reports, for each scene and channel, how many samples differ.
2. **Given** a build whose renders are identical, **When** self-parity runs at two thread counts, **Then** it passes.
3. **Given** two thread counts are available, **When** self-parity runs, **Then** every scene renders three consecutive times at each thread count, one of them 1, and every result is compared with the first.
4. **Given** the application has no working headless thread control, **When** self-parity runs, **Then** it still renders and compares at the available setting, but it does not report a pass, and it states that thread coverage is incomplete.
5. **Given** a scene whose color channel changes when the extra channels and statistics export are switched off, **When** self-parity runs, **Then** it fails that scene.

---

### User Story 3 - Detect Drift Against References (Priority: P2)

A CI job or engine developer confirms that a change did not alter any output for any scene in the Standard Scene Suite on a Reference Target.

**Why this priority**: This is the protection the constitution requires for refactors. It cannot run for real until Baseline 1 exists, but it can be fully exercised with a stand-in renderer.

**Independent Test**: With a stand-in renderer and prepared references, confirm that identical output passes and that one flipped bit in one sample fails.

**Acceptance Scenarios**:

1. **Given** references for the target and identical output, **When** the gate runs, **Then** it passes.
2. **Given** one bit differs in one sample of any channel, **When** the gate runs, **Then** it fails and names the scene, the channel, and the number of differing samples.
3. **Given** a difference that disappears when rounded to 8-bit, **When** the gate runs, **Then** it still fails.
4. **Given** a listed scene with no reference, or a reference with no listed scene, **When** the gate runs, **Then** it fails and names that scene.
5. **Given** a render that writes a channel with no reference, **When** the gate runs, **Then** it fails and names the channel.
6. **Given** no reference set exists for the selected target, **When** the gate runs, **Then** it fails and does not report a pass for zero comparisons.
7. **Given** an image whose dimensions or sample format differ from the reference, **When** the gate runs, **Then** it fails and reports the layout difference.

---

### User Story 4 - Capture References for a Baseline Change (Priority: P2)

A maintainer performing a Baseline Change produces a complete new reference set for a Reference Target, from a build proven to reproduce its own output.

**Why this priority**: The gate has nothing to compare against without references, and a bad capture would make every later comparison meaningless.

**Independent Test**: Use a stable stand-in renderer and confirm that capture writes a complete set. Use an unstable one and confirm that it writes nothing.

**Acceptance Scenarios**:

1. **Given** a build that passes Self-Parity at two thread counts, **When** capture runs, **Then** it writes a complete reference set for the target: every channel image, every statistics record, and the toolchain identity.
2. **Given** a build that fails Self-Parity, **When** capture runs, **Then** it fails and leaves any existing references unchanged.
3. **Given** no working headless thread control, **When** capture runs, **Then** it refuses before rendering and explains why.
4. **Given** a failure part way through the scene list, **When** capture stops, **Then** existing references are unchanged and no partial set remains.
5. **Given** a platform where a render cannot be isolated from per-user state, **When** capture runs, **Then** it refuses.

---

### User Story 5 - Catch Computation Drift With Statistics (Priority: P3)

An engine developer wants drift in the computation caught even when no pixel changes. A change in iteration or raymarch counts can reveal a behavior change that the fixed test resolution does not show.

**Why this priority**: Image comparison provides value by itself, and this story needs a small addition to the application.

**Independent Test**: Render one scene with the export requested and confirm that the record holds all five statistics. Change one count in a copy of the record and confirm that the comparison fails.

**Acceptance Scenarios**:

1. **Given** the export is requested, **When** a render finishes, **Then** a record holds total iterations, raymarch count, missed DE count, and the full iteration and step-count histograms, all as exact whole numbers.
2. **Given** two records that differ by 1 in any value, **When** they are compared, **Then** the comparison fails and names the statistic and both values.
3. **Given** two identical renders, **When** their records are compared, **Then** the records match exactly, because they contain no time, rate, process, host, or path values.
4. **Given** the export is not requested, **When** a render runs, **Then** the application behaves as it does today and writes no record.
5. **Given** a missing or malformed record, **When** the harness checks a scene, **Then** it fails that scene.

---

### Edge Cases

- **Comment-only scene list**: It is treated as empty and fails.
- **Same file name twice**: Two listed scenes with the same file name fail the run, because their outputs and references would collide.
- **Stale outputs**: Output from an earlier run is removed before each render, so it can never satisfy a check.
- **URL or absolute-path assets**: A listed scene that references an asset this way fails the run. 54 of the 791 example scenes do, and such assets depend on the network or on one machine's folders.
- **NaN and negative zero**: Samples are compared by their bits. A NaN matches only the same bit pattern, and positive zero does not match negative zero.
- **Interrupted capture**: Existing references survive, and no partial set is left behind.
- **Scene removed from the list**: The gate fails on the leftover reference, so coverage cannot shrink without a Baseline Change.
- **Toolchain mismatch**: The report shows when the toolchain differs from the one recorded with the references. The comparison result still decides the verdict.
- **Asset found by the alternate-location search**: This counts as a failure even if the render completes, because the search looks outside the repository, including the user's home folder.

## Requirements *(mandatory)*

### Functional Requirements

**Scene list**

- **FR-001**: The harness MUST read the Standard Scene Suite from `tests/scenes.txt`, a version-controlled file with one scene path per line, relative to the repository root. Blank lines and comment lines MUST be ignored.
- **FR-002**: The harness MUST fail before rendering if the list has no scenes, a listed scene is missing, two scenes share a file name, or a scene references an asset by URL or absolute path.

**Rendering**

- **FR-003**: Every render MUST run headlessly through the application's `--nogui` mode on the CPU engine, at one fixed resolution that is the same in all modes.
- **FR-004**: Every render MUST be isolated from per-user settings, caches, and the host locale.
- **FR-005**: On a platform where FR-004 cannot be met, the harness MAY run for debugging, but it MUST label the results as not gate-valid and MUST refuse to capture.
- **FR-006**: The harness MUST remove earlier output for a render before starting it.
- **FR-007**: Each render MUST have a time limit, and exceeding it MUST fail that scene.

**Failure detection**

- **FR-008**: The harness MUST NOT treat a zero exit status as success. A scene succeeds only if the exit status is zero, the application reported no settings load failure, no asset load failure, and no alternate-location asset search, every expected channel image exists and can be decoded, and the statistics record exists and is valid.
- **FR-009**: Any failed check in FR-008 MUST fail the run and identify the scene and the cause.

**Image channels**

- **FR-010**: Each gated render MUST write the color, z-buffer, and normal channels at 32-bit float precision, in TIFF format.
- **FR-011**: The harness MUST compare every channel image a render writes. A missing channel or an unexpected channel MUST fail the scene.
- **FR-012**: Self-parity MUST include one render of each scene with the extra channels and the statistics export switched off, and its color channel MUST match the gated renders.

**Comparison**

- **FR-013**: Images MUST be compared by their decoded samples, bit for bit, with no tolerance, rounding, or down-conversion. File metadata and compression MUST NOT affect the result.
- **FR-014**: A difference in image dimensions, channel count, bit depth, or sample format MUST fail the comparison.

**Modes**

- **FR-015**: The harness MUST provide gate, self-parity, and capture modes, one per run, with gate as the default.
- **FR-016**: Self-parity MUST perform three consecutive renders of each scene at each of at least two thread counts, one of them 1, and compare every image and statistics record with the first.
- **FR-017**: Without a working headless thread control, self-parity MUST NOT report a pass and MUST state that thread coverage is incomplete.
- **FR-018**: Capture MUST pass self-parity in the same run before it writes anything, and MUST replace the target's reference set completely or not at all.
- **FR-019**: The gate MUST render each scene once and compare all of its channels and its statistics against the selected target's references. It MUST fail on a missing reference set, a listed scene with no reference, and a reference with no listed scene.
- **FR-020**: Gate and self-parity modes MUST NOT modify references.

**References**

- **FR-021**: References MUST be stored per Reference Target, and each run MUST state which target it used.
- **FR-022**: A reference set MUST contain every channel image and statistics record for every listed scene, plus the compiler and math library versions of the build that produced it.
- **FR-023**: The gate MUST report a toolchain difference from the recorded versions, without changing its verdict.

**Statistics export**

- **FR-024**: The application MUST offer an opt-in way to write a statistics record to a requested location after a still render.
- **FR-025**: The record MUST contain total iterations, raymarch count, missed DE count, and every bucket of the iteration and step-count histograms, including the overflow bucket.
- **FR-026**: Every value MUST be an exact whole number in a fixed order, in a versioned format. The record MUST NOT contain time-based values, rates, process IDs, host names, file paths, or locale-dependent formatting.
- **FR-027**: The export MUST only read values the renderer has already collected. It MUST NOT add work to the render path or change any rendered output, and without the request the application MUST behave exactly as it does today.
- **FR-028**: Statistics MUST be compared exactly in self-parity, capture, and gate. A mismatch MUST name the statistic and both values.

**Reporting and scope**

- **FR-029**: The report MUST give a verdict for each scene and channel, the number of differing samples for each failed comparison, and an overall result. It MUST distinguish setup errors from parity failures.
- **FR-030**: The harness MUST return a success status only when every check passed, and MUST keep the outputs of failed comparisons for inspection, outside version control.
- **FR-031**: Apart from the statistics export, this feature MUST NOT change engine code. It MUST NOT create or commit references, because renders do not reproduce yet (KD-1).

### Decisions

**Channels: enable z-buffer and normal, alongside color.**

- **Why**: Color is the shaded end result, so clipping, fog, and dark regions can hide a change in geometry. Z-buffer and normal record hit distance and surface orientation directly, and they show a developer whether drift began in geometry or in shading.
- **Cost**: One render still produces all three. For `IFS_001.fract` at 240x180, output grows from 323 KB to 909 KB, about 2.8 times.
- **Risk**: The gated configuration is not the default one. The z-buffer is always computed, and the normal channel only transforms a vector the shader has already produced, so color is not expected to change. FR-012 confirms this rather than assuming it.
- **Limit**: The saved z-buffer is scaled to each image's depth range, so a very small depth change can round away in that channel. Color and normal still cover that case.
- **Not enabled**: Alpha, specular, diffuse, world position, world normal, shadow, global illumination, and not-denoised. Adding one later is a Baseline Change.

**Statistics: add a new export to the application.**

- **Why**: Existing outputs cannot be diffed exactly. Log lines start with a process ID and elapsed time, and `--stats` rounds to six significant digits (for example `3.16706e+08`). `--logfilepath` also wrote no file in testing.

### Key Entities *(include if feature involves data)*

- **Scene List**: The version-controlled Standard Scene Suite, an ordered list of `.fract` files.
- **Gated Render**: One headless CPU render of one scene, at the fixed resolution, in an isolated environment. It produces three channel images and a statistics record.
- **Channel Image**: One 32-bit float image for one scene and channel (color, z-buffer, or normal).
- **Statistics Record**: The final exact counts for one render, with no time-based values.
- **Reference Set**: All channel images and statistics records for one Reference Target, plus the toolchain identity.
- **Toolchain Identity**: The compiler and math library versions of the build that produced a reference set.
- **Run Report**: The per-scene and per-channel verdicts and the overall result of one run.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Every failure condition in this specification, seeded one at a time, produces a failing run that names the cause, with no false passes.
- **SC-002**: A single flipped bit in any one sample of any channel, and a change of 1 in any statistic, is detected in 100% of trials.
- **SC-003**: On today's build, self-parity reports non-reproducible output for every scene tested, matching KD-1. After Phase 3, it passes with no changes to the harness other than enabling the second thread count.
- **SC-004**: A full gate run of the Standard Scene Suite finishes in under 30 minutes in the automated build environment.
- **SC-005**: From the report alone, a developer can identify the failing scene, channel or statistic, and size of the difference without rerunning the harness.
- **SC-006**: A failed or interrupted capture leaves existing references byte-for-byte unchanged in 100% of trials.
- **SC-007**: After Phase 3, color output matches exactly with the extra channels and the statistics export on or off, for every scene in the suite.
- **SC-008**: One target's reference set stays under 100 MB.

## Assumptions

**Dependencies on Phase 3 (Determinism Foundation)**

- **Statistics will not match until the counters are fixed.** All render threads update one shared statistics object with no synchronization. On 2026-11-04, three identical renders of `IFS_001.fract` reported about 3.17e8, 2.70e8, and 2.43e8 total iterations. That spread suggests lost updates, though the cause is not proven. This feature exports the values as collected (FR-027). Making them exact belongs to KD-1, because Self-Parity includes statistics and KD-1 closes only when Self-Parity passes.
- **Two thread counts need a new control.** `limit_CPU_cores` showed no effect under `--nogui`, so self-parity cannot pass until Phase 3 adds a working control (FR-017).
- **FR-012 and SC-007 cannot be confirmed until renders reproduce.**

**Scope**

- This feature covers still renders on the CPU engine. OpenCL, animation, voxel export, and network rendering are out of scope.
- Engine determinism fixes, floating-point flag changes, CI jobs, and Baseline 1 are out of scope. They belong to Phases 3 and 4 of the playbook.
- Selecting the full Standard Scene Suite is an implementation task. The constitution requires coverage of Monte Carlo depth of field, global illumination, and ambient occlusion, and the top-level examples include 22, 9, and 77 such scenes respectively.

**Environment and defaults**

- The first Reference Target is the Linux x86-64 GCC build. Windows runs are for debugging only, because the application ignores a redirected home folder there.
- The fixed resolution is 320x240, which keeps runs short while still exercising each render path.
- References are stored as full images in the repository. At 320x240 this is about 1.6 MB per scene, estimated from one measured scene.
- Application behavior described here was observed on a Windows Release build from July 2026 and read from the current source. It has not yet been confirmed on a Linux build.
