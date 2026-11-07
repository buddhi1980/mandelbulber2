# Evidence: Strict Floating-Point Builds

This record says what changed when the builds stopped letting the compiler change floating-point results, what that costs, and how the check that keeps the prohibited options out was proved. [contracts/evidence-record.md](contracts/evidence-record.md) defines it. Every number here comes from a fresh build of the commit named beside it. Source lines are those of commit `8fd49ba59`.

A cell that reads "Pending" names the task that fills it.

## Header

| Field | Value |
|---|---|
| Date | 2026-11-07 |
| Computer | Intel Core i9-14900K, 32 logical CPUs. `/proc/cpuinfo` lists `fma` and `avx2` |
| Environment | WSL `Ubuntu-24.04`, kernel 6.18.54.1, for the Linux builds. Windows 11 Pro for the MSVC builds |
| Toolchain | GCC 13.3.0, glibc 2.39, Qt 6.4.2, and CMake 3.28.3 in WSL, with `mandelbulber-opencl.pro` in release mode. MSVC 19.51.36260, toolset v145, from Visual Studio 18 Community on Windows |
| Thread counts | 1, and all 32 |
| Base commit | `8fd49ba594ce46091a3d4d7165f1fb202b1b788c`, the tip of `ministic-regression` |
| Final commit | Pending, T027 |

## Builds

| Name | Source | Purpose | Result |
|---|---|---|---|
| Before | `8fd49ba59` | Output and time before this change | Compiled in `~/mandelbulber2-before` with `build_fresh.sh`. `CXXFLAGS = -pipe -ffast-math -fopenmp -O3 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)`, and `readelf -d` finds `libmvec` among the binary's needed libraries |
| After | The final commit | Output and time after this change | Pending, T027 |
| Before, host-tuned | `8fd49ba59`, built with `CXXFLAGS=-march=native` | What users of the setup scripts lose (FR-027) | Pending, T038 |
| Diagnostic | The final commit, plus a local patch that counts guard hits | Which items reach each guard (FR-020) | Pending, T032 |
| Guards off | The final commit, plus a local patch that makes `IsNotANumber()` return `false` | What the guards change, and where (FR-020) | Pending, T034 |
| No contraction option | The final commit, plus a local patch that removes the `gcc` block | Whether `-ffp-contract=off` changes anything (FR-022) | Pending, T035 |
| MSVC before | `8fd49ba59`, Release x64 | The comparison images, and the MSVC checks | Compiled in the new clone `C:\GIT\mb2-kd2-8fd49ba59` with toolset v145, compiler version 19.51.36260, and its `cl` line holds `/fp:precise`. `--collect tests/outputs/collection/msvc` exited 0 with 487 of 487 items collected, and recorded no uncommitted changes |
| MSVC after | The final commit, Release x64 | The MSVC checks | Pending, T043 |

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
| Pending, T051 | | None | |
| Pending, T052 | | `-ffast-math` on the CMake line | |
| Pending, T052 | | A job-level `CXXFLAGS` that resolves to `-march=native` | |

## Output

### Summary

Pending, T037.

| Measure | Value |
|---|---|
| Items with any difference | |
| Items identical in every buffer and statistic | |
| Items whose final image holds one value on one side only | |
| Items the guards change | |
| `-ffp-contract=off` | |
| Exit status of `--compare` | |

### Scenes

Pending, T037. One row for each of the 26 scenes.

| Scene | Samples changed | Average change | Guards | Review | Cause | Proof |
|---|---|---|---|---|---|---|

### Formulas

Pending, T037. The 20 swept formulas whose final image changed most.

| Formula | Samples changed | Average change | Guards | Review | Cause | Proof |
|---|---|---|---|---|---|---|

### Guards

Pending, T033.

| Site | Items reached | Infinite hits | NaN hits |
|---|---|---|---|
| `compute_fractal.cpp:316` | | | |
| `compute_fractal.cpp:500` | | | |
| `render_worker.cpp:776` | | | |
| `shader_calculate_normals.cpp:120` | | | |

### Failures found

Pending, T037.

## Measurements

### Time

Pending, T041. All times are seconds on the monotonic clock.

| Run | Build | First or second | All threads | Scenes | Formulas |
|---|---|---|---|---|---|
| Before build first | Before | First | | | |
| Before build first | After | Second | | | |
| After build first | After | First | | | |
| After build first | Before | Second | | | |

| Run | One CPU |
|---|---|
| Before | |
| After | |

| Run | All threads | Compared with before |
|---|---|---|
| Before, host-tuned | | |

| Stand-in for the gate | Minutes |
|---|---|
| Collect step on the pull request, on the 4-vCPU runner | Pending, T053 |

### Other builds

One row per build in [research.md](research.md) R14.

| Build | Before | After | Proof | Flags read |
|---|---|---|---|---|
| MSVC Debug x64 | Compiled on `8fd49ba59`, in the new clone `C:\GIT\mb2-kd2-debug-8fd49ba59`, so FR-033 binds it | Pending, T047 | `nuget restore`, then MSBuild with `/p:Configuration=Debug /p:Platform=x64` | Before: `/fp:fast` and `/arch:AVX2` |
| qmake without OpenCL, `mandelbulber.pro` | Pending, T046 | Pending, T046 | | |
| CMake | Pending, T046 | Pending, T046 | | |
| macOS, CircleCI | Pending, T054 | Pending, T054 | | |
| Apple Silicon, `mandelbulber-opencl-m1.pro` | Not known | Pending, T054 | | |

### MSVC and cross-build

| Check | Result |
|---|---|
| MSVC before against MSVC after | Pending, T044 |
| qmake after against MSVC after | Pending, T045 |

## CPU math path

Pending, T056. The record behind KD-5. [research.md](research.md) R18 describes the test program and how it runs.

| Function | Same or different with glibc's FMA and AVX2 paths turned off |
|---|---|
| `sin` | |
| `cos` | |
| `tan` | |
| `atan` | |
| `atan2` | |
| `asin` | |
| `exp` | |
| `log` | |
| `pow` | |
| `sqrt`, the control | |

## Success criteria

| Criterion | Result | Commit | Not covered |
|---|---|---|---|
| SC-001 | Pending, T051 | | |
| SC-002 | Every seed failed as expected, in one trial each: 33 of 33 `flags` cases and 23 of 23 `search` checks gave the expected exit code and named the option, or its file and line. The unseeded Makefile and the unseeded tree both pass | `49f25a38f` | None |
| SC-003 | Pending, T052 | | |
| SC-004 | Partly done, and T060 runs the repository search. `search` passes on 73 tracked files. The generated qmake flags hold `-ffp-contract=off` in `CFLAGS` and `CXXFLAGS`, as the contract expects, with no prohibited option on the `CFLAGS`, `CXXFLAGS`, `LFLAGS`, or `LIBS` lines, and the binary no longer needs `libmvec`. The CMake build holds `-fopenmp -ffp-contract=off` in `CXX_FLAGS`, sets it in `CMAKE_C_FLAGS`, and has nothing prohibited in `link.txt`. The pragma search finds only the six `diagnostic` lines in `mandelbulber2/src/radiance_hdr.cpp`. The setup scripts hold no `march=`, `mtune=`, or `mcpu=`, pass `sh -n` and `bash -n`, and `qmake6` with `CXXFLAGS=-msse2` or `CXXFLAGS=-DKD2_OWN_FLAG` passes that value to `CXXFLAGS` | `0383be876` for the flags, `49f25a38f` for the search and the scripts, which hold `9ded2b0ae`'s scripts unchanged | The macOS and Apple Silicon qmake flags are not read here: CircleCI is the only macOS proof (T054), and nothing builds `mandelbulber-opencl-m1.pro`. The compile path of `install` needs a packaged layout and `sudo`, so its one-line diff is the proof |
| SC-005 | Pending, T028 | | |
| SC-006 | Pending, T037 | | |
| SC-007 | Pending, T041 | | |
| SC-008 | Pending, T053 | | |
| SC-009 | Pending, T042 to T045 and T054 | | |
| SC-010 | Pending, T046, T047, and T054 | | |
| SC-011 | Pending, T060 | | |
| SC-012 | Pending, T057 and T058 | | |
