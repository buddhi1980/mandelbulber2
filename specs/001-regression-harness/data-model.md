# Data Model: Regression Harness

File formats are defined in [contracts/](contracts/). This document describes the entities and rules.

## Work Item

One thing the harness renders and checks.

| Field | Description |
|---|---|
| Kind | `scene` or `formula` |
| Name | Scene file name without its extension, or the formula's internal name |
| Source | The scene path, or the formula sweep base scene |
| Formula ID | For formulas only, from the `--list-formulas` output of the binary that renders it |
| Resolution | 320x240 for scenes, 32x24 for formulas |
| Time limit | 30 minutes for scenes, 2 minutes for formulas |

**Rules**

- Names are unique within their kind. Two scenes with the same file name fail the run (FR-002).
- The scene list must not be empty, and every listed file must exist (FR-002).
- A scene must not reference an asset by URL or absolute path (FR-002).
- A scene must not enable nebula mode (FR-002).
- Every formula from `--list-formulas` is a work item unless it is listed in `tests/formula_exclusions.txt` (FR-034).
- An exclusion needs a reason and must name a formula the binary offers.

## Render Settings

The fixed configuration for every gated render. Changing any of these requires a Baseline Change.

| Setting | Value |
|---|---|
| Engine | CPU (`opencl_enabled=0`) |
| Normal buffer | On (`normal_enabled=1`) |
| Monte Carlo cap | `DOF_samples=8`, `DOF_min_samples=4` |
| Export format | Version 1 |

The reference set records these settings. A difference is a setup error, not drift.

## Verification Export

The output of one successful render.

| Field | Description |
|---|---|
| Format version | Integer, currently 1 |
| Width, height | Image dimensions |
| Compiler, math library | Toolchain identity |
| Buffers | The seven engine buffers |
| Statistics | The Statistics Record |

**Validation**

- The manifest exists and ends with its `end` line. This proves completeness (FR-008).
- The format version is one the harness understands.
- Exactly the seven expected buffers are present (FR-011).
- Each buffer file is exactly `width × height × channels × sample size` bytes.
- The statistics record is complete and contains only whole numbers.

## Engine Buffer

| Name | Sample type | Channels | Bytes per pixel | Contents |
|---|---|---|---|---|
| `raw_render` | float32 | 3 | 12 | Float image before post effects |
| `post_effect` | float32 | 3 | 12 | Float image after SSAO, depth of field, and HDR blur |
| `final_image` | uint16 | 3 | 6 | Result of brightness, contrast, and gamma |
| `alpha` | uint16 | 1 | 2 | Transparency |
| `depth` | float32 | 1 | 4 | True hit distance |
| `normal` | float32 | 3 | 12 | Surface direction relative to the camera |
| `normal_world` | float32 | 3 | 12 | Surface direction in scene coordinates |

The total is 60 bytes per pixel.

**Comparison**: Two buffers match only if their dimensions, sample type, and every byte are identical (FR-013, FR-014). The report counts differing samples, not bytes.

## Statistics Record

| Statistic | Description |
|---|---|
| `total_iterations` | All fractal iterations in the render |
| `raymarch_count` | Number of raymarches |
| `missed_de` | Missed distance estimate count |
| `iteration_histogram` | Size, then every bucket, including overflow |
| `step_histogram` | Size, then every bucket, including overflow |

Time, rates, noise values, and other floating-point fields are excluded (FR-026). For two-pass stereo scenes the record covers the last pass only, because the application resets statistics between passes.

## Reference Set

All references for one Reference Target.

| Field | Description |
|---|---|
| Target | For example `linux-x86_64-gcc` |
| Toolchain identity | Compiler and math library of the capturing build, plus package versions where available |
| Render settings | As captured |
| Scene references | One per scene |
| Formula references | One per formula |

**Rules**

- A set is replaced whole or not at all (FR-018).
- Its stored size must stay under 100 MB (SC-008). Capture fails otherwise.
- Every work item has exactly one reference, and every reference has a work item (FR-019, FR-034).

## Run Report

| Field | Description |
|---|---|
| Mode | Gate, self-parity, capture, compare, collect, or cross-check |
| Environment | `wsl`, `windows`, `linux`, or `github-actions` |
| Target | Reference Target in use |
| Gate validity | Whether isolation could be guaranteed |
| Toolchain note | Any difference from recorded versions |
| Item results | Verdict per item, with per-buffer and per-statistic detail |
| Overall result | Passed, parity failure, Reference Target mismatch, or setup error |

## Environment

Where a run happens. The harness works this out for itself, in this order:

| Value | Detected by |
|---|---|
| `github-actions` | `GITHUB_ACTIONS` is `true` |
| `windows` | The platform is Windows |
| `wsl` | `WSL_DISTRO_NAME` is set |
| `linux` | Anything else |

## Collection

One build's results for every work item. [contracts/collection.md](contracts/collection.md) defines the files.

| Field | Description |
|---|---|
| Environment | Where it was collected |
| Isolated | Whether each render had its own home folder (FR-004) |
| Source commit | The commit the harness ran from |
| Source dirty | Whether `mandelbulber2/` or `tests/` had uncommitted changes |
| Application version | The first `Mandelbulber` line of `--version` |
| Compiler, math library | Toolchain identity, from the exports |
| Render settings | As in the reference set |
| Item results | Kind, name, outcome, and any failure cause, for every work item |
| Item data | One zip per item that rendered, in the reference format |

**Rules**

- A collection is complete only when `collection.txt` exists and ends with its `end` line (FR-041).
- A failed item stays in the collection, with its cause and no zip (FR-042).
- Collect mode replaces an earlier collection, and it refuses a folder that holds anything else (FR-042).

## Cross-Check Result

| Field | Description |
|---|---|
| First, second | Each collection's environment, compiler, math library, and isolation |
| Source commit | The commit both collections came from |
| Item verdicts | For each item, whether the builds agree, and the reason if not |
| Recorded differences | For each item, each differing buffer's sample count, and each differing statistic's two values |
| Identical items | How many items matched exactly in every buffer and statistic |
| Overall result | Functional agreement, functional disagreement, or setup error |

**Rules**

- Functional disagreement comes only from the conditions in FR-044.
- Recorded differences never change the overall result (FR-045).
- Equality is claimed only for identical items (FR-047).

## Pipeline Run

One run of Cross-Build Check for one commit. [contracts/ci-pipeline.md](contracts/ci-pipeline.md) defines it.

| Job | Produces | Fails when |
|---|---|---|
| `linux-qmake` | The `collection-qmake` artifact | The build, the stand-in tests, `--test`, or collect mode fails |
| `windows-msvc` | The `collection-msvc` artifact | The build, the stand-in tests, `--test`, or collect mode fails |
| `cross-check` | The run summary and the `cross-check-report` artifact | A collection is missing, or the cross-check fails |

## State Transitions

**Work item result**

```text
pending → rendering → checked → compared → passed
                 ↓          ↓         ↓
              render     invalid    parity failure, or Reference Target
              failure    export     mismatch when the toolchain differs
```

Each failure state is final for that item. The run continues with the remaining items, so one report shows every problem. A setup error before rendering stops the run.

**Capture**

```text
self-parity running → staged → promoted to tests/references/
        ↓                ↓
      failed           failed     (existing references unchanged)
```

Nothing reaches `tests/references/` before the final step, which moves any existing set aside, moves the new set into place, and then deletes the old set.

**Collection**

```text
folder prepared → rendering items → complete (collection.txt written)
                        ↓
                setup error (no collection.txt, so it can never be cross-checked)
```

A failed item does not stop a collection. It is recorded and the run continues.
