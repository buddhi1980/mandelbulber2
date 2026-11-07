# Contract: Build Flags

What each build sets before and after this feature, for FR-001 to FR-004, FR-006, and FR-007. [research.md](../research.md) R1 to R4 give the reasons. Line numbers are those of `8fd49ba59`.

## Options on each build

| Build | Definition | Before | After |
|---|---|---|---|
| Linux qmake, the gated build | `qmake/common.pri:127` | `-ffast-math -fopenmp` | `-fopenmp`, plus the `gcc` block |
| macOS qmake | `qmake/common.pri:128` | `-ffast-math -fopenmp` | `-fopenmp`, plus the `gcc` block |
| Apple Silicon qmake | `qmake/common.pri:130` | `-ffast-math -Xpreprocessor` | `-Xpreprocessor`, plus the `gcc` block |
| Commented-out Apple Silicon line | `qmake/common.pri:129` | `-ffast-math -Xpreprocessor -fopenmp` | `-Xpreprocessor -fopenmp` |
| CMake with GCC or Clang | `cmake/CMakeLists.txt:63` | `-ffast-math -fopenmp`, also at link time | `-fopenmp -ffp-contract=off` |
| CMake on Windows | `cmake/CMakeLists.txt:61` | `/GL /Ox`, `/fp:precise` by default | No change |
| CMake with the classic Intel compiler | `cmake/CMakeLists.txt` | Nothing set | No change |
| qmake on Windows, which no CI job uses | `qmake/mandelbulber-opencl-win64.pro:72` | `/fp:precise` | No change |
| MSVC Debug x64 | `msvc/mandelbulber2.vcxproj:97` | `/fp:fast` | `/fp:precise` |
| MSVC Release x64 and AnalysisRelease | `msvc/mandelbulber2.vcxproj` | `/fp:precise` by default | No change |

## The `gcc` block

In the `# compiler build flags` section of `common.pri`, below the environment import and above the three lines that set `-fopenmp`:

```text
gcc {
	QMAKE_CFLAGS += -ffp-contract=off
	QMAKE_CXXFLAGS += -ffp-contract=off
}
```

**Rules**

- The block stays above the `macx:m1` line, so `-Xpreprocessor` keeps taking `-I/opt/homebrew/include` and never takes `-ffp-contract=off`.
- The block stays below the environment import, so the project's value comes last.

## CMake

Inside `ELSEIF(NOT USING_INTEL)`, the C++ line carries `-fopenmp -ffp-contract=off`, and a new line adds `-ffp-contract=off` to `CMAKE_C_FLAGS`.

## Setup and install scripts

| Script | Before | After |
|---|---|---|
| `tools/prepare_for_dev_ubuntu.sh:12` | `export CXXFLAGS="-march=native -msse2"` | `export CXXFLAGS="-msse2"` |
| `tools/prepare_for_dev_debian_testing.sh:12` | `export CXXFLAGS="-march=native -msse2"` | `export CXXFLAGS="-msse2"` |
| `deploy/linux/install:13` | `export CXXFLAGS="-march=native"` | Line removed |

## What must not change

- The environment import in `common.pri`: `CXXFLAGS`, `CFLAGS`, `CPPFLAGS`, and `LDFLAGS` (FR-008).
- Optimization levels, including `-O3` in the release flags (FR-030).
- MSVC Debug's AVX2 setting (FR-003).
- OpenCL build options in `opencl_engine.cpp` (FR-032).
- `tools/DOE/NaNtester.sh`, which builds a test program of its own (FR-035).

## Expected generated flags on the gated build

```text
CFLAGS   = -pipe -ffp-contract=off -O2 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)
CXXFLAGS = -pipe -ffp-contract=off -fopenmp -O3 -D_REENTRANT -fPIC -Wall -Wextra $(DEFINES)
LFLAGS   = -Wl,-O1 -Wl,-rpath-link,/usr/lib/x86_64-linux-gnu
```

A trial `qmake6` run on a copy of `common.pri` generated these lines. `LFLAGS` is the same as today.
