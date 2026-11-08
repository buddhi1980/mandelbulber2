# Implementation Plan: Baseline 1 and the Parity Gate

**Branch**: `deterministic-regression`, with no feature branch | **Date**: 2026-11-08 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/004-baseline-parity-gate/spec.md`

## Summary

Make the gated renders independent of the runner's CPU, capture Baseline 1 in CI, turn on the gate, require it, prove it catches drift, and close KD-4 and KD-5. See [research.md](research.md) for the evidence behind each decision.

Seven steps, in order:

1. **Forced path**: The harness gives every Linux render `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`. A probe confirms it before every gate and capture run, and `settings.txt` records it. Local runs prove it, and it lands with the new capture workflow.
2. **Capture**: Pushing `baseline/1` starts capture on a hosted runner. Its references are checked and committed on that branch.
3. **Baseline 1**: The gate steps land by direct push, and the Baseline Change's pull request opens straight away. The maintainer merges it.
4. **Required check**: `parity-gate` becomes required on `deterministic-regression`, and the maintainer's direct pushes still land.
5. **Drift proof**: A pull request that changes one constant in `bristorbrot` fails the gate on that formula alone, and it never merges.
6. **Amendment**: The constitution moves to 1.1.1, with KD-4 and KD-5 closed, in a pull request of its own.
7. **Documents**: The playbook, `CLAUDE.md`, and `README.md` corrections land directly.

Pixels change only where the forced path gives other math results than glibc's FMA path. No references exist yet, so this is not drift, and Baseline 1 records it.

## Technical Context

**Language/Version**: Python 3.12 for the harness, GitHub Actions YAML, and Markdown. No C++ changes. The drift proof's one-line C++ change is never merged.

**Primary Dependencies**: glibc 2.39 and its tunables, the existing harness, the `compile-engine` and `build-linux` actions, `actions/checkout@v7`, `actions/upload-artifact@v7`, and `gh`. Nothing new is installed.

**Storage**: `tests/references/linux-x86_64-gcc/`, 489 files of about 83 MB, which arrive through their own pull request, and `specs/004-baseline-parity-gate/evidence.md`.

**Testing**:

- `tests/test_regression_runner.py`, with the stand-in renderer, on WSL and on Windows.
- Local runs of the probe, collect, and cross-check, and the drift proof's compare.
- In CI: the capture run, `parity-gate` on the Baseline 1 and drift proof pull requests and on pushes, and Cross-Build Check on each pull request.

**Target Platform**:

- **CI**: The hosted `ubuntu-24.04` runner, with 4 vCPUs, runs capture and the gate.
- **Local proof**: WSL `Ubuntu-24.04` and Windows, both on the i9-14900K.

**Project Type**: An existing desktop and command-line application. This feature changes its harness, its CI, its references, and its documents, not its engine.

**Performance Goals**:

- **Gate step**: Under 30 minutes on the 4-vCPU runner (SC-003). The Collect step, which also renders each item once, took 5 to 11 minutes.
- **Capture**: Inside its 350-minute limit, at about 2 hours.
- **Probe**: Under a second.

**Constraints**:

- **Files**: No change to the engine, its build files, the scene list, the formula sweep, or the render settings (FR-028). Only the two workflows change among CI files (FR-029).
- **Branches**: Direct pushes to `deterministic-regression`, apart from three pull requests, each from a branch deleted afterwards (FR-030).
- **Go-aheads**: Every push, every pull request, the protection change, and each merge need the maintainer's go-ahead. Auto-merge is never used.
- **Quiet computer**: Not needed, because every local check here measures output, not time.

**Scale/Scope**: 487 items, 489 reference files, 2 harness files, 2 workflows, 4 documents, 3 pull requests, and 3 short-lived branches.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Gate | Status | Notes |
|---|---|---|
| Principle I: can this change affect gated output, and which scenes cover it? | Yes, on purpose | The forced path changes some math results in their last bit, and fractal iteration can amplify that in what Linux renders produce. Baseline 1 records that output. No references exist before it, so this is not drift. All 487 items cover it. |
| Principle I: exact comparison | Pass | The gate compares every buffer and statistic exactly, with no tolerance (FR-012). |
| Principle I: expected output changes only through a Baseline Change | Pass | Baseline 1 is a Baseline Change (FR-015). |
| Principle II: does it add randomness, timing, threading, or external state? | No, it removes one | The CPU's features stop choosing glibc's math path for gated renders (FR-001). Capture runs Self-Parity at one thread and at four. |
| Principle II: a render is a pure function of the binary, scene, seeds, and resolution | KD-5, closed by this feature | The harness fixes the math path for every Linux render, so the runner's CPU stops mattering. KD-5 isn't reported as passing until the amendment closes it. |
| Principle III: does it touch build flags, compilers, math libraries, or arithmetic order? | The math library's path, yes. Flags, no | A run-time setting picks one of glibc's code paths. No build flag changes, and the build-file search and the flag check run in both workflows through `compile-engine`. |
| Reference Targets: the First Reference Target's compiler and math library versions are recorded with Baseline 1 | Pass | `toolchain.txt` holds them (FR-009). |
| Baseline Change rules | Pass | Baseline 1 has its own pull request, which holds only references and states why output changes. Its statement that no earlier references exist replaces the before-and-after evidence. It comes from a CI build that passes Self-Parity, and the maintainer approves it (FR-015 to FR-018). |
| Workflow 1: before Baseline 1, engine changes only to close a Known Deviation | Pass | No engine change lands. The harness change closes KD-5, and the drift proof's seed is never merged. An engine fix that capture turns out to need lands as its own reviewed direct push (FR-010, R13). |
| Workflow 2: after Baseline 1, every pull request passes the gate before it merges | Pass | The required check blocks a merge until `parity-gate` passes (FR-019). A direct push isn't a pull request. Its push run checks it after it lands, and a failure is a defect to fix at once (FR-021). |
| Workflow 3: no regenerating references to clear a failure | Pass | FR-021. |
| Workflow 4: refactors and output changes stay apart | Pass | Nothing here is a refactor. |
| Workflow 6: `--test` remains required | Pass | C++ CI Build (Qt6) and Cross-Build Check still run it. |
| Governance: amendments | Planned | One PATCH amendment, version 1.1.1, closes KD-4 and KD-5 in its own pull request. It states the reason, updates the dates, and names the affected plans and gates, and the maintainer approves it (FR-024). |

**Known Deviations**: These are not reported as passing.

- **KD-1, KD-2, and KD-3 (closed)**: They closed in 1.0.1 and 1.1.0. Capture's Self-Parity proves KD-1 again, on the hosted runner, at one thread and at four.
- **KD-4 (open)**: It closes when Baseline 1 merges, and the amendment records that.
- **KD-5 (open)**: It closes when the forced path lands and Baseline 1 records it, and the amendment records that.

**Expected effect on output**: On Linux, every render the harness runs takes glibc's plain math path. On 2026-11-08, 648 of 1.8 million sample results differed from the FMA path this computer takes by itself, and each of the 16 printed differed only in its last bit. Fractal iteration can amplify that, so FR-005 measures what it means for the 487 items. Outside the harness, the binary renders exactly as before, and MSVC output doesn't change.

**Post-design re-check**: Pass. The design adds no violations. The probe and the settings line confirm and record the forced path without touching the engine. The required check lets the maintainer's direct pushes through, which Workflow 2 permits, because it governs pull requests. FR-021 covers what that leaves open. Research changed nothing in the spec. The review checklist's fixes then added recovery paths, records, and clearer terms to the spec and this plan, with no new violation.

## Project Structure

### Documentation (this feature)

```text
specs/004-baseline-parity-gate/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── math-path.md
│   ├── capture-baseline.md
│   ├── parity-gate.md
│   ├── baseline-change.md
│   └── evidence-record.md
├── checklists/
│   └── requirements.md
├── evidence.md          # Created during implementation
└── tasks.md             # Created by /speckit-tasks
```

### Source Code (repository root)

```text
tests/
├── regression_runner.py        # The forced path, its probe, and the settings line
├── test_regression_runner.py   # Tests for all three
└── references/
    └── linux-x86_64-gcc/       # Baseline 1, through its own pull request

.github/workflows/
├── parity-gate.yml             # The gate steps
└── capture-baseline.yml        # New

.specify/memory/constitution.md # KD-4 and KD-5 closed at 1.1.1, through its own pull request
MIGRATION_PLAYBOOK.md           # Phase 4 and Phase 5 corrections
CLAUDE.md                       # The required check
README.md                       # The regression section
```

**Structure Decision**: The forced path lives in the harness, because the harness already builds every render's environment from scratch. So one change covers every mode and every computer, and no workflow can bypass it. Everything else edits an existing file by a few lines, apart from the new workflow and the reference set.

## Design Overview

### 1. The forced path

[math-path.md](contracts/math-path.md) is the contract. First, save the repository's settings, protected branches, and rulesets, as [parity-gate.md](contracts/parity-gate.md) says, so FR-022's comparison has a starting point.

1. Add the setting to `clean_environment()` on Linux, the probe to the gate and capture, and the `math_path` line to `settings_lines()`.
2. Add the tests the contract lists. The shared stand-in setup replaces the probe with one that passes, so existing tests stay as they are.
3. Prove it locally on a fresh build: the harness's tests on WSL and Windows, the probe and its two failing scratch copies, the forced and hidden collections that must match, the natural collection that shows the cost, and collect and cross-check across WSL and MSVC (R11, FR-004).

### 2. Capture

[capture-baseline.md](contracts/capture-baseline.md) is the contract.

1. Add `capture-baseline.yml`. With the go-ahead, step 1 and this workflow land in one direct push, state S1, and Parity Gate and C++ CI Build (Qt6) pass on it.
2. With the go-ahead, push `baseline/1` at S1, and confirm that capture started.
3. When it passes, check the artifact, commit it on `baseline/1`, compare the blob IDs, and push the branch.

### 3. Baseline 1

[baseline-change.md](contracts/baseline-change.md) is the contract.

1. When the maintainer can review and merge in the same sitting, and with the go-ahead, push the gate steps to `deterministic-regression`, state S2. The push run fails for lack of references.
2. Open the pull request at once. `parity-gate` passes on 487 of 487 items, and Cross-Build Check passes.
3. The maintainer merges it, state S3. The push run passes, and `baseline/1` is deleted on GitHub.
4. Record the window, the times, and each run's CPU line.

If the pull request's gate fails, the gate steps are reverted at once, the branch returns to S1, and capture repeats once the cause is known (FR-013).

### 4. The required check

[parity-gate.md](contracts/parity-gate.md) is the contract.

1. With the go-ahead, set the protection and read it back, state S4.
2. The next direct push, the evidence commit, must land, and its output names the bypassed rule.

### 5. The drift proof

1. Run the local compare, which must find `formula bristorbrot` alone.
2. With the go-ahead, push `seed/drift-proof` and open its pull request. The gate fails on that formula alone, and the merge shows as blocked.
3. Close the pull request, and delete the branch on GitHub.

### 6. The amendment

With the go-ahead, open a pull request from `amend/kd-4-kd-5` that edits only the constitution. `parity-gate` passes on it. The maintainer merges it, state S5, and the branch is deleted on GitHub.

### 7. Documents

Push the corrections that R12 lists, state S6. Then run the final checks in [quickstart.md](quickstart.md), and fill in the success criteria table.

If any step fails, its cause joins this work, and the steps after it wait. A capture that fails Self-Parity stops the feature until its fix lands as its own reviewed change (FR-010).

**Commits**: Every commit lands on `deterministic-regression` by direct push, except three. The references commit is on `baseline/1`, the seed on `seed/drift-proof`, and the amendment on `amend/kd-4-kd-5`.

**Modes by environment**

| Check | WSL, qmake build | Windows, MSVC build | GitHub Actions |
|---|---|---|---|
| The harness's tests | Yes | Yes | Cross-Build Check, capture, and `parity-gate` |
| The probe | Yes, through a gate run with no references | Not run: the gate runs there only for debugging, says the path isn't forced, and can't pass | Capture and `parity-gate` |
| Forced, hidden, and natural collections | Yes | Not needed | Not used |
| Collect and cross-check | Collect | Collect and cross-check | Cross-Build Check on each pull request |
| Capture | Refuses, because it isn't CI | Refuses | `capture-baseline` on the push of `baseline/1` |
| The gate | Setup error, because no references exist | Refuses | `parity-gate` |
| The drift proof's compare | Yes | Not needed | The drift proof's pull request |

## Follow-ups Outside This Feature

- **Old branches**: GitHub still holds branches of earlier, merged pull requests, such as `upgrade/regression-harness` and `upgrade/deterministic-rendering`. SC-009 covers only this feature's branches.
- **A new glibc**: The probe's values belong to glibc 2.39. A runner image with another glibc fails the probe until they are found again, and that toolchain needs a Baseline Change anyway.
- **The repository's size**: Each reference set adds about 83 MB to the history. Git LFS or another store is a later decision.
- **Other builds' math**: MSVC and macOS renders don't take a forced path. They have no references, and Cross-Build Check compares only their agreement.
- **Phase 5**: Engine refactors can begin once the amendment merges.

## Complexity Tracking

No constitution violations need justifying.
