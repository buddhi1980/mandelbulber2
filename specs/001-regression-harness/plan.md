# Implementation Plan: Regression Harness

**Branch**: `upgrade/regression-harness` | **Date**: 2026-11-04 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/001-regression-harness/spec.md`

## Summary

Build a headless differential testing engine that proves whether a build changes any render output. A standalone Python script, `tests/regression_runner.py`, renders the Standard Scene Suite and a sweep of every formula, then compares the results bit for bit. Gate is the default mode, with `--self-parity`, `--capture`, `--compare <binary>`, `--collect <dir>`, and `--cross-check <first> <second>`.

A small opt-in addition to the application provides the data. `--verification-export <dir>` writes the engine's seven buffers and exact statistics after a successful render, and `--list-formulas` prints the formulas the binary offers. Both only read values that already exist, so the render path is unchanged.

No references are created in this feature. Renders do not reproduce yet (KD-1), so the expected result is a harness that correctly reports that failure. See [research.md](research.md) for the evidence behind each decision.

Development uses two builds of one commit on one computer: qmake in WSL and MSVC on Windows. `--collect` records one build's results, and `--cross-check` compares two collections from their files alone. It fails only on exact functional facts and records output differences, so it works before renders reproduce and uses no tolerance.

A separate GitHub Actions pipeline, Cross-Build Check, runs the same commands on hosted runners. It is added beside the existing workflows and changes none of them.

## Technical Context

**Language/Version**: Python 3.10 or later for the harness (3.12 on Ubuntu 24.04, 3.10 on the local Windows machine). C++17 for the application changes, because Qt 6 headers reject anything older. `CONFIG += c++14` in `common.pri` has no effect under Qt 6.

**Primary Dependencies**: Python standard library only. The application changes use Qt Core, which is already a dependency.

**Storage**: Files. Exports are written to `tests/outputs/`, which is not version-controlled. References are stored in `tests/references/<target>/` as one zip per render.

**Testing**: Python `unittest` with a stand-in renderer that implements the export contract, plus validation against both real builds in [quickstart.md](quickstart.md).

**Target Platform**: Three environments (FR-040). The WSL `Ubuntu-24.04` distro runs the qmake build, with GCC 13.3.0, Qt 6.4.2, and glibc 2.39, matching the runner. Windows runs the MSVC Release build, with toolset v145 and Qt 6.11.1. GitHub Actions runs both, on `ubuntu-24.04` and `windows-2025-vs2026`. The first Reference Target is the Linux x86-64 GCC build, from `mandelbulber-opencl.pro` in release mode. Windows runs are not gate-valid, so they serve debugging, compare mode's local check, and the cross-check.

**Project Type**: Command-line test harness, plus a small addition to an existing desktop and CLI application.

**Performance Goals**: A full gate run under 30 minutes on a 4-vCPU runner (SC-004). The expected render time is about 15 minutes: 12 for scenes and 2 to 5 for the sweep, both estimated. The sweep measured 34 seconds on 32 threads. Capture is expected to take about 2 hours. A local cross-check of two ready builds should take under 15 minutes (SC-012), and a full pipeline run under 90 minutes (SC-014). Both are estimates, and the second allows for the 52-minute MSVC build measured on AppVeyor.

**Constraints**: No third-party Python packages. No added work in the render path. References under 100 MB per target. Each CI job within GitHub's 6-hour limit. A render time limit of 30 minutes for scenes and 2 minutes for sweep items.

**Scale/Scope**: About 25 scenes and 462 formulas. A gate run or a collection performs about 487 renders, and capture about 3,400.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Gate | Status | Notes |
|---|---|---|
| Principle I: can this change affect gated output? | Pass | The export and listing run after the render or instead of it, and only when requested. Scenes and every formula are covered. FR-012 proves the gated configuration matches a default render. |
| Principle I: exact, full-precision comparison | Pass | Raw engine buffers, covering every channel the engine writes in a gated render, are compared bit for bit, with no tolerance or down-conversion. This is why TIFF was replaced. |
| Principle I: no tolerances, thresholds, or perceptual metrics | Pass | The cross-check fails only on exact functional facts, and it compares output exactly. It is not a parity comparison, so it never replaces the gate. |
| Principle II: adds randomness, timing, threading, or external state? | Pass | Nothing is added to the render path. The harness isolates home, locale, and environment, and it fails a render that falls back to a system data folder. |
| Principle II: Self-Parity | Pass | Three runs at each of two thread counts, one of them 1. Capture refuses without it. |
| Principle III: touches build flags, compilers, math libraries, or arithmetic order? | Pass | No flags change. The manifest only reads compiler and math library versions. |
| Principle III: equality between targets is not assumed | Pass | The cross-check claims equality only for items where every buffer and statistic matched exactly. |
| Reference Targets: a difference between targets is not drift | Pass | A toolchain difference with differing output is a Reference Target mismatch, not a parity failure. The cross-check records such differences and does not fail on them. |
| Standard Scene Suite: each stochastic CPU path | Pass | FR-039 lists the paths, and `research.md` gives a candidate scene for each. |
| Workflow 1: engine changes only to close a Known Deviation | Pass | The two application additions exist to close KD-4. Nothing else changes. |
| Workflow 2: the parity gate blocks merges | Pass | Cross-Build Check does not count as the gate (FR-054), and it adds no route around it. |
| Workflow 3: no regenerating references to clear a failure | Pass | Gate, self-parity, compare, collect, and cross-check modes cannot write references, and the pipeline has read-only access. |
| Workflow 4: refactors and output changes stay separate | Pass | This feature changes no output. |
| Workflow 6: `--test` remains required | Pass | Each pipeline build job runs it, which adds the first CI run of it on Windows. |
| Baseline Change rules | Pass | Capture runs only in CI, requires Self-Parity, and is all or nothing. No references are created here. |

**Known Deviations**: These are not reported as passing.

- **KD-1 (open)**: `--self-parity` is expected to fail on the current build, and the statistics spread suggests a second cause beyond the random seed. Both are for playbook Phase 3 to fix.
- **KD-2, KD-3 (open)**: This feature doesn't touch them. The CI flag check arrives in playbook Phase 3. Cross-Build Check builds with today's flags and does not run that check, because it would fail until then.
- **KD-4 (partly closed)**: This feature delivers the harness, the scene list, and Cross-Build Check. References and the CI parity gate arrive in playbook Phase 4.

**Expected effect on output**: None. The constitution requires each pre-baseline engine change to state this. The application changes run after the render or instead of it, and only when requested.

**Post-design re-check**: Pass. The design adds no violations. `--list-formulas` required a small update to FR-031, recorded in the spec. The cross-check and the pipeline add no C++.

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
│   ├── ci-pipeline.md
│   ├── collection.md
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
├── scenes/hdr_blur.fract       # Purpose-built scene for the HDR blur path
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

.github/
├── actions/
│   ├── build-linux/action.yml   # New: the qmake build
│   └── build-windows/action.yml # New: the MSVC build
└── workflows/
    ├── cross-build-check.yml    # New: the separate pipeline
    ├── github-actions-ci.yml    # Unchanged
    └── codeql.yml               # Unchanged

.gitignore                      # Adds tests/outputs/
```

**Structure Decision**: A single script, as requested, keeps the harness easy to run anywhere Python exists, and collect and cross-check are modes of that script, because they reuse its isolation, failure detection, and comparison code. New C++ goes in its own file pair, so the existing files change as little as possible. qmake, CMake, and the MSVC project all use `src/*.cpp` wildcards, so no build file changes are needed. The build commands live in two reusable actions, so the pipeline and the later `parity-gate` workflow share one definition.

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

**Cross-check flow**

1. Each build runs `--collect <dir>`, which renders every work item once with the gate's settings and writes a [collection](contracts/collection.md). Its zips use the reference format, and its `collection.txt` file is written last.
2. Either machine runs `--cross-check <first> <second>`, which reads both collections and starts neither build.
3. The check fails on functional disagreements (FR-044), reports setup failures (FR-046), and records every difference in output values (FR-045).

**Local development**: The Windows checkout is the working copy. Commits reach the WSL clone with `git fetch` from `/mnt/c`, so both builds use one commit, and the clone keeps LF line endings. The [playbook](../../MIGRATION_PLAYBOOK.md) lists the exact commands, and [research.md](research.md) has the evidence (R15).

**Pipeline**: [contracts/ci-pipeline.md](contracts/ci-pipeline.md) defines Cross-Build Check. The build steps come from what already works: `.appveyor.yml` for MSVC, and `github-actions-ci.yml` for qmake. Two build jobs run in parallel, and each builds the commit, runs the stand-in tests and `--test`, collects, and uploads its collection. A third job downloads both collections, runs `--cross-check`, and writes the report to the run summary. It needs no build, so it runs on Linux with only Python and the repository.

**Modes by environment**

| Mode | WSL, qmake build | Windows, MSVC build | GitHub Actions |
|---|---|---|---|
| Stand-in tests | Yes | Yes | Both build jobs |
| `--collect` | Yes | Yes, labeled not isolated | Both build jobs |
| `--cross-check` | Yes | Yes | Its own job |
| `--compare` | Yes | Yes, labeled not gate-valid | Not used |
| `--self-parity` | Yes | Runs, cannot pass | Inside `--capture` on Linux (playbook Phase 4) |
| Gate | Yes | Runs, cannot pass | `parity-gate` (playbook Phase 4) |
| `--capture` | Refused | Refused | `capture-baseline` on Linux only (playbook Phase 4) |

## Follow-ups Outside This Feature

- The playbook Phase 3 thread control must set the OpenMP thread count too, and the harness then needs its second thread count enabled.
- The hosted runner image changes over time. If updates start to cause false failures, a pinned container and pinned packages are the next step.

## Complexity Tracking

No constitution violations need justifying.
