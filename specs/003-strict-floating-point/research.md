# Research: Strict Floating-Point Builds

Every decision here comes from reading the code and from small tests on 2026-11-07, with GCC 13.3.0, glibc 2.39, and Qt 6.4.2 in WSL, and from the GitHub API. The tests were one-file compiles, `qmake6` runs in temporary folders, and a prototype of the check in a scratch folder. No engine build or render has been measured, so nothing here says what this change does to output or render time. Line numbers are those of `8fd49ba59`.

## R1. Where `-ffp-contract=off` goes in qmake

- **Decision**: One `gcc` scope in the `# compiler build flags` section of `common.pri`, above the lines that held `-ffast-math`. It adds the option to `QMAKE_CFLAGS` and `QMAKE_CXXFLAGS`.
- **Rationale**: The scope is true for GCC and for Clang, whose mkspec lists `gcc clang llvm` in `QMAKE_COMPILER`, and false for MSVC, which lists `msvc`. That matches the constitution, which names compilers and not platforms. A trial on a copy of `common.pri` generated `-ffp-contract=off` in both `CFLAGS` and `CXXFLAGS`.
- **Order**: The block sits below the environment import, so a value from the environment comes first and the project's value comes last, and the compiler honors the last one. The block also sits above the `macx:m1` line, whose bare `-Xpreprocessor` takes the next option as its own. That line still takes `-I/opt/homebrew/include`, as it does today.
- **Alternatives considered**:
  - A `unix` scope: it misses a MinGW cross-compile, where GCC still contracts.
  - Adding the option to each of the three existing lines: it repeats the option, and on the Apple Silicon line it would land beside `-Xpreprocessor`.

## R2. Where `-ffp-contract=off` goes in CMake

- **Decision**: In the `ELSEIF(NOT USING_INTEL)` branch of `CMakeLists.txt`, `-ffast-math` leaves `CMAKE_CXX_FLAGS`, `-ffp-contract=off` joins it, and a new line adds the option to `CMAKE_C_FLAGS`.
- **Rationale**: That branch is the only one that sets `-ffast-math`, and it covers GCC and Clang on every platform but Windows. The engine has no C sources today, so `CMAKE_C_FLAGS` changes nothing now, but FR-004 asks for both, and CMake compiles any `.c` file that appears in `src` or `qt`.
- **Alternatives considered**: `add_compile_options`: it reaches both languages in one line, but the rest of the file sets `CMAKE_CXX_FLAGS` directly.

## R3. MSVC Debug

- **Decision**: `<FloatingPointModel>Precise</FloatingPointModel>` in place of `Fast`, on line 97 of the project file. `/fp:contract` stays unset.
- **Rationale**: One word changes, and the setting says what it means. Release and AnalysisRelease get the same model from the compiler's default.
- **Alternatives considered**: Deleting the line. The result is the same, but nothing would then show that the choice was made.

## R4. Setup and install scripts

- **Decision**: Both `prepare_for_dev_*.sh` scripts keep `export CXXFLAGS="-msse2"`. `deploy/linux/install` loses its `export` line, because `-march=native` is all it holds.
- **Rationale**: This is the smallest edit that closes KD-3. `-msse2` is part of the plain x86-64 target, so it does not depend on the build computer.
- **Alternatives considered**: Removing the `export` from the two `prepare_for_dev_*.sh` scripts as well. It would let a builder's own flags through, but it changes what those scripts do beyond KD-3.

## R5. The shape of the check

- **Decision**: One shell script beside the action, `.github/actions/compile-engine/check-flags.sh`, with two commands: `search`, and `flags <Makefile>`. It holds the only list of options. [flag-check.md](contracts/flag-check.md) is the contract.
- **Rationale**:
  - FR-015's seeds run the file CI runs. An inline step could only be tested through a copy.
  - One list serves both commands, so they can't drift apart.
  - It stays out of `tests/`, so the harness doesn't change (FR-034).
  - The playbook's check is already shell, and it needs nothing installed.
- **The list**: The constitution's 6 value-changing options, 3 host-tuned options, and 2 more that change values: `-fno-signed-zeros`, a part of `-ffast-math`, and `-ffp-model=fast`, Clang's short form of it. No list can be complete, so Phase 4's gate still catches what this one misses.
- **What it reads**: The `CC`, `CXX`, `LINK`, `CFLAGS`, `CXXFLAGS`, `LFLAGS`, and `LIBS` lines of the generated Makefile. `build-linux` runs `make` with no overrides, so those lines are what the compiler and linker receive. It matches whole options, so `-fno-fast-math`, `-march=x86-64`, and a path that holds the text do not match.
- **Tested**: A prototype handled every case in [flag-check.md](contracts/flag-check.md). All 24 seeded faults in a Makefile failed and named the option, including all 4 environment variables through real `qmake6` runs. A clean Makefile and 3 look-alikes passed. All 5 cases where the check can't run exited 2, including a run outside a checkout. In a throwaway clone with this feature's edits, the search passed, all 17 seeds in build files failed and named the file and line, and the 3 cases it must ignore passed.
- **Alternatives considered**:
  - Inline in the action, as the playbook lists it: it can't run outside CI.
  - Reading `make -n` output: it prints all 800 or so compile lines, and they are built from the same variables.
  - Python: it adds nothing for a check this size, and the check should not depend on the harness's language or tests.
  - Committing the seeds as a test: it would need its own CI step, which FR-009 rules out at this stage. The seeds run once and go in the evidence, as Phase 4's drift proof does.

## R6. What the build-file search covers

- **Decision**: `git grep` over tracked files by kind: `*.pro`, `*.pri`, `CMakeLists.txt`, `*.cmake`, `*.vcxproj`, `*.props`, `*.targets`, `*.sln`, `*.sh`, `*.bat`, `*.yml`, `*.yaml`, `Dockerfile`, and `mandelbulber2/deploy/linux/install`. It leaves out `.github/actions/compile-engine`, `mandelbulber2/tools/DOE/NaNtester.sh`, `specs`, `.specify`, and `.claude`.
- **Rationale**: Matching by kind means a new script joins the search by itself, which a list of names would miss. That is 72 files today. On `8fd49ba59` it reports the 9 lines this feature removes and nothing else: 4 in `common.pri`, 1 each in `CMakeLists.txt`, `install`, and the MSVC project, and 2 in the setup scripts.
- **Contraction values**: It lists every `-ffp-contract` value it finds and rejects all but `off`. A pattern for `fast` and `on` let Clang's `-ffp-contract=fast-honor-pragmas` through in a test.
- **Failing closed**: `git grep` exits 0 on a match, 1 on no match, and 128 on an error, so a broken search can't pass. The search also stops if the file list is empty.
- **Comments count**: A prohibited option in a comment fails the search. This is why FR-001 cleans the commented-out macOS line. Upstream still has that line, so a merge that brings it back fails too, which is the safe direction.
- **Alternatives considered**:
  - A fixed list of paths: a new script would go unsearched.
  - Every tracked file with exclusions: it reaches documentation and third-party headers, which need a growing list of exceptions.

## R7. Order inside the job

- **Decision**: Search, then build, then read the flags.
- **Rationale**: The search needs no build. On the last KD-1 pull request `build-linux` took 6.1 minutes, so a bad build file fails in seconds, and a clean run takes about 7 minutes. All of this lives in the action, so Phase 4's `capture-baseline.yml` gets it too.
- **Alternatives considered**: Running the search in the workflow file: Phase 4's capture workflow would then need its own copy.

## R8. Proving the check in GitHub Actions

- **Decision**: One throwaway branch and one draft pull request into `upgrade/strict-floating-point`, with one seed per commit:
  1. `-ffast-math` on the CMake line. The search fails before the build.
  2. That seed reverted, and a job-level `CXXFLAGS: -march=${{ 'native' }}` added to `parity-gate.yml`. The search can't match that text, qmake receives `-march=native`, and the flag check fails after the build. This is KD-3's closing condition on a real gated build.
- **Afterward**: Close the pull request unmerged, delete the branch, and cancel the other workflow runs it started. Record both run links in the evidence.
- **Rationale**: `parity-gate` only runs on pull requests and on pushes to `ministic-regression`, and a manual start isn't offered until a workflow is on the default branch. A pull request into the feature branch has the smallest diff. Each push needs the user's go-ahead.
- **Alternatives considered**: Temporary commits on the real pull request: they would stay in its history.

## R9. Timing

- **Decision**: KD-1's method, from its scripts in `~/verify/kd1`, copied to `~/verify/kd2`.
  - `--collect` renders all 487 items, each build from its own clone, once with the before build first and once with the after build first.
  - A scratch script runs `--collect` through the harness and wraps `render_item`, so one run gives the whole time, and scenes and formulas apart, from the monotonic clock. KD-1 needed a second pass for that split.
  - One CPU: the same run under `taskset -c 4`, once per build.
  - `-march=native`: a third clone of `8fd49ba59`, built by `CXXFLAGS=-march=native build_fresh.sh`, because qmake imports that variable.
- **Quiet computer**: Wait until `pgrep -f FractalEvaluator` has been empty for a minute, sample it every 5 seconds during each run, and log it against the monotonic clock. Repeat any run that overlaps. Nothing else from this feature runs during a timing run, because the process check can't see a compile. On 2026-11-07 that job was running through this planning, with up to 29 processes, so timing must wait for it. It must never be stopped.
- **Rationale**: Matching KD-1 keeps the numbers comparable. One CPU matters here because KD-1 found that all-thread times can move for reasons unrelated to arithmetic: `--collect` got 32% faster at all threads, while `IFS_001.fract` on one CPU showed no gain.
- **Alternatives considered**: Timing only at all threads: scheduling could hide or exaggerate the cost of the flags.

## R10. The output record

- **Decision**: Two tools.
  - `--compare <before binary>` meets FR-019 directly. It lists each item's differing buffers and statistics, and exits 1.
  - A scratch script reads the two builds' collections, using the harness's own `read_collection`, `read_reference`, `compare_exports`, and `holds_one_value`. It writes the record, counts the identical items, reports any final image that holds a single value on one side only, adds the average absolute difference of `final_image`, and ranks the swept formulas to choose FR-021's 20.
- **Format**: One row per item in `specs/003-strict-floating-point/output-record.tsv`, with the summary in `evidence.md`. [evidence-record.md](contracts/evidence-record.md) defines both.
- **Alternatives considered**:
  - `--cross-check` of the two collections: `check_collections_match` refuses collections from different commits, and FR-034 keeps the harness as it is.
  - Putting 487 rows in `evidence.md`: it would bury the summary.

## R11. Counting guard hits and isolating their effect

- **Decision**: Two throwaway patches on the final commit, neither ever committed (FR-020).
  - One counts each of the four `IsNotANumber()` sites when the test is true, splitting infinite values from NaN values, and writes the counts at exit to a file named after the render's process. A scratch script matches those files to items. `--compare` against the after build must report 0 differences, or the counts mean nothing.
  - One makes `IsNotANumber()` return `false`, which is what `-ffast-math` does today. `--compare` against the after build then lists exactly what the guards change, item by item. Everything else is strict arithmetic. A `--collect` of that build keeps its images, because `--compare` deletes each render once it has compared it, so the review can see where the guards change pixels.
- **Rationale**: A hit only shows that a guard fired, and not that it changed an image. The guards-off build shows that directly, so a reviewer can name a cause. An infinite `z` at line 316 is the case the escape test used to catch, so the split says whether that happens anywhere in the suite. It changes the distance estimate. `calculate_distance.cpp:607` drops the iteration limit mark unless iteration threshold mode is on, so the point counts as inside only there, or in interior mode. 2 of the 487 items use the first, and none use the second.
- **Found**: `compute_fractal.cpp:316` breaks out of the loop on the same test as line 500, so line 500 can only fire if something between them changes `z`. The count will show whether it ever does.
- **Alternatives considered**: Comparing against MSVC alone: it shows the same logic, but compilers and math libraries differ too, so it can't isolate the guards.

## R12. The image review

- **Decision**: A scratch script based on KD-1's `noise.py` writes three images for each of the 26 scenes and the 20 formulas: before, after, and an amplified difference. Where a shape appears or vanishes, it adds the MSVC before image, to show whether the Windows logic already gives that result. A person reviews them, as on KD-1, and the evidence names each cause, using R11's guards-off build as its proof: one panel marks where that build differs from the after build. A visible change is one that can be seen in the before and after panels at normal brightness, because the difference panel is amplified and nearly every sample is expected to change.
- **Rationale**: Exact comparison shows that everything changed, which can't separate a rounding shift from a new shape. The difference image shows where the change sits, and the first clarification accepts a guard's effect once its cause is named.

## R13. MSVC

- **Decision**: Fresh MSVC Release builds of the before and after commits, each with `--collect`.
  - R10's scratch script, run on the two MSVC collections, should find all 487 items identical, which proves the statement in FR-022.
  - `--cross-check` of the after qmake and after MSVC collections, which share a commit, prints the `identical` line SC-009 records.
  - The before MSVC collection supplies R12's comparison image.
- **Rationale**: Three checks come from two builds. The Release configuration and the sources don't change, so a difference there would expose a mistake in this feature.

## R14. Proving the other builds

| Build | Today | After this change |
|---|---|---|
| qmake with OpenCL | Compiles in CI and locally | Same, and it is the gated build |
| qmake without OpenCL | A fresh local build of `8fd49ba59` | A fresh local build |
| CMake | A fresh local build of `8fd49ba59` | A fresh local build, with `flags.make` and `link.txt` read to confirm the flags |
| MSVC Release | Compiles in CI and locally | Same |
| MSVC Debug | **Unknown** | A fresh local build, with its `cl` lines read for `/fp:precise` |
| macOS, `mandelbulber-opencl-mac.pro` | CircleCI builds it | CircleCI on the pull request. It uses LLVM Clang |
| macOS, `mandelbulber-opencl-m1.pro` | Nothing builds it | Changed by reading alone |

KD-1's T077 built both of these on `76316fd0f`, but every result here comes from a fresh build of the commit being tested, so they build again.

MSVC Debug is the gap. AppVeyor lists Debug, but it is not among the checks on this repository: pull request 6 ran the two GitHub workflows, Azure Pipelines, and CircleCI, and Azure and Cross-Build Check build Release only. So the first task builds Debug on `8fd49ba59`. If that fails, Debug does not compile today, FR-033 does not bind it, and the evidence says so.

## R15. Commits, the pull request, and the final commit

- **Decision**: One branch and one pull request into `ministic-regression`, in this order: build flags, setup scripts, the action and workflow, the local checks, the pull request and GitHub Actions proof, then the closing work.
- **Final commit**: "After" means the last commit that changes what compiles. CI files and documents can't change a binary, so they don't move it, and they don't repeat the final checks. A fix under FR-018 does move it.
- **Rationale**: The flag changes are one commit so that no commit has a half-strict build. The local checks come before the first push, because a fix under FR-018 would otherwise repeat every CI run. The constitution amendment comes last, because it needs `parity-gate` to pass. Constitution rule 1 asks that this lands as its own reviewed change, and nothing here is a refactor, so rule 4 doesn't split it.

## R16. The closing edits

- **Constitution**: The KD-2 and KD-3 rows gain `**Closed <date>**, see specs/003-strict-floating-point/evidence.md`. A new KD-5 row records the CPU math path (R18). The version becomes 1.1.0, and Last Amended changes. KD-4 and KD-5 stay open.
- **Playbook** (FR-029):

| Line | Statement | Correction |
|---|---|---|
| 87 | "Use Release, because Debug uses `/fp:fast` (KD-2)." | Keep Release, for the reason that remains: it is the binary the harness finds |
| 230 | The `/speckit-specify` prompt | Stays, as a record of the request |
| 207 | "It is not the parity gate, which arrives in Phase 4 as its own workflow." | Say the workflow arrives in Phase 3, and its gate steps in Phase 4 |
| 233 | Guards in `algebra.hpp` and `math_utilities.h` may take effect | Name `IsNotANumber()`, drop `math_utilities.h`, and say the guards catch infinity too, so an overflow keeps its last finite value |
| 235 to 259 | The check's description and the `compile-engine` listing | Match what is committed, and point to `check-flags.sh` |
| 263 to 279 | The `parity-gate.yml` listing | Add `permissions: contents: read` |
| 296 to 323 | The Phase 4 `parity-gate.yml` listing | Add `permissions: contents: read` |
| 352 | Phase 4 has no step for the math path | A new step before capture, which closes KD-5, so the later steps renumber. No other document cites those step numbers |
| 370 | "whichever of KD-2, KD-3, and KD-4 are still open" | KD-4 and KD-5, and record that KD-2 and KD-3 closed in 1.1.0 |

## R17. Why `parity-gate` needs an explicit read-only setting

- **Decision**: `permissions: contents: read` at the top of `parity-gate.yml`.
- **Rationale**: This repository's default workflow token is `write`, so a workflow with no setting gets write access. The playbook's listing has no setting. Cross-Build Check already sets this.

## R18. Recording the CPU math path

- **Decision**: This feature's amendment adds this row to the Known Deviations table, and the version becomes 1.1.0:

```text
| KD-5 | Renders depend on the CPU. glibc picks its math code path from the CPU's features at run time. On 2026-11-07, one binary gave different `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `exp`, `log`, and `pow` results with its FMA and AVX2 paths turned off. | glibc's math library | II | Playbook Phase 4 settles the path before Baseline 1 is captured, and records the result with the references |
```

- **Rationale**: Principle II leaves the CPU out of what a render may depend on, and Governance permits non-compliance only as a Known Deviation. The clarification keeps the work itself in Phase 4, and this only records it.
- **Version**: PATCH covers clarifications, wording fixes, and closing a Known Deviation, not adding one. MINOR covers materially expanded guidance, which a new recorded exception is. One amendment closes two deviations and adds one, so it takes the higher of the two.
- **Alternatives considered**:
  - An open gate in the plan: Governance doesn't allow one, and the Complexity Tracking table can't waive a principle.
  - A separate amendment: it adds a second pull request for one row.
  - Naming CPU features in the Reference Target now: that decides what Phase 4 is meant to settle.

## R19. Proving `-ffp-contract=off` changes nothing

- **Decision**: A throwaway patch on the final commit removes the `gcc` block from `common.pri`, and `--compare` against the after build reports 0 differences.
- **Rationale**: FR-022 tells reviewers the option has no effect on the gated build, and the test behind that was one line of arithmetic. All 487 items prove it on the engine, for one more build.
- **Alternatives considered**: Adding the option to the before build: it would prove the point on a build with `-ffast-math`, and not on the build this feature ships.

## Spec changes from this plan

| Requirement | Change | Why |
|---|---|---|
| FR-011 | Adds the compiler and linker commands | R5 |
| Assumptions, Environment | No CI job builds MSVC Debug on this repository, so FR-033 needs a fresh local build | R14 |
| Assumptions, Environment | Records the latest Collect time, 4.4 minutes on the Linux job of the last KD-1 pull request | The API's step times |
| Assumptions, Found by reading | Adds the MinGW cross-compile script, whose `qmake` line is commented out | R1 |
