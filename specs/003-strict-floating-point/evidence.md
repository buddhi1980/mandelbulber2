# Evidence: Strict Floating-Point Builds

This record says what changed when the builds stopped letting the compiler change floating-point results, what that costs, and how the check that keeps the prohibited options out was proved. [contracts/evidence-record.md](contracts/evidence-record.md) defines it. Every number here comes from a fresh build of the commit named beside it. Source lines are those of commit `8fd49ba59`.

A cell that reads "Pending" names the task that fills it.

## Header

| Field | Value |
|---|---|
| Date | 2026-11-07 |
| Computer | Intel Core i9-14900K, 32 logical CPUs. `/proc/cpuinfo` lists `fma` and `avx2` |
| Environment | WSL `Ubuntu-24.04`, kernel 6.18.54.1, for the Linux builds. Windows 11 Pro for the MSVC builds |
| Toolchain | GCC 13.3.0, glibc 2.39, Qt 6.4.2, and CMake 3.28.3 in WSL, with `mandelbulber-opencl.pro` in release mode. MSVC from Visual Studio 18 Community on Windows |
| Thread counts | 1, and all 32 |
| Base commit | `8fd49ba594ce46091a3d4d7165f1fb202b1b788c`, the tip of `ministic-regression` |
| Final commit | Pending, T027 |

## Builds

| Name | Source | Purpose | Result |
|---|---|---|---|
| Before | `8fd49ba59` | Output and time before this change | Pending, T004 |
| After | The final commit | Output and time after this change | Pending, T027 |
| Before, host-tuned | `8fd49ba59`, built with `CXXFLAGS=-march=native` | What users of the setup scripts lose (FR-027) | Pending, T038 |
| Diagnostic | The final commit, plus a local patch that counts guard hits | Which items reach each guard (FR-020) | Pending, T032 |
| Guards off | The final commit, plus a local patch that makes `IsNotANumber()` return `false` | What the guards change, and where (FR-020) | Pending, T034 |
| No contraction option | The final commit, plus a local patch that removes the `gcc` block | Whether `-ffp-contract=off` changes anything (FR-022) | Pending, T035 |
| MSVC before | `8fd49ba59`, Release x64 | The comparison images, and the MSVC checks | Pending, T005 |
| MSVC after | The final commit, Release x64 | The MSVC checks | Pending, T043 |

The local patches stayed in the throwaway clone `~/mandelbulber2-diag` and were never committed (FR-020).

## Flag check

Pending, T026. One row per seed in [contracts/flag-check.md](contracts/flag-check.md), with its exit code and the option it named.

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
| MSVC Debug x64 | Pending, T006 | Pending, T047 | | |
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
| SC-002 | Pending, T026 | | |
| SC-003 | Pending, T052 | | |
| SC-004 | Pending, T026 and T060 | | |
| SC-005 | Pending, T028 | | |
| SC-006 | Pending, T037 | | |
| SC-007 | Pending, T041 | | |
| SC-008 | Pending, T053 | | |
| SC-009 | Pending, T042 to T045 and T054 | | |
| SC-010 | Pending, T046, T047, and T054 | | |
| SC-011 | Pending, T060 | | |
| SC-012 | Pending, T057 and T058 | | |
