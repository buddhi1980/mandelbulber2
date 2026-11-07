# Contract: Evidence Record

`specs/003-strict-floating-point/evidence.md` records what this feature proved. `specs/003-strict-floating-point/output-record.tsv` holds one row per item. Every number comes from a fresh build of the commit named beside it.

## Header

States the date, the computer, the environment, the toolchain, the thread counts, and the CPU's model and whether it has FMA and AVX2.

## Builds

| Name | Source | Purpose |
|---|---|---|
| Before | `8fd49ba59` | Output and time before this change |
| After | The final commit | Output and time after this change |
| Before, host-tuned | `8fd49ba59`, built with `CXXFLAGS=-march=native` | What users of the setup scripts lose (FR-027) |
| Diagnostic | The final commit, plus a local patch that counts guard hits | Which items reach each guard (FR-020) |
| MSVC before and MSVC after | `8fd49ba59` and the final commit, Release x64 | The comparison images, and the MSVC checks |

The local patch is never committed (FR-020). The record describes it in words. "Final commit" means the last commit that changes what compiles.

## Flag check

One row per seed in [flag-check.md](flag-check.md), with its exit code and the option it named. A note states that the seeds ran the committed script.

## GitHub Actions

| Field | Content |
|---|---|
| Run | The link |
| Commit | The commit it ran on |
| Seed | None, or the seed |
| Result | Which step passed or failed, and the message |

One row for the passing `parity-gate` on the pull request, and one for each seeded run.

## Output

### Summary

| Measure | Value |
|---|---|
| Items with any difference | Count, of 487 |
| Items identical in every buffer and statistic | Count |
| Items whose final image holds one value on one side only | Count, which should be 0 |
| Exit status of `--compare` | 1 is expected |
| Each buffer | Items that differ |
| Each statistic | Items that differ |

### Scenes

One row for each of the 26 scenes.

| Field | Content |
|---|---|
| Scene | Its name |
| Samples changed | In `final_image`, of the total |
| Average change | The average absolute difference in `final_image`, whose samples run from 0 to 65,535 |
| Guards | Hits at each site, with infinite and NaN values apart |
| Review | No visible change, or the visible change |
| Cause | Rounding, a guard, vector math, or another named cause |

### Formulas

The same fields, for the 20 swept formulas whose final image changed most. A note gives the rule that picked them.

### Guards

| Site | Items reached | Infinite hits | NaN hits |
|---|---|---|---|
| `compute_fractal.cpp:316` | | | |
| `compute_fractal.cpp:500` | | | |
| `render_worker.cpp:776` | | | |
| `shader_calculate_normals.cpp:120` | | | |

### Failures found

One entry for each failure FR-018 covers, or the word none.

| Field | Content |
|---|---|
| Failure | What failed, and on which item |
| Cause | What the code does, with file and line |
| Fix | What changed |
| Commit | The commit, which becomes the new final commit |

## `output-record.tsv`

One header row, then one row per item, tab separated.

| Column | Content |
|---|---|
| `kind` | `scene` or `formula` |
| `name` | The item |
| `raw_render` to `normal_world` | One column per buffer: the samples that differ |
| `final_image_total` | The samples in `final_image` |
| `final_image_average` | The average absolute difference |
| `final_image_largest` | The largest absolute difference |
| `total_iterations`, `raymarch_count`, `missed_de` | One column each: the before and after values joined by `>`, or empty if equal |
| `iteration_histogram`, `step_histogram` | `differs`, or empty |

## Measurements

### Time

All times are seconds on the monotonic clock. A note states that no recorded run overlapped another render, and how that was checked.

| Run | Build | First or second | All threads | Scenes | Formulas |
|---|---|---|---|---|---|
| Before build first | Before | | | | |
| Before build first | After | | | | |
| After build first | After | | | | |
| After build first | Before | | | | |

| Run | One CPU |
|---|---|
| Before | |
| After | |

| Run | All threads | Compared with before |
|---|---|---|
| Before, host-tuned | | |

| Stand-in for the gate | Minutes |
|---|---|
| Collect step on the pull request, on the 4-vCPU runner | |

### Other builds

One row per build in [research.md](../research.md) R14.

| Field | Content |
|---|---|
| Build | Its name |
| Before | Compiled, failed, or not known |
| After | Compiled, or failed |
| Proof | The command or the CI check |
| Flags read | The floating-point flags seen in its build output |

### MSVC and cross-build

| Check | Result |
|---|---|
| MSVC before against MSVC after | 487 of 487 items identical, from the scratch script, because `--cross-check` refuses two commits |
| qmake after against MSVC after | The `identical` line of `--cross-check` (SC-009) |

## Success criteria

One row per success criterion, filled in when its check runs.

| Field | Content |
|---|---|
| Criterion | The SC number |
| Result | The measured result, in numbers where there are numbers |
| Commit | The commit of the fresh build that gave it |
| Not covered | Anything the check could not reach, or none |
