# Feature Specification: Strict Floating-Point Builds

**Feature Branch**: `upgrade/strict-floating-point`

**Created**: 2026-11-07

**Status**: Draft

**Input**: User description: "Close KD-2 and KD-3 from the constitution. Remove -ffast-math from mandelbulber2/qmake/common.pri and mandelbulber2/cmake/CMakeLists.txt, replace /fp:fast in the Debug x64 configuration of mandelbulber2/msvc/mandelbulber2.vcxproj, add -ffp-contract=off to GCC and Clang builds, remove -march=native from mandelbulber2/tools/prepare_for_dev_*.sh and mandelbulber2/deploy/linux/install, and add the GitHub Actions flag check from this playbook. Measure CPU render time on the Standard Scene Suite before and after, and record the cost."

## Clarifications

### Session 2026-11-07

- Q: Should a visible image change caused by the NaN guards be accepted? → A: Yes, once the evidence names its cause. The guards stay, Linux follows the logic MSVC Release already runs, and a Baseline Change can fix a guard that proves wrong.
- Q: Should the CI flag check cover more than the gated Linux build? → A: Yes. `parity-gate` also searches the tracked build files and setup scripts before it builds, so a flag coming back on CMake, macOS, or MSVC fails CI. It reads a build's real flags only on the gated build.
- Q: What should this feature do about the math library choosing its code path from the CPU? → A: Stay with compile flags, and add a step to Phase 4 of the playbook that settles this before Baseline 1 is captured.
- Q: What should happen to qmake's import of flags from the environment? → A: Keep it. Packagers' flags still work, and the CI check catches anything it brings into a gated build.
- Q: How should the slowdown be handled? → A: Record it, with no limit beyond the 30-minute gate and nothing kept from `-ffast-math`. An option such as `-fno-math-errno` can follow Baseline 1 as an ordinary change that the gate proves.

## User Scenarios & Testing *(mandatory)*

Terms such as Standard Scene Suite, Reference Target, Self-Parity, and Baseline Change are defined in the project constitution. KD numbers refer to its Known Deviations. The harness modes and the formula sweep come from `specs/001-regression-harness`, and `--threads` comes from `specs/002-deterministic-rendering`. An item is one scene or one swept formula. There are 487 today: 26 scenes and 461 formulas.

- A **gated build** is a build of a Reference Target. There is one today: the qmake release build of `mandelbulber-opencl.pro` on Linux x86-64 with GCC, which the `build-linux` action makes.
- A **prohibited option** is one that FR-012 lists.
- **Before** is a fresh build of `ministic-regression` at `8fd49ba59`, where this branch starts. **After** is a fresh build of the final commit.

### User Story 1 - Use Strict Rules in Every Build (Priority: P1)

A developer builds the engine with any of the project's build systems, and no build lets the compiler change a floating-point result.

**Why this priority**: This is KD-2 and Principle III. Fractal iteration amplifies rounding, so a compiler that reorders arithmetic or assumes values are finite makes results depend on the compiler.

**Independent Test**: Generate the build files for each build system, and read the flags they pass to the compiler and the linker.

**Acceptance Scenarios**:

1. **Given** a qmake build on Linux or macOS, or a CMake build with GCC or Clang, **When** its build files are generated, **Then** they hold no prohibited option, and they set `-ffp-contract=off` for C and C++.
2. **Given** the MSVC Debug x64 configuration, **When** it compiles, **Then** it uses `/fp:precise`, the same rule as Release.
3. **Given** the MSVC Release x64 and AnalysisRelease configurations, **When** they compile, **Then** their flags are the same as before.
4. **Given** a line that carried `-ffast-math`, **When** the option is removed, **Then** `-fopenmp` and every other option on that line stay.
5. **Given** the engine's source, **When** it is searched, **Then** no pragma or attribute turns these behaviors back on.

---

### User Story 2 - Build the Same Instructions on Any Computer (Priority: P1)

A developer or user runs a setup or install script and gets a build that does not depend on the CPU of the computer that ran it.

**Why this priority**: This is KD-3. `-march=native` chooses instructions from the build computer, so two people running one script can get different results. On this computer it turned `a * b + c` into a fused instruction, which rounds differently.

**Independent Test**: Read the flags each script exports, then generate build files with those flags.

**Acceptance Scenarios**:

1. **Given** either `prepare_for_dev_*.sh` script, **When** it reaches its compile step, **Then** the flags it exports hold no host-tuned option.
2. **Given** `deploy/linux/install`, **When** it compiles the program, **Then** it sets no host-tuned option, and the rest of the script works as it does today.
3. **Given** a builder's own `CXXFLAGS`, **When** qmake runs, **Then** those flags reach the build, as they do today.

---

### User Story 3 - Reject a Prohibited Option in CI (Priority: P1)

A contributor opens a pull request, and CI fails it if the gated build holds a prohibited option, wherever the option came from, or if a tracked build file sets one for any build.

**Why this priority**: KD-2 and KD-3 both close on this check. Without it, a merge from upstream, where these flags still exist, or one environment variable could bring them back with no sign.

**Independent Test**: Seed each prohibited option into a gated build's flags, one at a time, and confirm that the check fails and names it. Then seed one into each kind of build file, and confirm that the search fails and names the file.

**Acceptance Scenarios**:

1. **Given** a clean gated build, **When** `parity-gate` runs, **Then** it passes.
2. **Given** a prohibited option in a project file, **When** `parity-gate` runs, **Then** it fails and names the option.
3. **Given** a prohibited option in `CXXFLAGS`, `CFLAGS`, `CPPFLAGS`, or `LDFLAGS` when qmake runs, **When** the check reads the build, **Then** it fails and names the option.
4. **Given** `-ffp-contract=off` is missing from the C or C++ flags, or another `-ffp-contract` value is present, **When** the check reads the build, **Then** it fails and says which.
5. **Given** a missing or empty build file, **When** the check runs, **Then** it fails.
6. **Given** a push to `ministic-regression`, **When** it lands, **Then** `parity-gate` runs.
7. **Given** a prohibited option or an MSVC fast floating-point setting in a tracked build file or setup script, for a build that is not gated, **When** `parity-gate` runs, **Then** it fails before building, and names the file and the option.

---

### User Story 4 - Keep Renders Reproducible (Priority: P1)

A developer runs `--self-parity` on a fresh build with the new flags, and it still passes.

**Why this priority**: The KD-1 result came from a binary built with `-ffast-math`. Strict rules change which code paths run, so that result does not carry over, and the constitution accepts no baseline from a build that fails Self-Parity.

**Independent Test**: Build the final commit fresh on Linux and run `--self-parity`.

**Acceptance Scenarios**:

1. **Given** a fresh Linux build, **When** `--self-parity` runs, **Then** every item passes for every buffer and statistic at one thread and at all available threads, and it exits 0.
2. **Given** removing the options exposes a failed render, a crash, a hang, or output that no longer reproduces, **When** the work continues, **Then** the cause is found, fixed in this change, and recorded.

---

### User Story 5 - Know What Changed in the Output (Priority: P1)

A reviewer reads a record of how output changed, and can see that each visible change has a known cause.

**Why this priority**: The constitution requires every change before Baseline 1 to state its effect on output. This one changes logic as well as rounding, because NaN guards that are compiled away today start to act. Baseline 1 then locks in whatever this change produces, so a problem has to be found now.

**Independent Test**: Read the record alone. For each item, it says what changed and by how much, and for each reviewed image it says whether the change is visible and why.

**Acceptance Scenarios**:

1. **Given** fresh before and after builds, **When** compare mode runs, **Then** the record says which buffers and statistics changed on each item, and by how much.
2. **Given** a diagnostic build, **When** the suite renders, **Then** the record says which items reach each NaN guard with a non-finite value.
3. **Given** the 26 scenes and the 20 most-changed formulas, **When** a reviewer compares their before and after final images, **Then** each visible change has a named cause.
4. **Given** an item that stops rendering anything, **When** Cross-Build Check compares it with the MSVC build, **Then** the check fails.
5. **Given** the pull request, **When** a reviewer reads it, **Then** it states the expected effect on output and what was measured.

---

### User Story 6 - Know What It Costs (Priority: P2)

A maintainer reads how much render time this change costs.

**Why this priority**: The request asks for it. `-ffast-math` exists to make renders faster, so its removal has a price, and later upgrades need to know how much of any slowdown came from here.

**Independent Test**: Time the suite on the before and after builds on a quiet computer, and read the difference.

**Acceptance Scenarios**:

1. **Given** the before and after builds, **When** the suite renders at all available threads, in both build orders, **Then** the times are recorded, with scenes and formulas apart.
2. **Given** the same builds, **When** the suite renders on one CPU, **Then** the times are recorded, with no thread scheduling in them.
3. **Given** a build of the before source with `-march=native`, **When** the suite renders, **Then** the record shows what users of the setup scripts lose.
4. **Given** another job renders during a timing run, **When** the process check shows the overlap, **Then** that run is repeated and not recorded.
5. **Given** the after build, **When** the gate time is measured, **Then** it stays within SC-004 of `specs/001-regression-harness`.

---

### User Story 7 - Keep Everything Else Working (Priority: P2)

A user or developer sees no loss from this change: every build, test, and mode that works today still works.

**Why this priority**: These flags sit in shared build files that every build reads, and some of those builds have no CI job.

**Independent Test**: Build each configuration, then run the application's own tests and Cross-Build Check.

**Acceptance Scenarios**:

1. **Given** each build in FR-033, **When** it compiles from a fresh tree, **Then** it succeeds.
2. **Given** fresh qmake and MSVC builds, **When** the application's `--test` suite runs, **Then** it passes on both.
3. **Given** a pull request, **When** Cross-Build Check runs, **Then** it passes, and the number of items that match exactly between the two builds is recorded.
4. **Given** an OpenCL render, **When** it runs, **Then** it works as it does today, because OpenCL build options do not change.
5. **Given** the workflows and CI files that exist today, **When** this feature merges, **Then** they are byte-identical to before.

---

### User Story 8 - Close KD-2 and KD-3 (Priority: P2)

A maintainer reads the constitution and sees both deviations closed, with the evidence behind them.

**Why this priority**: Plans must not report an open deviation as passing, and Phase 4 cannot begin its baseline until these close.

**Independent Test**: Read the constitution's table and the evidence it cites.

**Acceptance Scenarios**:

1. **Given** `parity-gate` passes and the seeded failures are recorded, **When** this feature merges, **Then** the constitution shows KD-2 and KD-3 closed at version 1.0.2.
2. **Given** the closed deviations, **When** a reader checks the playbook, **Then** it agrees with what was built.
3. **Given** this feature, **When** it merges, **Then** KD-4 stays open, and no file exists under `tests/references/`.

### Edge Cases

- **An orbit that overflows in one step**: The first NaN guard runs before the escape test, and it catches infinity as well as NaN. Such an orbit will count as inside the fractal, where today's Linux build lets the escape test catch it. Shapes could appear or vanish, so the review looks for that and not only for rounding.
- **A ray or normal that turns NaN**: A ray now stops as a dead calculation instead of marching on, and a NaN normal becomes `(1, 0, 0)`.
- **Signed zero**: `-ffast-math` lets the compiler ignore the sign of zero. After this change the sign is kept, so a formula can change on an exact symmetry plane, where a coordinate is exactly 0.
- **A comparison with NaN**: Once the compiler stops assuming finite values, a comparison with NaN can take the other branch, so a loop that ended before might not. FR-018 covers a hang, and the sweep reaches every formula.
- **Subnormal numbers**: The gated build does not flush them to zero, before or after this change. CMake builds with GCC do today, and will not after it.
- **Two `-ffp-contract` values**: The compiler honors the last one, so finding `-ffp-contract=off` is not enough.
- **A bare `-Xpreprocessor`**: The Apple Silicon line in `common.pri` ends in `-Xpreprocessor`, which takes the next option as its own. `-ffp-contract=off` must not land right after it, or it will not reach the compiler as a compile option.
- **A merge from upstream**: Upstream still sets `-ffast-math`, so a merge into `ministic-regression` can bring it back. The build-file search catches that in any tracked build file, and the flag check catches it on the gated build.
- **A developer's own environment**: A local `CXXFLAGS=-march=native` still reaches a local build, as it does today. The check runs only in CI.
- **Debug builds**: `debug.pro` and `debug-opencl.pro` read `common.pri`, so they change too. They are not gated.

## Requirements *(mandatory)*

### Functional Requirements

**Strict rules**

- **FR-001**: `mandelbulber2/qmake/common.pri` MUST NOT set `-ffast-math` for any platform. This covers the Linux line, both macOS lines, and the commented-out macOS line. Every other option on those lines MUST stay.
- **FR-002**: `mandelbulber2/cmake/CMakeLists.txt` MUST NOT set `-ffast-math`, and `-fopenmp` MUST stay.
- **FR-003**: The Debug x64 configuration of `mandelbulber2/msvc/mandelbulber2.vcxproj` MUST use `/fp:precise`, with `/fp:contract` unset. Nothing else in the project file changes: Debug keeps its AVX2 setting, and Release and AnalysisRelease keep the compiler's default, which is `/fp:precise`.
- **FR-004**: Every GCC and Clang build MUST set `-ffp-contract=off` for C and C++, in both qmake and CMake, on Linux and macOS. On each macOS profile it MUST reach the compiler as a compile option.
- **FR-005**: Engine code MUST NOT use a pragma or attribute that Principle III forbids. A search on 2026-11-07 found none, so this is a check and not a change.

**Host-tuned options**

- **FR-006**: `mandelbulber2/tools/prepare_for_dev_ubuntu.sh` and `prepare_for_dev_debian_testing.sh` MUST NOT set `-march=native`. `-msse2` stays, because it names part of the plain x86-64 target and does not depend on the build computer.
- **FR-007**: `mandelbulber2/deploy/linux/install` MUST NOT set `-march=native`. Its compile path MUST otherwise work as it does today.
- **FR-008**: qmake MUST keep importing `CXXFLAGS`, `CFLAGS`, `CPPFLAGS`, and `LDFLAGS` from the environment. On a gated build, the flag check covers what they bring in.

**Flag check**

- **FR-009**: The repository MUST gain `.github/actions/compile-engine/action.yml`, which builds through the existing `build-linux` action and then runs the flag check, and `.github/workflows/parity-gate.yml`, whose `parity-gate` job runs that action on `ubuntu-24.04`. Apart from the search in FR-035, the job does nothing else at this stage, because Phase 4 adds the gate steps.
- **FR-010**: `parity-gate` MUST run on every pull request and on every push to `ministic-regression`. It MUST have read-only repository access and use no secrets, as Cross-Build Check does.
- **FR-011**: The check MUST read the flags of the gated build it just compiled, from that build's generated files, so it sees what the compiler was really told. It MUST cover the C flags, the C++ flags, and the whole link command, including the library list.
- **FR-012**: The check MUST fail if any of these options is present: `-ffast-math`, `-Ofast`, `-funsafe-math-optimizations`, `-ffinite-math-only`, `-fassociative-math`, `-freciprocal-math`, `-march=native`, `-mtune=native`, or `-mcpu=native`.
- **FR-013**: The check MUST fail unless `-ffp-contract=off` is in the C flags and the C++ flags. It MUST also fail if either holds any other `-ffp-contract` value.
- **FR-014**: Each failure MUST name the option and, for a missing option, the flags it is missing from. A missing or empty build file MUST fail the check and MUST NOT count as a pass.
- **FR-015**: Seeded failures MUST prove the check. Each option in FR-012 and each case in FR-013 and FR-014 is seeded one at a time, each of `CXXFLAGS`, `CFLAGS`, `CPPFLAGS`, and `LDFLAGS` carries a prohibited option at least once, and so does each kind of file in FR-035. At least one seeded failure MUST fail `parity-gate` in GitHub Actions. No seeded change may be merged.
- **FR-016**: The workflows that exist today, the `build-linux` action, `.appveyor.yml`, `azure-pipelines.yml`, and `.circleci/config.yml` MUST NOT change.
- **FR-035**: Before it builds, `parity-gate` MUST search every tracked file that can set a compiler flag on a build of the engine: the qmake, CMake, and MSVC build files, the setup, install, and packaging scripts, and the CI configuration files. It MUST fail, and name the file and the option, if it finds a prohibited option, an `-ffp-contract` value other than `off`, an MSVC fast floating-point setting, or `/fp:contract`. The check's own files and `tools/DOE/NaNtester.sh`, which does not build the engine, are outside the search.

**Reproducibility**

- **FR-017**: Self-Parity MUST pass for every item, every buffer, and every statistic, at one thread and at all available threads, on a fresh Linux build of the final commit.
- **FR-018**: If removing the options exposes a failed render, a crash, a hang, or output that no longer reproduces, the cause MUST be found, fixed in this change, and recorded. Tolerances, exclusions, and skip lists MUST NOT be used to hide it.

**Effect on output**

- **FR-019**: Compare mode MUST run between the before and after builds, and the evidence MUST record which buffers and statistics changed on each item, and by how much. It is expected to exit 1, because output is expected to change.
- **FR-020**: The evidence MUST say which items reach each NaN guard with a non-finite value, using a diagnostic build that MUST NOT be merged. The guards are the calls to `IsNotANumber()` in `compute_fractal.cpp`, `render_worker.cpp`, and `shader_calculate_normals.cpp`.
- **FR-021**: A reviewer MUST compare the before and after final images of all 26 scenes and of the 20 swept formulas whose final image changed most, and the evidence MUST name the cause of each visible change.
- **FR-022**: The pull request MUST state the expected effect on output, as the constitution requires of every change before Baseline 1. The compiler stops reordering arithmetic and assuming finite values, and four vector math functions give way to the ordinary math library, so every item's pixels and statistics may change. Images should look the same apart from small differences, except where a NaN guard now acts. `-ffp-contract=off` has no effect on the gated build, and MSVC Release output does not change. The pull request MUST then say what was measured.
- **FR-023**: The NaN guards MUST stay as they are, and a visible change they cause is accepted once FR-021 records it. FR-018 still applies to any failure they cause.

**Cost**

- **FR-024**: CPU render time on the Standard Scene Suite MUST be measured on the before and after builds and recorded with this change. The measurements MUST cover all 487 items, with scenes and formulas reported apart, at all available threads in both build orders, and on one CPU.
- **FR-025**: Timing MUST run with nothing else rendering. A process check during each run MUST confirm that, and a run that overlaps another render MUST be repeated and not recorded.
- **FR-026**: A full gate run MUST stay within SC-004 of `specs/001-regression-harness`.
- **FR-027**: The evidence MUST also record what the loss of `-march=native` costs, from a fresh build of the before source made with that option. The harness builds do not use it, so FR-024 cannot show what users of the setup scripts lose.

**Closing KD-2 and KD-3**

- **FR-028**: Closing KD-2 and KD-3 MUST mark them closed in the constitution as a PATCH amendment, version 1.0.2, stating the reason, as Governance requires. This MUST come after `parity-gate` has passed and FR-015 is recorded. KD-4 stays open.
- **FR-029**: Statements in `MIGRATION_PLAYBOOK.md` that this feature makes out of date MUST be corrected. These include its reason for using the MSVC Release build, its flag check and `parity-gate` listings, which must match what is committed, and its statement that guards in `math_utilities.h` may take effect, because `CheckNAN` has no callers. The playbook MUST also gain a Phase 4 step that settles the math library's CPU-chosen code path before Baseline 1 is captured.

**Boundaries**

- **FR-030**: Apart from the flags above and anything FR-018 requires, this feature MUST NOT change engine behavior. Optimization levels, other compile options, formulas, shading math, and default settings stay as they are.
- **FR-031**: This feature MUST NOT add any other floating-point or instruction-set option, including parts of `-ffast-math` that leave values alone, such as `-fno-math-errno`.
- **FR-032**: OpenCL build options MUST NOT change, including `-cl-fast-relaxed-math`, because the constitution does not gate OpenCL.
- **FR-033**: Every build that compiles the engine today MUST still compile: qmake with and without OpenCL, MSVC Release and Debug, CMake, and the macOS build. The application's `--test` suite MUST keep passing on the qmake and MSVC builds, and Cross-Build Check MUST keep passing.
- **FR-034**: This feature MUST NOT create references, change the harness, or change branch protection. Baseline 1 and making `parity-gate` a required check belong to playbook Phase 4.

### Decisions

**Flag check: the gated build's real flags, plus a search of the build files.**

- **Why**: KD-2 and KD-3 both close on gated builds, and reading generated build files shows what the compiler was really told. Fixes to the playbook's version close gaps a test found: `-mcpu=native`, other `-ffp-contract` values, the library list, and failures with no message. Principle III covers every build, so the search also stops a flag coming back on a build that nothing else reads, at a cost of seconds.
- **Not chosen**: Reading the real flags of the MSVC and macOS CI builds, which changes Cross-Build Check and CircleCI, against FR-016, and leaves CMake with no CI job. The gated build alone, as the playbook lists it, which leaves the other builds unguarded.

**Environment flags: keep the import.**

- **Why**: KD-3 names the import, but it closes when the check rejects host-tuned flags, and Principle III expects inherited flags, because it tells CI to check them. Packagers also pass their own flags this way.
- **Not chosen**: Removing the import, or filtering it in qmake. Removing it drops packagers' flags, and filtering hides an option instead of reporting it.

**MSVC Debug: `/fp:precise`, not `/fp:strict`.**

- **Why**: Release already uses it, so Debug and Release then follow one rule.
- **Not chosen**: `/fp:strict`. The constitution allows it, but nothing here reads the floating-point environment, and it would give Debug a rule that Release does not have.

**NaN guards: leave them, and accept what they change.**

- **Why**: They are the code's stated intent, MSVC Release already runs them, and the constitution and the playbook both expect output to change. Baseline 1 comes after this change, so the new output becomes the reference, and a Baseline Change can fix a guard that proves wrong.
- **Not chosen**: Changing the guards to keep today's Linux images. That preserves a side effect of `-ffast-math`, moves Linux further from Windows, and is an engine change that closes no Known Deviation.

**Cost: record it, with no limit beyond the gate's 30 minutes.**

- **Why**: Principle III is non-negotiable, so a slowdown limit could not stop this change, and the request asks for a record.
- **Not chosen**: Keeping `-fno-math-errno` or another part of `-ffast-math` that leaves values alone. It may win back speed, but before Baseline 1 the constitution allows only changes that close a Known Deviation, and this one is not needed to close one. It can follow Baseline 1 as an ordinary change that the gate proves.

**KD-2 and KD-3 close in this feature.**

- **Why**: KD-1 closed in its own feature, and both closing conditions are met on this pull request. Playbook Phase 4 closes only what is still open.
- **Not chosen**: Leaving them open until Phase 4, which would report finished work as open.

### Key Entities *(include if feature involves data)*

- **Gated Build**: A build of a Reference Target, and the build the flag check reads.
- **Prohibited Option**: A compile or link option the flag check rejects.
- **Flag Check**: The CI step that reads a gated build's flags and fails on a prohibited option or on a missing `-ffp-contract=off`.
- **Build-File Search**: The CI step that searches tracked build files and setup scripts for prohibited options, for every build, before anything compiles.
- **Seeded Failure**: A deliberate fault placed in a build's flags to prove that the check fails.
- **NaN Guard**: A call to `IsNotANumber()` in the engine. Today's Linux build compiles its test away.
- **Output Record**: The per-item result of comparing the before and after builds, with the reviewer's causes.
- **Cost Record**: The render times of the before and after builds, with the conditions of each run.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: On the pull request, `parity-gate` passes on a fresh gated build with no prohibited option in its compile or link flags.
- **SC-002**: Every seeded failure in FR-015 fails the flag check or the build-file search in 100% of trials and names the option, and an unseeded build passes both.
- **SC-003**: At least one seeded failure fails `parity-gate` in GitHub Actions.
- **SC-004**: A search of the tracked build files and scripts finds no prohibited option and no MSVC fast floating-point setting, apart from the check's own files, `tools/DOE/NaNtester.sh`, and the history in `deploy/NEWS`. Every GCC and Clang build definition sets `-ffp-contract=off`, and a search of engine code finds no forbidden pragma or attribute.
- **SC-005**: `--self-parity` passes for all 487 items, across all seven buffers and all statistics, at one thread and at all available threads, on a fresh Linux build, in three full runs in a row.
- **SC-006**: The evidence covers all 487 items and says which reach each NaN guard, a reviewer has compared all 26 scenes and the 20 most-changed formulas, and every visible change has a named cause, with none left unexplained.
- **SC-007**: Render times before and after are recorded for all 487 items at all available threads in both build orders and on one CPU, along with the `-march=native` build, and no recorded run overlaps another render.
- **SC-008**: A full gate run stays under 30 minutes on a 4-vCPU CI runner.
- **SC-009**: Every `--test` test passes on fresh qmake and MSVC builds, Cross-Build Check passes, and the number of items that match exactly between the two builds is recorded.
- **SC-010**: Each build in FR-033 compiles from a fresh tree.
- **SC-011**: Every file FR-016 names is byte-identical to `8fd49ba59`. Apart from anything FR-018 requires, no file under `mandelbulber2/src`, `mandelbulber2/qt`, or `mandelbulber2/formula` changes, and the build files differ only by what FR-001 to FR-007 require.
- **SC-012**: The constitution shows KD-2 and KD-3 closed at version 1.0.2 and KD-4 open, the playbook's listings match the committed flag check and workflow, its Phase 4 holds the math path step, and no file exists under `tests/references/`.

## Assumptions

**Tested on 2026-11-07**

These come from one-file compiles and `qmake6` runs with the WSL toolchain, GCC 13.3.0 and glibc 2.39, and from reading the existing WSL build of `76316fd0f`. No render has been measured, so User Story 5 must show what they mean for output.

- **The NaN guards are dead code today.** With `-O3 -ffast-math`, a function around `std::isfinite` compiled to a constant. Without `-ffast-math`, it compiled to a real test.
- **`-ffp-contract=off` changes nothing on the gated build.** The plain x86-64 target has no fused multiply-add, so `a * b + c` compiled with no fused instruction either way. With `-march=native` on this computer it compiled to one, unless `-ffp-contract=off` was set. So the option protects host-tuned builds and ARM builds, such as CircleCI's macOS build on Apple Silicon.
- **The gated build's instruction set is the plain x86-64 target.** GCC's default here is `-march=x86-64` with `-mtune=generic`, and the build sets neither. That fixes the instruction set for the First Reference Target, as Principle III requires, for the compiler version recorded with Baseline 1.
- **`-ffast-math` reaches only the compile line of the gated build.** The generated `LFLAGS` and `LIBS` lack it, and the unstripped binary has no `set_fast_math` symbol. A test program linked with `-ffast-math` started with flush-to-zero and denormals-are-zero set, and one that was compiled with it but linked without it did not. CMake passes its flags to the linker too, so CMake builds with GCC flush subnormal numbers today.
- **The engine uses the vector math library only because of `-ffast-math`.** Today's binary needs `libmvec` directly, for `sin` and `cos` in `ssao_worker` and `rendered_image_widget`, `powf` in `radiance_hdr`, and `logf` in `file_image`. A test loop of `sin` calls at `-O3` used it only with `-ffast-math`. Those calls move to the ordinary math library, which can round differently, so this is a third source of change alongside arithmetic order and the guards.
- **The playbook's check has gaps.** Run as written on seeded copies of a generated Makefile, it caught all 8 options it lists in `CXXFLAGS`, `-march=native` in `CFLAGS`, `-ffast-math` in `LFLAGS`, and a missing Makefile. It passed `-mcpu=native`, a later `-ffp-contract=fast`, and `-ffast-math` on the `LIBS` line, and it failed with no message when `-ffp-contract=off` was missing. A separate compile showed that GCC honors the later `-ffp-contract` value. FR-011 to FR-014 close these.
- **All four environment variables reach the generated flags.** `CXXFLAGS` lands in the C++ flags, `CFLAGS` in the C flags, `CPPFLAGS` in both, and `LDFLAGS` in the link flags.
- **The math library picks its code path from the CPU at run time.** One binary gave different results for `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `exp`, `log`, and `pow`, over 4 million values each, when the library's FMA and AVX2 paths were turned off. It gave the same results for `sqrt`. So no compile flag can make two computers agree if their CPUs take different paths. Scope covers what this means here.

**Found by reading the code on 2026-11-07**

None of these has been measured. Line numbers are those of `8fd49ba59`.

- **The guards.** `IsNotANumber()` is called at `compute_fractal.cpp:316` and `:500`, `render_worker.cpp:776`, and `shader_calculate_normals.cpp:120`. `CheckNAN` in `math_utilities.h`, which the playbook also names, has no callers, so it cannot change anything.
- **The first guard can change what counts as inside the fractal.** It tests `std::isfinite`, so it catches infinity as well as NaN, and it runs at line 316, before the escape test at line 325. An orbit that overflows to infinity then counts as inside, at its last finite value. Today's Linux build skips the guard, so the escape test counts that orbit as escaped.
- **MSVC Release already runs the guards.** It uses `/fp:precise`, so this change moves the Linux build toward the logic the Windows build has today. A before image from MSVC can show whether a visible change comes from the guards.
- **Other text that matches a prohibited option stays.** `tools/DOE/NaNtester.sh` compiles a small test program of its own, not the engine, to study NaN checks under `-ffast-math`. `deploy/NEWS` is history.
- **Stale build paths stay.** CMake's Intel branch sets no flags, and the `tools/DOE` build scripts pair the Intel compiler with Qt 5.7, which the CMake file no longer accepts. `mandelbulber-opencl-win64.pro` sets `/fp:precise`, and CMake's `/GL /Ox` path on Windows gets it by default. No CI job uses any of these.

**Dependencies**

- `specs/001-regression-harness` and `specs/002-deterministic-rendering` are merged. This feature uses the harness modes, the Standard Scene Suite, and `--threads`.
- **Order of work.** The constitution amendment comes last, because it needs a passing `parity-gate` and the seeded failures on record.
- Changed output is not drift here, because no references exist yet (KD-4). Baseline 1 follows this change.
- `parity-gate` is not a required check yet. Phase 4 adds its gate steps and makes it required.

**Scope**

- The gated scope stays the CPU engine on the First Reference Target. OpenCL, animation, voxel export, and network rendering stay outside it.
- The flag check reads only the gated build. This change brings the MSVC, CMake, and macOS builds into line with Principle III, and the build-file search then guards their tracked files, but nothing reads the flags those builds really use. An option that reaches one through its environment or command line stays unseen.
- This feature locks compile flags, not the CPU's math paths. The constitution's Reference Target names the math library, but not the CPU features that choose its code path. Hosted x86-64 runners are expected to have FMA and AVX2, so they should take one path, but this feature does not prove that. The references would depend on it, so FR-029 adds a Phase 4 step to settle it before Baseline 1 is captured.

**Environment and defaults**

- Every measurement comes from a fresh build of the commit being tested. No task may rely on an existing binary.
- Self-Parity can only report a pass on Linux, because the application ignores a redirected home folder on Windows. The local proof is a fresh qmake build in the WSL `Ubuntu-24.04` distro on 32 threads.
- Clang is not installed in WSL, and no Mac is available here. CircleCI's macOS build is the only proof that `-ffp-contract=off` compiles with Clang. No job or script builds `mandelbulber-opencl-m1.pro`, so that profile is changed by reading alone.
- No CI job builds with CMake, so FR-033 needs a fresh local build. AppVeyor builds MSVC Debug but allows it to fail, so FR-033 needs that job's result read, or a fresh local build.
- No references exist, so the Collect step of Cross-Build Check stands in for the gate time in SC-008, as it did for KD-1. It took 3.8 to 6.1 minutes before KD-1.
- Another job on this computer renders in bursts with no warning, so a check made only at the start of a timing run is not enough (FR-025).
- The 20 formulas in FR-021 limit a review. They are not a parity comparison, so the constitution's ban on tolerances does not apply.
