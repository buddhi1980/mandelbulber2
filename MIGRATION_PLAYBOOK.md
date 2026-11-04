# Mandelbulber2: Production Upgrade & Parity Testing Playbook

This document defines the automated, spec-driven engineering process to safely transition the `mandelbulber2` codebase into production-ready software. Because fractal math relies heavily on precise floating-point iterations, any code changes can alter image structures or break determinism.

We mitigate this risk by executing a strict **Zero-Drift Regression Harness** powered by **GitHub Spec Kit**, **Claude Code**, and an automated **CircleCI Pipeline**.

The [project constitution](.specify/memory/constitution.md) governs this process. Where this playbook and the constitution conflict, the constitution applies and this playbook must be corrected.

| Phase | Result | Closes | Complete when |
|---|---|---|---|
| 1. Setup | Spec Kit and the constitution are in place | None | Done |
| 2. Harness | The regression runner and verification export exist, with no references yet | Part of KD-4 | The runner's failure checks pass |
| 3. Determinism Foundation | Renders reproduce and floating-point flags are locked | KD-1, KD-2, KD-3 | `--self-parity` passes at two thread counts and the CI flag check passes |
| 4. Baseline 1 and Gate | References and the CI parity gate are live | KD-4 | Baseline 1 is merged and the gate is required |
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

## 📐 Phase 2: Building the Differential Testing Engine

We do not refactor production code until we build the automated harness capable of catching regressions.

**Do not capture references in this phase.** A reference captured now would fail against itself (KD-1). This phase ends with a runner that correctly reports that failure.

### 1. Define the Harness Specification
This step is complete. The specification is in [specs/001-regression-harness/spec.md](specs/001-regression-harness/spec.md). Its Clarifications section records five decisions that changed the original request:

- The harness compares the application's raw buffers through a new verification export, instead of TIFF files.
- Every render uses one fixed Monte Carlo sample cap.
- The gate covers a sweep of every formula as well as the scene suite.
- A fourth mode compares two builds on one machine, for local checks.
- The export is written only after a successful load and render, so its presence proves success.

### 2. Design the Implementation Breakdown
Convert the specification into technical tasks:
```bash
claude -c "/speckit-plan Design the architecture for the regression engine from specs/001-regression-harness/spec.md. Use a standalone Python script at tests/regression_runner.py that uses only the standard library and targets the installed binary at mandelbulber2/build/appdir/usr/bin/mandelbulber2. Gate is the default mode, with --self-parity, --capture, and --compare followed by the path of a second binary. Design the opt-in verification export in the application, including its file format. Settle these open items: how capture fits CI given that it renders everything about eight times, how references stay under 100 MB, where the formula list comes from because the CLI cannot list formulas, how toolchain identity is recorded because the binary does not store it, and how two-pass stereo scenes are handled because statistics reset on each pass."
claude -c "/speckit-tasks Break down the implementation into incremental, discrete engineering items inside tasks.md. Keep the runner isolated from core C++ production code. The only C++ change allowed is the verification export, which must only read values the renderer has already produced and must be written only after a successful settings load and render. Add tests/outputs/ to .gitignore. Do not capture references."
```

### 3. Review the Application Behaviors
The harness depends on these behaviors. They were tested on a Windows build. The Linux install layout and `HOME` handling come from reading the source, because they have not been run yet.

| Need | Use | Why |
|---|---|---|
| Headless render | `--nogui`, with the scene file as a positional argument | The app exits by itself after saving |
| Full precision | The new verification export | TIFF color at `color_quality=2` is 16-bit data stored as float, EXR color comes from before post effects, and no format saves the float image after them |
| Overrides | One `--override` option, with `#` between items, and `name=1` or `name=0` for booleans | `name=true` has no effect |
| Monte Carlo cost | `--override DOF_samples=8#DOF_min_samples=4` on every render | At 320x240 on 32 threads, scenes took 47 to 90+ seconds as authored and 5 to 41 seconds capped |
| Formula sweep | `--override formula_1=<id>` with `--res 32x24` on one base scene | The CLI cannot list formulas, so the plan must choose another source. `Custom OpenCL formula` has no CPU code and needs a listed exclusion |
| Binary location | The installed tree, `mandelbulber2/build/appdir/usr/bin/mandelbulber2` | On Linux the app finds its data at `../share/mandelbulber2` relative to the binary, or at `/usr/share/mandelbulber2`. A binary left in the build folder has neither |
| Isolation | A temporary `HOME` and a fixed `LC_ALL` | Otherwise the app reads per-user settings. Windows ignores this, so Windows runs are for debugging only |
| Failure detection | Require the export, then check the output text | The app exits 0 and prints `Image saved to:` even when the target folder is missing. A bad settings file or a missing texture also exits 0 |
| Computation logs | The statistics record in the export | Log lines start with a process ID and elapsed time, `--stats` prints values rounded to six significant digits, and `--logfilepath` wrote no file in testing |

An earlier TIFF-based prototype of the runner is in commit `fbcc70492`. Its comparison no longer applies, but its temporary home directory and failure checks were tested and can be reused.

### 4. Implement and Verify
`tests/scenes.txt` is the Standard Scene Suite, with one path per line, relative to the repository root. Start with these two, then add scenes that exercise Monte Carlo depth of field, global illumination, and ambient occlusion, as the constitution requires. A scene that references an asset by URL or absolute path fails the run, so leave those out:

```text
# Standard Scene Suite
mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract
mandelbulber2/deploy/share/mandelbulber2/examples/Makin3D-Julia_001.fract
```

Have Claude complete the implementation loop:
```bash
claude -c "/speckit-implement Iteratively write code to satisfy the checklist items found inside tasks.md. Wire tests/regression_runner.py to the scene list in tests/scenes.txt, the formula sweep, and the installed binary. Do not create anything under tests/references/ in this phase."
claude -c "/speckit-converge Confirm that one flipped bit in any buffer and a change of 1 in any statistic each fail the comparison, that an unstable render blocks capture and writes no references, and that a missing scene, a failed settings load, a missing texture, and a missing or incomplete export each fail. Confirm that --self-parity fails on the current build, which is the expected KD-1 result."
```

Build the installed tree on Linux, then run the check. The `mandelbulber2/qmake` folder holds several `.pro` files, so `qmake6` must be given one by name:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --self-parity
```

---

## 🎯 Phase 3: Determinism Foundation

These changes close KD-1, KD-2, and KD-3. They are the only engine changes permitted before Baseline 1. Each must land as its own reviewed change and state its expected effect on output. There are no references yet, so `--self-parity` is the check.

### 1. Make Renders Reproducible (KD-1)
Specify the fix on its own branch, then run `/speckit-plan`, `/speckit-tasks`, and `/speckit-implement`:
```bash
git checkout -b upgrade/deterministic-rendering
claude -c "/speckit-specify Close KD-1 from the constitution by making CPU renders reproducible. First confirm the cause, because it is not yet proven: render threads share the unsynchronized global gRandomSeed in common_math.cpp, and Random() runs on every raymarch step. Replace that shared state so each random value derives only from a seed fixed in the scene or harness and from the pixel and sample position. Cover every CPU caller of Random(): render_worker, dof, light, ssao_worker, shader_ambient_occlusion, shader_aux_shadow, shader_global_illumination, shader_specular_highlight, and shader_volumetric. Audit the OpenMP regions in dof and post_effect_hdr_blur for order-dependent results. Make the render statistics exact: all render threads update one shared statistics object with no synchronization, and identical renders reported total iterations that differed by about 30 percent. Total iterations, raymarch count, missed DE, and both histograms must give the same values at every thread count, without changing rendered output. Give --nogui a working thread-count control that also governs those OpenMP regions, because limit_CPU_cores is only applied by the GUI startup path and the queue. Success means --self-parity passes for all engine buffers and statistics at one thread and at all available threads."
```

Once the thread control exists, enable the second thread count in the runner. Until then, `--self-parity` cannot report a pass and `--capture` is refused.

### 2. Lock Floating-Point Behavior (KD-2, KD-3)
Specify this as a separate change:
```bash
git checkout -b upgrade/strict-floating-point
claude -c "/speckit-specify Close KD-2 and KD-3 from the constitution. Remove -ffast-math from mandelbulber2/qmake/common.pri and mandelbulber2/cmake/CMakeLists.txt, replace /fp:fast in the Debug x64 configuration of mandelbulber2/msvc/mandelbulber2.vcxproj, add -ffp-contract=off to GCC and Clang builds, remove -march=native from mandelbulber2/tools/prepare_for_dev_*.sh and mandelbulber2/deploy/linux/install, and add the CircleCI flag check from this playbook. Measure CPU render time on the Standard Scene Suite before and after, and record the cost."
```

Expect output to change. `-ffast-math` lets the compiler treat values as always finite, so the `std::isfinite` guards in `algebra.hpp` and `math_utilities.h` may start taking effect once it is removed.

Add the flag check to `.circleci/config.yml`. **Do not replace that file**, because it holds the macOS `build-and-pack` job. Add a top-level `commands` block and a new job, and add `parity-gate` to the existing `build-and-deploy` workflow:

```yaml
commands:
  build-engine:
    steps:
      - run:
          name: Install Build Dependencies
          command: |
            sudo apt-get update
            sudo apt-get install -y build-essential pkg-config python3 mesa-common-dev \
              libgl1-mesa-dev libpng-dev libgsl-dev libgomp1 liblzo2-dev libtiff-dev \
              libsndfile1-dev opencl-headers ocl-icd-libopencl1 ocl-icd-opencl-dev \
              qt6-base-dev qt6-tools-dev qt6-multimedia-dev qt6-svg-dev \
              qt6-tools-dev-tools qt6-declarative-dev
      - run:
          name: Build and Install to a Local Tree
          command: |
            mkdir -p mandelbulber2/build
            cd mandelbulber2/build
            qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
            make -j$(nproc)
            make INSTALL_ROOT=appdir install
      - run:
          name: Reject Prohibited Compile Flags
          command: |
            FLAGS=$(grep -E '^(CFLAGS|CXXFLAGS)[[:space:]]*=' mandelbulber2/build/Makefile)
            if echo "$FLAGS" | grep -E -- '-ffast-math|-Ofast|-funsafe-math-optimizations|-ffinite-math-only|-fassociative-math|-freciprocal-math|-march=native|-mtune=native'; then
              echo "Prohibited compile flag found"
              exit 1
            fi
            echo "$FLAGS" | grep -q -- '-ffp-contract=off'

jobs:
  parity-gate:
    docker:
      - image: cimg/base:2026.11-24.04
    steps:
      - checkout
      - build-engine
```

The image tag is pinned on purpose. A new image can change the compiler or math library, which makes it a Reference Target change.

Phase 3 is complete when `--self-parity` passes at two thread counts with both changes merged, and `parity-gate` passes in CircleCI.

---

## 🔄 Phase 4: Baseline 1 and the CircleCI Parity Gate

To ensure that no code mutations skip verification, this phase captures the first references and turns on the gate.

### 1. Add the Gate and Capture Jobs
Add the gate steps to the `parity-gate` job, add a `capture-baseline` job, and add it to the `build-and-deploy` workflow. `parity-gate` stays red until references exist, so merge this pull request immediately before the Baseline 1 pull request in the next step:

```yaml
jobs:
  parity-gate:
    docker:
      - image: cimg/base:2026.11-24.04
    steps:
      - checkout
      - build-engine
      - run:
          name: Run Parity Gate
          command: python3 tests/regression_runner.py
      - store_artifacts:
          path: tests/outputs
          destination: render-regression-snapshots

  capture-baseline:
    docker:
      - image: cimg/base:2026.11-24.04
    steps:
      - checkout
      - build-engine
      - run:
          name: Capture References
          command: python3 tests/regression_runner.py --capture
      - store_artifacts:
          path: tests/references
          destination: baseline-references

workflows:
  build-and-deploy:
    jobs:
      - build-and-pack
      - parity-gate
      - capture-baseline:
          filters:
            branches:
              only: /^baseline\/.*/
```

Capture renders every scene and formula about eight times, so it takes far longer than the gate. The harness plan decides how this job fits your CircleCI limits.

### 2. Capture Baseline 1
Baseline 1 is a Baseline Change, so its pull request contains only reference files. References must come from CI rather than a development machine:
```bash
git checkout -b baseline/1
git commit --allow-empty -m "ci: trigger Baseline 1 capture"
git push origin baseline/1
```

When `capture-baseline` finishes, download the artifact files under `baseline-references/`, copy them into `tests/references/`, and push them to the same branch. Then open the pull request. `parity-gate` passes once the references are present. The pull request description must state that no earlier references exist, which replaces the usual before-and-after evidence, and link the passing Self-Parity run.

### 3. Require the Gate
In the GitHub branch protection rules for `master`, mark `parity-gate` as a required status check. A CI job does not block a merge by itself.

### 4. Prove the Gate Catches Drift
```bash
claude -c "On a throwaway branch, change one constant in a formula that no scene in tests/scenes.txt uses, confirm that the parity-gate job fails on that formula in the sweep, then delete the branch without merging it."
```

### 5. Close the Known Deviations
Amend the constitution to remove KD-1 through KD-4, which is a PATCH version bump. Engine refactors can now begin.

---

## 🚀 Phase 5: The Production Upgrade Lifecycle (Task 1: Qt6 Decoupling)

Once Baseline 1 is merged and the gate is required, use this 5-step operational loop to isolate UI/Framework dependencies from your core math and business logic.

### Step 1: Specify the Abstraction Scope
Open a distinct feature branch for the abstraction task and specify the architectural boundary goals:
```bash
git checkout -b upgrade/qt6-abstraction-layer
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
Verify that changes compile cleanly and yield identical output before pushing. References come from CI and may not match your local toolchain, so compare your branch against a `master` build on the same machine. This is a local check, and the CircleCI gate stays the authority.

Build `master` once in a separate worktree:
```bash
git worktree add ../mandelbulber2-master master
(
  mkdir -p ../mandelbulber2-master/mandelbulber2/build
  cd ../mandelbulber2-master/mandelbulber2/build
  qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
  make -j$(nproc)
  make INSTALL_ROOT=appdir install
)
```

Then build your branch and compare it with that build:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --compare ../mandelbulber2-master/mandelbulber2/build/appdir/usr/bin/mandelbulber2
```

*If convergence parameters require adjustment:*
```bash
claude -c "/speckit-converge Audit the decoupled abstraction code, verify that no stray Qt types leak back into the engine modules, and confirm absolute math convergence against the main branch baseline."
```

A parity failure is a defect in the change. Do not regenerate references to clear it.

### Step 5: Merge and Push to CircleCI Verification Gating
When compare mode reports **no differences**, push your changes so the `parity-gate` job can verify them on the Reference Target:
```bash
git add mandelbulber2 specs
git commit -m "refactor(core): implemented abstract interface layer to decouple engine from Qt6 types"
git push origin upgrade/qt6-abstraction-layer
```

---

## 📝 Appendix: Project Documentation Addendum (`README.md`)

After Phase 4, add this structural snippet to the main project workspace documentation to inform development contributors of the regression gating tools:

## 🔄 Continuous Integration & Regression Testing

This project uses **CircleCI** alongside a custom differential testing framework to guarantee **Zero Behavioral Drift** across core fractal calculation models and rendering loops during upgrades or optimizations.

The regression gate builds the codebase with `qmake6`, headlessly renders a fixed scene list and every fractal formula, and compares the application's raw image buffers and render statistics against per-platform references, bit for bit.

### Locally Testing Parity
Before committing changes to feature branches, compare your branch with a `master` build on the same Linux machine. This works with any toolchain, and the CircleCI gate stays the authority:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --compare <path to a master build's installed binary>
```
