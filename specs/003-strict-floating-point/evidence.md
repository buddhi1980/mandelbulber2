# Evidence: Strict Floating-Point Builds

This record says what changed when the builds stopped letting the compiler change floating-point results, what that costs, and how the check that keeps the prohibited options out was proved. [contracts/evidence-record.md](contracts/evidence-record.md) defines it. Every number here comes from a fresh build of the commit named beside it. Source lines are those of commit `8fd49ba59`.

## Header

| Field | Value |
|---|---|
| Date | 2026-11-07 to 2026-11-08 |
| Computer | Intel Core i9-14900K, 32 logical CPUs. `/proc/cpuinfo` lists `fma` and `avx2` |
| Environment | WSL `Ubuntu-24.04`, kernel 6.18.54.1, for the Linux builds. Windows 11 Pro for the MSVC builds |
| Toolchain | GCC 13.3.0, glibc 2.39, Qt 6.4.2, and CMake 3.28.3 in WSL, with `mandelbulber-opencl.pro` in release mode. MSVC 19.51.36260, toolset v145, from Visual Studio 18 Community on Windows |
| Thread counts | 1, and all 32 |
| Base commit | `8fd49ba594ce46091a3d4d7165f1fb202b1b788c`, the tip of `ministic-regression` |
| Final commit | `0383be876097b3a6a938acd6285cbefb43979360`, the last commit that changes what compiles. The commits after it change scripts, CI files, and documents |

## Builds

| Name | Source | Purpose | Result |
|---|---|---|---|
| Before | `8fd49ba59` | Output and time before this change | Compiled in `~/mandelbulber2-before` with `build_fresh.sh`. `CXXFLAGS = -pipe -ffast-math -fopenmp -O3 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)`, and `readelf -d` finds `libmvec` among the binary's needed libraries |
| After | The final commit | Output and time after this change | Compiled in `~/mandelbulber2` with `build_fresh.sh`. `CFLAGS = -pipe -ffp-contract=off -O2 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)`, `CXXFLAGS = -pipe -ffp-contract=off -fopenmp -O3 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)`, and the binary no longer needs `libmvec` |
| Before, host-tuned | `8fd49ba59`, built with `CXXFLAGS=-march=native` | What users of the setup scripts lose (FR-027) | Compiled in `~/mandelbulber2-native` with `CXXFLAGS=-march=native build_fresh.sh`, so qmake imported it: `CXXFLAGS = -pipe -march=native -ffast-math -fopenmp -O3 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)` |
| Diagnostic | The final commit, plus a local patch that counts guard hits | Which items reach each guard (FR-020) | Compiled in `~/mandelbulber2-diag`. The patch adds `mandelbulber2/src/guard_counts.hpp`, one call inside each guard before it changes the value, and a static object that writes the counts on exit. `--compare` against the after build exited 0 with 0 of 487 items failed, so it renders exactly as the after build does |
| Guards off | The final commit, plus a local patch that makes `IsNotANumber()` return `false` | What the guards change, and where (FR-020) | Compiled in `~/mandelbulber2-diag`, with both `IsNotANumber()` methods in `mandelbulber2/src/algebra.hpp` returning `false`. `--compare` against the after build exited 0 with 0 of 487 items failed. `--collect` gave 487 of 487 items, and recorded the patch as an uncommitted change |
| No contraction option | The final commit, plus a local patch that removes the `gcc` block | Whether `-ffp-contract=off` changes anything (FR-022) | Compiled in `~/mandelbulber2-diag`, with no `-ffp-contract` in `CFLAGS` or `CXXFLAGS`. `--compare` against the after build exited 0 with 0 of 487 items failed |
| MSVC before | `8fd49ba59`, Release x64 | The comparison images, and the MSVC checks | Compiled in the new clone `C:\GIT\mb2-kd2-8fd49ba59` with toolset v145, compiler version 19.51.36260, and its `cl` line holds `/fp:precise`. `--collect tests/outputs/collection/msvc` exited 0 with 487 of 487 items collected, and recorded no uncommitted changes |
| MSVC after | The final commit, Release x64 | The MSVC checks | Compiled in the new clone `C:\GIT\mb2-kd2-0383be876`, and its `cl` line holds `/fp:precise`. `--test` passed 9 of 9 tests in 334 seconds, `tests/test_regression_runner.py` passed 224 tests, and `--collect tests/outputs/collection/msvc` exited 0 with 487 of 487 items collected and no uncommitted changes |

The local patches stayed in the throwaway clone `~/mandelbulber2-diag` and were never committed (FR-020).

## Flag check

Every seed in [contracts/flag-check.md](contracts/flag-check.md) ran the committed script, `.github/actions/compile-engine/check-flags.sh` at `49f25a38f`, from a fresh WSL build of that commit, through `~/verify/kd2/seed_flags.sh` and `~/verify/kd2/seed_search.sh`. Each row checked the exit code, every message, and the number of failures reported. All 33 `flags` cases and all 23 `search` checks gave the expected result. The logs are `~/verify/kd2/results/seed_flags.txt` and `seed_search.txt`.

### `flags`

Each fault went into a copy of the Makefile that `qmake6` generated from `49f25a38f`, which passes. Each named exactly one failure, apart from `CPPFLAGS`, which named the two lines it reaches.

| Seed | Exit | Named |
|---|---|---|
| `-ffast-math` added to `CXXFLAGS` | 1 | `-ffast-math in CXXFLAGS` |
| `-Ofast` added to `CXXFLAGS` | 1 | `-Ofast in CXXFLAGS` |
| `-funsafe-math-optimizations` added to `CXXFLAGS` | 1 | `-funsafe-math-optimizations in CXXFLAGS` |
| `-ffinite-math-only` added to `CXXFLAGS` | 1 | `-ffinite-math-only in CXXFLAGS` |
| `-fassociative-math` added to `CXXFLAGS` | 1 | `-fassociative-math in CXXFLAGS` |
| `-freciprocal-math` added to `CXXFLAGS` | 1 | `-freciprocal-math in CXXFLAGS` |
| `-fno-signed-zeros` added to `CXXFLAGS` | 1 | `-fno-signed-zeros in CXXFLAGS` |
| `-ffp-model=fast` added to `CXXFLAGS` | 1 | `-ffp-model=fast in CXXFLAGS` |
| `-march=native` added to `CXXFLAGS` | 1 | `-march=native in CXXFLAGS` |
| `-mtune=native` added to `CXXFLAGS` | 1 | `-mtune=native in CXXFLAGS` |
| `-mcpu=native` added to `CXXFLAGS` | 1 | `-mcpu=native in CXXFLAGS` |
| `-ffast-math` added to `CFLAGS` | 1 | `-ffast-math in CFLAGS` |
| `-ffast-math` added to `LFLAGS` | 1 | `-ffast-math in LFLAGS` |
| `-ffast-math` added to `LIBS` | 1 | `-ffast-math in LIBS` |
| `-ffast-math` added to `CXX` | 1 | `-ffast-math in CXX` |
| `-Ofast` added to `LINK` | 1 | `-Ofast in LINK` |
| `-ffp-contract=fast` after `-ffp-contract=off` in `CXXFLAGS` | 1 | `-ffp-contract=fast in CXXFLAGS` |
| `-ffp-contract=on` added to `CFLAGS` | 1 | `-ffp-contract=on in CFLAGS` |
| `-ffp-contract=off` removed from `CXXFLAGS` | 1 | `-ffp-contract=off missing from CXXFLAGS` |
| `-ffp-contract=off` removed from `CFLAGS` | 1 | `-ffp-contract=off missing from CFLAGS` |
| `qmake6` run with `CXXFLAGS=-march=native` | 1 | `-march=native in CXXFLAGS` |
| `qmake6` run with `CFLAGS=-march=native` | 1 | `-march=native in CFLAGS` |
| `qmake6` run with `CPPFLAGS=-Ofast` | 1 | `-Ofast in CFLAGS` and `-Ofast in CXXFLAGS` |
| `qmake6` run with `LDFLAGS=-ffast-math` | 1 | `-ffast-math in LFLAGS` |
| The clean Makefile | 0 | `passed` |
| `-fno-fast-math` added | 0 | `passed` |
| `-march=x86-64` added | 0 | `passed` |
| `-L/opt/x-ffast-math/lib` added to `LIBS` | 0 | `passed` |
| Missing Makefile | 2 | `cannot read missing.mk` |
| Empty Makefile | 2 | `cannot read empty.mk, because it is empty` |
| Makefile with no `LIBS` line | 2 | `nolibs.mk has no LIBS line` |
| No arguments | 2 | The usage line |
| `search` outside a git checkout | 2 | `no build files found, because /tmp is not in a git checkout` |

### `search`

A throwaway clone under `/tmp` started at `8fd49ba59` with the script copied in, then checked out `49f25a38f`. Each seed went on a new last line of its file, and the file was restored before the next row. Each fault named exactly one failure, with its file and line.

| Tree or file | Seed | Exit | Named |
|---|---|---|---|
| `8fd49ba59`, which searched 72 files | None | 1 | Exactly the 9 lines this feature removes: `-ffast-math` at `mandelbulber2/qmake/common.pri:127`, `:128`, `:129`, and `:130`, and `mandelbulber2/cmake/CMakeLists.txt:63`, `-march=native` at `mandelbulber2/tools/prepare_for_dev_ubuntu.sh:12`, `mandelbulber2/tools/prepare_for_dev_debian_testing.sh:12`, and `mandelbulber2/deploy/linux/install:13`, and `FloatingPointModel Fast` at `mandelbulber2/msvc/mandelbulber2.vcxproj:97` |
| `49f25a38f`, which searched 73 files | None | 0 | `passed` |
| `mandelbulber2/qmake/common.pri` | `-ffast-math` | 1 | `-ffast-math in mandelbulber2/qmake/common.pri:179` |
| `mandelbulber2/qmake/common.pri` | `-ffp-contract=fast-honor-pragmas` | 1 | `-ffp-contract=fast-honor-pragmas in mandelbulber2/qmake/common.pri:179` |
| `mandelbulber2/qmake/mandelbulber.pro` | `-Ofast` | 1 | `-Ofast in mandelbulber2/qmake/mandelbulber.pro:6` |
| `mandelbulber2/cmake/CMakeLists.txt` | `-ffinite-math-only` | 1 | `-ffinite-math-only in mandelbulber2/cmake/CMakeLists.txt:224` |
| `mandelbulber2/cmake/CMakeLists.txt` | `-ffp-model=fast` | 1 | `-ffp-model=fast in mandelbulber2/cmake/CMakeLists.txt:224` |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `<FloatingPointModel>Fast</FloatingPointModel>` | 1 | `FloatingPointModel Fast in mandelbulber2/msvc/mandelbulber2.vcxproj:372` |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `/fp:fast` | 1 | `/fp:fast in mandelbulber2/msvc/mandelbulber2.vcxproj:372` |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `/fp:contract` | 1 | `/fp:contract in mandelbulber2/msvc/mandelbulber2.vcxproj:372` |
| `mandelbulber2/tools/prepare_for_dev_ubuntu.sh` | `-march=native` | 1 | `-march=native in mandelbulber2/tools/prepare_for_dev_ubuntu.sh:100` |
| `mandelbulber2/deploy/linux/install` | `-mtune=native` | 1 | `-mtune=native in mandelbulber2/deploy/linux/install:39` |
| `mandelbulber2/make-package.sh` | `-mcpu=native` | 1 | `-mcpu=native in mandelbulber2/make-package.sh:116` |
| `mandelbulber2/tools/DOE/build.bot.sh` | `-fno-signed-zeros` | 1 | `-fno-signed-zeros in mandelbulber2/tools/DOE/build.bot.sh:88` |
| `azure-pipelines.yml` | `-ffp-contract=fast` | 1 | `-ffp-contract=fast in azure-pipelines.yml:39` |
| `.circleci/config.yml` | `-freciprocal-math` | 1 | `-freciprocal-math in .circleci/config.yml:115` |
| `.travis.yml` | `-fassociative-math` | 1 | `-fassociative-math in .travis.yml:129` |
| `Dockerfile` | `-funsafe-math-optimizations` | 1 | `-funsafe-math-optimizations in Dockerfile:73` |
| A new tracked script, `new-build.sh` | `-funsafe-math-optimizations` | 1 | `-funsafe-math-optimizations in new-build.sh:1`, with 74 files searched |
| The same script, untracked | `-funsafe-math-optimizations` | 0 | `passed`, with 73 files searched |
| `mandelbulber2/deploy/NEWS`, which is not a build file | `-ffast-math` | 0 | `passed` |
| `mandelbulber2/tools/DOE/NaNtester.sh`, which already holds 5 lines with `-ffast-math` | `-ffast-math` | 0 | `passed` |
| `.github/actions/compile-engine/check-flags.sh` | Its own list, at `49f25a38f` | 0 | `passed` |

## GitHub Actions

| Run | Commit | Seed | Result |
|---|---|---|---|
| [42462487815](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42462487815), on [pull request 7](https://github.com/Coastrnaut/mandelbulber2/pull/7) | `f1b5e0e5b`, which compiles as `0383be876` does | None | Passed in 5 minutes 23 seconds, on a fresh gated build. `Search the Build Files` printed "searched 73 build files" and "passed" before the build started, and `Reject Prohibited Compile Flags` printed "passed" after it. The token held `Contents: read` and `Metadata: read`. All 812 compile lines held `-ffp-contract=off`, and none held `-ffast-math` |
| [42490224807](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42490224807), on the throwaway [pull request 8](https://github.com/Coastrnaut/mandelbulber2/pull/8) into `upgrade/strict-floating-point` | `02ba3574a`, on the branch `seed/flag-check` | `-ffast-math` on the CMake line | Failed in 12 seconds. `Search the Build Files` printed "FAILED: -ffast-math in mandelbulber2/cmake/CMakeLists.txt:63" and "1 check(s) failed", and the build never started |
| [42490318167](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42490318167), on the same throwaway pull request | `a809b805a`, which reverts the first seed and adds `CXXFLAGS: "-march=${{ 'native' }}"` to the job | A job-level `CXXFLAGS` that resolves to `-march=native` | Failed after the build, in 8 minutes 13 seconds. The job's environment showed `CXXFLAGS: -march=native`. `Search the Build Files` printed "searched 73 build files" and "passed", because the file never holds the option as text. The build ran, and all 812 compile lines held `-march=native`. Then `Reject Prohibited Compile Flags` printed "FAILED: -march=native in CXXFLAGS" and "1 check(s) failed" |

Each push to the throwaway branch also started `C++ CI Build (Qt6)` and Cross-Build Check, and all six of those runs were cancelled at once, before any reached its release step. Pull request 8 was closed unmerged, `seed/flag-check` was deleted on GitHub and locally, and its worktree was removed. No commit on `upgrade/strict-floating-point` is a seed.

## Output

### Summary

From fresh WSL builds of `8fd49ba59` and `0383be876`. Each was collected from its own clone with the harness from its own commit, and both collections hold 487 of 487 items with no uncommitted changes. `~/verify/kd2/output_record.py` compared them with the harness's own functions, because `--cross-check` refuses two commits. `output-record.tsv` beside this file holds one row per item.

| Measure | Value |
|---|---|
| Items with any difference | 58 of 487: 25 of the 26 scenes, and 33 of the 461 swept formulas |
| Items identical in every buffer and statistic | 429 |
| Items whose final image holds one value on one side only | 0 |
| Items the guards change | 0. `--compare` against the guards-off build exited 0 with 0 of 487 items failed, so it names no item |
| `-ffp-contract=off` | `--compare` against the build without it exited 0, with 0 of 487 items failed |
| Exit status of `--compare` | 1, as expected: "parity failure (58 of 487 items failed)" |
| Each buffer | Items that differ: `raw_render` 28, `post_effect` 28, `final_image` 28, `alpha` 9, `depth` 10, `normal` 23, `normal_world` 46 |
| Each statistic | Items that differ: `total_iterations` 27, `raymarch_count` 2, `missed_de` 2, `iteration_histogram` 22, `step_histogram` 10 |

The final image changed on 28 items: 24 scenes and 4 formulas. On 23 of them, no pixel changed by more than 1% of the range. Five scenes have pixels that changed by more than 10%: `difs_tree` 1,669, `riemann_sphere_msltoe_001` 481, `hybrid007` 471, `menger cross mod1 001` 142, and `chromatic_aberration` 2. One formula, `transf_spherical_coord_invs`, has one such pixel.

The guards never act, and the guards-off build renders every item exactly as the after build does. Nothing but the compile options differs between the two builds. So every change below comes from strict arithmetic: arithmetic order, vector math, and signed zero together, which this feature doesn't separate (FR-020).

### Scenes

Samples changed counts `final_image` samples, of 230,400. Average change is the average absolute difference over all of them, with samples running from 0 to 65,535. Guards gives the hits at the four sites, which are 0 for every scene, so the column reads 0. The proof for every changed scene is the guards-off comparison, which shows the guards change nothing on any item.

| Scene | Samples changed | Average change | Guards | Review | Cause | Proof |
|---|---|---|---|---|---|---|
| `IFS_001` | 131 | 0.0006 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `Makin3D-Julia_001` | 404 | 0.0067 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `IFS_002` | 173 | 0.0010 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `amazing_surf 002` | 147 | 0.0006 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `EXR example` | 708 | 0.0057 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `benchmark-GPU` | 5 | Below 0.0001 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `menger octo 001` | 216 | 0.0010 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `clouds 005` | 99 | 0.0005 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `gradients - specular highlights` | 72 | 0.0003 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `hybrid007` | 20,251 | 94.7603 | 0 | No visible change. 471 pixels changed by more than 10%, the largest by 32,299, as single pixels along the edges of the small flower shapes. A crop enlarged 4 times shows no difference a viewer can pick out | Strict arithmetic | Guards-off comparison |
| `hybrid008 - collatz` | 5,952 | 0.1101 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `mandelbox_menger` | 4,487 | 0.0834 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `mandelbulb_eye` | 30 | 0.0001 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `menger cross mod1 001` | 8,104 | 38.7849 | 0 | No visible change. 142 pixels changed by more than 10%, the largest by 51,547, as isolated pixels in dark detail. A 4 times crop shows no difference a viewer can pick out | Strict arithmetic | Guards-off comparison |
| `menger prism shape2 001` | 65 | 0.0003 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `pseudo kleinian std de 001` | 493 | 0.0472 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `voxel_export_with_Bristorbrot4D` | 106 | 0.0009 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `beam_light` | 47 | 0.0002 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `hdr_blur` | 0 | 0 | 0 | No change in any buffer or statistic | None | None |
| `jos_kleinian` | 0 | 0 | 0 | No change in the final image. Only `normal_world` differs, in 22,145 samples | Strict arithmetic | Guards-off comparison |
| `chromatic_aberration` | 14,485 | 1.7386 | 0 | No visible change. 2 pixels changed by more than 10%, in the Monte Carlo grain of the crystal | Strict arithmetic | Guards-off comparison |
| `difs_tree` | 42,049 | 309.6822 | 0 | No visible change. 1,669 pixels changed by more than 10%, the largest by 43,748: the Monte Carlo grain on the tree lands on different pixels, and the tree looks the same, at full size and in a 4 times crop | Strict arithmetic | Guards-off comparison |
| `hex grid 001` | 65 | 0.0007 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `mandelnest pow 5` | 248 | 0.0011 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `monte carlo DOF 001` | 48 | 0.0002 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `riemann_sphere_msltoe_001` | 48,210 | 147.5060 | 0 | No visible change. 481 pixels changed by more than 10%, the largest by 23,539, in the grain on the floor of the bowl. A 4 times crop shows no difference a viewer can pick out | Strict arithmetic | Guards-off comparison |

### Formulas

The rule: rank the swept formulas by the average absolute change in `final_image`, with ties by name, and take the first 20. Only 4 of the 461 changed in the final image at all, so the other 16 are the first formulas by name, and their final images are byte-identical. Samples changed is of 2,304. The review showed each formula enlarged 10 times. 33 formulas differ in some buffer or statistic, most of them in `normal_world` only.

| Formula | Samples changed | Average change | Guards | Review | Cause | Proof |
|---|---|---|---|---|---|---|
| `transf_spherical_coord_invs` | 3 | 83.5152 | 0 | **Visible**: one pixel, at column 4, row 4, changed from near-white `[63446, 64575, 65396]` to near-black `[303, 330, 365]`, so one isolated bright pixel vanished. `missed_de` rose from 58 to 59 | Strict arithmetic | The guards-off build gives the near-black value too. Both MSVC builds give the near-white value, which is the before value, so here GCC now differs from MSVC |
| `scator_power2` | 555 | 6.9839 | 0 | No visible change. The changes are in near-black pixels, the largest by 269 | Strict arithmetic | Guards-off comparison |
| `riemann_sphere_msltoe` | 6 | 0.0161 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `transf_inv_cylindrical` | 3 | 0.0078 | 0 | No visible change | Strict arithmetic | Guards-off comparison |
| `abox4d`, `abox_donut4d`, `abox_donut4d_v2`, `abox_klein`, `abox_mb3d1`, `abox_mod1`, `abox_mod11`, `abox_mod12`, `abox_mod13`, `abox_mod14`, `abox_mod15`, `abox_mod2`, `abox_mod_kali`, `abox_mod_kali_eiffie`, `abox_mod_kali_v2`, and `abox_mod_kali_v3` | 0 | 0 | 0 | No change: the final images are byte-identical | None | None |

The one visible change has a named cause, so FR-023 accepts it once it is recorded here.

**Review**: Every image of the 26 scenes and 20 formulas was examined at full size, and so was a 4 times crop of the 80 by 60 region with the most large changes in each of the five scenes with pixels changed by more than 10%. The user reviewed the same images and crops on 2026-11-07, and agreed that the only visible change is the one pixel in `transf_spherical_coord_invs`. The images are in `~/verify/kd2/review/` in WSL, and they were never committed.

### Guards

From the guard-count build of `0383be876`, which rendered every item once, one at a time, at the harness's resolution and overrides (`~/verify/kd2/guard_counts.py`, with the table in `~/verify/kd2/results/guards.tsv`). Each render wrote its counts on exit, one line per site, even with no hits, so a missing file would have stopped the run.

| Site | Items reached | Infinite hits | NaN hits |
|---|---|---|---|
| `compute_fractal.cpp:316` | 0 of 487 | 0 | 0 |
| `compute_fractal.cpp:500` | 0 of 487 | 0 | 0 |
| `render_worker.cpp:776` | 0 of 487 | 0 | 0 |
| `shader_calculate_normals.cpp:120` | 0 of 487 | 0 | 0 |

No item reaches any guard with a non-finite value, so no point counts as inside through the first guard. The two items that turn on iteration threshold mode, `Makin3D-Julia_001` and `benchmark-GPU`, have no hits either, and no item turns on interior mode.

### Failures found

None. Every item rendered and collected on both builds, Self-Parity passed three times on the after build, and no final image became a single value (FR-018).

## Measurements

### Time

All times are seconds on the monotonic clock, from `~/verify/kd2/timed_collect.py`, which runs the harness's `--collect` and adds up the render time of scenes and of formulas apart. All threads means 32. Each figure is one run.

**How quiet each run was**: Before each group of runs, the script waited until no FractalEvaluator or other `mandelbulber2` process had run for 60 seconds. During every run, a watcher sampled `pgrep -f FractalEvaluator` every 5 seconds and logged the uptime. A group with a sample inside any of its runs was repeated whole and not recorded, and nothing else from this feature ran during timing. The first attempt overlapped FractalEvaluator in 4 of its 5 runs, so none of it is recorded. In the second, the "before build first" group ran clean on 2026-11-07. The "after build first" group overlapped once, and WSL restarted while it waited, so it and the remaining groups ran on 2026-11-08 in a new WSL session, where the watcher saw FractalEvaluator 0 times. No recorded run overlapped another render.

| Run | Build | First or second | All threads | Scenes | Formulas |
|---|---|---|---|---|---|
| Before build first, 2026-11-07 | Before | First | 59.73 | 31.20 | 18.84 |
| Before build first, 2026-11-07 | After | Second | 60.83 | 30.15 | 21.05 |
| After build first, 2026-11-08 | After | First | 51.16 | 25.51 | 16.17 |
| After build first, 2026-11-08 | Before | Second | 51.77 | 26.83 | 15.89 |

| Run | One CPU | Scenes | Formulas |
|---|---|---|---|
| Before | 455.97 | 416.15 | 30.53 |
| After | 459.23 | 419.82 | 30.13 |

| Run | All threads | Compared with before |
|---|---|---|
| Before, host-tuned | 54.28, with scenes 28.43 and formulas 16.47 | 2.51 seconds, or 4.8%, slower than the before build's 51.77 in the same session |

**The cost**: At all threads, the after build took 1.8% longer than the before build when it ran second, and 1.2% less when it ran first. The order moves the time more than the change does, so no cost shows above the spread between runs. The two pairs ran a day apart, and differ by about 8 seconds in level. On one CPU, where arithmetic dominates, the after build took 3.26 seconds, or 0.7%, longer: scenes took 0.9% longer and formulas 1.3% less. The host-tuned build was no faster than the before build at all threads, so users of the setup scripts lose no speed this suite can measure. With one run per figure, a difference of a few percent is within what the runs can show.

| Stand-in for the gate | Minutes |
|---|---|
| Collect step on the pull request, on the 4-vCPU runner | 10.5: 10 minutes 28 seconds in the `linux-qmake` job of Cross-Build Check run [42462487710](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42462487710), on `f1b5e0e5b`. The push run of the same commit, at the same time, took 5 minutes 34 seconds, and its `--test` step took half as long, so the runners differ by about twice. Both stay under 30 minutes. The last KD-1 pull request's Collect step took 4.4 |

### Other builds

One row per build in [research.md](research.md) R14. Every build that can be built on this computer compiled from a fresh tree on both commits. The macOS rows come from CircleCI on the pull request.

| Build | Before | After | Proof | Flags read |
|---|---|---|---|---|
| qmake with OpenCL, `mandelbulber-opencl.pro`, the gated build | Compiled on `8fd49ba59` | Compiled on `0383be876` | `build_fresh.sh` in WSL | Before: `-ffast-math -fopenmp` in `CXXFLAGS`. After: `-ffp-contract=off -fopenmp` in `CXXFLAGS`, and `-ffp-contract=off` in `CFLAGS` |
| MSVC Release x64 | Compiled on `8fd49ba59` | Compiled on `0383be876` | `nuget restore`, then MSBuild with `/p:Configuration=Release /p:Platform=x64` | `/fp:precise` both times |
| MSVC Debug x64 | Compiled on `8fd49ba59`, in the new clone `C:\GIT\mb2-kd2-debug-8fd49ba59`, so FR-033 binds it | Compiled on `0383be876`, in the new clone `C:\GIT\mb2-kd2-debug-0383be876` | `nuget restore`, then MSBuild with `/p:Configuration=Debug /p:Platform=x64`, with no errors either time | Before: `/fp:fast` and `/arch:AVX2`. After: `/fp:precise` and `/arch:AVX2`, and no `/fp:fast` |
| qmake without OpenCL, `mandelbulber.pro` | Compiled on `8fd49ba59` | Compiled on `0383be876` | `~/verify/kd2/run_t077.sh` in `~/mandelbulber2-diag`, from a clean tree each time | No compile line holds `USE_OPENCL` |
| CMake | Compiled on `8fd49ba59` | Compiled on `0383be876` | `~/verify/kd2/run_t077.sh`, from a clean build folder each time | After: `CXX_FLAGS` holds `-fopenmp -ffp-contract=off`, and `link.txt` holds neither `-ffast-math` nor `-Ofast` |
| macOS, CircleCI, `mandelbulber-opencl-mac.pro` | Compiled in CircleCI's `build-and-pack` on KD-1's pull request, whose head `bb7d00936` merged as `8fd49ba59` | Compiled in `build-and-pack` on pull request 7, commit `f1b5e0e5b`, in 4.5 minutes | CircleCI job 70, read through CircleCI's public API, which cuts the step's output at 400,000 characters | All 122 compile lines in the readable part use LLVM `clang++` for `-arch arm64`, with `-ffp-contract=off -fopenmp`, and none holds `-ffast-math`. So the `gcc` scope reaches Clang |
| Apple Silicon, `mandelbulber-opencl-m1.pro` | Not known | Not built, because nothing builds it | Read alone: the `gcc` block sits above the `macx:m1` line, so `-Xpreprocessor` keeps taking `-I/opt/homebrew/include` and never takes `-ffp-contract=off` | Not covered |

**OpenCL**: On the MSVC Release build of `0383be876`, `--help-opencl` lists one device, an NVIDIA RTX A6000. `IFS_002.fract` rendered with `--gpu --res 320x240 --override opencl_mode=3` finished with exit 0, and the log reads "Use NVIDIA RTX A6000 for OpenCL rendering". The same render with `--override opencl_mode=3#DOF_enabled=0` also finished, and its image differs, with the foreground sharp where depth of field had blurred it. WSL has no OpenCL device, so the Linux OpenCL host code is not covered. OpenCL's build options in `mandelbulber2/src/opencl_engine.cpp` did not change (FR-032).

### MSVC and cross-build

| Check | Result |
|---|---|
| MSVC before against MSVC after | 487 of 487 items identical in every buffer and statistic, from `output_record.py` on the `8fd49ba59` and `0383be876` collections, because `--cross-check` refuses two commits. Neither the Release flags nor the sources changed, as FR-022 says |
| qmake after against MSVC after | `--cross-check` on the qmake and MSVC collections of `0383be876` exited 0: "identical: 448 of 487 items matched exactly in every buffer and statistic", with functional agreement and 0 of 487 items failed. The other 39 items differ, which is not drift, because the two builds are different targets. The same check on the two collections of `8fd49ba59` found 429 of 487, so 19 more items now match MSVC exactly |
| Cross-Build Check on pull request 7 | Both runs on `f1b5e0e5b`, for the push and for the pull request, passed `linux-qmake`, `windows-msvc`, and `cross-check`, and the cross-check reported "identical: 448 of 487 items matched exactly in every buffer and statistic" with functional agreement, the same as the local check |

## CPU math path

The record behind KD-5. [research.md](research.md) R18 describes the test program and how it runs. `~/verify/kd2/cpu_math_path.cpp`, built with `g++ -O2 -ffp-contract=off`, computed each function over the same 4 million values twice: once as it is, and once with `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4,-AVX2_Usable,-FMA_Usable`, which turns off glibc's FMA and AVX2 paths. It compared the hash of each function's result bits, and counted the values whose bits differ.

| Function | Same or different with glibc's FMA and AVX2 paths turned off |
|---|---|
| `sin` | Different: 2,816 of 4,000,000 values |
| `cos` | Different: 2,661 |
| `tan` | Different: 35 |
| `atan` | Different: 211 |
| `atan2` | Different: 251 |
| `asin` | Different: 2,548 |
| `exp` | Different: 2,766 |
| `log` | Different: 35 |
| `pow` | Different: 2,775 |
| `sqrt`, the control | The same, in all 4,000,000 values |

Run on 2026-11-08 in WSL on the Intel Core i9-14900K, whose `/proc/cpuinfo` lists `fma` and `avx2`, with GCC 13.3.0 and glibc 2.39. It repeats the planning run of 2026-11-07, which the KD-5 row cites, and which found the same nine functions different and `sqrt` the same. One binary gives different results for nine math functions depending on which path glibc picks from the CPU, so no compile flag can make two computers agree if their CPUs take different paths.

## Success criteria

| Criterion | Result | Commit | Not covered |
|---|---|---|---|
| SC-001 | `parity-gate` passed on pull request 7, run 42462487815, on a fresh gated build. The search passed before the build, the flag check printed "passed" after it, and all 812 compile lines held `-ffp-contract=off` with no prohibited option | `f1b5e0e5b` | The push to `ministic-regression` after a merge, which the user is told about (FR-010) |
| SC-002 | Every seed failed as expected, in one trial each: 33 of 33 `flags` cases and 23 of 23 `search` checks gave the expected exit code and named the option, or its file and line. The unseeded Makefile and the unseeded tree both pass | `49f25a38f` | None |
| SC-003 | Both seeded runs failed `parity-gate` in GitHub Actions: `-ffast-math` on the CMake line stopped the search before the build, and a job-level `CXXFLAGS` that resolved to `-march=native` made the flag check fail after a real gated build | `02ba3574a` and `a809b805a`, on the throwaway branch | None |
| SC-004 | The repository search in section 3 of [quickstart.md](quickstart.md) finds only the check's own `check-flags.sh`, `mandelbulber2/tools/DOE/NaNtester.sh`, and the history in `mandelbulber2/deploy/NEWS`, and `search` passes on 73 tracked files. Every GCC and Clang build definition sets `-ffp-contract=off`: the gated qmake build holds it in `CFLAGS` and `CXXFLAGS`, with no prohibited option on the `CFLAGS`, `CXXFLAGS`, `LFLAGS`, or `LIBS` lines, CMake holds `-fopenmp -ffp-contract=off` in `CXX_FLAGS`, sets it in `CMAKE_C_FLAGS`, and has nothing prohibited in `link.txt`, and CircleCI's Clang compile lines hold it. The binary no longer needs `libmvec`. The pragma search finds only the six `diagnostic` lines in `mandelbulber2/src/radiance_hdr.cpp`. The setup scripts hold no `march=`, `mtune=`, or `mcpu=`, pass `sh -n` and `bash -n`, and `qmake6` with `CXXFLAGS=-msse2` or `CXXFLAGS=-DKD2_OWN_FLAG` passes that value to `CXXFLAGS` | `0383be876` for the flags, the branch tip for the search, and `f1b5e0e5b` on CircleCI | The Apple Silicon qmake flags, because nothing builds `mandelbulber-opencl-m1.pro`. The compile path of `install`, which needs a packaged layout and `sudo`, so its one-line diff is the proof |
| SC-005 | Three `--self-parity` runs in a row on a fresh build each exited 0, with 487 exact matches and 0 of 487 items failed across all seven buffers and all statistics, at 1 thread and at all 32. They took 721, 566, and 528 seconds, while other work from this feature shared the computer, and none mentioned thread coverage | `0383be876` | None |
| SC-006 | `output-record.tsv` covers all 487 items. No item reaches a NaN guard, from the guard-count build, and the guards change no item, from the guards-off comparison. The user reviewed all 26 scenes and the 20 most-changed formulas. The one visible change, one pixel in `transf_spherical_coord_invs`, has a named cause, strict arithmetic, and no visible change is left unexplained | `0383be876` | None |
| SC-007 | Render times for all 487 items are recorded at all 32 threads in both build orders, on one CPU, and for the `-march=native` build. The after build took 1.8% longer and 1.2% less in the two orders, and 0.7% longer on one CPU. The watcher saw 0 FractalEvaluator samples during every recorded run | `8fd49ba59` and `0383be876` | The two orders ran a day apart, in two WSL sessions. Each pair ran back to back |
| SC-008 | The Collect step of `linux-qmake` on pull request 7 took 10 minutes 28 seconds, and 5 minutes 34 seconds on the push run of the same commit, both under 30 minutes. No references exist yet, so this stands in for a gate run | `f1b5e0e5b` | A real gate run, which needs Baseline 1 |
| SC-009 | Locally: `--test` passed 9 of 9 tests on the fresh qmake build in 151 seconds, and 9 of 9 on the fresh MSVC build in 334 seconds. `tests/test_regression_runner.py` passed 224 tests on both. `--cross-check` of the two collections of the final commit exited 0 with functional agreement and 448 of 487 items identical. On pull request 7, Cross-Build Check passed both runs, with `--test` in both build jobs and "identical: 448 of 487" in `cross-check`, and `C++ CI Build (Qt6)` passed, with its own `--test` | `0383be876`, and `f1b5e0e5b` in CI | None |
| SC-010 | Locally, each compiled from a fresh tree on `8fd49ba59` and on `0383be876`: the gated qmake build, qmake without OpenCL, CMake, MSVC Release, and MSVC Debug. On pull request 7, CircleCI's macOS build and Azure Pipelines passed | `0383be876`, and `f1b5e0e5b` in CI | `mandelbulber-opencl-m1.pro`, which nothing builds |
| SC-011 | Against `8fd49ba59`, nothing changed under `mandelbulber2/src`, `mandelbulber2/qt`, `mandelbulber2/formula`, or `tests`, and nothing in any file FR-016 names: the three existing workflows, `build-linux`, `build-windows`, `.appveyor.yml`, `azure-pipelines.yml`, `.circleci`, and `.travis.yml`. The build files and scripts differ only in the six files the build-flags contract lists. FR-018 required no engine change | The branch tip | None |
| SC-012 | The constitution shows version 1.1.0, with KD-2 and KD-3 closed on 2026-11-08, KD-4 open, and KD-5 recorded as open. In the playbook, the `compile-engine` listing and the Phase 3 `parity-gate.yml` listing match the committed files byte for byte, both `parity-gate.yml` listings set read-only access, and Phase 4 holds the step "Settle the CPU Math Path". No file exists under `tests/references/` | The branch tip | A maintainer's approval of the amendment, which comes with the merge |
