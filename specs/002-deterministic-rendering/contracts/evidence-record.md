# Contract: Evidence Record

`specs/002-deterministic-rendering/evidence.md` records why renders differed and how each OpenMP region behaves. It is written before the shared seed is replaced, and the audit section is finished before the statistics work starts.

## Header

States the commit of each build, the toolchain, the computer, the date, and the thread counts used. Every build is a fresh build of that commit (FR-007).

## Builds

| Name | Source | Purpose |
|---|---|---|
| Thread control | The branch with `--threads` added | Whether one-thread renders reproduce today |
| Constant values | The same commit, plus a local change that makes `Random(max)` return `max / 2` | What still differs at all threads when random values can't depend on thread order |
| Constant values, one-thread blend | The constant values build, with the depth of field phase II `pragma` removed | Whether the blend explains what is left in the post-effect buffers |

The local changes are never committed (FR-011). The record describes them in words.

## Scenes

The record covers these scenes at the harness's resolution and overrides (FR-008).

| Scene | Stochastic path |
|---|---|
| `IFS_001.fract` | Named in KD-1, multi-ray ambient occlusion, volumetric auxiliary light |
| `Makin3D-Julia_001.fract` | Named in KD-1 |
| `chromatic_aberration.fract` | Monte Carlo depth of field, antialiasing, soft shadows, chromatic aberration |
| `difs_tree.fract` | Monte Carlo global illumination, rough surfaces, Perlin noise on materials |
| `IFS_002.fract` | Post-effect depth of field |
| `amazing_surf 002.fract` | Random screen-space ambient occlusion |
| `EXR example.fract` | Volumetric fog |
| `benchmark-GPU.fract` | Iteration fog |
| `gradients - specular highlights.fract` | Specular roughness |
| `menger octo 001.fract` | Random lights |
| `clouds 005.fract` | Clouds |
| `tests/scenes/beam_light.fract` | Beam light |
| `tests/scenes/hdr_blur.fract` | HDR blur |

## Runs

| Build | Thread count | Renders of each scene |
|---|---|---|
| Thread control | 1 | 3 |
| Thread control | All | 3 |
| Constant values | 1 | 3 |
| Constant values | All | 3 |
| Constant values, one-thread blend | All | 3 |

## Results

One row per scene, build, and thread count.

| Field | Content |
|---|---|
| Buffers that differ | Each of the seven buffers, with the number of differing samples, or none |
| Statistics that differ | Each statistic, or none |
| Spread | The lowest and highest value of each statistic across the runs (FR-009) |

## Causes

One entry per cause found (FR-009, SC-007).

| Field | Content |
|---|---|
| Cause | What the code does, with file and line |
| Scenes | The scenes that show it |
| Effect | The buffers and statistics it reaches |
| Proof | The comparison that isolates it |
| Fix | The requirement that removes it |

Every difference in Results must trace to a cause. If one-thread renders differ, or a difference has no cause, that cause joins the work (FR-010, FR-023), and the shared seed is still replaced.

## Order audit

One entry per OpenMP region a still render can reach (FR-021, SC-008).

| Field | Content |
|---|---|
| Region | File and line |
| Reads | What it reads, and whether that changes during the region |
| Writes | What it writes, and whether two threads can reach the same place |
| Verdict | Depends on order, or does not |
| Scene | The suite scene that reaches it |
| Proof | The Self-Parity result for that scene once random values are fixed |
| Result | Stays parallel, or moves to one thread |

## Other dependence

One entry for anything else Self-Parity exposes (FR-023), with the same fields as a cause.

## Measurements

| Measurement | Requirement |
|---|---|
| Standard Scene Suite render time, before and after | FR-039, SC-014 |
| Depth of field stage time on `IFS_002.fract` at 1920x1080, before and after | FR-039, SC-014 |
| Noise on the 6 Monte Carlo scenes, before and after | SC-010 |
| Collect step time in Cross-Build Check | SC-014 |

## Success criteria

One row per success criterion, filled in when its check runs, so a reviewer can read every result in one place.

| Field | Content |
|---|---|
| Criterion | The SC number |
| Result | The measured result, in numbers where there are numbers |
| Commit | The commit of the fresh build that gave it |
| Not covered | Anything the check could not reach, or none |
