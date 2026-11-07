# Implementation Plan: Strict Floating-Point Builds

**Branch**: `upgrade/strict-floating-point` | **Date**: 2026-11-07 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/003-strict-floating-point/spec.md`

## Summary

Stop the compiler from changing floating-point results in any build, and stop any script from tuning a build to its computer, so KD-2 and KD-3 close. See [research.md](research.md) for the evidence behind each decision.

Six steps, in order:

1. **Before state**: Fresh before builds, their collections, and the first look at whether MSVC Debug compiles today.
2. **Flags**: `-ffast-math` leaves qmake and CMake, a `gcc` scope adds `-ffp-contract=off`, MSVC Debug moves to `/fp:precise`, and the scripts lose `-march=native`.
3. **The check**: A new `compile-engine` action and `parity-gate` workflow run one script that searches the build files, then reads the gated build's real flags.
4. **Local proof**: Seeded failures prove the check on this computer.
5. **Measurement**: Self-Parity, the output record, the guard counts, the image review, the render times, and the other builds.
6. **Closing**: The pull request and seeded failures in GitHub Actions, then the constitution moves to 1.1.0 with KD-2 and KD-3 closed and the CPU math path recorded as KD-5, and the playbook is corrected, with a new Phase 4 step for that path.

Every item's pixels may change. The compiler stops reordering arithmetic and assuming finite values, four vector math functions give way to the ordinary math library, and NaN guards that are compiled away today start to act. No references exist yet, so this is not drift. Baseline 1 follows in playbook Phase 4.

## Technical Context

**Language/Version**: qmake project files, CMake, MSBuild project XML, POSIX shell, and GitHub Actions YAML. No C++ changes unless FR-018 requires one. The check is `bash`, with `git` and `grep`.

**Primary Dependencies**: The compilers in use: GCC 13.3.0 and Qt 6.4.2 in WSL and on the CI runner, MSVC 14.51 on Windows, and LLVM Clang on CircleCI. Nothing new.

**Storage**: `specs/003-strict-floating-point/evidence.md` and `output-record.tsv`. No references are created.

**Testing**: Seeded failures, `--self-parity`, `--compare`, `--collect`, `--test`, `--cross-check`, and the checks in [quickstart.md](quickstart.md). Scratch scripts stay in `~/verify/kd2` and out of the repository.

**Target Platform**: The local builds and hosted runners from `specs/001-regression-harness`: qmake in WSL `Ubuntu-24.04`, MSVC on Windows, Cross-Build Check, and the new `parity-gate`. CircleCI proves macOS. CMake and `mandelbulber.pro` must still compile (FR-033).

**Project Type**: An existing desktop and command-line application. This feature changes its build and CI, not its source.

**Performance Goals**: A full gate run under 30 minutes on a 4-vCPU runner (SC-008). The Collect step that stands in for it took 4.4 minutes on the last KD-1 pull request. The cost of this change is unknown until measured, and it is recorded with no other limit. `parity-gate` should take about 7 minutes.

**Constraints**: No engine source changes, unless FR-018 requires one. No harness changes (FR-034). No change to any existing workflow or CI file (FR-016). No references. No option kept from `-ffast-math` (FR-031). Timing needs a quiet computer, and FractalEvaluator must never be stopped. Every push needs the user's go-ahead.

**Scale/Scope**: 6 build and script files change, and 3 CI files are new. The check reads 7 Makefile lines and 72 tracked files today. Measurement covers 487 items, seven buffers, and 4 guard sites.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Gate | Status | Notes |
|---|---|---|
| Principle I: can this change affect gated output, and which scenes cover it? | Yes, on purpose | Every item may change. All 487 items cover it. This is not drift, because no references exist (KD-4). The change is allowed because it closes KD-2 and KD-3, and Baseline 1 comes after. |
| Principle I: exact comparison | Pass | The harness doesn't change (FR-034). The 20-formula review limit picks images for a person, and it is not a parity comparison. |
| Principle II: does it add randomness, timing, threading, or external state? | No | The flags change arithmetic rules only. Self-Parity runs three full passes on the new build (SC-005). Build-time environment flags are not state a render reads. |
| Principle II: a render is a pure function of the binary, scene, seeds, and resolution | Known Deviation, KD-5 | glibc picks a math code path from the CPU at run time, so one binary can differ between CPUs with different features. This feature does not change that. Governance permits non-compliance only as a Known Deviation, so the amendment records it as KD-5 (FR-028), and FR-029 adds a Phase 4 step to settle it before Baseline 1. This plan does not report it as passing. |
| Principle III: does it touch build flags, compilers, math libraries, or arithmetic order? | Yes, this is the feature | [build-flags.md](contracts/build-flags.md) lists every change. |
| Principle III: prohibited options | Pass after this feature | The search and the flag check enforce them (FR-012, FR-035). This plan does not report them as passing until SC-001 and SC-004 do. |
| Principle III: `-ffp-contract=off`, and MSVC `/fp:precise` with `/fp:contract` unset | Pass after this feature | FR-003, FR-004. CircleCI is the only Clang proof, and nothing builds the Apple Silicon profile. |
| Principle III: no pragmas or attributes in engine code | Pass | A search on 2026-11-07 found none (FR-005). The six `diagnostic` pragmas in `radiance_hdr.cpp` affect warnings. |
| Principle III: no host-tuned options, and a fixed instruction set | Pass after this feature | The gated build uses the plain x86-64 target, and it sets neither `-march` nor `-mtune`. |
| Principle III: CI checks effective flags, including inherited ones | This feature | The check reads the generated Makefile, and seeds prove all four environment variables (FR-011, FR-015). Only the gated build's flags are read. Other builds get the search. |
| Reference Targets | Pass | A difference between builds stays recorded and is not drift. SC-009 records equality only where the harness shows it. |
| Workflow 1: engine changes only to close a Known Deviation | Pass | Build files are part of the gated engine, and every change serves KD-2 or KD-3. FR-031 keeps out anything else. It lands as one reviewed pull request that states its expected effect. |
| Workflow 2: the parity gate blocks merges | Not active | `parity-gate` arrives here, but only checks flags. Phase 4 adds the gate and makes it required. |
| Workflow 3: no regenerating references | Pass | This feature creates none (FR-034). |
| Workflow 4: refactors and output changes stay separate | Pass | Nothing here is a refactor. |
| Workflow 6: `--test` remains required | Pass | It runs on both builds. |
| Baseline Change rules | Pass | No references change. |
| Governance: amendments | Planned | The pull request closes KD-2 and KD-3 and adds KD-5 as one MINOR amendment to 1.1.0, because PATCH doesn't cover a new deviation. It states the reason, updates the dates, and names the affected plans and gates. A maintainer approves it. |

**Known Deviations**: These are not reported as passing.

- **KD-1 (closed)**: Closed in 1.0.1. Self-Parity must still pass on the new build, because the KD-1 proof came from a build with `-ffast-math`.
- **KD-2 (open)**: This feature closes it. It stays open until the CI flag check passes on the gated build.
- **KD-3 (open)**: This feature closes it. It stays open until the CI flag check rejects a host-tuned flag on a gated build in GitHub Actions. qmake keeps its environment import, because KD-3 closes on the check.
- **KD-4 (partly closed)**: Baseline 1 and the gate arrive in playbook Phase 4. This feature adds `parity-gate`, and creates no references.
- **KD-5 (new)**: This feature records it and doesn't close it. It stays open until Phase 4 settles the math path before Baseline 1 is captured.

**Expected effect on output** (FR-022): The compiler stops reordering arithmetic and assuming finite values, so every item's pixels and statistics may change. `ssao_worker` moves from vector `sin` and `cos` to the ordinary math library. Images should look the same apart from small differences, except where a NaN guard now acts. An orbit that overflows to infinity now keeps its last finite value, as it does on MSVC Release, so its distance estimate changes, and it counts as inside on the 2 items that use iteration threshold mode. `-ffp-contract=off` has no effect on the gated build, and MSVC Release output does not change.

**Post-design re-check**: Pass, with KD-5 recorded by amendment. The design adds no violations. The math path is not a result of this feature, and it stays open until Phase 4 settles it. Research corrected the spec, and the changes are listed at the end of [research.md](research.md). None of them weakens a requirement.

## Project Structure

### Documentation (this feature)

```text
specs/003-strict-floating-point/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── build-flags.md
│   ├── evidence-record.md
│   ├── flag-check.md
│   └── parity-gate.md
├── checklists/
│   └── requirements.md
├── evidence.md          # Created during implementation
├── output-record.tsv    # Created during implementation
└── tasks.md             # Created by /speckit-tasks
```

### Source Code (repository root)

```text
mandelbulber2/
├── qmake/common.pri                       # -ffast-math out, the gcc block in
├── cmake/CMakeLists.txt                   # -ffast-math out, -ffp-contract=off in
├── msvc/mandelbulber2.vcxproj             # Debug x64 uses Precise
├── tools/prepare_for_dev_ubuntu.sh        # -march=native out, -msse2 stays
├── tools/prepare_for_dev_debian_testing.sh # -march=native out, -msse2 stays
└── deploy/linux/install                   # The export line out

.github/
├── actions/compile-engine/action.yml      # New: search, build, read the flags
├── actions/compile-engine/check-flags.sh  # New: the only list of options
└── workflows/parity-gate.yml              # New

.specify/memory/constitution.md            # KD-2 and KD-3 closed, KD-5 added, version 1.1.0
MIGRATION_PLAYBOOK.md                      # Corrections, and a Phase 4 step
```

**Structure Decision**: The check is one script beside the action, so the seeds run the file CI runs, and it stays out of `tests/`, so the harness doesn't change. Everything else edits an existing file by a few lines.

## Design Overview

### 1. Before state

1. Build MSVC Debug x64 fresh on `8fd49ba59`. Nothing has shown that it compiles today (R14). If it fails, FR-033 doesn't bind it, and the evidence says so.
2. Build the before state fresh, with `build_fresh.sh` in WSL, and in a clean MSVC clone.
3. Run `--collect` on the MSVC build. The WSL collection comes from the timing runs in step 5.

### 2. Flags

[build-flags.md](contracts/build-flags.md) is the contract.

1. One commit changes `common.pri`, `CMakeLists.txt`, and the MSVC project, so no commit has a half-strict build.
2. A second commit changes the two setup scripts and `install`.

After step 2, the final commit exists, unless FR-018 moves it.

### 3. The check

[flag-check.md](contracts/flag-check.md) and [parity-gate.md](contracts/parity-gate.md) are the contracts.

1. Add `check-flags.sh`, with `search` and `flags <Makefile>`.
2. Add `compile-engine/action.yml`: search, build through `build-linux`, then read the flags.
3. Add `parity-gate.yml`, with `permissions: contents: read`.

The repository's default workflow token is `write`, so that setting matters (R17).

### 4. Local proof

Run every local seed in [flag-check.md](contracts/flag-check.md) with the committed script, and record them.

### 5. Measurement

[evidence-record.md](contracts/evidence-record.md) is the contract. Output work can run while FractalEvaluator runs. Timing can't. All of this comes before the first push, because a fix would move the final commit and repeat every CI run.

1. **Self-Parity**: Three full passes on a fresh build of the final commit (SC-005). A failure goes to FR-018.
2. **Output**: `--collect` on both builds, `--compare`, then the scratch comparison and `output-record.tsv` (R10).
3. **Guards**: One throwaway patch counts each site, and `--compare` shows it changes nothing. A second turns the guards off, and `--compare` shows exactly what they change (R11).
4. **Contraction**: A throwaway patch removes `-ffp-contract=off`, and `--compare` shows the option changes nothing on the gated build (R19).
5. **Review**: A person reviews the 26 scenes and 20 formulas, and the evidence names each visible change's cause (R12).
6. **Time**: The timed `--collect` in both orders, then on one CPU, then on the host-tuned build, with nothing else from this feature running (R9).
7. **Other builds**: Each row of R14, and `--test` on qmake and MSVC.
8. **MSVC**: The final commit's MSVC collection against the before one finds all 487 identical, and `--cross-check` against the qmake collection gives SC-009's line (R13).

If anything in this step fails, the cause joins this work (FR-018), the final commit moves, and the checks from that commit repeat. A visible change from a guard isn't a failure, once its cause is recorded (FR-023).

### 6. Closing

1. With the user's go-ahead, push the branch and open a draft pull request. It states the expected effect on output, then what was measured (FR-022). `parity-gate` passes (SC-001).
2. With the user's go-ahead, run the two seeded GitHub Actions runs on a throwaway branch (R8), then close and delete it, and cancel the other runs it started.
3. Read the Collect step on the pull request (SC-008), and confirm Cross-Build Check and CircleCI pass (SC-009, SC-010).
4. Mark KD-2 and KD-3 closed in the constitution and add KD-5, at version 1.1.0 (FR-028). This comes after `parity-gate` passes and the seeds are recorded.
5. Correct the playbook, as R16 lists, including the new Phase 4 step (FR-029).
6. With the user's go-ahead, push the closing commit, and mark the pull request ready.

**Commits**: One branch and one pull request into `ministic-regression`. CI files and documents can't change a binary, so they don't move the final commit.

**Modes by environment**

| Check | WSL, qmake build | Windows, MSVC build | GitHub Actions |
|---|---|---|---|
| `check-flags.sh`, seeds | Yes | Not needed | Two seeded runs, and `parity-gate` on the pull request |
| `--self-parity` | Passes (SC-005) | Not needed, because its flags and sources don't change | Not used until Phase 4 |
| `--compare`, before against after | Yes | Not needed | Not used |
| `--collect` | Yes, and it gives the timing | Yes, before and after | Cross-Build Check |
| `--test` | Yes | Yes | Both build jobs |
| `--cross-check`, qmake against MSVC | Local | Local | On the pull request |
| Image review | Before and after images | The before image, where a shape changes | Not used |

## Follow-ups Outside This Feature

- **CPU math path**: KD-5 records it, and FR-029 adds the step. Phase 4 decides what the gate does about it before capture, and a Reference Target that names CPU features would be a constitution amendment.
- **Winning back speed**: `-fno-math-errno` and similar options leave values alone, so each can follow Baseline 1 as an ordinary change the gate proves.
- **The overflow guard**: If review shows the first guard handles an overflow wrongly, a Baseline Change can fix it after Baseline 1. That fix changes MSVC output too.
- **Other builds' real flags**: The search guards tracked files only. An option from the environment on MSVC, macOS, or CMake stays unseen.
- **The Apple Silicon profile**: Nothing builds `mandelbulber-opencl-m1.pro`, so its flags are unproven.
- **Stale paths**: CMake's Intel branch, the `tools/DOE` build scripts, and `mandelbulber-opencl-win64.pro` stay as they are.

## Complexity Tracking

No constitution violations need justifying.
