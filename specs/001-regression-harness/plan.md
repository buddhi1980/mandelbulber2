# Implementation Plan: Regression Harness

**Branch**: `upgrade/regression-harness` | **Date**: 2026-11-04 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/001-regression-harness/spec.md`

## Summary

Build a headless differential testing engine that proves whether a build changes any render output. A standalone Python script, `tests/regression_runner.py`, renders the Standard Scene Suite and a sweep of every formula, then compares the results bit for bit. Gate is the default mode, with `--self-parity`, `--capture`, and `--compare <binary>`.

A small opt-in addition to the application provides the data. `--verification-export <dir>` writes the engine's five buffers and exact statistics after a successful render, and `--list-formulas` prints the formulas the binary offers. Both only read values that already exist, so the render path is unchanged.

No references are created in this feature. Renders do not reproduce yet (KD-1), so the expected result is a harness that correctly reports that failure. See [research.md](research.md) for the evidence behind each decision.

## Technical Context

**Language/Version**: Python 3.10 or later for the harness (3.12 on Ubuntu 24.04, 3.10 on the local Windows machine). C++14 for the application changes, because qmake builds Linux with `CONFIG += c++14`.

**Primary Dependencies**: Python standard library only. The application changes use Qt Core, which is already a dependency.

**Storage**: Files. Exports are written to `tests/outputs/`, which is not version-controlled. References are stored in `tests/references/<target>/` as one zip per render.

**Testing**: Python `unittest` with a stand-in renderer that implements the export contract, plus validation against the real binary in [quickstart.md](quickstart.md).

**Target Platform**: Linux x86-64 with GCC, built from `mandelbulber-opencl.pro` in release mode, on GitHub-hosted `ubuntu-24.04` runners (currently GCC 13.3.0 and glibc 2.39). Windows runs are for debugging only.

**Project Type**: Command-line test harness, plus a small addition to an existing desktop and CLI application.

**Performance Goals**: A full gate run under 30 minutes on a 4-vCPU runner (SC-004). The expected render time is about 15 minutes: 12 for scenes and 2 to 5 for the sweep, both estimated. The sweep measured 34 seconds on 32 threads. Capture is expected to take about 2 hours.

**Constraints**: No third-party Python packages. No added work in the render path. References under 100 MB per target. Each CI job within GitHub's 6-hour limit. A render time limit of 30 minutes for scenes and 2 minutes for sweep items.

**Scale/Scope**: About 25 scenes and 462 formulas. A gate run performs about 487 renders, and capture about 3,400.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Gate | Status | Notes |
|---|---|---|
| Principle I: can this change affect gated output? | Pass | The export and listing run after the render or instead of it, and only when requested. Scenes and every formula are covered. FR-012 proves the gated configuration matches a default render. |
| Principle I: exact, full-precision comparison | Pass | Raw engine buffers are compared bit for bit, with no tolerance or down-conversion. This is why TIFF was replaced. |
| Principle II: adds randomness, timing, threading, or external state? | Pass | Nothing is added to the render path. The harness isolates home, locale, and environment. |
| Principle II: Self-Parity | Pass | Three runs at each of two thread counts, one of them 1. Capture refuses without it. |
| Principle III: touches build flags, compilers, math libraries, or arithmetic order? | Pass | No flags change. The manifest only reads compiler and math library versions. |
| Reference Targets: a difference between targets is not drift | Pass | A toolchain difference with differing output is a Reference Target mismatch, not a parity failure. |
| Standard Scene Suite: each stochastic CPU path | Pass | FR-039 lists the paths, and `research.md` gives a candidate scene for each. |
| Workflow 1: engine changes only to close a Known Deviation | Pass | The two application additions exist to close KD-4. Nothing else changes. |
| Workflow 3: no regenerating references to clear a failure | Pass | Gate, self-parity, and compare modes cannot write references. |
| Workflow 4: refactors and output changes stay separate | Pass | This feature changes no output. |
| Baseline Change rules | Pass | Capture runs only in CI, requires Self-Parity, and is all or nothing. No references are created here. |

**Known Deviations**: These are not reported as passing.

- **KD-1 (open)**: `--self-parity` is expected to fail on the current build, and the statistics spread suggests a second cause beyond the random seed. Both are for playbook Phase 3 to fix.
- **KD-2, KD-3 (open)**: This feature doesn't touch them. The CI flag check arrives in playbook Phase 3.
- **KD-4 (partly closed)**: This feature delivers the harness and scene list. References and the CI gate arrive in playbook Phase 4.

**Expected effect on output**: None. The constitution requires each pre-baseline engine change to state this. The application changes run after the render or instead of it, and only when requested.

**Post-design re-check**: Pass. The design adds no violations. `--list-formulas` required a small update to FR-031, recorded in the spec.

## Project Structure

### Documentation (this feature)

```text
specs/001-regression-harness/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── application-cli.md
│   ├── reference-set.md
│   ├── runner-cli.md
│   └── verification-export.md
├── checklists/
│   └── requirements.md
└── tasks.md             # Created by /speckit-tasks
```

### Source Code (repository root)

```text
tests/
├── regression_runner.py        # The harness
├── scenes.txt                  # Standard Scene Suite
├── scenes/beam_light.fract     # Purpose-built scene for the beam light path
├── formula_sweep_base.fract    # Fixed base scene for the sweep
├── formula_exclusions.txt      # Excluded formulas, with reasons
├── stub_renderer.py            # Stand-in renderer for tests
├── test_regression_runner.py   # Harness tests
├── references/<target>/        # Created in playbook Phase 4
└── outputs/                    # Not version-controlled

mandelbulber2/src/
├── verification_export.hpp     # New
├── verification_export.cpp     # New: writes buffers, statistics, manifest
├── command_line_interface.hpp  # New options, settings load result
├── command_line_interface.cpp
├── headless.h                  # New RenderStillImage parameters
└── headless.cpp                # Writes the export after a successful render

.gitignore                      # Adds tests/outputs/
```

**Structure Decision**: A single script, as requested, keeps the harness easy to run anywhere Python exists. New C++ goes in its own file pair, so the existing files change as little as possible. qmake, CMake, and the MSVC project all use `src/*.cpp` wildcards, so no build file changes are needed.

## Design Overview

**Application changes**

1. `handleArgs` records whether `LoadFromFile` or `LoadFromString`, and then `Decode`, succeeded.
2. `RenderStillImage` keeps the result of `Execute()`, then calls the export if a directory was requested and both results are true.
3. The export removes any old manifest, writes the buffers and statistics, then writes the manifest last.
4. `--list-formulas` prints `newFractalList` and exits, alongside the existing `--list` handling.

**Harness flow**

1. Load and check the scene list, then read the formula list from the binary and apply exclusions.
2. For each item, render it in isolation, check the output text, and validate the export.
3. Compare the result with references, with earlier runs, or with a second build, depending on the mode.
4. Report per item and buffer, keep failed outputs, and return an exit code that separates parity failures, setup errors, and Reference Target mismatches.

**Self-parity order**: All-thread and neutral renders run one item at a time. Single-thread renders then run for several items at once, one worker per CPU, and each item's own runs stay consecutive.

**Capture in CI**: One manually started GitHub Actions workflow builds the binary, runs `--capture`, and uploads the reference set as an artifact. Capture refuses to run outside CI. A maintainer then commits the set in a Baseline Change pull request. The workflows belong to Phases 3 and 4 of the playbook, and this feature provides the runner they call.

## Follow-ups Outside This Feature

- The playbook Phase 3 thread control must set the OpenMP thread count too, and the harness then needs its second thread count enabled.
- The hosted runner image changes over time. If updates start to cause false failures, a pinned container and pinned packages are the next step.

## Complexity Tracking

No constitution violations need justifying.
