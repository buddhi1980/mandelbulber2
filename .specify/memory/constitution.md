# Mandelbulber2 Constitution

## Core Principles

### I. Zero Behavioral Drift (NON-NEGOTIABLE)

On every Reference Target, a change MUST NOT alter any value in the rendered image buffer, or in
the computation logs the regression harness records, for any scene in the Standard Scene Suite.

- The comparison MUST be exact. One differing pixel value or log line fails the gate. Tolerances,
  thresholds, and perceptual metrics MUST NOT be used.
- The comparison MUST use decoded values at the full precision the engine can output. Lossy
  formats and down-conversion, such as JPEG or reducing 16-bit or float data to 8-bit, MUST NOT be
  used. All channels the engine writes MUST be compared.
- This rule applies to every kind of change, including refactors, optimizations, bug fixes,
  dependency upgrades, and compiler upgrades.
- Expected output MUST change only through a Baseline Change.

Rationale: fractal iteration amplifies rounding differences, so a change that looks harmless can
alter image structure, and small changes are not reliably visible to a reviewer.

### II. Deterministic Testing (NON-NEGOTIABLE)

A gated render MUST be a pure function of the binary, the scene file, the recorded seeds, and the
resolution. Running it again MUST produce identical output.

- Gated renders MUST run headless, with no GUI, display, or operator input.
- Every random value MUST derive from a seed fixed in the scene file or the harness. A seed setting
  that a scene file does not name takes its default value, which counts as fixed in the scene file.
  Wall-clock time, process IDs, and unseeded generators MUST NOT be seed sources.
- Thread count, core count, thread scheduling, and elapsed time MUST NOT be able to change any
  output value. Random values MUST NOT depend on shared mutable state whose order of access varies
  with scheduling.
- Camera position, rotation, target, field of view, and resolution MUST be fixed by the scene file
  and harness. Nothing MUST adjust them at run time.
- Gated renders MUST NOT read state from outside the repository and build output, including
  per-user settings files, caches, the host locale, and environment variables that affect
  rendering.
- A build MUST pass Self-Parity before any baseline is accepted from it. Self-Parity means three
  consecutive runs at each of at least two thread counts, one of them 1, all producing identical
  output.

Rationale: without determinism a parity failure cannot be told apart from noise, and a gate that
fails on noise gets ignored or bypassed.

### III. Architecture Agnosticism (NON-NEGOTIABLE)

Builds of the CPU engine MUST NOT enable optimizations that can change floating-point results.

- These options MUST NOT be used: `-ffast-math`, `-Ofast`, `-funsafe-math-optimizations`,
  `-ffinite-math-only`, `-fassociative-math`, `-freciprocal-math`, and MSVC `/fp:fast`.
- GCC and Clang builds MUST set `-ffp-contract=off`. MSVC builds MUST use `/fp:precise` or
  `/fp:strict`, with `/fp:contract` unset.
- Pragmas and attributes that re-enable these behaviors in source, such as `#pragma GCC optimize`
  or `#pragma float_control`, MUST NOT be used in engine code.
- `-march=native` and other host-tuned options MUST NOT be used. The instruction set MUST be fixed
  for each Reference Target.
- CI MUST check the effective compile flags, including flags inherited from `CXXFLAGS`, `CFLAGS`,
  and `CPPFLAGS`, and MUST fail if a prohibited option is present.
- Equality between two Reference Targets MUST NOT be assumed. It MAY be claimed only for targets
  where the harness has demonstrated it.

Rationale: these optimizations let the compiler reorder arithmetic, fuse operations, and discard
NaN, infinity, and subnormal handling, so results depend on the compiler and CPU. Math libraries
also differ between platforms, which is why build flags alone do not guarantee cross-target
equality.

## Parity Scope and Reference Baselines

- **Gated engine**: the CPU renderer, meaning everything that can affect its output, including
  `mandelbulber2/src`, `mandelbulber2/formula`, build files, and third-party math dependencies.
- **OpenCL**: not gated. Its output depends on the device and driver, its kernels are seeded with
  `std::rand()`, and they compile with relaxed math by default. Any parity claim for OpenCL
  requires an amendment to this constitution.
- **Standard Scene Suite**: a version-controlled list of `.fract` files, drawn from
  `mandelbulber2/deploy/share/mandelbulber2/examples` plus purpose-built scenes. It MUST include
  scenes that exercise each stochastic CPU path, including Monte Carlo depth of field, global
  illumination, and ambient occlusion.
- **Reference Target**: one combination of operating system, CPU architecture, compiler version,
  and math library. Each target has its own references. A difference between commits on one target
  is drift. A difference between two targets is recorded and is not drift.
- **First Reference Target**: the Linux x86-64 GCC build that the CI gate runs. Its exact compiler
  and math library versions MUST be recorded with Baseline 1.
- **Baseline Change**: the only permitted way to replace references. It MUST be a separate pull
  request containing no other change, MUST state why output changes, MUST include before-and-after
  evidence for affected scenes, MUST regenerate references from a CI build that passes Self-Parity,
  and MUST be approved by a maintainer.

## Development Workflow and Quality Gates

1. **Determinism Foundation first.** Engine refactors MUST NOT begin until Baseline 1 exists.
   Before Baseline 1, the only permitted engine changes are those that close a Known Deviation.
   Each MUST land as its own reviewed change, stating its expected effect on output.
2. **Parity gate.** After Baseline 1, every pull request MUST pass the regression harness locally
   and in CI before merge. A failing or skipped gate blocks the merge.
3. **Failure response.** A parity failure MUST be treated as a defect in the change. Regenerating
   references to clear a failure is prohibited outside a Baseline Change.
4. **Isolation.** A refactor and a change that alters output MUST NOT share a pull request.
5. **Constitution Check.** Every `/speckit-plan` MUST answer these gates:
   - Principle I: can this change affect gated output, and which scenes cover it?
   - Principle II: does it add randomness, timing, threading, or external state to the render path?
   - Principle III: does it touch build flags, compilers, math libraries, or arithmetic order?
6. **Existing tests.** The `--test` suite remains required but does not satisfy the parity gate.
   It renders examples at 5x5 pixels and does not compare them against references.

### Known Deviations at Ratification

The codebase did not comply with this constitution when it was ratified. Each item below is a
recorded exception until closed. Plans MUST cite the relevant item and MUST NOT report it as
passing. New deviations MUST NOT be added except by amendment.

| ID | Deviation | Location | Principle | Closed when |
|----|-----------|----------|-----------|-------------|
| KD-1 | Renders do not reproduce. On 2026-11-04 a Windows Release build rendered `IFS_001.fract` 6 times and `Makin3D-Julia_001.fract` 4 times at 240x180 on 32 threads. All 10 outputs were distinct, and 90-99% of pixels differed between runs. Render threads share the unsynchronized global `gRandomSeed`, the leading suspect; the cause is not yet proven. `limit_CPU_cores` showed no effect under `--nogui`. | `mandelbulber2/src/common_math.cpp`, `mandelbulber2/src/render_worker.cpp` | II | Self-Parity passes on the Standard Scene Suite. **Closed 2026-11-07**, see `specs/002-deterministic-rendering/evidence.md` |
| KD-2 | Default Linux and macOS builds enable `-ffast-math`; the MSVC Debug x64 configuration uses `/fp:fast`. | `mandelbulber2/qmake/common.pri`, `mandelbulber2/cmake/CMakeLists.txt`, `mandelbulber2/msvc/mandelbulber2.vcxproj` | III | The CI flag check passes on all gated builds. **Closed 2026-11-08**, see `specs/003-strict-floating-point/evidence.md` |
| KD-3 | Install and setup scripts export `-march=native`, and qmake imports environment flags into the build. | `mandelbulber2/tools/prepare_for_dev_*.sh`, `mandelbulber2/deploy/linux/install`, `mandelbulber2/qmake/common.pri` | III | The CI flag check rejects host-tuned flags on gated builds. **Closed 2026-11-08**, see `specs/003-strict-floating-point/evidence.md` |
| KD-4 | No regression harness, Standard Scene Suite, references, or CI parity gate exists. | `tests/` does not exist | I | Baseline 1 is merged |
| KD-5 | Renders depend on the CPU. glibc picks its math code path from the CPU's features at run time. On 2026-11-07, one binary gave different `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `exp`, `log`, and `pow` results with its FMA and AVX2 paths turned off, see `specs/003-strict-floating-point/evidence.md`. | glibc's math library | II | Playbook Phase 4 settles the path before Baseline 1 is captured, and records the result with the references |

Closing KD-1, KD-2, and KD-3 will change output, so Baseline 1 MUST be captured after they close.

## Governance

This constitution takes precedence over `MIGRATION_PLAYBOOK.md`, specifications, plans, and all
other project guidance. `MIGRATION_PLAYBOOK.md` is the runtime development guide; where it conflicts
with this constitution, this constitution applies and the playbook MUST be corrected.

- **Amendments**: an amendment MUST be a pull request that edits this file, states the reason,
  updates the version and dates, and identifies affected plans and gates. A maintainer MUST approve
  it.
- **Versioning**: this constitution uses semantic versioning. MAJOR covers removal or redefinition
  of a principle. MINOR covers a new principle or section, or materially expanded guidance. PATCH
  covers clarifications, wording fixes, and closing a Known Deviation.
- **Compliance**: every pull request and plan review MUST verify compliance with this constitution.
- **Waivers**: the Complexity Tracking table in a plan MUST NOT waive a principle. Non-compliance is
  permitted only as a Known Deviation, and expected output changes only through a Baseline Change.

**Version**: 1.1.0 | **Ratified**: 2026-11-04 | **Last Amended**: 2026-11-08
