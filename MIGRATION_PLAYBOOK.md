# Mandelbulber2: Production Upgrade & Parity Testing Playbook

This document defines the automated, spec-driven engineering process to safely transition the `mandelbulber2` codebase into production-ready software. Because fractal math relies heavily on precise floating-point iterations, any code changes can alter image structures or break determinism. 

We mitigate this risk by executing a strict **Zero-Drift Regression Harness** powered by **GitHub Spec Kit**, **Claude Code**, and an automated **CircleCI Pipeline**.

---

## 🛠️ Phase 1: Environment & Tooling Setup

Before initiating any codebase modifications, ensure the verification stack is locked down inside your local repository root.

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

We do not modify production files until we build the automated harness capable of catching regressions.

### 1. Define the Harness Specification
Execute the design step to generate a comprehensive testing specification:
```bash
claude -c "/speckit-specify Create a comprehensive specification for a headless differential testing engine in specs/regression-harness/spec.md. The engine must read an array of fixed .fract parameter settings files, run the mandelbulber2 CLI headlessly to generate raw image buffers and internal computation logs, and execute a pixel-for-pixel validation check alongside a line-by-line diff of computation logs (tracking raymarching step counts and voxel hits) against reference data."
```

### 2. Design the Implementation Breakdown
Convert your specification into technical tasks, tailoring the build strategy directly to the native `qmake` and `make` pipelines:
```bash
claude -c "/speckit-clarify Analyze the mandelbulber2 build system, headless execution flags, and C++ structure to surface any hidden ambiguities before creating the technical implementation design plan."
claude -c "/speckit-plan Design the architecture for the regression engine using a standalone Python wrapper script located in tests/regression_runner.py that targets the qmake compiled binary location (mandelbulber2/qmake/mandelbulber2) and handles binary stream image diffing."
claude -c "/speckit-tasks Break down the implementation into incremental, discrete engineering items inside tasks.md, keeping the framework build completely isolated from core C++ production code modifications."
```

### 3. Generate the Harness Boilerplate
Instruct Claude Code to output the operational regression runner template directly to your repository filesystem. The target binary configuration explicitly maps to the repository's native `qmake` directory output:

```bash
claude -c "Write the full Python boilerplate template to tests/regression_runner.py. The file must handle programmatic binary execution via subprocess, parameter file parsing, and bitwise/pixel validation hooks. Use the copy-pasteable script structure below as your baseline template."
```

```python
#!/usr/bin/env python3
import os
import sys
import subprocess
import hashlib
from PIL import Image

def run_headless_render(binary_path, fract_file, output_img, log_file):
    """Executes the mandelbulber2 CLI headlessly to generate a render and log dump."""
    cmd = [
        binary_path,
        "--cli",
        "--settings", fract_file,
        "--savepng", output_img,
        "--log-file", log_file,
        "--quit"
    ]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return True, result.stdout
    except subprocess.CalledProcessError as e:
        print(f"[-] Execution failed for {fract_file}: {e.stderr}", file=sys.stderr)
        return False, e.stderr

def verify_pixel_parity(ref_img_path, current_img_path):
    """Checks for exact pixel-for-pixel visual match (L2 distance == 0)."""
    if not os.path.exists(ref_img_path) or not os.path.exists(current_img_path):
        return False, "Missing image file for comparison."
    
    img_ref = Image.open(ref_img_path).convert('RGB')
    img_cur = Image.open(current_img_path).convert('RGB')
    
    if img_ref.size != img_cur.size:
        return False, f"Resolution mismatch: {img_ref.size} vs {img_cur.size}"
    
    if img_ref.tobytes() == img_cur.tobytes():
        return True, "Exact match"
        
    return False, "Pixel mismatch detected in render matrix output."

def main():
    REF_DIR = "tests/references"
    OUT_DIR = "tests/outputs"
    BINARY = "./mandelbulber2/qmake/mandelbulber2"
    print("[*] Starting Mandelbulber2 Differential Parity Verification...")
    sys.exit(0)

if __name__ == "__main__":
    main()
```

### 4. Implement and Verify
Have Claude complete the implementation loop to tie your parameters and test folders together:
```bash
claude -c "/speckit-implement Iteratively write code to satisfy the checklist items found inside tasks.md. Fully wire tests/regression_runner.py to execute the scenario suite using the qmake binary target, establish directory structures for reference assets under tests/references/, and map output telemetry logging hooks."
claude -c "/speckit-converge Execute the verification suite locally, compile mock scenarios, and ensure the newly constructed regression harness accurately flags arbitrary artificial bitwise mutations inside a dummy C++ branch."
```

---

## 🔄 Phase 3: CircleCI Continuous Integration Pipeline

To ensure that no code mutations skip verification, save the following setup file to `.circleci/config.yml` in your repository root. This configures a Linux executor node with essential build tools, hooks into the project's native `qmake`/`make` targets, and runs the regression script to gate PR merges.

```yaml
version: 2.1

jobs:
  build_and_verify:
    docker:
      - image: cimg/openjdk:22.0.2-node
    steps:
      - checkout

      - run:
          name: Install System Dependencies & Qt Development Packages
          command: |
            sudo apt-get update
            sudo apt-get install -y build-essential qt6-base-dev qt6-base-dev-tools libgl1-mesa-dev python3-pip python3-pillow

      - run:
          name: Compile Codebase via Qmake & Make
          command: |
            cd mandelbulber2/qmake
            qmake mandelbulber.pro
            make -j$(nproc)

      - run:
          name: Run Full Parity and Pixel-to-Pixel Regression Harness
          command: |
            python3 tests/regression_runner.py

      - store_artifacts:
          path: tests/outputs
          destination: render-regression-snapshots
```

---

## 🚀 Phase 4: The Production Upgrade Lifecycle (Task 1: Qt6 Decoupling)

Once your pipeline configuration is checked in, use this 5-step operational loop to isolate UI/Framework dependencies from your core math and business logic.

### Step 1: Specify the Abstraction Scope
Open a distinct feature branch for the abstraction task and specify the architectural boundary goals:
```bash
git checkout -b upgrade/qt6-abstraction-layer
claude -c "/speckit-specify Create a software specification in specs/qt6-abstraction/spec.md to design a clean API abstraction layer between Qt6 and the core rendering engine. The objective is to decouple UI types, custom Qt container wrappers (like QString, QVector, QPoint), and Qt event loops from the mathematical formulas and engine pipelines, substituting them with pure standard library types (std::string, std::vector) without modifying rendering behavior or data structures."
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
Verify that changes compile cleanly with your native qmake configuration and yield identical math output streams before pushing:
```bash
cd mandelbulber2/qmake && qmake && make -j\$(nproc) && cd ../..
python3 tests/regression_runner.py
```

*If convergence parameters require adjustment:*
```bash
claude -c "/speckit-converge Audit the decoupled abstraction code, verify that no stray Qt types leak back into the engine modules, and confirm absolute math convergence against the main branch baseline."
```

### Step 5: Merge and Push to CircleCI Verification Gating
When all test scenarios report an **exact match (L₂ distance = 0)**, push your changes to let the CircleCI runner validate cross-compilation stability:
```bash
git add .
git commit -m "refactor(core): implemented abstract interface layer to decouple engine from Qt6 types"
git push origin upgrade/qt6-abstraction-layer
```

---

## 📝 Appendix: Project Documentation Addendum (`README.md`)

To inform development contributors of the newly introduced regression gating tools, add this structural snippet directly into the main project workspace documentation:

## 🔄 Continuous Integration & Regression Testing

This project uses **CircleCI** alongside a custom differential testing framework to guarantee **Zero Behavioral Drift** across core fractal calculation models and rendering loops during upgrades or optimizations.

The regression gate compiles the codebase via `qmake` and executes headless renders using predefined `.fract` parameters, verifying output matrices using an absolute pixel-for-pixel comparison.

### Locally Testing Parity
Before committing changes to feature branches, run the local gatekeeper check to confirm your refactored code has an exact math match (L₂ distance = 0) against the reference baseline:
```bash
cd mandelbulber2/qmake && qmake && make -j$(nproc) && cd ../..
python3 tests/regression_runner.py
```
