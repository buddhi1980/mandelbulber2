# Mandelbulber2: Production Upgrade & Parity Testing Playbook

This document defines the automated, spec-driven engineering process to safely transition the `mandelbulber2` codebase into production-ready software. Because fractal math relies heavily on precise floating-point iterations, any code changes can alter image structures or break determinism.

We mitigate this risk by executing a strict **Zero-Drift Regression Harness** powered by **GitHub Spec Kit**, **Claude Code**, and an automated **CircleCI Pipeline**.

The [project constitution](.specify/memory/constitution.md) governs this process. Where this playbook and the constitution conflict, the constitution applies and this playbook must be corrected.

| Phase | Result | Closes | Complete when |
|---|---|---|---|
| 1. Setup | Spec Kit and the constitution are in place | None | Done |
| 2. Harness | The regression runner exists, with no references yet | Part of KD-4 | The runner's failure checks pass |
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
Create the feature branch, then generate the specification. Spec Kit places it in a numbered folder, such as `specs/001-regression-harness/`:
```bash
git checkout -b upgrade/regression-harness
claude -c "/speckit-specify Create a specification for a headless differential testing engine. It reads a version-controlled scene list from tests/scenes.txt, renders each .fract file with the mandelbulber2 CPU engine using --nogui, and compares decoded 32-bit float TIFF samples against per-target reference data with exact bitwise equality and no tolerance. It needs three modes: self-parity, reference capture, and gate. It must fail on an empty scene list, a missing scene, a failed settings load, or a missing output file, and must not rely on the exit code, because the application exits 0 after a failed load or a failed save. It must also record final render statistics from cStatistics (total iterations, raymarch count, missed DE, and the iteration and step-count histograms) and diff them exactly, excluding time-based values. No deterministic export of these statistics exists, so the spec must define one. All written image channels must be compared, and the spec must decide whether to enable the z-buffer and normal channels as well."
```

### 2. Design the Implementation Breakdown
Convert your specification into technical tasks:
```bash
claude -c "/speckit-clarify Analyze the mandelbulber2 build system, headless execution flags, and C++ structure to surface any hidden ambiguities before creating the technical implementation design plan."
claude -c "/speckit-plan Design the architecture for the regression engine using a standalone Python script at tests/regression_runner.py that uses only the standard library, targets the installed binary at mandelbulber2/build/appdir/usr/bin/mandelbulber2, and compares decoded TIFF samples bit for bit."
claude -c "/speckit-tasks Break down the implementation into incremental, discrete engineering items inside tasks.md. Keep the runner isolated from core C++ production code. The only C++ change allowed is the statistics export, which must only read existing counters. Add tests/outputs/ to .gitignore. Do not capture references."
```

### 3. Generate the Harness Boilerplate
These application behaviors shape the runner. They were tested on a Windows build. The Linux install layout and `HOME` handling come from reading the source, because they have not been run yet.

| Need | Use | Why |
|---|---|---|
| Headless render | `--nogui`, with the scene file as a positional argument | The app exits by itself after saving |
| Full precision | `--format tiff --override color_quality=2` | Plain `--format tiff` saves 8-bit, which hides small drift. This saves 32-bit float RGB |
| Boolean overrides | `name=1` or `name=0` | `name=true` has no effect |
| Binary location | The installed tree, `mandelbulber2/build/appdir/usr/bin/mandelbulber2` | On Linux the app finds its data at `../share/mandelbulber2` relative to the binary, or at `/usr/share/mandelbulber2`. A binary left in the build folder has neither |
| Isolation | A temporary `HOME` and a fixed `LC_ALL` | Otherwise the app reads per-user settings. Windows ignores this, so Windows runs are for debugging only |
| Failure detection | Check the scene, the output file, and the output text | The app exits 0 and prints `Image saved to:` even when the target folder is missing. A bad settings file also exits 0, after rendering default parameters |
| Computation logs | A dedicated statistics export | Log lines start with a process ID and elapsed time, `--stats` prints values rounded to six significant digits, and `--logfilepath` wrote no file in testing |

Instruct Claude Code to write the runner to your repository, then create `tests/scenes.txt`:

```bash
claude -c "Write the Python template below to tests/regression_runner.py and create tests/scenes.txt. Keep the template's checks. Add statistics comparison once the export exists."
```

```python
#!/usr/bin/env python3
"""Zero-drift regression runner for the mandelbulber2 CPU engine.

  python3 tests/regression_runner.py                compare renders against references (the gate)
  python3 tests/regression_runner.py --self-parity  require repeated renders to match each other
  python3 tests/regression_runner.py --capture      write new references (Baseline Change only)
"""
import argparse
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib

BINARY = os.environ.get("MANDELBULBER_BINARY", "mandelbulber2/build/appdir/usr/bin/mandelbulber2")
TARGET = os.environ.get("PARITY_TARGET", "linux-x86_64-gcc")
SCENE_LIST = os.path.join("tests", "scenes.txt")
REF_DIR = os.path.join("tests", "references", TARGET)
OUT_DIR = os.path.join("tests", "outputs")
RESOLUTION = "320x240"
RENDER_TIMEOUT_SECONDS = 1800
SELF_PARITY_RUNS = 3
# Extra CLI arguments for each Self-Parity thread setting. When KD-1 gives --nogui a working
# thread control, add a second entry that forces one thread. Capture is refused until then.
THREAD_SETTINGS = [[]]
# color_quality=2 saves 32-bit float. The default is 8-bit, which hides small drift.
OVERRIDES = "color_quality=2#opencl_enabled=0"
# The app exits 0 after these failures, so the exit code alone cannot be trusted.
FAILURE_MARKERS = ("not valid Mandelbulber settings file", "No materials defined")


class RenderError(Exception):
    pass


def scene_name(scene):
    return os.path.splitext(os.path.basename(scene))[0]


def load_scenes():
    with open(SCENE_LIST, encoding="utf-8") as file:
        scenes = [line.strip() for line in file if line.strip() and not line.startswith("#")]
    if not scenes:
        raise RenderError(f"{SCENE_LIST} lists no scenes")
    missing = [scene for scene in scenes if not os.path.isfile(scene)]
    if missing:
        raise RenderError(f"scene files not found: {missing}")
    names = [scene_name(scene) for scene in scenes]
    if len(set(names)) != len(names):
        raise RenderError("scene file names must be unique")
    return scenes


def run_headless_render(scene, output_base, extra_args=()):
    """Renders one scene with an isolated home directory and returns the saved TIFF path."""
    image = output_base + ".tiff"
    if os.path.exists(image):
        os.remove(image)
    # The app reports "Image saved" and exits 0 when the target folder does not exist.
    os.makedirs(os.path.dirname(output_base), exist_ok=True)
    binary = os.path.abspath(BINARY)
    cmd = [binary, "--nogui", "--format", "tiff", "--res", RESOLUTION, "--override", OVERRIDES]
    cmd += [*extra_args, "--output", os.path.abspath(output_base), os.path.abspath(scene)]
    with tempfile.TemporaryDirectory(prefix="mandelbulber-home-") as home:
        env = dict(os.environ, HOME=home, LC_ALL="C.UTF-8")
        # On Windows the app loads its program data from the working directory.
        result = subprocess.run(
            cmd,
            cwd=os.path.dirname(binary),
            env=env,
            capture_output=True,
            encoding="utf-8",
            errors="replace",
            timeout=RENDER_TIMEOUT_SECONDS,
        )
    output = result.stdout + result.stderr
    with open(output_base + ".log", "w", encoding="utf-8") as log:
        log.write(output)
    if result.returncode != 0:
        raise RenderError(f"{scene}: exit code {result.returncode}, see {output_base}.log")
    for marker in FAILURE_MARKERS:
        if marker in output:
            raise RenderError(f"{scene}: {marker}, see {output_base}.log")
    if not os.path.isfile(image):
        raise RenderError(f"{scene}: no image written, see {output_base}.log")
    return image


def read_tiff(path):
    """Decodes a deflate TIFF and returns its layout and raw sample bytes, without conversion."""
    with open(path, "rb") as file:
        data = file.read()
    order = {b"II": "<", b"MM": ">"}.get(data[:2])
    if order is None or struct.unpack(order + "H", data[2:4])[0] != 42:
        raise RenderError(f"{path}: not a classic TIFF")
    ifd = struct.unpack(order + "I", data[4:8])[0]
    tags = {}
    for index in range(struct.unpack(order + "H", data[ifd : ifd + 2])[0]):
        entry = ifd + 2 + index * 12
        tag, kind, count = struct.unpack(order + "HHI", data[entry : entry + 8])
        code = {3: "H", 4: "I"}.get(kind)
        if code is None:
            continue
        size = count * struct.calcsize(code)
        offset = entry + 8
        if size > 4:
            offset = struct.unpack(order + "I", data[offset : offset + 4])[0]
        tags[tag] = struct.unpack(f"{order}{count}{code}", data[offset : offset + size])
    if tags.get(259) not in ((8,), (32946,)) or tags.get(317, (1,)) != (1,):
        raise RenderError(f"{path}: unsupported TIFF compression or predictor")
    width, height = tags[256][0], tags[257][0]
    bits, samples_per_pixel = tags[258], tags[277][0]
    strips = zip(tags[273], tags[279])
    samples = b"".join(zlib.decompress(data[start : start + size]) for start, size in strips)
    if len(samples) != width * height * samples_per_pixel * bits[0] // 8:
        raise RenderError(f"{path}: decoded size does not match the image layout")
    return (width, height, bits, samples_per_pixel, tags.get(339)), samples


def compare_images(reference, current):
    """Compares decoded samples bit for bit. There is no tolerance."""
    ref_layout, ref_samples = read_tiff(reference)
    cur_layout, cur_samples = read_tiff(current)
    if ref_layout != cur_layout:
        return False, f"layout differs: {ref_layout} vs {cur_layout}"
    if ref_samples == cur_samples:
        return True, "exact match"
    step = ref_layout[2][0] // 8
    offsets = range(0, len(ref_samples), step)
    differing = sum(ref_samples[i : i + step] != cur_samples[i : i + step] for i in offsets)
    return False, f"{differing} of {len(offsets)} samples differ"


def check_self_parity(scenes):
    passed = True
    for scene in scenes:
        name = scene_name(scene)
        first = None
        for setting, extra_args in enumerate(THREAD_SETTINGS):
            for run in range(SELF_PARITY_RUNS):
                base = os.path.join(OUT_DIR, "self-parity", f"{name}-s{setting}-r{run}")
                image = run_headless_render(scene, base, extra_args)
                first = first or image
                same, detail = compare_images(first, image)
                passed = passed and same
                print(f"[{'+' if same else '-'}] {name} s{setting} r{run}: {detail}", flush=True)
    return passed


def capture_references(scenes):
    if len(THREAD_SETTINGS) < 2:
        raise RenderError("Self-Parity needs a second thread setting, see KD-1")
    if not check_self_parity(scenes):
        return False
    os.makedirs(REF_DIR, exist_ok=True)
    for scene in scenes:
        name = scene_name(scene)
        image = run_headless_render(scene, os.path.join(OUT_DIR, name))
        shutil.copyfile(image, os.path.join(REF_DIR, name + ".tiff"))
        print(f"[+] {name}: reference captured", flush=True)
    return True


def compare_against_references(scenes):
    passed = True
    for scene in scenes:
        name = scene_name(scene)
        reference = os.path.join(REF_DIR, name + ".tiff")
        if not os.path.isfile(reference):
            passed = False
            print(f"[-] {name}: no reference at {reference}", flush=True)
            continue
        image = run_headless_render(scene, os.path.join(OUT_DIR, name))
        same, detail = compare_images(reference, image)
        passed = passed and same
        print(f"[{'+' if same else '-'}] {name}: {detail}", flush=True)
    return passed


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--self-parity", action="store_true")
    mode.add_argument("--capture", action="store_true")
    args = parser.parse_args()
    try:
        scenes = load_scenes()
        if args.self_parity:
            passed = check_self_parity(scenes)
        elif args.capture:
            passed = capture_references(scenes)
        else:
            passed = compare_against_references(scenes)
    except (RenderError, OSError, subprocess.TimeoutExpired) as error:
        print(f"[-] {error}", file=sys.stderr)
        return 1
    print("[+] All scenes passed" if passed else "[-] Parity failed", flush=True)
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
```

`tests/scenes.txt` is the Standard Scene Suite, with one path per line, relative to the repository root. Start with these two, then add scenes that exercise Monte Carlo depth of field, global illumination, and ambient occlusion, as the constitution requires:

```text
# Standard Scene Suite
mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract
mandelbulber2/deploy/share/mandelbulber2/examples/Makin3D-Julia_001.fract
```

### 4. Implement and Verify
Have Claude complete the implementation loop:
```bash
claude -c "/speckit-implement Iteratively write code to satisfy the checklist items found inside tasks.md. Wire tests/regression_runner.py to the scene list in tests/scenes.txt and the installed binary. Do not create anything under tests/references/ in this phase."
claude -c "/speckit-converge Confirm that one flipped bit in a float sample fails the comparison, that an unstable render blocks capture and writes no references, and that a missing scene, a failed settings load, and a missing output file each fail. Confirm that --self-parity fails on the current build, which is the expected KD-1 result."
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
claude -c "/speckit-specify Close KD-1 from the constitution by making CPU renders reproducible. First confirm the cause, because it is not yet proven: render threads share the unsynchronized global gRandomSeed in common_math.cpp, and Random() runs on every raymarch step. Replace that shared state so each random value derives only from a seed fixed in the scene or harness and from the pixel and sample position. Cover every CPU caller of Random(): render_worker, dof, light, ssao_worker, shader_ambient_occlusion, shader_aux_shadow, shader_global_illumination, shader_specular_highlight, and shader_volumetric. Audit the OpenMP regions in cimage, denoiser, dof, and post_effect_hdr_blur for order-dependent results. Give --nogui a working thread-count control, because limit_CPU_cores is only applied by the GUI startup path and the queue. Success means --self-parity passes at one thread and at all available threads."
```

When the thread control exists, add its one-thread arguments as a second entry in `THREAD_SETTINGS` in the runner. Until then, `--self-parity` covers only one thread setting and `--capture` is refused.

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

Phase 3 is complete when `--self-parity` passes at two thread settings with both changes merged, and `parity-gate` passes in CircleCI.

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
          command: |
            python3 tests/regression_runner.py --capture
            { g++ --version | head -1; ldd --version | head -1; } > tests/references/linux-x86_64-gcc/toolchain.txt
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
claude -c "On a throwaway branch, change one constant in a formula used by a scene in tests/scenes.txt, confirm that the parity-gate job fails on that scene, then delete the branch without merging it."
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
Verify that changes compile cleanly and yield identical output before pushing:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py
```

*If convergence parameters require adjustment:*
```bash
claude -c "/speckit-converge Audit the decoupled abstraction code, verify that no stray Qt types leak back into the engine modules, and confirm absolute math convergence against the main branch baseline."
```

A parity failure is a defect in the change. Do not regenerate references to clear it.

### Step 5: Merge and Push to CircleCI Verification Gating
When all test scenarios report an **exact match**, push your changes so the `parity-gate` job can verify them on the Reference Target:
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

The regression gate builds the codebase with `qmake6`, renders a fixed scene list headlessly, and compares the decoded 32-bit float image samples against per-platform references, bit for bit.

### Locally Testing Parity
Before committing changes to feature branches, run the local gatekeeper check on Linux to confirm that your change produces an exact match against the reference baseline:
```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py
```
