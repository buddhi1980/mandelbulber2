# Mandelbulber2: Production Upgrade & Parity Testing Playbook

This document defines the automated, spec-driven engineering process to safely transition the `mandelbulber2` codebase into production-ready software. Because fractal math relies heavily on precise floating-point iterations, any code changes can alter image structures or break determinism.

We mitigate this risk by executing a strict **Zero-Drift Regression Harness** powered by **GitHub Spec Kit**, **Claude Code**, and an automated **GitHub Actions Pipeline**.

The [project constitution](.specify/memory/constitution.md) governs this process. Where this playbook and the constitution conflict, the constitution applies and this playbook must be corrected.

| Phase | Result | Closes | Complete when |
|---|---|---|---|
| 1. Setup | Spec Kit and the constitution are in place | None | Done |
| 2. Harness | The regression runner, verification export, and Cross-Build Check pipeline exist, with no references yet | Part of KD-4 | The runner's failure checks pass, and Cross-Build Check passes in GitHub Actions |
| 3. Determinism Foundation | Renders reproduce and floating-point flags are locked | KD-1, KD-2, KD-3 | `--self-parity` passes at two thread counts and the CI flag check passes |
| 4. Baseline 1 and Gate | References and the CI parity gate are live | KD-4, KD-5 | Baseline 1 is merged and the gate is required |
| 5. Upgrades | Refactoring can begin | None | Repeats for each upgrade |

The order matters. Renders do not reproduce today, and the Phase 3 changes alter pixels, so references captured before Phase 3 would be useless. KD numbers refer to the Known Deviations in the constitution.

---

## 🛠️ Phase 1: Environment & Tooling Setup

This phase is complete. The commands below are a record of how the repository was set up, and do not need to be run again.

### 1. Initialize Spec Kit
Inject the Spec Kit framework directly into your existing C++ workspace:
```bash
specify init --here --force --integration claude
```

### 2. Lock Down the Project Constitution
Run the constitution command to enforce strict non-negotiable parity constraints:
```bash
claude -c "/speckit-constitution Define core constraints for the mandelbulber2 codebase: 1. Zero Behavioral Drift (bitwise-identical math and pixel-perfect render matching across standard scenes). 2. Deterministic Testing (headless execution, hardcoded parameter seeds, fixed camera matrices). 3. Architecture Agnosticism (prevent fast-math optimizations that break cross-compiler floating-point reproducibility)."
```

---

## 🖥️ Local Development: Two Builds on One Computer

Development uses two builds of one commit on one Windows computer, so a problem on either build system shows up before a push. The same steps run in GitHub Actions through the Cross-Build Check pipeline, so the design works in all three environments.

| | WSL build | MSVC build |
|---|---|---|
| Environment | The `Ubuntu-24.04` WSL distro | Windows with Visual Studio 2026 |
| Build system | `qmake6` and GNU `make` | MSBuild with `mandelbulber2/msvc/mandelbulber2.sln`, Release x64 |
| Toolchain | GCC 13.3.0, Qt 6.4.2, glibc 2.39, matching the CI runner | MSVC 14.51, toolset v145, Qt 6.11.1 |
| Source | A Linux clone at `~/mandelbulber2` | The Windows checkout at `C:\GIT\mandelbulber2` |
| Binary | `mandelbulber2/build/appdir/usr/bin/mandelbulber2` | `build-win/Release/x64/mandelbulber2.exe` |
| Python | `python3` | `python` |
| Gate and self-parity | Can pass | Run, but cannot pass, because the app ignores a redirected home folder |

The harness finds the right binary on each side, so its commands are the same apart from the Python launcher.

### 1. Set Up WSL
Run these once. The first creates a Linux user, so type `exit` when it finishes. The default `Ubuntu` distro is a newer release with no Qt, so these name the distro every time:
```bash
wsl --install -d Ubuntu-24.04
wsl -d Ubuntu-24.04 -- sudo apt-get update
wsl -d Ubuntu-24.04 -- sudo apt-get install -y build-essential mesa-common-dev libgl1-mesa-dev libpng-dev libgsl-dev libgomp1 liblzo2-2 liblzo2-dev libtiff-dev libsndfile1-dev tree wget opencl-headers ocl-icd-libopencl1 ocl-icd-opencl-dev qt6-base-dev qt6-tools-dev qt6-multimedia-dev qt6-svg-dev qt6-tools-dev-tools qt6-declarative-dev git python3
```

Then, inside WSL, make the Linux clone and configure the build. Keep it out of `/mnt/c`, because the Windows checkout has CRLF line endings, and the build would write generated headers into it. These commands use the current branch, so replace `upgrade/regression-harness` with yours:
```bash
git clone --depth 1 --branch upgrade/regression-harness https://github.com/Coastrnaut/mandelbulber2.git ~/mandelbulber2
mkdir -p ~/mandelbulber2/mandelbulber2/build && cd ~/mandelbulber2/mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
```

### 2. Build Both From One Commit
WSL can only see commits, so commit on Windows first. Then bring that commit into the Linux clone and build it. Run `qmake6` again first if a source file was added, because it expands the source wildcards only when it runs:
```bash
cd ~/mandelbulber2
git fetch /mnt/c/GIT/mandelbulber2 upgrade/regression-harness
git checkout --detach FETCH_HEAD
cd mandelbulber2/build
make -j$(nproc)
make INSTALL_ROOT=appdir install
```

On Windows, in PowerShell from the repository root. These are the same restore and build steps as `.appveyor.yml`:
```powershell
nuget restore mandelbulber2/msvc/mandelbulber2.sln
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" mandelbulber2/msvc/mandelbulber2.sln /m /p:Configuration=Release /p:Platform=x64
```

Delete `build-win` before the first build, because the project compiles every `.cpp` file in that folder, including generated files from older source. Use Release, because that is the binary the harness finds. Each build deletes `build-win/Release/x64` first, so don't keep anything there. If the binary won't start because `OpenCL.dll` is missing, copy it there from `mandelbulber2/msvc/packages/opencl-nuget-msvc-x64.*/build/native/bin/`. A clean build doesn't put it beside the binary, and this computer has a copy in `System32`.

### 3. Run the Application Tests on Both
The existing suite is still required, and it does not replace the parity gate. In WSL, from the clone's root:
```bash
env -i HOME="$(mktemp -d)" PATH="$PATH" LC_ALL=C.UTF-8 mandelbulber2/build/appdir/usr/bin/mandelbulber2 --test
```

On Windows, in PowerShell from `build-win/Release/x64`. Windows can't redirect the home folder, so each run uses your real data folder and adds one autosave file to its `history` folder:
```powershell
$env:QT_FORCE_STDERR_LOGGING = '1'
.\mandelbulber2.exe --test
```

### 4. Cross-Check the Two Builds
Once the harness exists, this confirms that both builds work on every scene and formula. In WSL, from the clone's root:
```bash
python3 tests/regression_runner.py --collect /mnt/c/GIT/mandelbulber2/tests/outputs/collection/qmake
```

On Windows, from the repository root:
```bash
python tests/regression_runner.py --collect tests/outputs/collection/msvc
python tests/regression_runner.py --cross-check tests/outputs/collection/qmake tests/outputs/collection/msvc
```

The cross-check fails only on exact functional facts, such as an item that rendered on one build but not the other. It compares output exactly too, but only to record it, because the constitution says a difference between two targets is not drift. It uses no tolerance, it does not replace the gate, and it passes before renders reproduce.

---

## 📐 Phase 2: Building the Differential Testing Engine

We do not refactor production code until we build the automated harness capable of catching regressions.

**Do not capture references in this phase.** A reference captured now would fail against itself (KD-1). This phase ends with a runner that correctly reports that failure.

### 1. Define the Harness Specification
This step is complete. The commands below are a record of how the specification was created, shown as they were run, and do not need to be run again. The result is in [specs/001-regression-harness/spec.md](specs/001-regression-harness/spec.md). Spec Kit places each specification in a numbered folder, and this prompt still asks for TIFF comparison and three modes, which clarification later replaced:
```bash
git checkout -b upgrade/regression-harness
claude -c "/speckit-specify Create a specification for a headless differential testing engine. It reads a version-controlled scene list from tests/scenes.txt, renders each .fract file with the mandelbulber2 CPU engine using --nogui, and compares decoded 32-bit float TIFF samples against per-target reference data with exact bitwise equality and no tolerance. It needs three modes: self-parity, reference capture, and gate. It must fail on an empty scene list, a missing scene, a failed settings load, or a missing output file, and must not rely on the exit code, because the application exits 0 after a failed load or a failed save. It must also record final render statistics from cStatistics (total iterations, raymarch count, missed DE, and the iteration and step-count histograms) and diff them exactly, excluding time-based values. No deterministic export of these statistics exists, so the spec must define one. All written image channels must be compared, and the spec must decide whether to enable the z-buffer and normal channels as well."
```

### 2. Design the Implementation Breakdown
These steps are also complete, and the commands are a record, shown as they were run. The documents they produced are the source of truth: the [plan](specs/001-regression-harness/plan.md) and the [task list](specs/001-regression-harness/tasks.md). The plan prompt predates the move to GitHub Actions, so its CI wording is out of date:
```bash
claude -c "/speckit-clarify Analyze the mandelbulber2 build system, headless execution flags, and C++ structure to surface any hidden ambiguities before creating the technical implementation design plan."
claude -c "/speckit-plan Design the architecture for the regression engine from specs/001-regression-harness/spec.md. Use a standalone Python script at tests/regression_runner.py that uses only the standard library and targets the installed binary at mandelbulber2/build/appdir/usr/bin/mandelbulber2. Gate is the default mode, with --self-parity, --capture, and --compare followed by the path of a second binary. Design the opt-in verification export in the application, including its file format. Settle these open items: how capture fits CI given that it renders everything about eight times, how references stay under 100 MB, where the formula list comes from because the CLI cannot list formulas, how toolchain identity is recorded because the binary does not store it, and how two-pass stereo scenes are handled because statistics reset on each pass."
claude -c "/speckit-tasks Break down the implementation into incremental, discrete engineering items inside tasks.md. Keep the runner isolated from core C++ production code. The only C++ changes allowed are the verification export and the read-only --list-formulas option, which must only read values that already exist. The export must be written only after a successful settings load and render. Add tests/outputs/ to .gitignore. Do not capture references."
```

The spec's Clarifications section records ten decisions that changed the original request:

- The harness compares the application's raw buffers through a new verification export, instead of TIFF files.
- Every render uses one fixed Monte Carlo sample cap.
- The gate covers a sweep of every formula as well as the scene suite.
- A fourth mode compares two builds on one machine, for local checks.
- The export is written only after a successful load and render, so its presence proves success.
- A toolchain difference passes with a note if output matches. If output differs, it is a Reference Target mismatch rather than drift.
- Compare mode can pass where renders cannot be fully isolated, labeled as not gate-valid.
- Local development uses a qmake build in WSL and an MSVC build on Windows, from one commit.
- A cross-check between those builds fails only on exact functional facts, and it records output differences.
- CI runs the two builds in a separate pipeline, beside the existing workflows, using the same commands.

Before implementing, you can optionally check the three documents for gaps and contradictions. This command only reports findings and changes no files:
```bash
claude -c "/speckit-analyze Check spec.md, plan.md, and tasks.md in specs/001-regression-harness for gaps, contradictions, and conflicts with the constitution."
```

### 3. Review the Application Behaviors
The harness depends on these behaviors. They were tested on a Windows build, and a fresh Linux build confirmed the headless render, overrides, binary location, isolation, and failure detection rows. The other rows have not been run on Linux.

| Need | Use | Why |
|---|---|---|
| Headless render | `--nogui`, with the scene file as a positional argument | The app exits by itself after saving |
| Full precision | The new verification export | TIFF color at `color_quality=2` is 16-bit data stored as float, EXR color comes from before post effects, and no format saves the float image after them |
| Overrides | One `--override` option, with `#` between items, and `name=1` or `name=0` for booleans | `name=true` has no effect |
| Monte Carlo cost | `--override DOF_samples=8#DOF_min_samples=4` on every render | At 320x240 on 32 threads, scenes took 47 to 90+ seconds as authored and 5 to 41 seconds capped |
| Formula sweep | `--override fractal1_formula=<id>` with `--res 32x24` on one base scene | The new `--list-formulas` option supplies the list. The base scene runs each formula first in a hybrid with `mandelbulb`, because 188 of 461 formulas render one flat value alone. `Custom OpenCL formula` has no CPU code and needs a listed exclusion |
| Binary location | The installed tree, `mandelbulber2/build/appdir/usr/bin/mandelbulber2` | On Linux the app finds its data at `../share/mandelbulber2` relative to the binary, or at `/usr/share/mandelbulber2`. A binary left in the build folder has neither. The harness fails a render that falls back to the system folder, which is outside the repository and build output |
| Isolation | A temporary `HOME`, a fixed `LC_ALL`, and a clean environment | Otherwise the app reads per-user settings. Windows ignores a redirected home folder, so the gate and self-parity cannot pass there, and capture refuses. Compare mode can still pass, labeled as not gate-valid. On Linux, each new `HOME` also prints five `cannot move folder` lines at startup, which are harmless |
| Failure detection | Require the export, then check the output text | The app exits 0 and prints `Image saved to:` even when the target folder is missing. A missing scene, a bad settings file, or a missing texture also exits 0, a failed load saves a black image, and up to 3 bad settings lines still count as a successful load |
| Computation logs | The statistics record in the export | Log lines start with a process ID and elapsed time, `--stats` prints values rounded to six significant digits, and `--logfilepath` wrote no file in testing |

An earlier TIFF-based prototype of the runner is in commit `9b59d130d`. Its comparison no longer applies, but its temporary home directory and failure checks were tested and can be reused.

### 4. Implement and Verify
`tests/scenes.txt` is the Standard Scene Suite, with one path per line, relative to the repository root. Start with these two, then add scenes until the suite exercises each stochastic CPU path, as the constitution requires. FR-039 in the spec lists the paths, and `research.md` names a candidate scene for each. A scene that references an asset by URL or absolute path fails the run, and so does a nebula scene, because nebula mode always renders through OpenCL. Leave those out:

```text
# Standard Scene Suite
mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract
mandelbulber2/deploy/share/mandelbulber2/examples/Makin3D-Julia_001.fract
```

Have Claude complete the implementation loop:
```bash
claude -c "/speckit-implement Iteratively write code to satisfy the checklist items found inside tasks.md. Wire tests/regression_runner.py to the scene list in tests/scenes.txt, the formula sweep, and the installed binary. Do not create anything under tests/references/ in this phase."
claude -c "/speckit-converge Confirm that one flipped bit in any buffer and a change of 1 in any statistic each fail the comparison, that an unstable render blocks capture and writes no references, and that a missing scene, a failed settings load, a missing texture, and a missing or incomplete export each fail. Confirm that --self-parity fails on the current build, which is the expected KD-1 result. Confirm that the cross-check fails on each functional disagreement and records a flipped bit without failing on it."
```

Build the installed tree in WSL, as described under Local Development, then run the check. The `mandelbulber2/qmake` folder holds several `.pro` files, so `qmake6` must be given one by name:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --self-parity
```

### 5. Add the Cross-Build Check Pipeline
This is User Story 8 in the task list, and it comes after the cross-check works locally. It adds one new workflow and two reusable actions, with the exact content in [contracts/ci-pipeline.md](specs/001-regression-harness/contracts/ci-pipeline.md). It runs in addition to the existing workflows, and it must not change any of them.

| Job | Runner | Steps |
|---|---|---|
| `linux-qmake` | `ubuntu-24.04` | Build, stand-in tests, `--test`, collect, upload the collection |
| `windows-msvc` | `windows-2025-vs2026` | Build, stand-in tests, `--test`, collect, upload the collection |
| `cross-check` | `ubuntu-24.04` | Download both collections, run `--cross-check`, publish the report to the run summary |

The build steps come from what already works in CI: `.appveyor.yml` for MSVC, and `github-actions-ci.yml` for qmake. The pipeline has read-only access, uses no secrets, and never publishes a release. It is not the parity gate, whose workflow arrives in Phase 3 and whose gate steps arrive in Phase 4. This is also the first CI run of the Windows binary, because AppVeyor and Azure Pipelines only build it, so treat the first run as a test of the design. Expect the Windows job to take up to about 75 minutes, because AppVeyor's Release build alone takes about 52.

---

## 🎯 Phase 3: Determinism Foundation

These changes close KD-1, KD-2, and KD-3. They are the only engine changes permitted before Baseline 1. Each must land as its own reviewed change and state its expected effect on output. There are no references yet, so `--self-parity` is the check.

### 1. Make Renders Reproducible (KD-1)
Specify the fix on its own branch, then run `/speckit-plan`, `/speckit-tasks`, and `/speckit-implement`:
```bash
git checkout -b upgrade/deterministic-rendering
claude -c "/speckit-specify Close KD-1 from the constitution by making CPU renders reproducible. First confirm the cause, because it is not yet proven: render threads share the unsynchronized global gRandomSeed in common_math.cpp, and Random() runs on every raymarch step. Replace that shared state so each random value derives only from a seed fixed in the scene or harness and from the pixel and sample position. Cover every CPU caller of Random(): render_worker, dof, light, ssao_worker, shader_ambient_occlusion, shader_aux_shadow, shader_global_illumination, shader_specular_highlight, and shader_volumetric. Audit the OpenMP regions in dof and post_effect_hdr_blur for order-dependent results. Make the render statistics exact: all render threads update one shared statistics object with no synchronization, and identical renders reported total iterations that differed by about 30 percent. Total iterations, raymarch count, missed DE, and both histograms must give the same values at every thread count, without changing rendered output. Give --nogui a working thread-count control that also governs those OpenMP regions, because limit_CPU_cores is only applied by the GUI startup path and the queue. Success means --self-parity passes for all engine buffers and statistics at one thread and at all available threads."
```

This change added `--threads` and made it the runner's thread option, so `--self-parity` can report a pass, and missing thread coverage no longer stops `--capture`.

This change also filled in `total_iterations`. The CPU path in `calculate_distance.cpp` used to copy only `iters` back from its local result, so `--stats` and the verification export both reported 0. The fix changes the statistics record, not the image.

### 2. Lock Floating-Point Behavior (KD-2, KD-3)
Specify this as a separate change:
```bash
git checkout -b upgrade/strict-floating-point
claude -c "/speckit-specify Close KD-2 and KD-3 from the constitution. Remove -ffast-math from mandelbulber2/qmake/common.pri and mandelbulber2/cmake/CMakeLists.txt, replace /fp:fast in the Debug x64 configuration of mandelbulber2/msvc/mandelbulber2.vcxproj, add -ffp-contract=off to GCC and Clang builds, remove -march=native from mandelbulber2/tools/prepare_for_dev_*.sh and mandelbulber2/deploy/linux/install, and add the GitHub Actions flag check from this playbook. Measure CPU render time on the Standard Scene Suite before and after, and record the cost."
```

Expect output to change. `-ffast-math` lets the compiler treat values as always finite, so it compiles away the `IsNotANumber()` guards in `compute_fractal.cpp`, `render_worker.cpp`, and `shader_calculate_normals.cpp`, and they take effect once it is removed. They catch infinity as well as NaN, so an orbit that overflows keeps its last finite value, as it already does on MSVC Release. None of them fires on the Standard Scene Suite, and `specs/003-strict-floating-point/evidence.md` records what changed.

Add the flag check as a reusable action and a new workflow. A script beside the action, `check-flags.sh`, holds the only list of prohibited options. It searches the tracked build files, scripts, and CI files before the build, then reads the gated build's compile and link flags, because GCC's `-ffast-math` also changes floating-point behavior when it is used at link time. The existing `.circleci/config.yml` only packages macOS builds and is not part of this process, so leave it as it is.

Create `.github/actions/compile-engine/action.yml`. It reuses the `build-linux` action from Phase 2, so the build commands live in one place:

```yaml
name: Compile Engine
description: Search the build files, build and install the release binary, then reject prohibited compile flags.
runs:
  using: composite
  steps:
    - name: Search the Build Files
      shell: bash
      run: bash "$GITHUB_ACTION_PATH/check-flags.sh" search
    - name: Build and Install to a Local Tree
      uses: ./.github/actions/build-linux
    - name: Reject Prohibited Compile Flags
      shell: bash
      run: bash "$GITHUB_ACTION_PATH/check-flags.sh" flags mandelbulber2/build/Makefile
```

`.github/actions/compile-engine/check-flags.sh` holds the list and both commands, and `specs/003-strict-floating-point/contracts/flag-check.md` gives its rules, messages, and exit codes.

Create `.github/workflows/parity-gate.yml`:

```yaml
name: Parity Gate

on:
  pull_request:
  push:
    branches:
      - deterministic-regression

permissions:
  contents: read

jobs:
  parity-gate:
    runs-on: ubuntu-24.04
    timeout-minutes: 120
    steps:
      - uses: actions/checkout@v7
      - uses: ./.github/actions/compile-engine
```

The `ubuntu-24.04` label fixes the operating system, but GitHub typically updates the software on the image weekly, so the compiler or math library can change with no commit. The gate reports this against the toolchain recorded with the references. If output changes too, that needs a Baseline Change.

Phase 3 is complete when `--self-parity` passes at two thread counts with both changes merged, and `parity-gate` passes in GitHub Actions.

---

## 🔄 Phase 4: Baseline 1 and the GitHub Actions Parity Gate

To ensure that no code mutations skip verification, this phase captures the first references and turns on the gate.

### 1. Add the Gate and Capture Workflows
Add `.github/workflows/capture-baseline.yml` first, and the gate steps in `.github/workflows/parity-gate.yml` after capture, since `parity-gate` stays red until references exist. The repository's default branch is `master`, which follows upstream and holds no harness, so a manually started workflow can't run here. Capture starts instead when a push creates a `baseline/` branch.

`.github/workflows/parity-gate.yml`, with the gate steps:

```yaml
name: Parity Gate

on:
  pull_request:
  push:
    branches:
      - deterministic-regression

permissions:
  contents: read

jobs:
  parity-gate:
    runs-on: ubuntu-24.04
    timeout-minutes: 120
    steps:
      - uses: actions/checkout@v7
      - uses: ./.github/actions/compile-engine
      - name: Test the Harness
        run: python3 tests/test_regression_runner.py
      - name: Run Parity Gate
        run: python3 tests/regression_runner.py
      - name: Upload Failed Outputs
        if: failure()
        uses: actions/upload-artifact@v7
        with:
          name: render-regression-snapshots
          path: tests/outputs
          retention-days: 7
```

`.github/workflows/capture-baseline.yml`:

```yaml
name: Capture Baseline

on:
  push:
    branches:
      - 'baseline/**'

permissions:
  contents: read

jobs:
  capture-baseline:
    if: github.event.created
    runs-on: ubuntu-24.04
    timeout-minutes: 350
    steps:
      - uses: actions/checkout@v7
      - uses: ./.github/actions/compile-engine
      - name: Test the Harness
        run: python3 tests/test_regression_runner.py
      - name: Capture References
        run: python3 tests/regression_runner.py --capture
      - name: Upload References
        uses: actions/upload-artifact@v7
        with:
          name: baseline-references
          path: tests/references
          if-no-files-found: error
          retention-days: 90
      - name: Upload Failed Outputs
        if: failure()
        uses: actions/upload-artifact@v7
        with:
          name: capture-failure-outputs
          path: tests/outputs
          retention-days: 7
```

Capture tests the harness, passes Self-Parity at one thread and at all of the runner's threads, then writes the references. Baseline 1's capture took 31 minutes 9 seconds, far inside the 350-minute limit.

### 2. Settle the CPU Math Path
glibc picks its math code path from the CPU at run time, so one binary can give different results on two computers whose CPUs have different features. On 2026-11-07, one binary gave different `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `exp`, `log`, and `pow` results, over 4 million values each, when the library's FMA and AVX2 paths were turned off, and the same `sqrt` results. `specs/003-strict-floating-point/evidence.md` records the test. No compile flag changes this, so hosted runners with different CPUs could disagree with the references.

The harness settles it by forcing glibc's plain x86-64 path for every Linux render, with `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`. Before any render, a probe checks six known answers, for `sin`, `cos`, `exp`, `log`, `atan`, and `asin`, in the render environment. It stops with a setup error if the forcing didn't take effect or if glibc's plain results changed, and it prints a line naming the forced setting, the CPU's own path, and the CPU. Each reference set records the setting as the last line of its `settings.txt`, `math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`, and the gate refuses a set made without it. Forced renders match renders with every optional CPU feature hidden from glibc, for every item. `specs/004-baseline-parity-gate/evidence.md` records the checks. This closed KD-5.

### 3. Capture Baseline 1
Baseline 1 is a Baseline Change, so its pull request contains only reference files. References must come from CI rather than a development machine. Start capture by creating the branch on GitHub, at the commit with the capture workflow:
```bash
git push origin deterministic-regression:refs/heads/baseline/1
```

When the run finishes, download its `baseline-references` artifact, unpack it into `tests/references/`, and check that every file's blob ID matches the downloaded copy. Commit the files on `baseline/1` and push it, which doesn't start capture again. Then push the gate steps to `deterministic-regression`, whose push run fails as a setup error because no reference set exists yet, and open the pull request straight away. Its description gives the before-and-after evidence, the comparison of glibc's FMA path with the forced path, and links the capture run that passed Self-Parity. `parity-gate` passes once the references are present. Merge it with a merge commit, and delete `baseline/1`.

### 4. Require the Gate
Mark `parity-gate` as the required status check on `deterministic-regression`, with classic branch protection:
```bash
gh api -X PUT repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection --input protection.json
```
`protection.json` requires `parity-gate` alone, with `strict` and `enforce_admins` false, and no required reviews or restrictions. A pull request can't merge until `parity-gate` passes on it. The maintainer's direct pushes bypass the required check, and GitHub reports the rule each one bypassed. Each push run then checks the push afterwards, and a red push run is a defect to fix at once, by the next commit, never by regenerating references.

### 5. Prove the Gate Catches Drift
On a throwaway branch, `seed/drift-proof`, change `2.0 * z.x - z.z` to `2.001 * z.x - z.z` on line 36 of `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`, a formula no scene uses. Open a pull request into `deterministic-regression`. `parity-gate` must fail on `formula bristorbrot` alone and upload `render-regression-snapshots`, and the merge state must be `BLOCKED`. Then close the pull request unmerged and delete the branch.

### 6. Close the Known Deviations
Amend the constitution to close KD-4 and KD-5, which is a PATCH version bump, to 1.1.1. The amendment's pull request edits only the constitution. KD-1 closed in version 1.0.1, and KD-2 and KD-3 closed in version 1.1.0. Engine refactors can now begin.

---

## 🚀 Phase 5: The Production Upgrade Lifecycle (Task 1: Qt6 Decoupling)

Once Baseline 1 is merged and the gate is required, use this 5-step operational loop to isolate UI/Framework dependencies from your core math and business logic.

### Step 1: Specify the Abstraction Scope
Work on `deterministic-regression` itself, where the work lands by direct push, and specify the architectural boundary goals:
```bash
git switch deterministic-regression
claude -c "/speckit-specify Create a software specification to design a clean API abstraction layer between Qt6 and the core rendering engine. The objective is to decouple UI types, custom Qt container wrappers (like QString, QVector, QPoint), and Qt event loops from the mathematical formulas and engine pipelines, substituting them with pure standard library types (std::string, std::vector) without modifying rendering behavior or data structures."
```

### Step 2: Establish the Architectural Blueprint
Map the specific data structures and interface files that need replacement or bridging:
```bash
claude -c "/speckit-plan Formulate a refactoring blueprint defining abstract interface classes (e.g., IImageBuffer, ILogOutput) that insulate the fractal engine from direct Qt header inclusions. Ensure the regression testing harness is configured to monitor that data passed across this new boundary maintains identical bit-accuracy."
claude -c "/speckit-tasks Output a strict implementation checklist file (tasks.md) for the Qt6 abstraction layer, focusing first on isolating global type aliases and fundamental configuration parsing vectors."
```

### Step 3: Execute Code Transformation
Let Claude safely refactor data types and inject the abstract structural interfaces:
```bash
claude -c "/speckit-implement Implement the Qt6 abstraction interfaces and type translations sequentially matching the generated task roadmap, working module-by-module through the rendering initialization code."
```

### Step 4: Run the Parity Gatekeeper
Verify that changes compile cleanly and yield identical output before pushing. References come from CI and may not match your local toolchain, so compare your change against a build of the `deterministic-regression` commit it starts from, on the same machine. This is a local check, and the GitHub Actions gate stays the authority. `deterministic-regression` is the base for regression features, and `master` follows upstream.

Build that commit once in a separate worktree, without creating a branch:
```bash
git worktree add --detach ../mandelbulber2-base origin/deterministic-regression
(
  mkdir -p ../mandelbulber2-base/mandelbulber2/build
  cd ../mandelbulber2-base/mandelbulber2/build
  qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
  make -j$(nproc)
  make INSTALL_ROOT=appdir install
)
```

Then build your change and compare it with that build:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --compare ../mandelbulber2-base/mandelbulber2/build/appdir/usr/bin/mandelbulber2
```

Then cross-check your change on both builds, as described under Local Development, so a change that breaks only one build system shows up before you push.

*If convergence parameters require adjustment:*
```bash
claude -c "/speckit-converge Audit the decoupled abstraction code, verify that no stray Qt types leak back into the engine modules, and confirm absolute math convergence against the main branch baseline."
```

A parity failure is a defect in the change. Do not regenerate references to clear it.

### Step 5: Merge and Push to GitHub Actions Verification Gating
When compare mode reports **no differences**, push your changes to `deterministic-regression` directly. The push bypasses the required check, and its push run of `parity-gate` verifies it on the Reference Target afterwards. A red push run is a defect to fix at once with the next commit:
```bash
git add mandelbulber2 specs
git commit -m "refactor(core): implemented abstract interface layer to decouple engine from Qt6 types"
git push origin deterministic-regression
```

---

## 📝 Appendix: Project Documentation Addendum (`README.md`)

After Phase 4, add this structural snippet to the main project workspace documentation to inform development contributors of the regression gating tools:

## 🔄 Continuous Integration & Regression Testing

This project uses **GitHub Actions** alongside a custom differential testing framework to guarantee **Zero Behavioral Drift** across core fractal calculation models and rendering loops during upgrades or optimizations.

The regression gate builds the codebase with `qmake6`, headlessly renders a fixed scene list and every fractal formula, and compares the application's raw image buffers and render statistics against per-platform references, bit for bit. Its `parity-gate` job runs on every pull request and on every push to `deterministic-regression`, and it is the required check on `deterministic-regression`: a pull request can't merge until it passes. On Linux, every render takes glibc's plain math path, so the runner's CPU doesn't change the results.

### Locally Testing Parity
Before each push, compare your change with a build of the `deterministic-regression` commit it starts from, on the same machine. This works with any toolchain, and the GitHub Actions gate stays the authority. On Linux:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --compare <path to a deterministic-regression build's installed binary>
```

### Cross-Checking Two Build Systems
The Cross-Build Check workflow builds every pull request with `qmake6` on Linux and MSVC on Windows, tests each build, and confirms that both work on every scene and formula. You can run the same check locally, with a qmake build in WSL and an MSVC build on Windows:
```bash
python3 tests/regression_runner.py --collect <folder for the qmake results>
python tests/regression_runner.py --collect <folder for the MSVC results>
python tests/regression_runner.py --cross-check <qmake folder> <MSVC folder>
```
Run the first command in WSL and the other two on Windows. This check does not replace the parity gate.
