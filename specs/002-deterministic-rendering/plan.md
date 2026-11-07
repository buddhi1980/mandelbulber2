# Implementation Plan: Deterministic CPU Rendering

**Branch**: `upgrade/deterministic-rendering` | **Date**: 2026-11-06 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/002-deterministic-rendering/spec.md`

## Summary

Make every CPU render a pure function of the binary, the scene, the seed, and the resolution, so `--self-parity` passes and KD-1 closes. See [research.md](research.md) for the evidence behind each decision.

Six changes, in order:

1. **Thread control**: `--threads N` sets the render workers, and a `num_threads` clause carries it into every OpenMP region, because `omp_set_num_threads()` doesn't reach later threads.
2. **Evidence**: Fresh builds show why renders differ before anything is replaced, with one diagnostic build that holds random values constant.
3. **Random values**: A new header-only generator, `cPositionRandom`, replaces the shared `gRandomSeed`. Each value comes from the seed, the frame, and its position, using only 64-bit integer arithmetic.
4. **Order-dependent stages**: The depth of field blend moves to one thread, in the order a one-thread run uses today.
5. **Exact statistics**: Each worker counts on its own and adds its counts under a lock at the end of each line. A line has one owner, and total iterations reports the real count.
6. **Closing**: Self-Parity passes on all 487 items, the constitution moves to 1.0.1 with KD-1 closed, and the playbook is corrected.

Every scene's pixels change, because raymarch step jitter applies to every scene. No references exist yet, so this is not drift. Baseline 1 follows in playbook Phase 4.

## Technical Context

**Language/Version**: C++17 for the application, because Qt 6 headers reject anything older. Python 3.10 or later for the one harness change.

**Primary Dependencies**: Qt Core and OpenMP, both already in use. Nothing new. The generator is a single header with no dependencies beyond `<cstdint>`.

**Storage**: One new scene setting, `render_random_seed`. Evidence goes in `specs/002-deterministic-rendering/evidence.md`. No references are created.

**Testing**: A new known-answer test in the application's `--test` suite, the harness's own tests, `--self-parity`, `--compare`, and the checks in [quickstart.md](quickstart.md).

**Target Platform**: The two local builds and hosted runners from `specs/001-regression-harness`: qmake in WSL `Ubuntu-24.04`, MSVC Release on Windows, and Cross-Build Check on GitHub Actions. Self-Parity can only report a pass on Linux. CMake and non-OpenCL builds must still compile (FR-037).

**Project Type**: An existing desktop and command-line application.

**Performance Goals**: A full gate run under 30 minutes on a 4-vCPU runner (SC-014). Render time should not get worse overall. One random draw takes 1.09 ns, against 2.72 ns today, and removing shared counters and the shared seed from the hottest path should speed up all-thread renders. Both are measured and recorded (FR-039).

**Constraints**: No new compile flags, because KD-2 and KD-3 belong to `upgrade/strict-floating-point`. No workflow changes (FR-038). No references. No GUI control for the seed. The statistics work must not change a buffer on a suite item (FR-028). Diagnostic changes are never committed (FR-011).

**Scale/Scope**: 487 items and seven buffers. 27 random calls in the gated path and 14 outside it. About 45 C++ files change, most of them by a few lines.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Gate | Status | Notes |
|---|---|---|
| Principle I: can this change affect gated output, and which scenes cover it? | Yes, on purpose | Every scene's pixels change, and total iterations changes from 0 to the real count. All 487 items cover it. This is not drift, because no references exist (KD-4). The change is allowed because it closes KD-1, and Baseline 1 comes after. |
| Principle I: exact comparison | Pass | The harness comparison doesn't change (FR-031). A seeded one-bit difference must still fail (SC-011). The 5% noise limit is a one-time quality check, not a parity comparison. |
| Principle II: does it add randomness, timing, threading, or external state? | It removes them | Random values depend only on the seed, frame, eye, and position. Thread identity, line order, time, and addresses can't reach them (FR-014). The shared seed is gone (SC-015). `OMP_NUM_THREADS` can't override `--threads`, and the harness strips it from the environment anyway. |
| Principle II: every random value derives from a fixed seed | Needs the amendment | The seed is a scene setting, but no suite scene names it, so its value comes from the default in the binary. The principle says a seed fixed in the scene file or the harness, and its words don't cover a default. The closing amendment clarifies that a seed setting a scene file doesn't name counts as fixed in the scene file at its default, so the feature can't merge without it. The frame number comes from the animation, and a still render uses 0. |
| Principle II: Self-Parity | This feature's success criterion | Three runs at each of two thread counts, one of them 1, in three full passes (SC-001). This plan does not report it as passing. |
| Principle III: does it touch build flags, compilers, math libraries, or arithmetic order? | No flags or libraries | The generator is integer-only, so it can't bring in a floating-point difference (FR-018). The blend's order becomes fixed: it is the order one thread already uses. |
| Reference Targets | Pass | A difference between builds stays recorded and is not drift. Identical random values on both builds narrow what the cross-check can report. |
| Workflow 1: engine changes only to close a Known Deviation | Pass | Every engine change serves KD-1. It lands as one reviewed pull request that states its expected effect. |
| Workflow 2: the parity gate blocks merges | Not active | `parity-gate` arrives in playbook Phase 4. |
| Workflow 3: no regenerating references | Pass | This feature creates none (FR-033). |
| Workflow 4: refactors and output changes stay separate | Pass | Nothing here is a refactor. Each change fixes a behavior KD-1 needs fixed, and separate commits let SC-004 prove the statistics work leaves buffers alone. |
| Workflow 6: `--test` remains required | Pass | It runs on both builds, with one new test. |
| Baseline Change rules | Pass | No references change. |
| Governance: amendments | Planned | The pull request closes KD-1 and clarifies the seed rule as one PATCH amendment to 1.0.1. It states the reason for both, updates the dates, and names the affected plans and gates. A maintainer approves it. |

**Known Deviations**: These are not reported as passing.

- **KD-1 (open)**: This feature closes it. It stays open until Self-Parity passes on the Standard Scene Suite.
- **KD-2, KD-3 (open)**: This feature doesn't touch them. Self-Parity compares one binary with itself, so compile flags don't block it.
- **KD-4 (partly closed)**: Baseline 1 and the gate arrive in playbook Phase 4. This feature makes `--capture` possible, and creates no references.

**Expected effect on output** (FR-035): Every scene's pixels change, because raymarch step jitter applies to every scene. Images should look the same apart from noise. Scenes with post-effect depth of field reproduce the order of a one-thread run. Total iterations changes from 0 to the real count.

**Post-design re-check**: Pass, with the Principle II amendment above. The design adds no other violations. Research corrected the spec, and the changes are listed at the end of [research.md](research.md). None of them weakens a requirement.

## Project Structure

### Documentation (this feature)

```text
specs/002-deterministic-rendering/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── application-cli.md
│   ├── evidence-record.md
│   └── random-values.md
├── checklists/
│   └── requirements.md
├── evidence.md          # Created during implementation
└── tasks.md             # Created by /speckit-tasks
```

### Source Code (repository root)

```text
mandelbulber2/src/
├── position_random.hpp             # New: the generator
├── common_math.h, .cpp             # gRandomSeed, RandomInt, and Random removed
├── command_line_interface.hpp, .cpp # --threads, its checks, and removal before QTest
├── system_data.hpp, .cpp           # threadsFromCommandLine, GetOpenMpThreads
├── queue.cpp                       # --threads beats limit_CPU_cores
├── initparameters.cpp              # render_random_seed
├── fractparams.hpp, .cpp           # renderRandomSeed
├── stereo.h                        # GetForcedEye becomes const
├── render_worker.hpp, .cpp         # Own generator and own statistics
├── light.h, .cpp                   # Generator passed in
├── shader_ambient_occlusion.cpp    # Own generator and own statistics
├── shader_aux_shadow.cpp           # Own generator and own statistics
├── shader_calculate_normals.cpp    # Own statistics
├── shader_fast_ambient_occlusion.cpp # Own statistics
├── shader_global_illumination.cpp  # Own generator
├── shader_light_shading.cpp        # Passes the generator to the light
├── shader_specular_highlight.cpp   # Own generator
├── shader_volumetric.cpp           # Own generator, and passes it to the light
├── ssao_worker.cpp                 # Local generator, with a position per pixel
├── dof.hpp, .cpp                   # Generator per entry, thread clause, blend on one thread
├── render_image.cpp                # Builds the generator, copies statistics under the lock
├── render_job.cpp                  # Builds the generator for a post-effects refresh
├── opencl_engine_render_dof.cpp    # Thread clause and local generators
├── post_effect_hdr_blur.cpp        # Thread clause
├── voxel_export.cpp                # Thread clause, buffers sized to match
├── marchingcubes.cpp               # Thread clause, buffers sized to match
├── audio_track.cpp                 # Thread clause
├── scheduler.cpp                   # InitFirstLine claims under the lock
├── statistics.h, .cpp              # Add, and Reset clears totalNoise
├── render_data.hpp                 # statisticsMutex
├── calculate_distance.hpp, .cpp    # Iteration count
├── interface.cpp                   # Local generator in the view reset
└── test.hpp, .cpp                  # Known-answer test

mandelbulber2/qt/
├── gradient_edit_widget.cpp        # Local cRandom
└── thumbnail_widget.h, .cpp        # Generator per widget

tests/
├── regression_runner.py            # THREAD_OPTION = "--threads"
└── test_regression_runner.py       # Keeps covering the case with no thread control

.specify/memory/constitution.md     # KD-1 closed, version 1.0.1
MIGRATION_PLAYBOOK.md               # Statements this feature makes out of date
```

**Structure Decision**: The generator is one new header, because `Random` sits on the hottest path and must inline. Everything else changes existing files as little as possible. qmake, CMake, and the MSVC project all use `src/*.cpp` wildcards, and a header needs no build file change.

## Design Overview

### 1. Thread control

[application-cli.md](contracts/application-cli.md) is the contract.

1. The `cCommandLineInterface` constructor reads `--threads`, right after it reads the other options.
2. It refuses a bad value, and it refuses the option in the GUI, with `--gpu` or `--gpuall`, and with options that render nothing. That check has to run there, before `QApplication` exists, or a refusal in the GUI would open a message box that waits for a click.
3. It sets `systemData.numberOfThreads` and `systemData.threadsFromCommandLine`.
4. `cQueue::RenderQueue` leaves `numberOfThreads` alone when the option was used.
5. Every OpenMP region gets `num_threads(systemData.GetOpenMpThreads())`, and voxel and mesh export size their per-thread buffers with the same value.
6. `tests/regression_runner.py` sets `THREAD_OPTION = "--threads"`, and the harness's tests patch it back to `None` where they cover the case with no thread control (FR-031).

### 2. Evidence

[evidence-record.md](contracts/evidence-record.md) is the contract. Three fresh builds, 13 scenes, every buffer and statistic. The result is `evidence.md`, committed before step 3. If it shows a cause this plan doesn't cover, that cause joins the work (FR-010), and the shared seed is still replaced.

### 3. Random values

[random-values.md](contracts/random-values.md) is the contract, and it includes known answers.

1. Add `position_random.hpp`, the `render_random_seed` setting, and `sParamRender::renderRandomSeed`.
2. `cRenderWorker::doWork` builds its generator, then sets the position at the top of the sample loop.
3. The worker and its shaders call `random.Random(max)`. `cLight` takes a pointer to the generator.
4. `cSSAOWorker::doWork` sets a position per pixel. `cPostRenderingDOF::Render` takes a generator from its caller, and sets the pass and entry per entry of the shuffle.
5. The callers outside the render get their own local state.
6. Remove `gRandomSeed`, `RandomInt()`, and `Random()`. Any call left behind stops the build, so the compiler checks FR-013 and FR-015.
7. Add the known-answer test, which runs on both builds (FR-018).

### 4. Order-dependent stages

Remove the `pragma` on depth of field phase II. Record each region's verdict in `evidence.md` once Self-Parity can prove it (FR-021).

### 5. Exact statistics

1. `cStatistics` gains `Add`, and `Reset` clears `totalNoise`.
2. `sRenderData` gains `statisticsMutex`.
3. `cScheduler::InitFirstLine` takes the lock and claims only a free line.
4. `cRenderWorker` gets a `mutable cStatistics statistics`, sizes its histograms in `doWork`, and uses it wherever the worker and its shaders used `data->statistics`.
5. At the end of each line, the worker adds its counts under the lock and clears them. If it gave up on the line, it only clears them.
6. `cRenderer` copies the shared object under the lock before it emits `updateStatistics`.
7. `sDistanceOut` starts `iters` and `totalIters` at 0, and the object tree adds each node's `totalIters` (FR-026). On the limit box exit, `iters` changes from an old stack value to 0, the one case outside the suite in FR-028.

The verification export doesn't change (FR-030).

### 6. Closing

1. Run the checks in [quickstart.md](quickstart.md), on fresh builds of the final commit (FR-032). The evidence builds in section 2 and the comparison around the statistics work in section 4 use the commits those checks name.
2. Record the time and noise results in `evidence.md`.
3. Mark KD-1 closed in the constitution and add the seed clarification, at version 1.0.1 (FR-034).
4. Correct the playbook: the Phase 3 text about enabling the thread count and filling in `total_iterations`, and the Phase 4 line that removes KD-1 through KD-4.
5. State the expected effect on output in the pull request (FR-035), along with any noise drop above 5% (SC-010).

**Commits**: One branch and one pull request, with each step above in its own commit, so SC-004 can compare builds on either side of step 5.

**Modes by environment**

| Check | WSL, qmake build | Windows, MSVC build | GitHub Actions |
|---|---|---|---|
| `--self-parity` | Passes (SC-001) | Runs, reports 0 differences (SC-013) | Not used until Phase 4 |
| Other thread counts (SC-002) | Yes | Not needed | Not used |
| `--compare` around the statistics work (SC-004) | Yes | Not needed | Not used |
| `--test`, with the known-answer test | Yes | Yes | Both build jobs |
| Harness tests | Yes | Yes | Both build jobs |
| Cross-Build Check | Local `--collect` and `--cross-check` | Local `--collect` | On the pull request (SC-012) |

## Follow-ups Outside This Feature

- **Depth of field speed**: One parallel region around phase I, and a parallel blend with the same order, can each follow Baseline 1 as an ordinary change the gate proves. Why 32 threads lose so much time is still unexplained.
- **Network rendering, GUI, and animation** stay outside the gate, so nothing proves they reproduce, even though the random values no longer depend on threads there.
- **Audio analysis**: The OpenMP region in `audio_track.cpp` updates two shared maximums with no lock. It stays out of this feature, because no gated render reaches it.
- **`upgrade/strict-floating-point`** follows this feature and closes KD-2 and KD-3.

## Complexity Tracking

No constitution violations need justifying.
