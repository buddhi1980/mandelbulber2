# Research: Regression Harness

Resolved decisions for the implementation plan. Measurements were taken on 2026-11-04 with a July 2026 Windows Release build on 32 threads, unless stated otherwise. That build came from `mandelbulber2/msvc/mandelbulber2.sln` with MSVC 14.51, `/fp:precise`, and `/std:c++20`, not from the qmake and GCC build the gate uses. Its numbers are estimates for the gate's build and say nothing about bit-for-bit results there. On the same day, a fresh qmake build in WSL Ubuntu 24.04 (GCC 13.3.0, Qt 6.4.2, glibc 2.39) confirmed the Linux results noted in R9 and R10, and fresh builds of both kinds gave the results in R15. Other Linux behavior still comes from reading the current source.

## R1. Verification export layout

- **Decision**: The application writes a directory of raw, uncompressed files for each render: one file per buffer, a text statistics record, and a text manifest written last.
- **Rationale**: Raw files need no new library in the application, and Python can read them with no decoding step. Writing the manifest last gives a simple completeness check. If the manifest is missing, the export is incomplete.
- **Alternatives considered**:
  - One binary container file: more C++ for the same information, and harder to inspect when debugging.
  - Compression in the application: adds work and code to a path that only needs to be exact. The harness compresses references instead (R4).

## R2. Export trigger and proof of success

- **Decision**: A new `--verification-export <dir>` option. `cHeadless::RenderStillImage` writes the export after `renderJob->Execute()`, only if the settings loaded and `Execute()` returned true.
- **Rationale**: `cSettings::Decode` returns false for an invalid file, because `DecodeHeader` clears `textPrepared`. This covers the path where a nonexistent filename falls through to `LoadFromString`, which always returns true. `handleArgs` ignores both results today, and `RenderStillImage` ignores the result of `Execute()`, so the export only has to record them. Everything happens after the render, so it adds no work to the render path (FR-027).
- **Alternatives considered**: Change the exit code on failure. This would alter default behavior, and it would not help a failure that happens after the export is written. The spec also says not to rely on the exit code.

## R3. Statistics record format

- **Decision**: UTF-8 text with one `name value` pair per line in a fixed order, and one line per histogram bucket, including the overflow bucket.
- **Rationale**: A line-based record lets the harness name the exact statistic and both values on a difference (FR-028). The values come from the public `cStatistics` fields, not the `double` getters, so they stay exact. `QString::number` on integers does not depend on the locale.
- **Alternatives considered**: JSON adds nothing here and risks losing precision on large integers. A binary record would be harder to review in a Baseline Change.

## R4. Keeping references under 100 MB

- **Decision**: Store one zip file per render. Each buffer is split into byte planes, then compressed with deflate at level 9. Capture fails if the set exceeds 100 MB.
- **Rationale**: Plain deflate does poorly on float data because the low mantissa bytes are close to random. Splitting bytes into planes lets the exponent and high mantissa bytes compress well. Python's `zipfile` also provides a checksum for every entry, and a fixed timestamp makes a recapture of unchanged data byte-identical.

Measured at 320x240, mean of `IFS_001`, `Makin3D-Julia_001`, and `difs_tree`, with the post-effect image counted as a second copy of the raw render:

| Method | Per scene |
|---|---|
| Uncompressed | 3,450 KB |
| Deflate | 2,579 KB |
| LZMA | 2,239 KB |
| Byte planes plus deflate | 2,163 KB |
| Byte planes plus LZMA | 2,049 KB |

That table covers the first five buffers. Alpha and world normal came later (R14). With byte planes plus deflate, they add 33 KB and 457 KB per scene, and 0.2 KB and 1.1 KB for a sweep render of the base scene.

- **Expected size**: 25 scenes at about 2.7 MB, plus 462 sweep renders at about 21 KB each (estimated), comes to about 77 MB.
- **Known cost**: Each Baseline Change adds about that much to git history, because compressed data does not delta well. This is acceptable for infrequent baselines. If they become frequent, references can move to Git LFS without changing their format.
- **Alternatives considered**:
  - LZMA: about 5% smaller, but slower, and the module is optional in Python builds.
  - Digests only: far smaller, but the gate could not report how many samples differ (FR-029).
  - Storing the post-effect image as an alias when it equals the raw render: saves more, but it isn't needed to meet the limit.

## R5. Fitting the gate and capture into CI

- **Decision**: Run the gate and capture on GitHub-hosted `ubuntu-24.04` runners. The gate runs on every pull request and every push to `master`. Capture is one manually started job in its own workflow, and its single-thread renders run in parallel across items. Cross-Build Check is a separate workflow (R17).
- **Rationale**: The repository is public, so [standard runners are free, with 4 vCPU and 16 GB of RAM](https://docs.github.com/en/actions/reference/runners/github-hosted-runners), and [a job can run for 6 hours](https://docs.github.com/en/actions/reference/limits). The existing Linux workflow already builds on this runner in 4.5 to 7.5 minutes, and `nproc` reports the real CPU count, so `make -j$(nproc)` is safe.
- **Capture time**: Each item renders 7 times: 3 at each thread count, plus 1 neutral render. With about 15 minutes of render time in a gate, the 3 all-thread runs and the neutral render take about 1 hour. Run one after another, the 3 single-thread runs would add about 3 hours, because each takes about 4 times as long. Rendering 4 items at once brings that down to about 45 minutes, for a total of about 2 hours (estimated). An item's own runs stay consecutive, as the constitution requires.
- **Runner image**: The `ubuntu-24.04` label fixes the OS, but [GitHub typically updates the software weekly](https://github.com/actions/runner-images). The [October 2026 image](https://raw.githubusercontent.com/actions/runner-images/main/images/ubuntu/Ubuntu2404-Readme.md) has GCC 13.3.0 and glibc 2.39. A patch update can therefore reach the gate with no commit. The gate reports this against the recorded toolchain (FR-023), and if output changes, that needs a Baseline Change.
- **Runner hardware**: Hosted runners may not all use the same CPU model, and glibc selects some math routines by CPU features. This is a small risk of a false failure, and it can only be measured once renders reproduce.
- **Capture location**: The constitution requires references from a CI build, so capture refuses unless `CI` is `true`. [GitHub Actions always sets it](https://docs.github.com/en/actions/reference/workflows-and-actions/variables). This stops a local run from replacing references by accident.
- **Alternatives considered**:
  - CircleCI: its [1-hour job limit on the Free plan](https://support.circleci.com/hc/en-us/articles/4411086979867-Updates-to-maximum-duration-of-jobs) would force capture into 8 jobs and a merge step, it uses credits, and its Docker executors [report the host's CPU count](https://ideas.circleci.com/cloud-feature-requests/p/have-nproc-accurately-reporter-number-of-cpus-available-to-container).
  - Matrix shards on GitHub Actions: not needed at this size. With [20 concurrent jobs and a 6-hour limit](https://docs.github.com/en/actions/reference/limits), this remains an option if the suite grows.
  - A pinned container: this would freeze the OS image, but `apt-get` would still install current packages, so packages would need pinning too. Worth revisiting if runner updates cause failures.

## R6. Formula list and sweep

- **Decision**: A new read-only `--list-formulas` option prints every entry in `newFractalList`, with its ID and internal name. Sweep references are named by internal name. Exclusions live in `tests/formula_exclusions.txt`. The base scene is a new file, `tests/formula_sweep_base.fract`.
- **Rationale**: The binary under test is the only true source for what it offers (FR-034), and this also covers the second build in compare mode. `DefineFractalList` runs in `main` before `ReadCLI`, so the option needs about 15 lines. Internal names stay valid if an enum value changes.
- **Measured sweep**: On 2026-11-04, the 461 swept formulas at 32x24 took 23 seconds of render time on a fresh WSL build, 0.05 seconds on average and 0.39 seconds at most, with no failures and no asset messages. Version 2.35 files have no `formula_1` parameter, so the sweep selects a formula with `fractal1_formula=<id>`.
- **Black renders**: 188 of those 461 formulas render solid black, because the camera sits inside the set: the `abox` and `mandelbox` families at this distance, and the `transf_` transforms, which do not escape on their own. These renders run the formula code, but they give a flat image and one iteration bucket, so they protect little. A camera 1.5 times farther away leaves 163 black. Pairing each formula with `mandelbulb` in a hybrid leaves none, but a hybrid replaces some single-formula distance estimates. Neither change is in this feature. An earlier figure of 459 distinct images came from a binary built from older source.
- **CI estimate**: 2 to 5 minutes on a 4-vCPU runner, depending on how much of each run is startup. The earlier estimate of about 10 minutes used a costlier scene.
- **Known exclusion**: `Custom OpenCL formula`, because its CPU `FormulaCode` is empty.
- **Scope note**: `--list-formulas` is the only C++ change outside the export. FR-031 now names it.
- **Alternatives considered**:
  - Parsing `all_fractal_list.cpp`: no C++ change, but brittle, and it reads the source rather than the binary.
  - Listing formulas inside each export: mixes two unrelated jobs.

## R7. Toolchain identity

- **Decision**: The export manifest records the compiler from compile-time macros and the math library from a runtime call. GCC uses `__VERSION__`, Clang uses `__clang_version__`, MSVC uses `_MSC_FULL_VER`, and glibc uses `gnu_get_libc_version()`. Capture copies these into `toolchain.txt`, and it adds package versions from `dpkg-query` for `gcc-[0-9]*`, `g++-[0-9]*`, `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1`, skipping entries with no version.
- **Rationale**: The manifest identifies the binary that actually rendered, not whichever compiler happens to be installed where the harness runs. It matters more on hosted runners, because the image changes over time (R5). Package versions are needed as well, because `__VERSION__` and `gnu_get_libc_version()` stay at `13.3.0` and `2.39` across Ubuntu patch updates.
- **Behavior**: A difference between targets is not drift, so the gate passes with a note if output still matches. If output differs, it reports a Reference Target mismatch (FR-023). Compare mode prints both identities. Identity lines are never part of the parity comparison.
- **Alternatives considered**: Running `g++ --version` and `ldd --version` from the harness or CI. This describes the machine rather than the binary, and it could be wrong in compare mode. Package versions have the same limit, so they supplement the manifest rather than replace it, and they apply only where capture and the gate build on the same machine.

## R8. Two-pass stereo scenes

- **Decision**: No special handling. Stereo scenes are allowed, and the limit is documented.
- **Rationale**: Two passes happen only for red-cyan stereo with screen-space AO or non-Monte Carlo depth of field. That is 2 of the 791 example scenes. `InitStatistics` resets on each pass, so the statistics and every buffer except the final image describe the last eye only. `MixImages` writes both eyes into the 16-bit final image, so drift in the first eye is still caught there. All of it is deterministic, so comparisons stay valid.
- **Alternatives considered**:
  - Accumulating statistics across passes: changes engine behavior for a rare case.
  - Rejecting stereo scenes: leaves that path with no coverage.

## R9. Isolation

- **Decision**: Each render gets a new temporary `HOME` and a clean environment built from an allowlist, with `LC_ALL=C.UTF-8`.
- **Rationale**: `CreateDefaultFolders` only creates folders and copies a few small files, so a new home per render stays cheap. An allowlist meets the constitution's rule on environment variables. It leaves out `LD_LIBRARY_PATH`, because that can select a different math library. `setlocale(LC_ALL, "")` and `QLocale::system()` affect number parsing, and a fixed locale also keeps application messages in English for the text checks.
- **Windows**: The app ignores a redirected home folder, so Windows runs are labeled not gate-valid (FR-005). The gate and self-parity cannot pass there, and capture is refused. Compare mode can still pass, because both builds read the same per-user state, which keeps a local check available on Windows. Collect mode completes there, labeled as not isolated, because the cross-check is not a parity claim (R16). The clean environment itself works: the MSVC build rendered with only the allowlisted variables. `LC_ALL` does not set the language on Windows, so a machine using another language could print translated messages and miss a text check. The export check still catches a failed load there.
- **Linux**: Confirmed on the fresh build. A render succeeds with no display and only `HOME`, `PATH`, and `LC_ALL` set, stays inside the temporary `HOME`, and leaves about 200 KB there. Each new `HOME` also prints five `cannot move folder` lines at startup, because the app tries to migrate old data folders and finds none. These lines are not failures.
- **Alternatives considered**: One shared home per run. This is slightly faster, but a state leak between renders would be harder to find.

## R10. Failure detection

- **Decision**: Three layers. Check scenes before rendering. Require a complete, valid export. Scan the output text for known failure messages.
- **Rationale**: The export proves that the load and render succeeded. Text checks remain because a missing texture does not change those results: the render still exits 0 and saves an image. `Decode` also tolerates up to 3 bad lines and still returns true, so a partly loaded scene would otherwise get a valid export. The scan looks for `Can't load texture`, `Looking for the file at`, `not valid Mandelbulber settings file`, `No materials defined`, `Error in settings file`, and `Too many errors in settings file`. Healthy renders, including a `$SHARED_DIR` texture scene and all 462 sweep renders, printed none of these, while `hybrid002.fract` prints the settings error. The scan also looks for `Trying to use /usr/share/mandelbulber2`, which `system.cpp` prints on Linux when it finds no data folder beside the binary. That folder is outside the repository and build output, so the constitution forbids reading it.
- **Linux**: Confirmed on the fresh build. A missing scene, a bad settings file, and a missing texture all exit 0 and print the messages above, and a failed load saves a black image. A binary left in the build folder prints `Trying to use /usr/share/mandelbulber2`, exits 0, and saves an image. A missing target folder prints `Image saved to:` but writes no file and prints none of these messages, so the harness must check for the file itself. `No materials defined`, `Error in settings file`, and `Too many errors in settings file` were not triggered.
- **Static asset check**: A parameter whose name contains `file` or `texture` and whose value starts with a URL or absolute path fails the run. This matches 54 of the 791 example scenes.

## R11. Neutral render

- **Decision**: One render per item with the normal buffer and export off, saved as a 16-bit TIFF, and compared with the `final_image` buffer.
- **Rationale**: `--override color_quality=1` writes `GetPixelImage16` directly, so its samples should equal the final-image buffer bit for bit. The TIFF reader in the earlier prototype (commit `9b59d130d`) has been tested on real files.

## R12. Harness structure and testing

- **Decision**: One standard-library script, `tests/regression_runner.py`, for Python 3.10 and later. Tests use `unittest` with a stand-in renderer that implements the export contract.
- **Rationale**: Real renders cannot pass until playbook Phase 3, so a stand-in is the only way to exercise the success paths. It can also produce every failure on demand. This approach found real defects in the prototype. The gate job runs these tests first (playbook Phase 4), so a change that weakens a check cannot pass unnoticed.

## R13. Standard Scene Suite budget

- **Decision**: Keep the scene portion of the gate to about 90 seconds of 32-thread render time, which is about 12 minutes on a 4-vCPU runner (estimated at 8 times slower). For example, 19 ordinary scenes at up to 2 seconds and 6 Monte Carlo scenes at up to 10 seconds. The suite must exercise each stochastic CPU path (FR-039).
- **Rationale**: With the sweep, this keeps render time to about 15 minutes, well inside the 30-minute target (SC-004), and keeps capture to about 2 hours. Capped Monte Carlo scenes ranged from 5 to 41 seconds, so scenes need to be chosen by cost.
- **Available scenes**: 151 top-level examples have no external assets. Four of those are nebula scenes, which always render through OpenCL whatever `opencl_enabled` says, so the harness rejects them. That leaves 147: 55 plain, 71 with ambient occlusion, 12 with Monte Carlo, 1 with Monte Carlo and ambient occlusion, and 8 with Monte Carlo and global illumination.

The stochastic paths come from the CPU callers of `Random()`, plus the seeded generators that run during a render: `cRandom` in `lights.cpp`, and the Perlin noise generator created in `render_job.cpp`. These are the candidates among the 147 usable scenes. Each one rendered with the gated settings, exited 0, and printed no failure messages. Times are seconds on 32 threads, including startup and image saving, and they total about 30 seconds across the 11 scenes.

| Path | Usable scenes | Candidate | Seconds |
|---|---|---|---|
| Raymarch step jitter | 147 | Any scene | - |
| Monte Carlo depth of field | 21 | `chromatic_aberration.fract` | 13.0 |
| Monte Carlo antialiasing | 21 | `chromatic_aberration.fract` | 13.0 |
| Monte Carlo soft shadows | 9 | `chromatic_aberration.fract` | 13.0 |
| Monte Carlo chromatic aberration | 1 | `chromatic_aberration.fract` | 13.0 |
| Monte Carlo global illumination | 8 | `difs_tree.fract` | 6.2 |
| Rough surfaces | 5 | `difs_tree.fract` | 6.2 |
| Specular roughness | Most scenes, because it defaults on for metallic materials | Any scene with a specular highlight | - |
| Post-effect depth of field | 38 | `IFS_002.fract` | 0.3 |
| Multi-ray ambient occlusion | 10 | `IFS_001.fract` | 1.9 |
| Random screen-space ambient occlusion | 7 | `amazing_surf 002.fract` | 1.1 |
| Volumetric fog | 70 | `EXR example.fract` | 2.0 |
| Iteration fog | 7 | `benchmark-GPU.fract` | 2.5 |
| Volumetric auxiliary light | 2 | `IFS_001.fract` | 1.9 |
| Beam light | 0 | `tests/scenes/beam_light.fract`, purpose-built | 0.2 |
| Random lights | 4 | `menger octo 001.fract` | 1.0 |
| Perlin noise on materials | 3 | `difs_tree.fract` | 6.2 |
| Clouds | 6 | `clouds 005.fract` | 1.5 |
| HDR blur | 0 | `tests/scenes/hdr_blur.fract`, purpose-built | 0.2 |

Scene choice matters for cost: `mandelbulb_bermarte 001.fract` also has random lights but took 27 seconds, and `clouds 001.fract` took 15 seconds. `chromatic_aberration.fract` is over the 10-second guide, and it stays because no other scene has Monte Carlo chromatic aberration.

HDR blur is not random. It is in the table because it runs in an OpenMP region that playbook Phase 3 audits, and Self-Parity can only measure that audit with a scene that reaches it. From reading the code, each pixel reads a snapshot of the image, so it is not expected to depend on thread order.

A beam light is light type 4. Test scenes for the beam light and HDR blur, built on the sweep base, rendered with no errors. Renders do not reproduce yet, so counting differing pixels proves nothing, and these checks compare the size of each difference at 320x240, as a mean gap in 16-bit samples:

| Comparison | Mean gap |
|---|---|
| Sweep base rendered twice | 0.6 |
| HDR blur scene rendered twice | 0.3 |
| HDR blur on against off | 145 |
| Beam light scene rendered twice | 659 |

The HDR blur effect is far larger than the noise, so that path runs. The beam light's noise is far larger than the base scene's, so its random sampling runs.

## R14. Compared channels

- **Decision**: Compare every channel the engine writes in a gated render: the three color stages, alpha, depth, normal, and world normal.
- **Rationale**: The constitution requires every channel the engine writes to be compared. `render_worker.cpp` writes alpha on every render, and post-effect depth of field blends it separately from color in `dof.cpp`, so comparing color does not cover it. In three measured scenes it held 1, 8,791, and 17,982 distinct values, so it is not a simple mask. `render_job.cpp` turns world normal on for every render, with a FIXME, because the denoiser reads it.
- **Not compared**: Specular, diffuse, world position, shadow, global illumination, and not-denoised are written only when enabled. They are application settings, and no example scene sets them. Opacity and object color are working inputs to SSAO, and no image format can save them, so they are not channels.
- **If the FIXME is fixed**: The engine would stop writing world normal, so every render would fail export validation. A change to that behavior therefore cannot pass unnoticed.
- **Size**: 60 bytes per pixel in total. The reference set stays under the 100 MB limit (R4).

## R15. Local development environments

- **Decision**: Develop on two builds of one commit on one Windows computer: qmake in the WSL `Ubuntu-24.04` distro, and MSVC Release from `mandelbulber2/msvc/mandelbulber2.sln`.
- **Rationale**: The WSL build has the same compiler, Qt, and glibc versions as the CI runner, so it stands in for the gate's toolchain locally. The MSVC build adds a second compiler, Visual Studio debugging, and a way to run the Windows binary, which no CI job does yet. Having both on one computer finds build-system-specific problems before a push.

Both were built from clean clones and run on 2026-11-04:

| | WSL, qmake | Windows, MSVC |
|---|---|---|
| Toolchain | GCC 13.3.0, Qt 6.4.2, glibc 2.39 | MSVC 14.51, toolset v145, Qt 6.11.1 |
| Language level | gnu++17, the GCC default, with no `-std` flag | `/std:c++20` |
| Floating point | `-ffast-math` | `/fp:precise` |
| Build on 32 threads | 53 seconds, 811 compile lines | 116 to 188 seconds, 916 compile lines |
| Warnings | 2 | 6 |
| `--test` | 8 of 8 passed, 165 seconds | 8 of 8 passed, about 200 seconds |
| Headless render | Passed | Passed |

Both `--test` runs tried the same 791 example scenes. Two identical renders of `IFS_001.fract` differed in 27.3% of the samples of an 8-bit TIFF on the WSL build and 27.4% on the MSVC build, which matches KD-1.

- **Source flow**: The Windows checkout is the working copy. Git stores LF, and `core.autocrlf=true` writes CRLF to disk there, so the WSL build uses its own clone. A commit reaches it with `git fetch /mnt/c/GIT/mandelbulber2 <branch>`, then `git checkout --detach FETCH_HEAD`. This was tested: the fetch worked from a shallow clone, and the fetched files have LF endings.
- **Commit first**: WSL can only see commits, so uncommitted Windows edits do not reach the WSL build. The cross-check enforces this (FR-046).
- **qmake**: `qmake6` expands the source wildcards when it runs, so it must run again after a source file is added.
- **MSVC**: Use Release, because Debug uses `/fp:fast` (KD-2). The `Build-Package` step deletes `build-win\<configuration>\x64` on every build, so nothing kept there survives. The project also compiles every `build-win\*.cpp`, so start from an empty `build-win` the first time, or generated files from older source are compiled too.
- **Package restore**: `nuget restore` installed all 26 packages from nuget.org alone in 38 seconds, with no user config and no cached copies. A user-level `openclnuget` source on this machine points at a folder with no packages, so nothing depends on it.
- **`--test` on Windows**: Windows has no redirected home folder, so the suite uses the real data folder. Two runs each added one autosave file to `history`, and they changed and removed nothing else. With no console attached, Qt sends test results to the debugger stream, so set `QT_FORCE_STDERR_LOGGING=1` to capture them. The exit status is the number of failed tests either way.
- **Alternatives considered**:
  - Building from `/mnt/c`: the shell scripts have CRLF endings there, and `UI_DIR` would write generated Linux headers into the Windows checkout.
  - The default `Ubuntu` distro: it is 26.04, with GCC 15.2 and Qt 6.10.2, so its results would not carry over to the gate.
  - CMake: [the project calls it beta and not feature complete](../../mandelbulber2/deploy/README), and no CI job uses it.

## R16. Cross-checking two build systems

- **Decision**: Two steps. `--collect <dir>` records one build's results, and `--cross-check <first> <second>` compares two collections. The check fails only on exact functional facts (FR-044) and records every difference in output values (FR-045).
- **Rationale**: The constitution rules out tolerances, thresholds, and perceptual metrics, and it says a difference between two targets is recorded and is not drift. It also allows equality to be claimed only where the harness has demonstrated it. So the check fails on what must be true of two working builds, compares output exactly, and reports the result without judging it.
- **Why two steps**: The builds run on different operating systems, and on different machines in CI, so no single process can start both. A collection is just files, so the second step works anywhere, including on a machine with neither build.
- **Single-value image check**: A render that draws nothing can still exit cleanly and write a valid export, so an item whose final image holds one value on one build only is a functional failure. A failed load does not reach this check, because it writes no export. This is an exact fact, not a threshold, and it also passes when both builds agree.
- **Report size**: Each render has a histogram of `N` buckets and one of 1000 buckets, about 1,100 values at the sweep's `N` of 50. Before playbook Phase 3 many of those differ on every item, which could reach tens of megabytes across 487 items. GitHub limits a run summary to [1 MiB per step](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-commands). So the report has one line per differing buffer and one per item's statistics, about 300 KB at most, and `tests/outputs/cross-check-details.txt` lists every differing statistic.
- **Stale binaries**: The binary does not record its source, so the commit in a collection comes from the repository the harness ran in. Collect mode compares the binary's modified time with the commit time and notes an older binary. It stays a note, because a documentation-only commit leaves a correct binary older than the commit.
- **Before playbook Phase 3**: The check can pass, because it does not need equal output. Its recorded differences mean nothing until both builds pass Self-Parity.
- **After playbook Phase 3**: The strict floating-point work removes `-ffast-math`, so the two builds may match on many items. The report then demonstrates equality item by item, as the constitution allows.
- **Alternatives considered**:
  - Extending `--compare`: it requires equal output, and it starts both binaries on one operating system, so it cannot work across CI jobs.
  - An image similarity score: the constitution rules it out, and renders differ too much between runs to set a meaningful limit. Any limit would only measure noise.
  - A separate script: it would repeat the harness's isolation, failure detection, and comparison code.
  - Comparing `--test` results only: that suite renders examples at 5x5 pixels and compares nothing between builds.

## R17. Cross-Build Check pipeline

- **Decision**: One new workflow, `.github/workflows/cross-build-check.yml`, with jobs `linux-qmake`, `windows-msvc`, and `cross-check`, plus two reusable actions that hold the build commands. [contracts/ci-pipeline.md](contracts/ci-pipeline.md) has the details.
- **Rationale**: A new file leaves `github-actions-ci.yml` and `codeql.yml` untouched. That existing workflow runs on every branch and replaces the `continuous` release, so it must not change, and the new one must never publish. The reusable actions keep the build commands in one place, and the later `compile-engine` action can add its flag check to `build-linux`.
- **Build steps**: `.appveyor.yml` already defines the MSVC CI build, so `build-windows` follows it: `nuget restore` from `before_build`, then `msbuild` with `/m` from `build_script`, on a Visual Studio 2026 image. `build-linux` follows the package lists and `qmake6` steps in `github-actions-ci.yml`. [AppVeyor build 7386](https://ci.appveyor.com/project/buddhi1980/mandelbulber2) on 2026-11-01 shows those steps working, with the same MSVC 14.51.36231 used locally. The pipeline leaves out AppVeyor's remote desktop `init` step, its `/logger` option, its zip and artifact steps, and its `Debug` configuration, which AppVeyor keeps covering.
- **Installer step**: The solution runs Inno Setup after linking whenever `ISCC.exe` exists at its default path. This machine has none, so local builds make no installer. AppVeyor has it, and so does the runner image, so the Windows job should build an installer that nothing uses. It took 105 seconds on AppVeyor. The step cannot be turned off from the command line, because its only condition is that path.
- **Linux runner**: `ubuntu-24.04`, the same label as the gate. It is also what `ubuntu-latest` means today, but a pinned label won't move on its own.
- **Windows runner**: [`windows-2025-vs2026`](https://github.com/actions/runner-images). The [image from October 2026](https://raw.githubusercontent.com/actions/runner-images/main/images/windows/Windows2025-VS2026-Readme.md) has Visual Studio 2026 18.11, NuGet 7.9, and Python 3.12.10. The project sets toolset v145, so a runner with only Visual Studio 2022 would need an override, as `azure-pipelines.yml` does. `windows-latest` points at the same image today, but it can move.
- **`OpenCL.dll`**: The binary links to it, and a clean build does not put it beside the binary. The project looks in the repository's top-level `packages` folder, while NuGet restores into `mandelbulber2/msvc/packages`. It runs on this computer because `System32` has a copy, the Khronos OpenCL ICD Loader, and the image's documentation does not list one. AppVeyor's file listing has no `OpenCL.dll` either, which nothing notices because AppVeyor never runs the binary. `build-windows` therefore copies the library from the restored `opencl-nuget-msvc-x64` package. The wildcard in that step selected exactly one file, and the binary ran with that copy beside it.
- **Why Windows `--test` is new**: AppVeyor and Azure Pipelines build the solution but do not run it, because AppVeyor has `test: off` and Azure has no test step. So no CI configuration here runs the Windows binary, and only the pipeline's first run can prove it works on a hosted runner.
- **QtTest time limit**: QtTest stops a test function after 5 minutes by default, and one function renders all 791 examples. The whole suite took 199 seconds on 32 threads here, so a slower Windows runner could reach that limit. The project already raises it with `QTEST_FUNCTION_TIMEOUT` for `--benchmark`, so the Windows job sets it to 30 minutes.
- **Keeping collections through failures**: GitHub skips later steps after a failure, so a failed `--test` would otherwise stop the collection. The `Collect` step therefore runs whenever the build itself succeeded, each build job uploads with `if: always()`, and `cross-check` runs under `if: ${{ !cancelled() }}`. One build's failure still produces a report from the other.
- **Action versions**: As of November 2026 the latest releases are `actions/checkout` v7, `actions/upload-artifact` v7, `actions/download-artifact` v8, `actions/setup-python` v7, and `microsoft/setup-msbuild` v3. The existing workflow uses `actions/checkout@v4`, whose Node.js 20 runtime GitHub now warns about, so the new one does not copy that.
- **Time estimate**: Linux about 25 minutes: 4.5 to 7.5 to build, 2 to 4 for `--test`, and up to 15 to collect. Windows about 50 to 75 minutes: 1 to restore, 27 to 52 to build, 4 for `--test`, and up to 15 to collect. The whole run takes as long as the slower build job plus the cross-check, so SC-014 allows 90 minutes.
- **Windows build range**: AppVeyor's Release job took 53.8 minutes on 2026-11-01. `nuget restore` took 36 seconds, and `msbuild` took 51.5 minutes: about 45 to compile, 4 to link, and 2 for the installer. That is the upper figure. The lower one comes from the Linux build, which is 5 to 8.5 times slower on a hosted runner than in WSL, and 8.5 times the local 188 seconds is about 27 minutes. A hosted runner has 4 vCPU, and AppVeyor's core count is not in its log, so the real figure is unknown until the first run. T089 replaces this estimate.
- **Not included**: The flag check, because it fails until playbook Phase 3 closes KD-2. References, because none exist. macOS, because the request names two build systems and CircleCI already builds it.
- **Alternatives considered**:
  - Adding jobs to `github-actions-ci.yml`: this would change an existing workflow, and a failure there could stop the AppImage upload.
  - One Windows and Linux matrix job: their steps share almost nothing, so a matrix would hide the difference behind conditions.
  - Caching NuGet packages: restore takes under a minute, so a cache would add little.
