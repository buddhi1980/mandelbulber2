---

description: "Task list for the regression harness"
---

# Tasks: Regression Harness

**Input**: Design documents from `specs/001-regression-harness/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

**Tests**: Included. The spec's independent tests, SC-001, and SC-002 depend on them, and the plan calls for a stand-in renderer because real renders cannot pass until the playbook's Phase 3.

**Organization**: Tasks are grouped by user story, in priority order, so User Story 5 comes before User Stories 3 and 4. Phase numbers in this file are task phases. "Playbook Phase 3" means the Determinism Foundation in `MIGRATION_PLAYBOOK.md`.

## Rules for Every Task

- The runner talks to the application only through its command line and the export files. It does not include or import any C++.
- The only C++ changes allowed are the verification export and the read-only `--list-formulas` option, and they must only read values that already exist.
- The export is written only after a successful settings load and render.
- Do not capture, create, or commit references. Test fixtures live in temporary directories, and `tests/references/` must not exist when this feature is finished.
- Python uses the standard library only and runs on Python 3.10 or later.
- New C++ must compile as C++17, the lowest standard Qt 6 accepts. `CONFIG += c++14` in `common.pri` has no effect under Qt 6. The MSVC project builds it as C++20, so it must compile there too.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (for example US1)

## Path Conventions

- Harness: `tests/` at the repository root
- Application: `mandelbulber2/src/`

## Build Environments

Development uses two builds of one commit on one computer. The playbook's Local Development section has the exact commands.

- Harness tasks use the stand-in renderer and need no application build. They run in WSL and on Windows.
- Real binary tasks (T018, T044, T064, T069, T081, T092, and T095) need a fresh build of the current source with the C++ changes. No task relies on an existing binary.
- WSL: use the `Ubuntu-24.04` distro, which has the playbook's packages, with GCC 13.3.0 and Qt 6.4.2 to match the runner. The default `Ubuntu` distro is 26.04 and has no Qt, so name the distro with `wsl -d Ubuntu-24.04`. Build from a Linux clone, not `/mnt/c`, because the Windows checkout has CRLF line endings. A commit reaches that clone with `git fetch /mnt/c/GIT/mandelbulber2 <branch>`. Run `qmake6` again after adding a source file, such as `verification_export.cpp`, because it expands the source wildcards only when it runs.
- Windows: run `nuget restore` on `mandelbulber2/msvc/mandelbulber2.sln`, then build it with MSBuild as Release x64. Delete `build-win` before the first build, because the project compiles every `.cpp` file in that folder. This covers Scenario 7 and half of Scenario 8, but the other real binary scenarios need Linux, and a Windows gate or self-parity run cannot pass.
- GitHub Actions: T086 to T089 and T095 need the branch pushed. The Cross-Build Check pipeline from User Story 8 runs the harness on both builds. The existing `.github/workflows/github-actions-ci.yml` does not run the harness, and it must not change.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create the harness files that everything else builds on

- [X] T001 Add `tests/outputs/` to the build directories section of `.gitignore`
- [X] T002 [P] Create `tests/scenes.txt` with the comment `# Standard Scene Suite`, then the lines `mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract` and `mandelbulber2/deploy/share/mandelbulber2/examples/Makin3D-Julia_001.fract`
- [X] T003 [P] Create `tests/formula_sweep_base.fract` with exactly the content below, which rendered all 462 formulas without failures

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

- [X] T004 [P] Create `tests/formula_exclusions.txt` with a comment explaining the `name reason` format, then the line `custom Custom OpenCL formula has an empty CPU implementation`. This formula exists only in the OpenCL build, which is the build the harness expects
- [X] T005 Create the skeleton of `tests/regression_runner.py` with a shebang, a docstring listing the six modes, `EXIT_PASSED = 0`, `EXIT_PARITY = 1`, `EXIT_SETUP = 3`, `SetupError` and `RenderError` exceptions, and `main(argv=None)` using `argparse` with a mutually exclusive group of `--self-parity`, `--capture`, `--compare BINARY`, `--collect DIR`, and `--cross-check FIRST SECOND`, each returning `EXIT_SETUP` until implemented. `main` catches `SetupError` and any unexpected exception, prints the cause, and returns `EXIT_SETUP`, so exit 1 only ever means that a comparison failed
- [X] T006 Add constants to `tests/regression_runner.py`: `SCENE_LIST`, `FORMULA_BASE`, `FORMULA_EXCLUSIONS`, `REF_ROOT = tests/references`, `OUT_DIR = tests/outputs`, `SCENE_RESOLUTION = "320x240"`, `FORMULA_RESOLUTION = "32x24"`, `RENDER_OVERRIDES = "opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4"`, `SCENE_TIMEOUT = 1800`, `FORMULA_TIMEOUT = 120`, `SELF_PARITY_RUNS = 3`, `MAX_REFERENCE_BYTES = 100 * 1024 * 1024`, `EXPORT_VERSION = 1`, and `THREAD_OPTION = None` with a comment that playbook Phase 3 sets it, plus functions that read `MANDELBULBER_BINARY` (default `mandelbulber2/build/appdir/usr/bin/mandelbulber2` on Linux and `build-win/Release/x64/mandelbulber2.exe` on Windows) and `PARITY_TARGET` (default `linux-x86_64-gcc` on Linux and `windows-x86_64-msvc` on Windows) from the environment at call time

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: The application contract and shared harness code. Every user story needs both.

**⚠️ CRITICAL**: No user story work can begin until T007 to T017 and T019 to T022 are complete. T018 blocks only the real binary tasks, because harness tasks run on the stand-in

### Application: verification export and formula list

See `contracts/verification-export.md` and `contracts/application-cli.md`.

- [X] T007 [P] Create `mandelbulber2/src/verification_export.hpp` with the standard project license header and a declaration of `bool WriteVerificationExport(const QString &directory, const std::shared_ptr<cImage> &image, const cStatistics &statistics);`
- [X] T008 Implement the buffer step of `WriteVerificationExport` in `mandelbulber2/src/verification_export.cpp`: call `QDir().mkpath`, delete any existing `manifest.txt` first, then write `raw_render.bin` from `GetImageFloat()`, `post_effect.bin` from `GetPostImageFloat()`, `final_image.bin` from `GetImage16()`, `alpha.bin` from `GetAlphaBuf()`, and `depth.bin` from `GetZBuffer()`. Then, only when `GetImageOptional()->optionalNormal` is true, write `normal.bin` by looping rows and columns over `GetPixelNormal(x, y)`, and only when `optionalNormalWorld` is true, write `normal_world.bin` the same way over `GetPixelNormalWorld(x, y)`. Files are raw little-endian samples with no header. Add `static_assert` checks that `sizeof(sRGBFloat) == 12` and `sizeof(sRGB16) == 6`. On any write failure, report it with `qCritical()` and return false
- [X] T009 Implement the statistics step in `mandelbulber2/src/verification_export.cpp`: write `statistics.txt` as UTF-8 with LF line endings, in this order: `total_iterations` from `totalNumberOfIterations`, `raymarch_count` from `numberOfRaymarchings`, `missed_de` from `missedDE`, `iteration_histogram.size`, then `iteration_histogram.<i>` for `i` from 0 to the size inclusive using `GetHist(i)`, then the same for `step_histogram`. Read the public fields rather than the `double` getters, and format with `QString::number` on integer values
- [X] T010 Implement the manifest step in `mandelbulber2/src/verification_export.cpp`: add a compiler helper that returns `clang ` plus `__clang_version__`, `gcc ` plus `__VERSION__`, `msvc ` plus `_MSC_FULL_VER`, or `unknown`, checking clang before GCC, and a math library helper that returns `glibc ` plus `gnu_get_libc_version()` when `__GLIBC__` is defined, or `unknown`. Write `manifest.txt` last with `QSaveFile`, using the exact line order in `contracts/verification-export.md`: `mandelbulber-verification-export 1`, `width`, `height`, `compiler`, `math_library`, one `buffer` line per file, `statistics statistics.txt`, and `end`
- [X] T011 Update `mandelbulber2/src/command_line_interface.hpp`: add `bool listFormulas` and `QString verificationExportDir` to `sCliData`, add `bool settingsLoaded` beside `settingsSpecified`, add `cliErrorVerificationExportMode = -80` to `cliErrors`, and declare `[[noreturn]] static void printFormulasAndExit();`
- [X] T012 Define the options in `mandelbulber2/src/command_line_interface.cpp`: add `QCommandLineOption` entries for `verification-export` (with a value) and `list-formulas` beside `statsOption`, register them with `parser.addOption`, read them into `cliData`, initialize `settingsLoaded` to false, and set `cliData.nogui = true` when `listFormulas` is set, as `listParameters` does
- [X] T013 Implement `printFormulasAndExit` in `mandelbulber2/src/command_line_interface.cpp`: print `mandelbulber-formula-list 1`, then the numeric ID, a tab, and the internal name for every entry in `newFractalList` except `fractal::none`, then `end`, using `getInternalId()` and `getInternalName()`. Include `formula/definition/all_fractal_list.hpp` and call it from `ReadCLI` next to the `listParameters` check
- [X] T014 Record the load result in `handleArgs` in `mandelbulber2/src/command_line_interface.cpp`: set `settingsLoaded` to true only when both the load call and `parSettings.Decode(...)` return true, on both the `LoadFromFile` path and the `LoadFromString` path. Do not change any other behavior of those paths
- [X] T015 Reject unsupported modes at the start of `ReadCLI` in `mandelbulber2/src/command_line_interface.cpp`, before the test, benchmark, server, and client handling. Run this check only when `verificationExportDir` is set, so nothing changes without the option. Then, if `nogui` is false, if any of `keyframe`, `flight`, `queue`, `voxel`, `server`, `test`, or `benchmark` is set, or if `host` is not empty, show an error with `cErrorMessage::showMessage` and call `parser.showHelp(cliErrorVerificationExportMode)`. The check must come first, because `--server` stays in `modeStill` and `--host` returns early
- [X] T016 Update `RenderStillImage` in `mandelbulber2/src/headless.h` and `mandelbulber2/src/headless.cpp`: add `QString verificationExportDir` and `bool settingsLoaded` parameters, keep the `bool` result of `renderJob->Execute()`, and after the existing image save, call `WriteVerificationExport` with `renderJob->GetStatistics()` only when the directory is set, `settingsLoaded` is true, and the render result is true. Make no other change to the render or save steps
- [X] T017 Pass `cliData.verificationExportDir` and `settingsLoaded` to `RenderStillImage` in `ProcessCLI` in `mandelbulber2/src/command_line_interface.cpp`
- [ ] T018 Build the application with both build systems, as described under Build Environments, because the new C++ must compile with GCC and with MSVC. On the WSL build, run Scenarios 2 and 3 in `specs/001-regression-harness/quickstart.md`, confirming the file sizes, that `--list-formulas` prints every formula, and that a failed load writes no `manifest.txt`. Also confirm that a render without the new options still works, and that `--verification-export` with `--server` or `--host` is rejected. On the Windows build, confirm that one render writes a complete export and that `--list-formulas` prints the same formulas

### Harness: shared code

- [ ] T019 [P] Create `tests/stub_renderer.py`, an executable stand-in that implements `contracts/application-cli.md`. It reads its mode from `stub_mode.txt` in its working directory, because the runner's clean environment removes unknown variables, and defaults to `same`. It appends each call's arguments and environment to `stub_calls.jsonl` in that folder, accepts and ignores `--threads N`, handles `--list-formulas` and `--version`, printing `Mandelbulber 2.35-test` for the latter, writes a complete export for `--verification-export` from fixed sample data, and writes a 16-bit deflate TIFF to `<output>.tiff` without that option, so the neutral render can match `final_image.bin`. A mode written as `only:<item>:<mode>` applies to one scene name or formula ID, and every other item behaves as `same`. Modes: `same`, `flip-bit:<buffer>`, `flip-mantissa:<buffer>`, `stat-plus-one:<name>`, `unstable` (using a counter file), `unstable:<buffer>`, `unstable-stat:<name>`, `no-export`, `no-end`, `bad-version`, `wrong-size:<buffer>`, `wrong-type:<buffer>`, `missing-buffer:<buffer>`, `extra-buffer`, `bad-statistics`, `no-statistics`, `bad-settings-text`, `settings-line-error`, `texture-error`, `alternate-lookup`, `system-data-dir`, `print:<text>` (prints the text and otherwise behaves as `same`), `exit-3`, `hang` (sleeps 30 seconds and then exits, because on Windows a timeout kills the `.cmd` launcher but not its child), `neutral-differs`, `no-neutral-image`, `resize`, `other-toolchain`, `other-toolchain-drift`, `extra-formula`, `missing-formula`, `renumber-formulas`, `no-list-end`, `blank` (every `final_image` sample holds one value), `other-version` (prints a different `--version` line), `other-histogram-size` (adds one bucket to the iteration histogram)
- [ ] T020 [P] Create the scaffolding in `tests/test_regression_runner.py`: a `unittest` base class that builds a temporary root with a `tests/` folder, a scene list, a base scene, and exclusions, using `tempfile.TemporaryDirectory(ignore_cleanup_errors=True)`, because a stand-in that timed out can still hold files open on Windows, copies or wraps `tests/stub_renderer.py` as a launcher in its own folder (a `.cmd` wrapper on Windows), writes `stub_mode.txt`, changes into the temporary root, sets `MANDELBULBER_BINARY` and `PARITY_TARGET`, because the default target differs on Windows, patches `gate_valid` to return true and sets `CI=true` unless a test overrides them, patches `source_state` to a fixed commit and commit time with no uncommitted changes, patches the time limits to a few seconds, and calls `regression_runner.main([...])` in-process. End the file with a `unittest.main()` entry point, so running it directly exits non-zero on a failure
- [ ] T021 Add a `Report` class to `tests/regression_runner.py` with the fields in `data-model.md`: "Mode", "Environment", "Target", "Gate validity", "Toolchain note", "Item results", and "Overall result". It writes every line to `tests/outputs/report.txt` and to the console with `flush=True`, in the format in `contracts/runner-cli.md`, except that the console shows at most the first 5 statistic differences per item and comparison, followed by a count of the rest. Both are guarded by a `threading.Lock` because self-parity uses worker threads. It tracks parity failures, setup failures, and Reference Target mismatches separately, and follows the exit code rules in that contract. A functional disagreement in a cross-check counts as a parity failure for the exit code, but the result line calls it a functional disagreement
- [ ] T022 Add `clean_environment(home)` and `gate_valid()` to `tests/regression_runner.py`: build the environment from an allowlist of `PATH`, `SYSTEMROOT`, `TEMP`, `TMP`, and `TMPDIR`, then set `HOME`, `LC_ALL=C.UTF-8`, and `LANG=C.UTF-8`. Leave out `LD_LIBRARY_PATH`, because it can select a different math library. `gate_valid()` returns false on Windows, because the application ignores a redirected home folder there (FR-004, FR-005)

**Checkpoint**: The application contract is in place and the harness can drive a stand-in renderer

---

## Phase 3: User Story 1 - Never Pass What Was Not Verified (Priority: P1) 🎯 MVP

**Goal**: The harness decides each outcome from evidence it checks itself, so a bad scene, a failed render, or an invalid export can never count as a pass.

**Independent Test**: Run the broken inputs one at a time and confirm that each fails and names the cause. No references or reproducible renders are needed.

### Tests for User Story 1

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T023 [US1] Add scene list tests to `tests/test_regression_runner.py`: an empty list, a comment-only list, a missing scene, two scenes with the same file name, a scene with a URL asset, a scene with an absolute-path asset, and a scene with `nebula_mode true` each raise a setup failure that names the cause before any render runs
- [ ] T024 [US1] Add formula list tests to `tests/test_regression_runner.py`: a missing binary raises a setup failure that names the path, `no-list-end` fails, an exclusion with no reason fails, an exclusion that names a formula the binary does not offer fails, and excluded formulas do not become work items
- [ ] T025 [US1] Add render failure tests to `tests/test_regression_runner.py` using the modes `bad-settings-text`, `settings-line-error`, `texture-error`, `alternate-lookup`, `system-data-dir`, `exit-3`, and `hang`, confirming that each fails that item, names the cause, and that a stale export from an earlier run cannot satisfy the check. Also use `print:<text>` with each failure message in T032, so a mistyped message cannot go unnoticed
- [ ] T026 [US1] Add export validation tests to `tests/test_regression_runner.py` using the modes `no-export`, `no-end`, `bad-version`, `wrong-size:<buffer>`, `wrong-type:<buffer>`, `missing-buffer:<buffer>`, `extra-buffer`, `bad-statistics`, and `no-statistics`, confirming that each fails that item, and with `only:<item>:<mode>`, that the remaining items still run and pass
- [ ] T027 [US1] Add command and isolation tests to `tests/test_regression_runner.py`, reading `stub_calls.jsonl`: scene and formula renders use `--nogui`, the fixed resolution, and `RENDER_OVERRIDES` in one `--override` option, formula renders add `formula_1=<id>`, every path is absolute, the output folder exists before the render starts, each render gets a different `HOME`, `LC_ALL` is `C.UTF-8`, and variables outside the allowlist, including `LD_LIBRARY_PATH` and `OMP_NUM_THREADS`, do not reach the render. The `--list-formulas` call gets the same environment rules

### Implementation for User Story 1

- [ ] T028 [US1] Implement `load_scenes()` in `tests/regression_runner.py`, enforcing: "Names are unique within their kind. Two scenes with the same file name fail the run (FR-002).", "The scene list must not be empty, and every listed file must exist (FR-002).", "A scene must not reference an asset by URL or absolute path (FR-002).", and "A scene must not enable nebula mode (FR-002)." Ignore blank lines and lines starting with `#`, detect assets with the tested pattern `^[A-Za-z0-9_]*(file|texture)[A-Za-z0-9_]* +"?(https?://|/|[A-Za-z]:[/\\])`, and detect nebula mode with `^nebula_mode true`
- [ ] T029 [US1] Implement `load_formulas(binary)` in `tests/regression_runner.py`: run the binary with `--list-formulas` in a temporary home, with `clean_environment`, the binary's folder as the working directory, and `FORMULA_TIMEOUT` as the time limit. Decode its output as UTF-8 with `errors="replace"`, because Windows output may not be UTF-8. Read the lines between `mandelbulber-formula-list 1` and `end`, and raise a setup failure that names the binary if it cannot start, times out, or exits non-zero, if either marker is missing, or if the list is empty
- [ ] T030 [US1] Implement `load_exclusions()` and `build_work_items(binary)` in `tests/regression_runner.py`, with a Work Item of kind, name, source, formula ID, resolution, and time limit, enforcing: "Every formula from `--list-formulas` is a work item unless it is listed in `tests/formula_exclusions.txt` (FR-034)." and "An exclusion needs a reason and must name a formula the binary offers." Ignore blank lines and lines starting with `#` in the exclusions file. Scenes use `SCENE_RESOLUTION` and `SCENE_TIMEOUT`, and formulas use `FORMULA_BASE`, `FORMULA_RESOLUTION`, `FORMULA_TIMEOUT`, and `formula_1=<id>` added to the overrides
- [ ] T031 [US1] Implement `render_item(item, binary, output_dir, thread_args=(), neutral=False)` in `tests/regression_runner.py`: remove earlier output, create `output_dir` before rendering because the app reports success when the folder is missing, create a temporary home for this render, build the command from `contracts/application-cli.md` with absolute paths, because the working directory changes, and run it with `clean_environment`, the item's time limit, and the binary's folder as the working directory. Save its output to a log file. A neutral render leaves out `normal_enabled=1` and `--verification-export`, and adds `color_quality=1`
- [ ] T032 [US1] Implement output checks in `tests/regression_runner.py`: read the log as UTF-8 with `errors="replace"`, then treat a non-zero exit, a timeout, and any of `not valid Mandelbulber settings file`, `No materials defined`, `Error in settings file`, `Too many errors in settings file`, `Can't load texture`, `Looking for the file at`, or `Trying to use /usr/share/mandelbulber2` as a render failure that names the item and points to its log. A zero exit status alone is never success (FR-008)
- [ ] T033 [US1] Implement `read_export(directory)` in `tests/regression_runner.py`, returning width, height, compiler, math library, buffers, and statistics, and enforcing: "The manifest exists and ends with its `end` line. This proves completeness (FR-008).", "The format version is one the harness understands.", "Exactly the seven expected buffers are present (FR-011).", "Each buffer file is exactly `width × height × channels × sample size` bytes.", and "The statistics record is complete and contains only whole numbers." The expected buffers are `raw_render` f32 3, `post_effect` f32 3, `final_image` u16 3, `alpha` u16 1, `depth` f32 1, `normal` f32 3, and `normal_world` f32 3. An unexpected buffer is named in the failure, and so is a statistic name that `contracts/verification-export.md` does not list
- [ ] T034 [US1] Add `render_and_validate(item, ..., neutral=False)` to `tests/regression_runner.py`: it combines the render, output checks, and export validation, records a failure in the `Report`, and lets the run continue with the remaining items. For a neutral render it skips export validation and requires that `<output>.tiff` exists and can be decoded. A setup error before rendering stops the run

**Checkpoint**: Broken inputs fail with a named cause, and the User Story 1 tests pass

---

## Phase 4: User Story 2 - Prove a Build Reproduces Its Own Output (Priority: P1)

**Goal**: `--self-parity` shows whether a build reproduces its own output. It reports KD-1 today and measures playbook Phase 3.

**Independent Test**: Run self-parity against today's build and confirm it reports differences. Run it against the stand-in in `same` mode and confirm it passes.

### Tests for User Story 2

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T035 [US2] Add self-parity tests to `tests/test_regression_runner.py`: with `THREAD_OPTION` patched to `--threads`, `same` mode passes and `stub_calls.jsonl` shows every item rendering three times at each thread count plus one neutral render, `unstable` fails and reports differing samples per item and buffer, `unstable:<buffer>` is caught for every buffer, a self-parity run leaves every reference file unchanged, outputs of failing items stay in `tests/outputs/` and outputs of passing items are removed, and the report names the target and is written to `tests/outputs/report.txt`
- [ ] T036 [US2] Add coverage tests to `tests/test_regression_runner.py`: with `THREAD_OPTION = None`, a run with identical output exits 3 and states that thread coverage is incomplete, a run with differing output exits 1 and reports both, a missing binary exits 3 rather than 1, and two modes at once exit 2
- [ ] T037 [US2] Add neutral render tests to `tests/test_regression_runner.py`: `neutral-differs` fails that item, `no-neutral-image` fails that item, a matching neutral render passes, and `stub_calls.jsonl` shows that the neutral render leaves out `normal_enabled=1` and `--verification-export` and adds `color_quality=1`
- [ ] T038 [US2] Add a platform test to `tests/test_regression_runner.py`: with `gate_valid` returning false, self-parity with identical output states that the run is not gate-valid and exits 3

### Implementation for User Story 2

- [ ] T039 [US2] Implement `compare_exports(first, second)` in `tests/regression_runner.py`, returning a difference list. "Two buffers match only if their dimensions, sample type, and every byte are identical (FR-013, FR-014)." Report differing samples rather than bytes, using the sample size of each buffer. Compiler and math library lines are never part of the comparison
- [ ] T040 [US2] Port `read_tiff` from the prototype in `git show 9b59d130d:MIGRATION_PLAYBOOK.md` to `tests/regression_runner.py`, and add `compare_neutral(export, tiff_path)`, which compares the decoded 16-bit samples with the `final_image` buffer (FR-012)
- [ ] T041 [US2] Implement `thread_settings()` in `tests/regression_runner.py`: when `THREAD_OPTION` is set, return thread counts of 1 and the CPU count, or 1 and 2 on a single-CPU machine. When it is `None`, return one default setting and mark coverage as incomplete (FR-017)
- [ ] T042 [US2] Implement `check_self_parity(items)` in `tests/regression_runner.py`: run the all-thread renders and the neutral render one item at a time, then run the single-thread renders with a `concurrent.futures.ThreadPoolExecutor` of one worker per CPU, keeping each item's three runs consecutive. Compare every result with the first export for that item (FR-016), and compare the neutral image with `compare_neutral` (FR-012)
- [ ] T043 [US2] Wire `--self-parity` into `main` in `tests/regression_runner.py`: report the target and gate validity, report results, remove outputs of passing items, keep outputs of failing items, and return the correct exit code, including exit 3 when the run is not gate-valid (FR-005, FR-017, FR-029, FR-030)
- [ ] T044 [US2] Run Scenario 4 and Scenario 7 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm differences are reported per item and buffer

**Checkpoint**: `--self-parity` passes on the stand-in and correctly reports KD-1 on the real build

---

## Phase 5: User Story 5 - Catch Computation Drift With Statistics (Priority: P1)

**Goal**: A change of 1 in any statistic fails the comparison, even when no pixel changes. This completes Self-Parity, which playbook Phase 3 depends on.

**Independent Test**: With identical buffers, a change of 1 in one count fails and names the statistic and both values.

### Tests for User Story 5

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T045 [US5] Add statistics comparison tests to `tests/test_regression_runner.py`: `unstable-stat:<name>` fails self-parity for `total_iterations`, `raymarch_count`, `missed_de`, a normal bucket, and the overflow bucket of each histogram, and each failure names the statistic and both values. With more than 5 differing statistics on one item, the console shows 5 and a count of the rest, and `tests/outputs/report.txt` holds them all
- [ ] T046 [US5] Add statistics format tests to `tests/test_regression_runner.py`: the stand-in record parses into a fixed order, and a value that is not a whole number or a record with time-based names is rejected

### Implementation for User Story 5

- [ ] T047 [US5] Add `compare_statistics(first, second)` to `tests/regression_runner.py` and call it from `compare_exports`, so every mode compares statistics exactly and each difference names the statistic and both values (FR-028)
- [ ] T048 [US5] Confirm that `mandelbulber2/src/verification_export.cpp` writes only the fields in `contracts/verification-export.md`, excluding `time`, `totalNoise`, `maskedPixels`, `tilesDone`, and every rate (FR-026)

**Checkpoint**: Self-parity compares buffers and statistics. Real builds are expected to differ until playbook Phase 3 fixes the shared counters

---

## Phase 6: User Story 3 - Detect Drift Against References (Priority: P2)

**Goal**: The default gate mode fails on any difference from a target's references, including formulas no scene uses.

**Independent Test**: With the stand-in and references written to a temporary folder, identical output passes and one flipped bit fails.

### Tests for User Story 3

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T049 [US3] Add reference format tests to `tests/test_regression_runner.py`: a reference zip round-trips every buffer exactly, entries appear in the order in `contracts/reference-set.md`, a corrupted entry is rejected, and writing the same data twice gives byte-identical files
- [ ] T050 [US3] Add gate result tests to `tests/test_regression_runner.py`: identical output passes, `flip-bit:<buffer>` names the item, buffer, and sample count for every buffer, `flip-mantissa:<buffer>` fails on the float buffers even though it disappears at 16-bit, `stat-plus-one:<name>` names the statistic and both values, and `resize` reports a layout difference
- [ ] T051 [US3] Add gate coverage tests to `tests/test_regression_runner.py`: a scene or formula with no reference fails and is named, a leftover reference fails and is named, `extra-buffer` fails, `only:<formula ID>:flip-bit:<buffer>` fails on that formula alone, a missing reference set exits 3 and does not pass with zero comparisons, and changed render settings exit 3
- [ ] T052 [US3] Add gate safety tests to `tests/test_regression_runner.py`: `other-toolchain` passes with a note, `other-toolchain-drift` exits 3 and reports a Reference Target mismatch rather than a parity failure, a difference only in package versions behaves the same way, a leftover reference alongside a toolchain difference still exits 1, a run where `gate_valid` is false exits 3 and states that it is not gate-valid, and a gate run leaves every reference file unchanged

### Implementation for User Story 3

- [ ] T053 [US3] Implement `split_planes` and `join_planes` in `tests/regression_runner.py`, using one plane per sample byte: 4 for `f32` and 2 for `u16`
- [ ] T054 [US3] Implement `write_reference(path, export)` and `read_reference(path)` in `tests/regression_runner.py` with `zipfile`, following `contracts/reference-set.md`: entries in the listed order, deflate at level 9, a fixed `ZipInfo` timestamp of 1980-01-01 00:00:00, and checksum verification on read
- [ ] T055 [US3] Implement `toolchain_identity(export)` in `tests/regression_runner.py`: combine the compiler and math library from the export with `package` lines from `dpkg-query -W` for `gcc-[0-9]*`, `g++-[0-9]*`, `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1`, in sorted order, skipping entries with no version. Where `dpkg-query` is missing, use the manifest values alone
- [ ] T056 [US3] Implement `load_reference_set(target)` in `tests/regression_runner.py`: read `toolchain.txt` and `settings.txt`, and raise a setup failure if the set is missing or its settings differ from the current `export_format`, `scene_resolution`, `formula_resolution`, or `overrides`
- [ ] T057 [US3] Implement `compare_against_references(items)` in `tests/regression_runner.py`: render each item once and compare with `compare_exports`. Enforce "Every work item has exactly one reference, and every reference has a work item (FR-019, FR-034).", treating a missing reference or a leftover reference as a parity failure that names the item
- [ ] T058 [US3] Wire the default gate into `main` in `tests/regression_runner.py`: report the target and gate validity, and never write to `tests/references/` (FR-020). If the toolchain differs from `toolchain.txt`, add a note when every comparison matches, and otherwise report each comparison difference as a Reference Target mismatch instead of a parity failure (FR-023). `Report` then sets the exit code. Exit 3 when the run is not gate-valid (FR-005), remove outputs of passing items, and keep outputs of failing items

**Checkpoint**: The gate passes and fails correctly against test references, and no reference exists in the repository

---

## Phase 7: User Story 4 - Capture References for a Baseline Change (Priority: P2)

**Goal**: `--capture` writes a complete reference set only in CI, and only from a build proven to reproduce its own output.

**Independent Test**: A stable stand-in writes a complete set to a temporary folder. An unstable one writes nothing.

### Tests for User Story 4

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T059 [US4] Add capture success tests to `tests/test_regression_runner.py`: with `THREAD_OPTION` patched, capture writes every scene and formula reference plus `toolchain.txt` and `settings.txt`, a gate run then passes against that set, and a second capture of unchanged data is byte-identical
- [ ] T060 [US4] Add capture refusal tests to `tests/test_regression_runner.py`: an `unstable` build fails and leaves existing references byte-for-byte unchanged, `THREAD_OPTION = None` refuses before any render runs, a not gate-valid platform refuses, a run with `CI` unset refuses before any render runs, a set over `MAX_REFERENCE_BYTES` exits 3, `only:<scene>:exit-3` on a scene part way through the list leaves no partial set, a failed move during promotion restores the old set byte for byte, and a `KeyboardInterrupt` during rendering leaves references unchanged

### Implementation for User Story 4

- [ ] T061 [US4] Implement `stage_references(items, exports)` in `tests/regression_runner.py`: reuse each item's first self-parity export, build the set in a staging folder under `tests/outputs/`, and write `toolchain.txt` and `settings.txt` from `contracts/reference-set.md`. Fail if toolchains differ between items
- [ ] T062 [US4] Implement `promote_references(staging, target)` in `tests/regression_runner.py`, enforcing "A set is replaced whole or not at all (FR-018)." and "Its stored size must stay under 100 MB (SC-008). Capture fails otherwise." Check completeness and size, move any existing set aside, move the new set into place, then delete the old one, restoring it if the move fails
- [ ] T063 [US4] Wire `--capture` into `main` in `tests/regression_runner.py`: refuse before rendering if `CI` is not `true` (FR-038), if thread coverage is incomplete, or if the run is not gate-valid. Then run self-parity with its exports kept, and stage and promote only if every item passed
- [ ] T064 [US4] Run Scenario 5 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm that capture refuses and that no `tests/references/` folder appears

**Checkpoint**: Capture is all or nothing, and nothing was captured from a real build

---

## Phase 8: User Story 6 - Check a Change Locally Before Pushing (Priority: P3)

**Goal**: `--compare BINARY` compares two builds on one machine, on any toolchain, with no references.

**Independent Test**: Two stand-in binaries with identical output pass, and one flipped bit fails.

### Tests for User Story 6

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T065 [US6] Add compare mode tests to `tests/test_regression_runner.py` with two stand-in launchers in separate folders: identical output passes, a flipped bit or a `stat-plus-one:<name>` change in either build fails and names the item, a failed render on either build names that build, a missing second binary exits 3 and names the path, `extra-formula` and `missing-formula` are reported as differences, `renumber-formulas` on one build gives each build its own ID for a formula, confirmed from `stub_calls.jsonl`, and the run works with no references and leaves any reference file untouched
- [ ] T066 [US6] Add report tests to `tests/test_regression_runner.py`: compare mode prints both toolchains and states that it is a local check that does not replace the gate, and with `gate_valid` returning false, identical output exits 0 and is labeled as not gate-valid

### Implementation for User Story 6

- [ ] T067 [US6] Implement `compare_builds(items, other_binary)` in `tests/regression_runner.py`: render each item with both binaries using the same fixed settings as the gate, match formulas by internal name and give each binary its own ID, compare with `compare_exports`, and name the build in any render failure (FR-035, FR-036)
- [ ] T068 [US6] Wire `--compare BINARY` into `main` in `tests/regression_runner.py`: build the formula list from both binaries, report a formula offered by only one as a difference, print both toolchains and the local-check statement, keep outputs of failing items, and do not read or modify references (FR-037). Where the run is not gate-valid, label the result and still allow a pass (FR-005)
- [ ] T069 [US6] Run Scenario 6 in `specs/001-regression-harness/quickstart.md` against the real binary, and confirm that it reports differences until renders reproduce

**Checkpoint**: The gate, self-parity, capture, and compare modes all work

---

## Phase 9: User Story 7 - Cross-Check Two Build Systems on One Computer (Priority: P3)

**Goal**: `--collect DIR` records one build's results, and `--cross-check FIRST SECOND` compares two collections. It fails on functional disagreements and records output differences.

**Independent Test**: Collect from two stand-in builds and cross-check the results. A failed item on one build fails the check, and a flipped bit is recorded without failing it.

### Tests for User Story 7

> **NOTE: Write these tests FIRST, and confirm they FAIL before implementation**

- [ ] T070 [US7] Add environment tests to `tests/test_regression_runner.py`: with the platform and variables patched, `detect_environment()` returns `github-actions` when `GITHUB_ACTIONS` is `true`, then `windows` on Windows, then `wsl` when `WSL_DISTRO_NAME` is set, and `linux` otherwise. The default binary is the Linux or Windows path in T006, `MANDELBULBER_BINARY` overrides both, and every mode's report names the environment
- [ ] T071 [US7] Add collect tests to `tests/test_regression_runner.py`: `--collect` renders each item once with `RENDER_OVERRIDES`, and writes a `collection.txt` that matches `contracts/collection.md` and ends with `end`, plus one zip per item that `read_reference` can read. With `only:<item>:exit-3` it still writes a complete collection, records that item with its cause and no zip, keeps that item's output, removes the raw outputs of passed items, and exits 3. A scene name containing a space survives the tab-separated `item` line. A second run replaces the first, and a folder left by an interrupted run is replaced too. A missing parent folder is created. A folder holding any other file is refused with exit 3 and left unchanged. A missing binary exits 3 and leaves the folder unchanged, including any earlier collection. With `gate_valid` returning false, the run completes and records `isolated false`. A binary older than the commit records `binary_older_than_commit true`, adds a note, and still exits 0. The report states that the run does not replace the gate, and every reference file stays unchanged
- [ ] T072 [US7] Add cross-check result tests to `tests/test_regression_runner.py`, collecting from two stand-in launchers in separate folders, and seeding each condition on the first build and then on the second. Identical output exits 0 and reports every item as identical. `flip-bit:<buffer>` and `stat-plus-one:<name>` on one build still exit 0: the report has a `[~]` line for the buffer, or a count and the first statistic, and `tests/outputs/cross-check-details.txt` holds the statistic and both values. `only:<item>:exit-3` exits 1 and names the item, the build, and the cause. `extra-formula` and `missing-formula` exit 1 and name the formula and the build. `renumber-formulas` exits 0, because formulas match by name. `resize`, `other-histogram-size`, and `other-version` exit 1. `blank` on one build exits 1, and `blank` on both exits 0. A collection with `isolated false` can still pass, and the report labels that side
- [ ] T073 [US7] Add cross-check setup tests to `tests/test_regression_runner.py`: a missing folder, a `collection.txt` with no `end` line, an unknown format version, a passed item with no zip, a corrupted zip entry, differing `source_commit`, `source_dirty true`, and a difference in `export_format`, either resolution, or `overrides` each exit 3 and name the cause. A run with `MANDELBULBER_BINARY` pointing at a missing file still works, which proves the cross-check starts no build. Two collections with one compiler add a note, and so does `binary_older_than_commit true`. Every report states that the check does not replace the gate, and every reference file stays unchanged

### Implementation for User Story 7

- [ ] T074 [US7] Add `detect_environment()` and `source_state()` to `tests/regression_runner.py`. `detect_environment()` follows the order in `data-model.md`. `source_state()` returns the output of `git rev-parse HEAD`, the commit time from `git log -1 --format=%ct`, and whether `git status --porcelain -- mandelbulber2 tests` printed anything, raising a setup failure if git cannot run. Pass the environment to `Report`, so every mode prints it (FR-040)
- [ ] T075 [US7] Add `application_version(binary)` to `tests/regression_runner.py`: run the binary with `--version` in a temporary home, with `clean_environment`, the binary's folder as the working directory, and `FORMULA_TIMEOUT` as the time limit, and return the first output line that starts with `Mandelbulber `, because startup messages can come first. Raise a setup failure if there is no such line
- [ ] T076 [US7] Implement `prepare_collection(path)` and `write_collection(path, header, results)` in `tests/regression_runner.py`, following `contracts/collection.md`. `prepare_collection` creates the folder and its parents, refuses a folder that holds anything except `collection.txt`, `scenes/`, and `formulas/`, then deletes any earlier collection with `collection.txt` first. `write_collection` writes one zip per passed item with `write_reference`, then writes `collection.txt` last with LF line endings, tab-separated `item` lines, and a temporary file moved into place (FR-041, FR-042)
- [ ] T077 [US7] Implement `read_collection(path)` in `tests/regression_runner.py`: read `collection.txt` as UTF-8, require the `mandelbulber-collection 1` first line and the final `end` line, parse the header and `item` lines, and raise a setup failure that names the path if any of these fail or if a passed item has no zip
- [ ] T078 [US7] Implement `cross_check(first, second)` in `tests/regression_runner.py`, following the three lists under Cross-Checking in `contracts/collection.md`. Raise a setup failure for the setup conditions (FR-046). Match items by kind and name, read each pair with `read_reference`, compare width, height, and statistic names first, and record a functional disagreement for the conditions in FR-044, naming the item, the build, and the cause. Call `compare_exports` only for pairs whose layouts match, and record its differences without counting them as failures (FR-045). Write one report line per differing buffer and one per item's statistics, giving the count and the first statistic, and write every differing statistic to `tests/outputs/cross-check-details.txt`. An item's final image holds one value when every `final_image` sample is the same. Count items with no difference as identical, and claim equality only for them (FR-047)
- [ ] T079 [US7] Wire `--collect DIR` into `main` in `tests/regression_runner.py`: build the work items, read `application_version` and `source_state`, then call `prepare_collection`, so a setup failure leaves the folder unchanged. Render each item once with `render_and_validate`, and call `write_collection`. Then remove the raw outputs of passed items, and keep those of failed items. Report the environment and whether the run is isolated, and record `isolated false` where `gate_valid()` is false. Compare the binary's modified time with the commit time, record `binary_older_than_commit`, and note an older binary. State that the run does not replace the gate. Exit 3 if any item failed, and never read or write references (FR-030, FR-041, FR-042, FR-047)
- [ ] T080 [US7] Wire `--cross-check FIRST SECOND` into `main` in `tests/regression_runner.py`: call `read_collection` for both, call `cross_check`, print each side's environment, isolation, and toolchain and the shared commit, use `[~]` for recorded differences as in `contracts/runner-cli.md`, and state that the check does not replace the gate. Add a note when both sides used one compiler, and when either binary was older than its commit. Exit 0 on functional agreement, 1 on functional disagreement, and 3 on a setup failure. Do not start a binary, and do not read or write references (FR-043, FR-047)
- [ ] T081 [US7] Run Scenario 8 in `specs/001-regression-harness/quickstart.md` with both real builds of one commit, confirm the expected report, and record the result against SC-013. T095 times it on the full suite

**Checkpoint**: All six modes work, and both builds pass the cross-check on one computer

---

## Phase 10: User Story 8 - Run the Cross-Check in a Separate CI Pipeline (Priority: P3)

**Goal**: Every pull request builds one commit with both build systems, tests each build, and cross-checks them, with no change to any existing workflow.

**Independent Test**: Push a branch that adds the pipeline, and confirm that all three jobs pass and the existing workflows are unchanged. Then seed a failure on a throwaway branch, and confirm that the run summary names it.

See `contracts/ci-pipeline.md`, which has the exact content of all three files.

### Implementation for User Story 8

- [ ] T082 [P] [US8] Create `.github/actions/build-linux/action.yml` with exactly the content in `contracts/ci-pipeline.md`
- [ ] T083 [P] [US8] Create `.github/actions/build-windows/action.yml` with exactly the content in `contracts/ci-pipeline.md`
- [ ] T084 [US8] Create `.github/workflows/cross-build-check.yml` with exactly the content in `contracts/ci-pipeline.md`
- [ ] T085 [US8] Check the three files before pushing: parse each one as YAML, and confirm that every composite `run` step has a `shell`, that every `steps.<id>` reference exists, that `Collect` depends only on the build step, and that `cross-check` reads the folders the two artifacts unpack to. Then confirm that `git diff --exit-code master -- .github/workflows/github-actions-ci.yml .github/workflows/codeql.yml .circleci .appveyor.yml azure-pipelines.yml .travis.yml` prints nothing (FR-048, SC-015)

### Validation for User Story 8

- [ ] T086 [US8] Push the branch and run Scenario 9 in `specs/001-regression-harness/quickstart.md`: confirm that all three jobs pass, that the run summary shows the report, that the three artifacts exist, and that the workflow's token is read-only. This is the first run of the Windows binary on a hosted runner, so check that `--test` passes on both jobs, that the `OpenCL.dll` copy works, and that the Inno Setup step does not fail the build (FR-049 to FR-053)
- [ ] T087 [US8] Confirm the failure paths on a throwaway branch that is never merged, using the `#ifdef _WIN32` change in Scenario 9: `cross-check` fails and the run summary names the formula and the build. Then break the Windows build on purpose and confirm that `linux-qmake` still finishes and `cross-check` names the missing collection (FR-050, FR-051, SC-016)
- [ ] T088 [US8] Confirm that the existing `C++ CI Build (Qt6)` workflow still passes on the same commit, and that a newer push cancels a run still going for an older commit (FR-052, SC-015)
- [ ] T089 [US8] Record the measured job times in R17 of `specs/001-regression-harness/research.md`, replacing the estimates. T095 checks the total on the full suite. Compare each harness command in the workflow with its local form, and confirm that only paths and the Python launcher differ (SC-017)
- [ ] T090 [US8] Confirm that the pipeline is not a required check, and state in the pull request that it does not replace the parity gate from playbook Phase 4 (FR-054)

**Checkpoint**: The cross-check runs in all three environments, and nothing existing changed

---

## Phase 11: Polish & Cross-Cutting Concerns

**Purpose**: Complete the scene suite and confirm the feature's limits

- [ ] T091 [P] Create `tests/scenes/beam_light.fract` and `tests/scenes/hdr_blur.fract` with exactly the content below, which cover the beam light and HDR blur paths that no example scene uses

  `tests/scenes/beam_light.fract`:

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
  light1_is_defined true;
  light1_enabled true;
  light1_type 4;
  light1_position -2 -3 2;
  light1_target 2 -3 2;
  light1_intensity 2;
  ```

  `tests/scenes/hdr_blur.fract`:

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
  hdr_blur_enabled true;
  ```

- [ ] T092 Expand `tests/scenes.txt` to about 25 scenes, adding `tests/scenes/beam_light.fract` and `tests/scenes/hdr_blur.fract`, with a total capped render time of about 90 seconds on 32 threads: about 19 ordinary scenes at up to 2 seconds and 6 Monte Carlo scenes at up to 10 seconds. `chromatic_aberration.fract` takes about 13 seconds, and it stays because no other scene has Monte Carlo chromatic aberration. The suite must exercise every path in FR-039, using the candidates in the table under R13 in `specs/001-regression-harness/research.md`, and each added scene must be timed and pass the checks in `load_scenes()`
- [ ] T093 Review `tests/test_regression_runner.py` against the requirements in `specs/001-regression-harness/spec.md`, and add a test for any failure condition without one, so SC-001 and SC-002 are covered
- [ ] T094 Format `mandelbulber2/src/verification_export.hpp`, `mandelbulber2/src/verification_export.cpp`, and the changed lines in `mandelbulber2/src/command_line_interface.cpp`, `mandelbulber2/src/command_line_interface.hpp`, `mandelbulber2/src/headless.cpp`, and `mandelbulber2/src/headless.h` to match `.clang-format`
- [ ] T095 Run `python3 tests/test_regression_runner.py` in WSL and `python tests/test_regression_runner.py` on Windows, then run all scenarios in `specs/001-regression-harness/quickstart.md`, and record any result that differs from what the guide expects. Now that the suite is complete, time Scenario 8 against SC-012 and the Scenario 9 pipeline run against SC-014
- [ ] T096 Check the feature's limits with `git diff --stat master`: the only changed C++ files are the six listed in T094, nothing changed in the render path, no existing CI file changed, and `tests/references/` does not exist. State in the pull request that the C++ changes close part of KD-4 and have no expected effect on output, as the constitution requires

---

## Requirement Coverage

| Requirements | Tasks |
|---|---|
| FR-001, FR-002 | T002, T023, T028 |
| FR-003, FR-004 | T006, T022, T027, T031 |
| FR-005 | T022, T038, T043, T052, T058, T060, T063, T066, T068, T071, T079 |
| FR-006, FR-007 | T025, T031 |
| FR-008, FR-009 | T025, T026, T032, T033, T034 |
| FR-010 to FR-012 | T008, T026, T033, T037, T040 |
| FR-013, FR-014 | T026, T035, T039, T050 |
| FR-015 to FR-020 | T005, T035, T036, T041 to T043, T057, T058, T061 to T063, T079, T080 |
| FR-021 to FR-023 | T006, T021, T035, T052, T055, T056, T058, T061 |
| FR-024 to FR-028 | T008, T009, T010, T012, T014, T016, T045 to T048, T050 |
| FR-029 to FR-031 | T021, T035, T043, T096 |
| FR-032 to FR-034 | T003, T004, T024, T029, T030, T051 |
| FR-035 to FR-037 | T065 to T068 |
| FR-038 | T060, T063 |
| FR-039 | T091, T092 |
| FR-040 | T006, T018, T020, T021, T070, T074 |
| FR-041, FR-042 | T071, T075, T076, T079 |
| FR-043 to FR-047 | T072, T073, T077, T078, T080 |
| FR-048 | T082 to T085 |
| FR-049 to FR-053 | T082 to T084, T086 to T088 |
| FR-054 | T090 |

| Success criterion | Tasks | Can close in this feature |
|---|---|---|
| SC-001 | T023 to T027, T093 | Yes |
| SC-002 | T035, T045, T050 | Yes |
| SC-003 | T044 | First half only. The rest needs playbook Phase 3 |
| SC-004 | T092 | No. It needs the CI gate from playbook Phase 4 |
| SC-005 | T021, T050 | Yes |
| SC-006 | T060 | Yes |
| SC-007 | T037 | Logic only. Real results need playbook Phase 3 |
| SC-008 | T060, T062 | Enforced here, measured in playbook Phase 4 |
| SC-009 | T030, T051 | Yes |
| SC-010 | T065, T069 | Logic only. Real results need playbook Phase 3 |
| SC-011 | T072 | Yes |
| SC-012 | T081, T095 | Yes |
| SC-013 | T081 | Yes |
| SC-014 | T089, T095 | Yes |
| SC-015 | T085, T088 | Yes |
| SC-016 | T087 | Yes |
| SC-017 | T089 | Yes |

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies
- **Foundational (Phase 2)**: Depends on Setup. Blocks all user stories, except that T018 blocks only the real binary tasks
- **User Stories (Phases 3 to 10)**: Depend on Foundational
- **Polish (Phase 11)**: Depends on every story being complete

### User Story Dependencies

- **User Story 1 (P1)**: Starts after Foundational. Every other story uses its checked render path
- **User Story 2 (P1)**: Depends on US1
- **User Story 5 (P1)**: Depends on `compare_exports` from T039
- **User Story 3 (P2)**: Depends on US1, and on `compare_exports` and `compare_statistics` from T039 and T047
- **User Story 4 (P2)**: Depends on US2 and on the reference format from US3
- **User Story 6 (P3)**: Depends on US1, and on `compare_exports` and `compare_statistics` from T039 and T047
- **User Story 7 (P3)**: Depends on US1, on `compare_exports` and `compare_statistics` from T039 and T047, and on `write_reference` and `read_reference` from T054
- **User Story 8 (P3)**: Depends on US7, because the pipeline calls `--collect` and `--cross-check`

### Within Each User Story

- Tests are written first and must fail before implementation
- Helpers come before the mode that uses them
- Real binary checks come last

### Parallel Opportunities

- **Phase 1**: T002, T003, and T004
- **Phase 2**: T007 can run with T019 and T020. The C++ tasks (T007 to T018) and the shared harness tasks (T019 to T022) are separate tracks, and one person can work on each
- **After US5**: US3 and US6 are independent of each other, but they edit the same files, so run them one at a time unless working on separate branches
- **Phase 10**: T082 and T083, because each creates its own new file
- **Phase 11**: T091 can run alongside other Polish work, because it creates new files
- **Within a story**: Stories 1 to 7 have no `[P]` tasks, because each one edits `tests/regression_runner.py` and `tests/test_regression_runner.py`

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

### MVP First (User Stories 1, 2, and 5)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational
3. Complete Phase 3: User Story 1
4. Complete Phase 4: User Story 2
5. Complete Phase 5: User Story 5
6. **STOP and VALIDATE**: `--self-parity` reports KD-1 on the real build, with every failure named, and it compares buffers and statistics

This is the smallest useful result, and it is what playbook Phase 3 needs, because KD-1's success check covers statistics. User Story 1 alone has no mode to run.

### Incremental Delivery

1. Add User Story 3, so the gate works against test references
2. Add User Story 4, so capture is ready for playbook Phase 4
3. Add User Story 6, so developers have a local check for playbook Phase 5
4. Add User Story 7, so both local builds can be cross-checked on one computer
5. Add User Story 8, so GitHub Actions runs the same cross-check

---

## Notes

- Commit after each task or logical group, and after each checkpoint at the latest
- `--self-parity`, the gate, and compare mode are expected to report differences on real builds until playbook Phase 3 is done. That is a correct result, not a bug in the harness. The cross-check can pass before then, because it does not need equal output
- Keep the C++ commits separate from the harness commits, so the application change is easy to review
- Cross-Build Check is the only CI this feature adds. The `parity-gate` and `capture-baseline` workflows belong to playbook Phases 3 and 4
- Never change an existing workflow or CI configuration in this feature
