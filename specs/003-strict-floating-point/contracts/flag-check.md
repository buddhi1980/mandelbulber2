# Contract: Flag Check

`.github/actions/compile-engine/check-flags.sh` holds the list of options, and it runs the build-file search and the flag check. It needs only `bash`, `git`, and `grep`. [research.md](../research.md) R5 and R6 give the reasons.

## Commands

| Command | Reads | Runs |
|---|---|---|
| `check-flags.sh search` | Tracked build files in the current checkout | Before the build |
| `check-flags.sh flags <Makefile>` | One generated Makefile | After the build |

Run it as `bash <path>/check-flags.sh`, so it needs no executable bit.

## Exit Codes

| Code | Meaning |
|---|---|
| 0 | Every rule passed |
| 1 | A rule failed. Every failure is listed, not only the first |
| 2 | The check could not run: a missing or empty file, a missing variable line, a failed search, no build files found, or a bad command line |

Nothing but a finished, clean run exits 0 (FR-014).

## Prohibited Options

Both commands use one list (FR-012):

```text
-ffast-math  -Ofast  -funsafe-math-optimizations  -ffinite-math-only
-fassociative-math  -freciprocal-math  -march=native  -mtune=native  -mcpu=native
```

An option matches only as a whole option. So `-fno-fast-math`, `-march=x86-64`, and a path such as `-L/opt/x-ffast-math/lib` do not match.

## `flags`

Reads the `CC`, `CXX`, `LINK`, `CFLAGS`, `CXXFLAGS`, `LFLAGS`, and `LIBS` lines (FR-011). `build-linux` runs `make` with no overrides, so those lines are what the compiler and linker receive. Flags from the environment are already in them, because qmake imported them. The rules come from FR-012 and FR-013.

| Rule | Failure |
|---|---|
| No line holds an option from the list | `FAILED: -ffast-math in CXXFLAGS` |
| No line holds an `-ffp-contract` value other than `off` | `FAILED: -ffp-contract=fast in CXXFLAGS` |
| `CFLAGS` holds `-ffp-contract=off` | `FAILED: -ffp-contract=off missing from CFLAGS` |
| `CXXFLAGS` holds `-ffp-contract=off` | `FAILED: -ffp-contract=off missing from CXXFLAGS` |

A failure ends with a count, `2 check(s) failed`, and exit code 1. A pass prints `passed`.

## `search`

Searches tracked files of these kinds with `git grep`, so an untracked file is ignored (FR-035):

```text
*.pro  *.pri  CMakeLists.txt  *.cmake  *.vcxproj  *.props  *.targets  *.sln
*.sh  *.bat  *.yml  *.yaml  Dockerfile  mandelbulber2/deploy/linux/install
```

It leaves out `.github/actions/compile-engine`, `mandelbulber2/tools/DOE`, `specs`, `.specify`, and `.claude`.

| Rule | Failure |
|---|---|
| No file holds an option from the list | `FAILED: -ffast-math in mandelbulber2/qmake/common.pri:127` |
| No file holds `-ffp-contract=fast` or `-ffp-contract=on` | `FAILED: -ffp-contract=fast in <file>:<line>` |
| No file holds `/fp:fast` | `FAILED: /fp:fast in <file>:<line>` |
| No file holds `/fp:contract` | `FAILED: /fp:contract in <file>:<line>` |
| No file sets `<FloatingPointModel>` to `Fast` | `FAILED: FloatingPointModel Fast in <file>:<line>` |

A comment counts as a match. A pass prints how many files it searched, then `passed`.

## Seeded Failures

FR-015's local proof. Each row runs once, on a copy of a clean generated Makefile or a clean checkout, and the column gives the result.

### `flags`: faults

| Seed | Expected |
|---|---|
| Each of the 9 options added to `CXXFLAGS` | Exit 1, names the option and `CXXFLAGS` |
| `-ffast-math` added to `CFLAGS` | Exit 1, names it |
| `-ffast-math` added to `LFLAGS` | Exit 1, names it |
| `-ffast-math` added to `LIBS` | Exit 1, names it |
| `-ffast-math` added to `CXX` | Exit 1, names it |
| `-Ofast` added to `LINK` | Exit 1, names it |
| `-ffp-contract=fast` after `-ffp-contract=off` in `CXXFLAGS` | Exit 1, names it |
| `-ffp-contract=on` added to `CFLAGS` | Exit 1, names it |
| `-ffp-contract=off` removed from `CXXFLAGS` | Exit 1, says it is missing |
| `-ffp-contract=off` removed from `CFLAGS` | Exit 1, says it is missing |

### `flags`: through a real `qmake6` run

| Environment | Expected |
|---|---|
| `CXXFLAGS=-march=native` | Exit 1, names `CXXFLAGS` |
| `CFLAGS=-march=native` | Exit 1, names `CFLAGS` |
| `CPPFLAGS=-Ofast` | Exit 1, names `CFLAGS` and `CXXFLAGS` |
| `LDFLAGS=-ffast-math` | Exit 1, names `LFLAGS` |

### `flags`: must pass

| Case | Expected |
|---|---|
| The clean Makefile | Exit 0 |
| `-fno-fast-math` added | Exit 0 |
| `-march=x86-64` added | Exit 0 |
| `-L/opt/x-ffast-math/lib` added to `LIBS` | Exit 0 |

### Cannot run

| Case | Expected |
|---|---|
| Missing Makefile | Exit 2 |
| Empty Makefile | Exit 2 |
| Makefile with no `LIBS` line | Exit 2 |
| No arguments | Exit 2 |
| `search` outside a git checkout | Exit 2 |

### `search`

Each row adds one line to a tracked file in a throwaway clone with this feature's edits applied, where the search passes. On `8fd49ba59`, it reports exactly the 9 lines this feature removes.

| File | Seed | Expected |
|---|---|---|
| `mandelbulber2/qmake/common.pri` | `-ffast-math` | Exit 1, names the file and line |
| `mandelbulber2/qmake/mandelbulber.pro` | `-Ofast` | Exit 1, names them |
| `mandelbulber2/cmake/CMakeLists.txt` | `-ffinite-math-only` | Exit 1, names them |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `<FloatingPointModel>Fast` | Exit 1, names them |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `/fp:fast` | Exit 1, names them |
| `mandelbulber2/msvc/mandelbulber2.vcxproj` | `/fp:contract` | Exit 1, names them |
| `mandelbulber2/tools/prepare_for_dev_ubuntu.sh` | `-march=native` | Exit 1, names them |
| `mandelbulber2/deploy/linux/install` | `-mtune=native` | Exit 1, names them |
| `mandelbulber2/make-package.sh` | `-mcpu=native` | Exit 1, names them |
| `azure-pipelines.yml` | `-ffp-contract=fast` | Exit 1, names them |
| `.circleci/config.yml` | `-freciprocal-math` | Exit 1, names them |
| `.travis.yml` | `-fassociative-math` | Exit 1, names them |
| `Dockerfile` | `-funsafe-math-optimizations` | Exit 1, names them |
| A new tracked script, `new-build.sh` | `-funsafe-math-optimizations` | Exit 1, names them |
| The same script, untracked | `-funsafe-math-optimizations` | Exit 0 |
| `mandelbulber2/deploy/NEWS`, which is not a build file | `-ffast-math` | Exit 0 |
| `mandelbulber2/tools/DOE/NaNtester.sh` | Its own `-ffast-math` lines | Exit 0 |
| `.github/actions/compile-engine/check-flags.sh` | Its own list | Exit 0 |

### GitHub Actions

[research.md](../research.md) R8 sets out these two runs. Both go on a throwaway branch that is never merged.

| Seed | Expected |
|---|---|
| `-ffast-math` on the CMake line | The search fails, and the build never starts |
| A job-level `CXXFLAGS` that resolves to `-march=native` | The search passes, the build runs, and the flag check fails on `-march=native in CXXFLAGS` |
