---

description: "Task list for the regression harness"
---

# Tasks: Regression Harness

**Input**: Design documents from `specs/001-regression-harness/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: Included. The spec's independent tests, SC-001, and SC-002 depend on them, and the plan calls for a stand-in renderer because real renders cannot pass until the playbook's Phase 3.

**Organization**: Tasks are grouped by user story. Phase numbers in this file are task phases. "Playbook Phase 3" means the Determinism Foundation in `MIGRATION_PLAYBOOK.md`.

## Rules for Every Task

- The runner talks to the application only through its command line and the export files. It does not include or import any C++.
- The only C++ changes allowed are the verification export and the read-only `--list-formulas` option, and they must only read values that already exist.
- The export is written only after a successful settings load and render.
- Do not capture, create, or commit references. Test fixtures live in temporary directories, and `tests/references/` must not exist when this feature is finished.
- Python uses the standard library only and runs on Python 3.10 or later.
- New C++ must compile as C++14, because qmake sets `CONFIG += c++14` on Linux.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (for example US1)

## Path Conventions

- Harness: `tests/` at the repository root
- Application: `mandelbulber2/src/`

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create the harness files that everything else builds on

- [ ] T001 Add `tests/outputs/` to the build directories section of `.gitignore`
- [ ] T002 [P] Create `tests/scenes.txt` with the comment `# Standard Scene Suite`, then the lines `mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract` and `mandelbulber2/deploy/share/mandelbulber2/examples/Makin3D-Julia_001.fract`
- [ ] T003 [P] Create `tests/formula_sweep_base.fract` with exactly the content below, which rendered all 462 formulas without failures

  ```text
  # Mandelbulber settings file
  # version 2.35
  # only modified parameters
  [main_parameters]
  camera 3 -6 2;
  camera_distance_to_target 7;
  camera_rotation 26.565 -16.60154 0;
  camera_top -0.1277753 0.2555506 0.958314;
  target 0 0 0;
  N 50;
  detail_level 1;
  formula_1 2;
  mat1_is_defined true;
  ```

- [ ] T004 [P] Create `tests/formula_exclusions.txt` with a comment explaining the `name reason` format, then the line `custom Custom OpenCL formula has an empty CPU implementation`
- [ ] T005 Create the skeleton of `tests/regression_runner.py` with a shebang, a docstring listing the four modes, `EXIT_PASSED = 0`, `EXIT_PARITY = 1`, `EXIT_SETUP = 3`, `SetupError` and `RenderError` exceptions, and `main(argv=None)` using `argparse` with a mutually exclusive group of `--self-parity`, `--capture`, and `--compare BINARY`, each returning `EXIT_SETUP` until implemented
- [ ] T006 Add constants to `tests/regression_runner.py`: `SCENE_LIST`, `FORMULA_BASE`, `FORMULA_EXCLUSIONS`, `REF_ROOT = tests/references`, `OUT_DIR = tests/outputs`, `SCENE_RESOLUTION = "320x240"`, `FORMULA_RESOLUTION = "32x24"`, `RENDER_OVERRIDES = "opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4"`, `SCENE_TIMEOUT = 1800`, `FORMULA_TIMEOUT = 120`, `SELF_PARITY_RUNS = 3`, `MAX_REFERENCE_BYTES = 100 * 1024 * 1024`, `EXPORT_VERSION = 1`, and `THREAD_OPTION = None` with a comment that playbook Phase 3 sets it, plus functions that read `MANDELBULBER_BINARY` (default `mandelbulber2/build/appdir/usr/bin/mandelbulber2`) and `PARITY_TARGET` (default `linux-x86_64-gcc`) from the environment at call time

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: The application contract and shared harness code. Every user story needs both.

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

### Application: verification export and formula list

See `contracts/verification-export.md` and `contracts/application-cli.md`.

- [ ] T007 [P] Create `mandelbulber2/src/verification_export.hpp` with the standard project license header and a declaration of `bool WriteVerificationExport(const QString &directory, const std::shared_ptr<cImage> &image, const cStatistics &statistics);`
- [ ] T008 Implement the buffer step of `WriteVerificationExport` in `mandelbulber2/src/verification_export.cpp`: call `QDir().mkpath`, delete any existing `manifest.txt` first, then write `raw_render.bin` from `GetImageFloat()`, `post_effect.bin` from `GetPostImageFloat()`, `final_image.bin` from `GetImage16()`, `depth.bin` from `GetZBuffer()`, and, only when `GetImageOptional()->optionalNormal` is true, `normal.bin` by looping rows and columns over `GetPixelNormal(x, y)`. Files are raw little-endian samples with no header. Add `static_assert` checks that `sizeof(sRGBFloat) == 12` and `sizeof(sRGB16) == 6`. On any write failure, report it with `qCritical()` and return false
- [ ] T009 Implement the statistics step in `mandelbulber2/src/verification_export.cpp`: write `statistics.txt` as UTF-8 with LF line endings, in this order: `total_iterations` from `totalNumberOfIterations`, `raymarch_count` from `numberOfRaymarchings`, `missed_de` from `missedDE`, `iteration_histogram.size`, then `iteration_histogram.<i>` for `i` from 0 to the size inclusive using `GetHist(i)`, then the same for `step_histogram`. Read the public fields rather than the `double` getters, and format with `QString::number` on integer values
- [ ] T010 Implement the manifest step in `mandelbulber2/src/verification_export.cpp`: add a compiler helper that returns `clang ` plus `__clang_version__`, `gcc ` plus `__VERSION__`, `msvc ` plus `_MSC_FULL_VER`, or `unknown`, checking clang before GCC, and a math library helper that returns `glibc ` plus `gnu_get_libc_version()` when `__GLIBC__` is defined, or `unknown`. Write `manifest.txt` last with `QSaveFile`, using the exact line order in `contracts/verification-export.md`: `mandelbulber-verification-export 1`, `width`, `height`, `compiler`, `math_library`, one `buffer` line per file, `statistics statistics.txt`, and `end`
- [ ] T011 Update `mandelbulber2/src/command_line_interface.hpp`: add `bool listFormulas` and `QString verificationExportDir` to `sCliData`, add `bool settingsLoaded` beside `settingsSpecified`, add `cliErrorVerificationExportMode = -80` to `cliErrors`, and declare `[[noreturn]] static void printFormulasAndExit();`
- [ ] T012 Define the options in `mandelbulber2/src/command_line_interface.cpp`: add `QCommandLineOption` entries for `verification-export` (with a value) and `list-formulas` beside `statsOption`, register them with `parser.addOption`, read them into `cliData`, initialize `settingsLoaded` to false, and set `cliData.nogui = true` when `listFormulas` is set, as `listParameters` does
- [ ] T013 Implement `printFormulasAndExit` in `mandelbulber2/src/command_line_interface.cpp`: print `mandelbulber-formula-list 1`, then the numeric ID, a tab, and the internal name for every entry in `newFractalList` except `fractal::none`, then `end`, using `getInternalId()` and `getInternalName()`. Include `formula/definition/all_fractal_list.hpp` and call it from `ReadCLI` next to the `listParameters` check
- [ ] T014 Record the load result in `handleArgs` in `mandelbulber2/src/command_line_interface.cpp`: set `settingsLoaded` to true only when both the load call and `parSettings.Decode(...)` return true, on both the `LoadFromFile` path and the `LoadFromString` path. Do not change any other behavior of those paths
- [ ] T015 Reject unsupported modes at the end of `ReadCLI` in `mandelbulber2/src/command_line_interface.cpp`: if `verificationExportDir` is set and the operational mode is not `modeStill`, show an error with `cErrorMessage::showMessage` and call `parser.showHelp(cliErrorVerificationExportMode)`
- [ ] T016 Update `RenderStillImage` in `mandelbulber2/src/headless.h` and `mandelbulber2/src/headless.cpp`: add `QString verificationExportDir` and `bool settingsLoaded` parameters, keep the `bool` result of `renderJob->Execute()`, and after the existing image save, call `WriteVerificationExport` with `renderJob->GetStatistics()` only when the directory is set, `settingsLoaded` is true, and the render result is true. Make no other change to the render or save steps
- [ ] T017 Pass `cliData.verificationExportDir` and `settingsLoaded` to `RenderStillImage` in `ProcessCLI` in `mandelbulber2/src/command_line_interface.cpp`
- [ ] T018 Build the application and run Scenarios 2 and 3 in `specs/001-regression-harness/quickstart.md`, confirming the file sizes, that `--list-formulas` prints every formula, and that a failed load writes no `manifest.txt`. Also confirm that a render without the new options still works. The existing `.github/workflows/github-actions-ci.yml` compiles Linux on push, if no local Linux build is available

### Harness: shared code

- [ ] T019 [P] Create `tests/stub_renderer.py`, an executable stand-in that implements `contracts/application-cli.md`. It reads its mode from `stub_mode.txt` in its working directory, because the runner's clean environment removes unknown variables, and defaults to `same`. It handles `--list-formulas`, writes a complete export for `--verification-export` from fixed sample data, and writes a 16-bit deflate TIFF to `<output>.tiff` without that option, so the neutral render can match `final_image.bin`. Modes: `same`, `flip-bit:<buffer>`, `flip-mantissa:<buffer>`, `stat-plus-one:<name>`, `unstable` (using a counter file), `no-export`, `no-end`, `bad-version`, `wrong-size:<buffer>`, `missing-buffer:<buffer>`, `extra-buffer`, `bad-statistics`, `bad-settings-text`, `texture-error`, `alternate-lookup`, `exit-3`, `hang`, `neutral-differs`, `resize`, `extra-formula`, `missing-formula`, `no-list-end`
- [ ] T020 [P] Create the scaffolding in `tests/test_regression_runner.py`: a `unittest` base class that builds a temporary root with a `tests/` folder, a scene list, a base scene, and exclusions, copies or wraps `tests/stub_renderer.py` as a launcher in its own folder (a `.cmd` wrapper on Windows), writes `stub_mode.txt`, changes into the temporary root, sets `MANDELBULBER_BINARY`, and calls `regression_runner.main([...])` in-process
- [ ] T021 Add a `Report` class to `tests/regression_runner.py` with the fields in `data-model.md`: "Mode", "Target", "Gate validity", "Toolchain note", "Item results", and "Overall result". It writes each line to the console with `flush=True` and to `tests/outputs/report.txt`, in the format in `contracts/runner-cli.md`, and it tracks parity failures and setup failures separately. If a run has both, it exits 1 and lists both
- [ ] T022 Add `clean_environment(home)` and `gate_valid()` to `tests/regression_runner.py`: build the environment from an allowlist of `PATH`, `LD_LIBRARY_PATH`, `SYSTEMROOT`, `TEMP`, `TMP`, and `TMPDIR`, then set `HOME`, `LC_ALL=C.UTF-8`, and `LANG=C.UTF-8`. `gate_valid()` returns false on Windows, because the application ignores a redirected home folder there (FR-004, FR-005)

**Checkpoint**: The application contract is in place and the harness can drive a stand-in renderer

---

## Phase 3: User Story 1 - Never Pass What Was Not Verified (Priority: P1) 🎯 MVP

**Goal**: The harness decides each outcome from evidence it checks itself, so a bad scene, a failed render, or an invalid export can never count as a pass.

**Independent Test**: Run the broken inputs one at a time and confirm that each fails and names the cause. No references or reproducible renders are needed.

### Tests for User Story 1

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T023 [US1] Add scene list tests to `tests/test_regression_runner.py`: an empty list, a comment-only list, a missing scene, two scenes with the same file name, a scene with a URL asset, and a scene with an absolute-path asset each raise a setup failure that names the cause before any render runs
- [ ] T024 [US1] Add formula list tests to `tests/test_regression_runner.py`: `no-list-end` fails, an exclusion with no reason fails, an exclusion that names a formula the binary does not offer fails, and excluded formulas do not become work items
- [ ] T025 [US1] Add render failure tests to `tests/test_regression_runner.py` using the modes `bad-settings-text`, `texture-error`, `alternate-lookup`, `exit-3`, and `hang`, confirming that each fails that item, names the cause, and that a stale export from an earlier run cannot satisfy the check
- [ ] T026 [US1] Add export validation tests to `tests/test_regression_runner.py` using the modes `no-export`, `no-end`, `bad-version`, `wrong-size:<buffer>`, `missing-buffer:<buffer>`, `extra-buffer`, and `bad-statistics`, confirming that each fails that item and that the remaining items still run

### Implementation for User Story 1

- [ ] T027 [US1] Implement `load_scenes()` in `tests/regression_runner.py`, enforcing: "Names are unique within their kind. Two scenes with the same file name fail the run (FR-002).", "The scene list must not be empty, and every listed file must exist (FR-002).", and "A scene must not reference an asset by URL or absolute path (FR-002)." Ignore blank lines and lines starting with `#`, and detect assets with the tested pattern `^[A-Za-z0-9_]*(file|texture)[A-Za-z0-9_]* +"?(https?://|/|[A-Za-z]:[/\\])`
- [ ] T028 [US1] Implement `load_formulas(binary)` in `tests/regression_runner.py`: run the binary with `--list-formulas`, read the lines between `mandelbulber-formula-list 1` and `end`, and raise a setup failure if either marker is missing or the list is empty
- [ ] T029 [US1] Implement `load_exclusions()` and `build_work_items(binary)` in `tests/regression_runner.py`, with a Work Item of kind, name, source, formula ID, resolution, and time limit, enforcing: "Every formula from `--list-formulas` is a work item unless it is listed in `tests/formula_exclusions.txt` (FR-034)." and "An exclusion needs a reason and must name a formula the binary offers." Scenes use `SCENE_RESOLUTION` and `SCENE_TIMEOUT`, and formulas use `FORMULA_BASE`, `FORMULA_RESOLUTION`, `FORMULA_TIMEOUT`, and `formula_1=<id>` added to the overrides
- [ ] T030 [US1] Implement `render_item(item, binary, output_dir, thread_args=(), neutral=False)` in `tests/regression_runner.py`: remove earlier output, create a temporary home for this render, build the command from `contracts/application-cli.md`, run it with `clean_environment`, the item's time limit, and the binary's folder as the working directory, and save its output to a log file. A neutral render leaves out `normal_enabled=1` and `--verification-export`, and adds `color_quality=1`
- [ ] T031 [US1] Implement output checks in `tests/regression_runner.py`: treat a non-zero exit, a timeout, and any of `not valid Mandelbulber settings file`, `No materials defined`, `Can't load texture`, or `Looking for the file at` as a render failure that names the item and points to its log. A zero exit status alone is never success (FR-008)
- [ ] T032 [US1] Implement `read_export(directory)` in `tests/regression_runner.py`, returning width, height, compiler, math library, buffers, and statistics, and enforcing: "The manifest exists and ends with its `end` line. This proves completeness (FR-008).", "The format version is one the harness understands.", "Exactly the five expected buffers are present (FR-011).", "Each buffer file is exactly `width × height × channels × sample size` bytes.", and "The statistics record is complete and contains only whole numbers." The expected buffers are `raw_render` f32 3, `post_effect` f32 3, `final_image` u16 3, `depth` f32 1, and `normal` f32 3
- [ ] T033 [US1] Add `render_and_validate(item, ...)` to `tests/regression_runner.py`: it combines the render, output checks, and export validation, records a failure in the `Report`, and lets the run continue with the remaining items. A setup error before rendering stops the run

**Checkpoint**: Broken inputs fail with a named cause, and the User Story 1 tests pass

---

## Phase 4: User Story 2 - Prove a Build Reproduces Its Own Output (Priority: P1)

**Goal**: `--self-parity` shows whether a build reproduces its own output. It reports KD-1 today and measures playbook Phase 3.

**Independent Test**: Run self-parity against today's build and confirm it reports differences. Run it against the stand-in in `same` mode and confirm it passes.

### Tests for User Story 2

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T034 [US2] Add self-parity tests to `tests/test_regression_runner.py`: with `THREAD_OPTION` patched to `--threads`, `same` mode passes and every item renders three times at each thread count plus one neutral render, `unstable` fails and reports differing samples per item and buffer, and `flip-mantissa:<buffer>` is caught for every buffer
- [ ] T035 [US2] Add coverage tests to `tests/test_regression_runner.py`: with `THREAD_OPTION = None`, a run with identical output exits 3 and states that thread coverage is incomplete, and a run with differing output exits 1 and reports both
- [ ] T036 [US2] Add neutral render tests to `tests/test_regression_runner.py`: `neutral-differs` fails that item, and a matching neutral render passes

### Implementation for User Story 2

- [ ] T037 [US2] Implement `compare_exports(first, second)` in `tests/regression_runner.py`, returning a difference list. "Two buffers match only if their dimensions, sample type, and every byte are identical (FR-013, FR-014)." Report differing samples rather than bytes, using the sample size of each buffer
- [ ] T038 [US2] Port `read_tiff` from the prototype in `git show fbcc70492:MIGRATION_PLAYBOOK.md` to `tests/regression_runner.py`, and add `compare_neutral(export, tiff_path)`, which compares the decoded 16-bit samples with the `final_image` buffer (FR-012)
- [ ] T039 [US2] Implement `thread_settings()` in `tests/regression_runner.py`: when `THREAD_OPTION` is set, return thread counts of 1 and the CPU count, or 1 and 2 on a single-CPU machine. When it is `None`, return one default setting and mark coverage as incomplete (FR-017)
- [ ] T040 [US2] Implement `check_self_parity(items)` in `tests/regression_runner.py`: run the all-thread renders and the neutral render one item at a time, then run the single-thread renders with a `concurrent.futures.ThreadPoolExecutor` of one worker per CPU, keeping each item's three runs consecutive. Compare every result with the first export for that item (FR-016)
- [ ] T041 [US2] Wire `--self-parity` into `main` in `tests/regression_runner.py`: report results, remove outputs of passing items, keep outputs of failing items, and return the correct exit code (FR-017, FR-029, FR-030)
- [ ] T042 [US2] Run Scenario 4 and Scenario 7 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm differences are reported per item and buffer

**Checkpoint**: `--self-parity` passes on the stand-in and correctly reports KD-1 on the real build

---

## Phase 5: User Story 3 - Detect Drift Against References (Priority: P2)

**Goal**: The default gate mode fails on any difference from a target's references, including formulas no scene uses.

**Independent Test**: With the stand-in and references written to a temporary folder, identical output passes and one flipped bit fails.

### Tests for User Story 3

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T043 [US3] Add reference format tests to `tests/test_regression_runner.py`: a reference zip round-trips every buffer exactly, entries appear in the order in `contracts/reference-set.md`, a corrupted entry is rejected, and writing the same data twice gives byte-identical files
- [ ] T044 [US3] Add gate result tests to `tests/test_regression_runner.py`: identical output passes, `flip-bit:<buffer>` names the item, buffer, and sample count for every buffer, `flip-mantissa:<buffer>` fails even though it disappears at 16-bit, and `resize` reports a layout difference
- [ ] T045 [US3] Add gate coverage tests to `tests/test_regression_runner.py`: a scene or formula with no reference fails and is named, a leftover reference fails and is named, `extra-buffer` fails, a formula-only change fails on that formula, a missing reference set exits 3 and does not pass with zero comparisons, and changed render settings exit 3
- [ ] T046 [US3] Add gate safety tests to `tests/test_regression_runner.py`: a toolchain difference appears in the report without changing a passing verdict, and a gate run leaves every reference file unchanged

### Implementation for User Story 3

- [ ] T047 [US3] Implement `split_planes` and `join_planes` in `tests/regression_runner.py`, using one plane per sample byte: 4 for `f32` and 2 for `u16`
- [ ] T048 [US3] Implement `write_reference(path, export)` and `read_reference(path)` in `tests/regression_runner.py` with `zipfile`, following `contracts/reference-set.md`: entries in the listed order, deflate at level 9, a fixed `ZipInfo` timestamp of 1980-01-01 00:00:00, and checksum verification on read
- [ ] T049 [US3] Implement `load_reference_set(target)` in `tests/regression_runner.py`: read `toolchain.txt` and `settings.txt`, and raise a setup failure if the set is missing or its settings differ from the current `export_format`, `scene_resolution`, `formula_resolution`, or `overrides`
- [ ] T050 [US3] Implement `compare_against_references(items)` in `tests/regression_runner.py`: render each item once and compare with `compare_exports`. Enforce "Every work item has exactly one reference, and every reference has a work item (FR-019, FR-034).", treating a mismatch as a parity failure that names the item
- [ ] T051 [US3] Wire the default gate into `main` in `tests/regression_runner.py`: report the target and a toolchain note when versions differ, without changing the verdict (FR-023), remove outputs of passing items, and never write to `tests/references/` (FR-020)

**Checkpoint**: The gate passes and fails correctly against test references, and no reference exists in the repository

---

## Phase 6: User Story 4 - Capture References for a Baseline Change (Priority: P2)

**Goal**: `--capture` writes a complete reference set only from a build proven to reproduce its own output.

**Independent Test**: A stable stand-in writes a complete set to a temporary folder. An unstable one writes nothing.

### Tests for User Story 4

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T052 [US4] Add capture success tests to `tests/test_regression_runner.py`: with `THREAD_OPTION` patched, capture writes every scene and formula reference plus `toolchain.txt` and `settings.txt`, a gate run then passes against that set, and a second capture of unchanged data is byte-identical
- [ ] T053 [US4] Add capture refusal tests to `tests/test_regression_runner.py`: an `unstable` build fails and leaves existing references byte-for-byte unchanged, `THREAD_OPTION = None` refuses before any render runs, a not gate-valid platform refuses, a set over `MAX_REFERENCE_BYTES` fails, and a failure part way through the list leaves no partial set

### Implementation for User Story 4

- [ ] T054 [US4] Implement `stage_references(items, exports)` in `tests/regression_runner.py`: reuse each item's first self-parity export, build the set in a staging folder under `tests/outputs/`, and write `toolchain.txt` and `settings.txt`. Fail if toolchains differ between items
- [ ] T055 [US4] Implement `promote_references(staging, target)` in `tests/regression_runner.py`, enforcing "A set is replaced whole or not at all (FR-018)." and "Its stored size must stay under 100 MB (SC-008). Capture fails otherwise." Check completeness and size, move any existing set aside, move the new set into place, then delete the old one, restoring it if the move fails
- [ ] T056 [US4] Wire `--capture` into `main` in `tests/regression_runner.py`: refuse before rendering if thread coverage is incomplete or the run is not gate-valid, run self-parity with its exports kept, then stage and promote only if every item passed
- [ ] T057 [US4] Run Scenario 5 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm that capture refuses and that no `tests/references/` folder appears

**Checkpoint**: Capture is all or nothing, and nothing was captured from a real build

---

## Phase 7: User Story 5 - Catch Computation Drift With Statistics (Priority: P3)

**Goal**: A change of 1 in any statistic fails every mode, even when no pixel changes.

**Independent Test**: With identical buffers, a change of 1 in one count fails and names the statistic and both values.

### Tests for User Story 5

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T058 [US5] Add statistics comparison tests to `tests/test_regression_runner.py`: `stat-plus-one:<name>` fails for `total_iterations`, `raymarch_count`, `missed_de`, a normal bucket, and the overflow bucket of each histogram, in self-parity, gate, and compare modes, and each failure names the statistic and both values
- [ ] T059 [US5] Add statistics format tests to `tests/test_regression_runner.py`: the stand-in record parses into a fixed order, and a value that is not a whole number or a record with time-based names is rejected

### Implementation for User Story 5

- [ ] T060 [US5] Add `compare_statistics(first, second)` to `tests/regression_runner.py` and call it from `compare_exports`, so every mode compares statistics exactly and each mismatch names the statistic and both values (FR-028)
- [ ] T061 [US5] Confirm that `mandelbulber2/src/verification_export.cpp` writes only the fields in `contracts/verification-export.md`, excluding `time`, `totalNoise`, `maskedPixels`, `tilesDone`, and every rate (FR-026)

**Checkpoint**: Statistics are compared exactly in every mode. Real builds are expected to differ until playbook Phase 3 fixes the shared counters

---

## Phase 8: User Story 6 - Check a Change Locally Before Pushing (Priority: P3)

**Goal**: `--compare BINARY` compares two builds on one machine, on any toolchain, with no references.

**Independent Test**: Two stand-in binaries with identical output pass, and one flipped bit fails.

### Tests for User Story 6

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T062 [US6] Add compare mode tests to `tests/test_regression_runner.py` with two stand-in launchers in separate folders: identical output passes, a flipped bit in either build fails and names the item, a failed render on either build names that build, `extra-formula` and `missing-formula` are reported as differences, and the run works with no references and leaves any reference file untouched
- [ ] T063 [US6] Add a report test to `tests/test_regression_runner.py`: compare mode prints both toolchains and states that it is a local check that does not replace the gate

### Implementation for User Story 6

- [ ] T064 [US6] Implement `compare_builds(items, other_binary)` in `tests/regression_runner.py`: render each item with both binaries using the same fixed settings as the gate, compare with `compare_exports`, and name the build in any render failure (FR-035, FR-036)
- [ ] T065 [US6] Wire `--compare BINARY` into `main` in `tests/regression_runner.py`: build the formula list from both binaries, report a formula offered by only one as a difference, print both toolchains and the local-check statement, and do not read or modify references (FR-037)
- [ ] T066 [US6] Run Scenario 6 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm that it reports differences until renders reproduce

**Checkpoint**: All four modes work

---

## Phase 9: Polish & Cross-Cutting Concerns

**Purpose**: Complete the scene suite and confirm the feature's limits

- [ ] T067 Expand `tests/scenes.txt` to about 25 scenes with a total capped render time of about 90 seconds on 32 threads: about 19 ordinary scenes at up to 2 seconds and 6 Monte Carlo scenes at up to 9 seconds. It must exercise Monte Carlo depth of field, global illumination, and ambient occlusion, including `difs_tree.fract`, `chromatic_aberration.fract`, and `IFS_001.fract`, and it must pass the asset check in `load_scenes()`
- [ ] T068 Review `tests/test_regression_runner.py` against the requirements in `specs/001-regression-harness/spec.md`, and add a test for any failure condition without one, so SC-001 and SC-002 are covered
- [ ] T069 Format `mandelbulber2/src/verification_export.hpp`, `mandelbulber2/src/verification_export.cpp`, and the changed lines in `mandelbulber2/src/command_line_interface.cpp`, `mandelbulber2/src/command_line_interface.hpp`, `mandelbulber2/src/headless.cpp`, and `mandelbulber2/src/headless.h` to match `.clang-format`
- [ ] T070 Run `python3 tests/test_regression_runner.py` and all scenarios in `specs/001-regression-harness/quickstart.md`, and record any result that differs from what the guide expects
- [ ] T071 Check the feature's limits with `git diff --stat master`: the only changed C++ files are the six listed in T069, nothing changed in the render path, and `tests/references/` does not exist

---

## Requirement Coverage

| Requirements | Tasks |
|---|---|
| FR-001, FR-002 | T002, T023, T027 |
| FR-003 to FR-007 | T006, T022, T025, T030 |
| FR-008, FR-009 | T025, T026, T031, T032, T033 |
| FR-010 to FR-012 | T008, T026, T032, T036, T038 |
| FR-013, FR-014 | T034, T037, T044 |
| FR-015 to FR-020 | T005, T034, T035, T039 to T041, T050, T051, T054 to T056 |
| FR-021 to FR-023 | T006, T021, T046, T049, T051, T054 |
| FR-024 to FR-028 | T008, T009, T010, T012, T014, T016, T058 to T061 |
| FR-029 to FR-031 | T021, T041, T071 |
| FR-032 to FR-034 | T003, T004, T024, T028, T029, T045 |
| FR-035 to FR-037 | T062 to T065 |

| Success criterion | Tasks | Can close in this feature |
|---|---|---|
| SC-001 | T023 to T026, T068 | Yes |
| SC-002 | T034, T044, T058 | Yes |
| SC-003 | T042 | First half only. The rest needs playbook Phase 3 |
| SC-004 | T067 | No. It needs the CI gate from playbook Phase 4 |
| SC-005 | T021, T044 | Yes |
| SC-006 | T053 | Yes |
| SC-007 | T036 | Logic only. Real results need playbook Phase 3 |
| SC-008 | T053, T055 | Enforced here, measured in playbook Phase 4 |
| SC-009 | T029, T045 | Yes |
| SC-010 | T062, T066 | Logic only. Real results need playbook Phase 3 |

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies
- **Foundational (Phase 2)**: Depends on Setup. Blocks all user stories
- **User Stories (Phases 3 to 8)**: Depend on Foundational
- **Polish (Phase 9)**: Depends on every story being complete

### User Story Dependencies

- **User Story 1 (P1)**: Starts after Foundational. Every other story uses its checked render path
- **User Story 2 (P1)**: Depends on US1
- **User Story 3 (P2)**: Depends on US1, and on `compare_exports` from T037
- **User Story 4 (P2)**: Depends on US2 and on the reference format from US3
- **User Story 5 (P3)**: Depends on `compare_exports` from T037
- **User Story 6 (P3)**: Depends on US1, and on `compare_exports` from T037

### Within Each User Story

- Tests are written first and must fail before implementation
- Helpers come before the mode that uses them
- Real binary checks come last

### Parallel Opportunities

- **Phase 1**: T002, T003, and T004
- **Phase 2**: T007 can run with T019 and T020. The C++ tasks (T007 to T018) and the shared harness tasks (T019 to T022) are separate tracks, and one person can work on each
- **After US2**: US5 and US6 are independent of US3 and US4, but they edit the same files, so run them one at a time unless working on separate branches
- **Within a story**: There are no `[P]` tasks, because every story edits `tests/regression_runner.py` and `tests/test_regression_runner.py`

---

## Parallel Example: Setup and Foundational

```bash
# Phase 1: different files, no dependencies
Task: "Create tests/scenes.txt"
Task: "Create tests/formula_sweep_base.fract"
Task: "Create tests/formula_exclusions.txt"

# Phase 2: separate tracks
Task: "Create mandelbulber2/src/verification_export.hpp"
Task: "Create tests/stub_renderer.py"
Task: "Create the scaffolding in tests/test_regression_runner.py"
```

---

## Implementation Strategy

### MVP First (User Stories 1 and 2)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational
3. Complete Phase 3: User Story 1
4. Complete Phase 4: User Story 2
5. **STOP and VALIDATE**: `--self-parity` reports KD-1 on the real build, with every failure named

This is the smallest useful result, and it is what playbook Phase 3 needs in order to start. User Story 1 alone has no mode to run.

### Incremental Delivery

1. Add User Story 3, so the gate works against test references
2. Add User Story 4, so capture is ready for playbook Phase 4
3. Add User Story 5, so statistics are compared
4. Add User Story 6, so developers have a local check for playbook Phase 5

---

## Notes

- Commit after each task or logical group, and after each checkpoint at the latest
- `--self-parity`, the gate, and compare mode are expected to report differences on real builds until playbook Phase 3 is done. That is a correct result, not a bug in the harness
- Keep the C++ commits separate from the harness commits, so the application change is easy to review
- Do not add CI workflows here. They belong to playbook Phases 3 and 4
