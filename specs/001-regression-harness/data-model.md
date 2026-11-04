# Data Model: Regression Harness

File formats are defined in [contracts/](contracts/). This document describes the entities and rules.

## Work Item

One thing the harness renders and checks.

| Field | Description |
|---|---|
| Kind | `scene` or `formula` |
| Name | Scene file name without its extension, or the formula's internal name |
| Source | The scene path, or the formula sweep base scene |
| Formula ID | For formulas only, from `--list-formulas` |
| Resolution | 320x240 for scenes, 32x24 for formulas |
| Time limit | 30 minutes for scenes, 2 minutes for formulas |

**Rules**

- Names are unique within their kind. Two scenes with the same file name fail the run (FR-002).
- The scene list must not be empty, and every listed file must exist (FR-002).
- A scene must not reference an asset by URL or absolute path (FR-002).
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
| Mode | Gate, self-parity, capture, or compare |
| Target | Reference Target in use |
| Gate validity | Whether isolation could be guaranteed |
| Toolchain note | Any difference from recorded versions |
| Item results | Verdict per item, with per-buffer and per-statistic detail |
| Overall result | Passed, parity failure, Reference Target mismatch, or setup error |

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
