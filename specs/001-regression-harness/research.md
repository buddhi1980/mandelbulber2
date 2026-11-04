# Research: Regression Harness

Resolved decisions for the implementation plan. Measurements were taken on 2026-11-04 with a July 2026 Windows Release build on 32 threads, unless stated otherwise. Linux behavior comes from reading the current source and has not been run.

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

- **Decision**: Run the gate and capture on GitHub-hosted `ubuntu-24.04` runners. The gate runs on every pull request and every push to `master`. Capture is one manually started job in its own workflow, and its single-thread renders run in parallel across items.
- **Rationale**: The repository is public, so [standard runners are free, with 4 vCPU and 16 GB of RAM](https://docs.github.com/en/actions/reference/runners/github-hosted-runners), and [a job can run for 6 hours](https://docs.github.com/en/actions/reference/limits). The existing Linux workflow already builds on this runner in 4.5 to 7.5 minutes, and `nproc` reports the real CPU count, so `make -j$(nproc)` is safe.
- **Capture time**: Each item renders 7 times: 3 at each thread count, plus 1 neutral render. With about 15 minutes of render time in a gate, the 3 all-thread runs and the neutral render take about 1 hour. Run one after another, the 3 single-thread runs would add about 3 hours, because each takes about 4 times as long. Rendering 4 items at once brings that down to about 45 minutes, for a total of about 2 hours (estimated). An item's own runs stay consecutive, as the constitution requires.
- **Runner image**: The `ubuntu-24.04` label fixes the OS, but [GitHub typically updates the software weekly](https://github.com/actions/runner-images). The [October 2026 image](https://raw.githubusercontent.com/actions/runner-images/main/images/ubuntu/Ubuntu2404-Readme.md) has GCC 13.3.0 and glibc 2.39. A patch update can therefore reach the gate with no commit. The gate reports this against the recorded toolchain (FR-023), and if output changes, that needs a Baseline Change.
- **Runner hardware**: Hosted runners may not all use the same CPU model, and glibc selects some math routines by CPU features. This is a small risk of a false failure, and it can only be measured once renders reproduce.
- **Capture location**: The constitution requires references from a CI build, so capture refuses unless `CI` is set. [GitHub Actions always sets it](https://docs.github.com/en/actions/reference/workflows-and-actions/variables). This stops a local run from replacing references by accident.
- **Alternatives considered**:
  - CircleCI: its [1-hour job limit on the Free plan](https://support.circleci.com/hc/en-us/articles/4411086979867-Updates-to-maximum-duration-of-jobs) would force capture into 8 jobs and a merge step, it uses credits, and its Docker executors [report the host's CPU count](https://ideas.circleci.com/cloud-feature-requests/p/have-nproc-accurately-reporter-number-of-cpus-available-to-container).
  - Matrix shards on GitHub Actions: not needed at this size. With [20 concurrent jobs and a 6-hour limit](https://docs.github.com/en/actions/reference/limits), this remains an option if the suite grows.
  - A pinned container: this would freeze the OS image, but `apt-get` would still install current packages, so packages would need pinning too. Worth revisiting if runner updates cause failures.

## R6. Formula list and sweep

- **Decision**: A new read-only `--list-formulas` option prints every entry in `newFractalList`, with its ID and internal name. Sweep references are named by internal name. Exclusions live in `tests/formula_exclusions.txt`. The base scene is a new file, `tests/formula_sweep_base.fract`.
- **Rationale**: The binary under test is the only true source for what it offers (FR-034), and this also covers the second build in compare mode. `DefineFractalList` runs in `main` before `ReadCLI`, so the option needs about 15 lines. Internal names stay valid if an enum value changes.
- **Measured sweep**: 462 formulas at 32x24 took 34 seconds in total, 0.07 seconds on average and 0.19 seconds at most. There were no failures, no asset messages, and 459 distinct images. `pseudoKleinianTrig` and `pseudoKleinianTrigV3` render no surface from this camera, but their statistics still cover them.
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
- **Windows**: The app ignores a redirected home folder, so Windows runs are labeled not gate-valid (FR-005). The gate and self-parity cannot pass there, and capture is refused. Compare mode can still pass, because both builds read the same per-user state, which keeps a local check available on Windows.
- **Alternatives considered**: One shared home per run. This is slightly faster, but a state leak between renders would be harder to find.

## R10. Failure detection

- **Decision**: Three layers. Check scenes before rendering. Require a complete, valid export. Scan the output text for known failure messages.
- **Rationale**: The export proves that the load and render succeeded. Text checks remain because a missing texture does not change those results: the render still exits 0 and saves an image. `Decode` also tolerates up to 3 bad lines and still returns true, so a partly loaded scene would otherwise get a valid export. The scan looks for `Can't load texture`, `Looking for the file at`, `not valid Mandelbulber settings file`, `No materials defined`, `Error in settings file`, and `Too many errors in settings file`. Healthy renders, including a `$SHARED_DIR` texture scene and all 462 sweep renders, printed none of these, while `hybrid002.fract` prints the settings error. The scan also looks for `Trying to use /usr/share/mandelbulber2`, which `system.cpp` prints on Linux when it finds no data folder beside the binary. That folder is outside the repository and build output, so the constitution forbids reading it. This case comes from reading the source and has not been run.
- **Static asset check**: A parameter whose name contains `file` or `texture` and whose value starts with a URL or absolute path fails the run. This matches 54 of the 791 example scenes.

## R11. Neutral render

- **Decision**: One render per item with the normal buffer and export off, saved as a 16-bit TIFF, and compared with the `final_image` buffer.
- **Rationale**: `--override color_quality=1` writes `GetPixelImage16` directly, so its samples should equal the final-image buffer bit for bit. The TIFF reader in the earlier prototype (commit `fbcc70492`) has been tested on real files.

## R12. Harness structure and testing

- **Decision**: One standard-library script, `tests/regression_runner.py`, for Python 3.10 and later. Tests use `unittest` with a stand-in renderer that implements the export contract.
- **Rationale**: Real renders cannot pass until playbook Phase 3, so a stand-in is the only way to exercise the success paths. It can also produce every failure on demand. This approach found real defects in the prototype.

## R13. Standard Scene Suite budget

- **Decision**: Keep the scene portion of the gate to about 90 seconds of 32-thread render time, which is about 12 minutes on a 4-vCPU runner (estimated at 8 times slower). For example, 19 ordinary scenes at up to 2 seconds and 6 Monte Carlo scenes at up to 10 seconds. The suite must exercise each stochastic CPU path (FR-039).
- **Rationale**: With the sweep, this keeps render time to about 15 minutes, well inside the 30-minute target (SC-004), and keeps capture to about 2 hours. Capped scene times ranged from 5 to 41 seconds, so scenes need to be chosen by cost. `difs_tree` measured 5.0 seconds, `chromatic_aberration` 9.8 seconds, and `IFS_001` 1.8 seconds.
- **Available scenes**: 151 top-level examples have no external assets: 55 plain, 75 with ambient occlusion, 12 with Monte Carlo, 1 with Monte Carlo and ambient occlusion, and 8 with Monte Carlo and global illumination.

The stochastic paths come from the CPU callers of `Random()`, plus the seeded generators that run during a render: `cRandom` in `lights.cpp`, and the Perlin noise generator created in `render_job.cpp`. These are the candidates among the 151 usable scenes. Scenes other than the three timed above have not been timed yet.

| Path | Usable scenes | Candidate |
|---|---|---|
| Raymarch step jitter | 151 | Any scene |
| Monte Carlo depth of field | 21 | `chromatic_aberration.fract` |
| Monte Carlo antialiasing | 21 | `chromatic_aberration.fract` |
| Monte Carlo soft shadows | 9 | `chromatic_aberration.fract` |
| Monte Carlo chromatic aberration | 1 | `chromatic_aberration.fract` |
| Monte Carlo global illumination | 8 | `difs_tree.fract` |
| Rough surfaces | 5 | `difs_tree.fract` |
| Specular roughness | Most scenes, because it defaults on for metallic materials | Any scene with a specular highlight |
| Post-effect depth of field | 38 | `IFS_002.fract` |
| Multi-ray ambient occlusion | 10 | `IFS_001.fract` |
| Random screen-space ambient occlusion | 7 | `amazing_surf 002.fract` |
| Volumetric fog | 70 | `EXR example.fract` |
| Iteration fog | 7 | `benchmark-GPU.fract` |
| Volumetric auxiliary light | 2 | `IFS_001.fract` |
| Beam light | 0 | `tests/scenes/beam_light.fract`, purpose-built |
| Random lights | 4 | `mandelbulb_bermarte 001.fract` |
| Perlin noise on materials | 3 | `difs_tree.fract` |
| Clouds | 6 | `clouds 001.fract` |

A beam light is light type 4. A test scene built on the sweep base rendered with no errors, and its image differed from a point light in all but 175 of 19,200 pixels.

## R14. Compared channels

- **Decision**: Compare every channel the engine writes in a gated render: the three color stages, alpha, depth, normal, and world normal.
- **Rationale**: The constitution requires every channel the engine writes to be compared. `render_worker.cpp` writes alpha on every render, and post-effect depth of field blends it separately from color in `dof.cpp`, so comparing color does not cover it. In three measured scenes it held 1, 8,791, and 17,982 distinct values, so it is not a simple mask. `render_job.cpp` turns world normal on for every render, with a FIXME, because the denoiser reads it.
- **Not compared**: Specular, diffuse, world position, shadow, global illumination, and not-denoised are written only when enabled. They are application settings, and no example scene sets them. Opacity and object color are working inputs to SSAO, and no image format can save them, so they are not channels.
- **If the FIXME is fixed**: The engine would stop writing world normal, so every render would fail export validation. A change to that behavior therefore cannot pass unnoticed.
- **Size**: 60 bytes per pixel in total. The reference set stays under the 100 MB limit (R4).
