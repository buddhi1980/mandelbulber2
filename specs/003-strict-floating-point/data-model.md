# Data Model: Strict Floating-Point Builds

This feature adds no scene setting, and it changes no export or reference format. Its data is the check's rules and the evidence.

## Gated Build

A build of a Reference Target, and the build the flag check reads.

| Field | Value today |
|---|---|
| Reference Target | `linux-x86_64-gcc`, the First Reference Target |
| Project | `mandelbulber2/qmake/mandelbulber-opencl.pro`, `CONFIG+=release` |
| Built by | `.github/actions/build-linux` |
| Generated file | `mandelbulber2/build/Makefile` |
| Instruction set | The plain x86-64 target. GCC's default here is `-march=x86-64` with `-mtune=generic` |

**Rules**

- Every value the check reads comes from the generated file, so a flag from the environment counts.
- Equality with another Reference Target is recorded, and never assumed.

## Prohibited Option

A compile or link option the build-file search and the flag check reject, because it can change a floating-point result or tune a build to its computer.

| Field | Meaning |
|---|---|
| Option | One of 11 in [flag-check.md](contracts/flag-check.md) |
| Group | Value-changing: 8 options. Host-tuned: 3 options |
| Source | The constitution's list, plus `-mtune=native` and `-mcpu=native`, which are Principle III's other host-tuned options, and `-fno-signed-zeros` and `-ffp-model=fast`, which change values |

**Rules**

- An option matches as a whole option.
- Both the build-file search and the flag check use this one list.
- `-ffp-contract` isn't on the list, because `off` is required. Any other value of it is a failure, in a Makefile line or in a build file.
- MSVC settings are searched for in files only, because no gated build uses MSVC.

## Build File

A tracked file the search reads.

| Field | Meaning |
|---|---|
| Path | Any tracked file of the kinds in [flag-check.md](contracts/flag-check.md) |
| Exclusions | `.github/actions/compile-engine`, `mandelbulber2/tools/DOE/NaNtester.sh`, `specs`, `.specify`, `.claude` |

**Rules**

- An untracked file is not searched.
- A new tracked file of a listed kind joins the search with no change to the check.
- A comment counts.

## Check Result

| Exit code | State | Next |
|---|---|---|
| 0 | Passed | The job continues |
| 1 | A rule failed | The job fails, and every failure is listed |
| 2 | The check could not run | The job fails |

## Seeded Failure

A deliberate fault that proves a rule. It lives only in a copy of a Makefile, a throwaway clone, or a throwaway branch.

| Field | Meaning |
|---|---|
| Place | A Makefile line, an environment variable, a build file, or a GitHub Actions run |
| Fault | The option added or removed |
| Expected | The exit code and the name it must report |
| Result | What happened |

**Rules**

- Every option and every rule has at least one seed (FR-015).
- Each of the 4 environment variables carries one, through a real `qmake6` run.
- No seed is merged.

## NaN Guard

A call to `IsNotANumber()` whose test `-ffast-math` removes today.

| Site | Effect when the test is true |
|---|---|
| `compute_fractal.cpp:316` | `z` returns to its last value, the point counts as reaching the iteration limit, and the loop ends. It runs before the escape test |
| `compute_fractal.cpp:500` | `z` returns to the last good value, and the loop ends |
| `render_worker.cpp:776` | The ray stops as a dead calculation |
| `shader_calculate_normals.cpp:120` | The normal becomes `(1, 0, 0)` |

**Rules**

- The test uses `std::isfinite`, so it is true for infinity as well as NaN.
- `calculate_distance.cpp:607` drops the iteration limit mark unless iteration threshold mode is on, so a hit at line 316 counts as inside only in that mode or in interior mode. Otherwise it changes the distance estimate.
- The guards do not change (FR-023). Only the flags that hid them do.

## Guard Count

One row per item and site, from the diagnostic build.

| Field | Meaning |
|---|---|
| Item | A scene or a swept formula |
| Site | One of the four above |
| Infinite | Hits where a value was infinite |
| NaN | Hits where no value was infinite, and at least one was NaN |
| Mode | Whether the item's scene turns on iteration threshold mode or interior mode |

**Rules**

- The diagnostic build must give 0 differences against the after build, or its counts mean nothing.
- It is never committed.

## Guards-Off Build

The final commit with `IsNotANumber()` returning `false`, which is what `-ffast-math` does today.

**Rules**

- A difference between it and the after build is the guards' own effect.
- Its collection keeps the images, because `--compare` deletes each render once it has compared it, and the review marks the guards' area from them.
- A change that doesn't show there is strict arithmetic: arithmetic order, vector math, and signed zero together, which this feature doesn't separate.
- It is never committed.

## Output Record

One row per item, comparing the before and after builds. [evidence-record.md](contracts/evidence-record.md) defines the columns.

**Rules**

- Every value comes from fresh builds of the two commits, in the same harness.
- The record describes a difference. It is not a pass or a failure, because the constitution expects these changes before Baseline 1.
- The 20 formulas with the largest `final_image_average` join the review.

## Review Verdict

One per reviewed image set.

| Field | Meaning |
|---|---|
| Item | One of the 26 scenes or 20 formulas |
| Visible | No visible change, or what changed |
| Cause | A guard, strict arithmetic, or another named cause |
| Proof | The guards-off comparison, or whatever else isolates the cause |

**Rules**

- A visible change is one that can be seen in the before and after images at normal brightness. The amplified difference only shows where to look.
- A visible change with no cause blocks the feature (SC-006).
- A visible change from a guard or from strict arithmetic is accepted once it is recorded (FR-023). A failure is not: FR-018 applies.

## Cost Record

| Field | Meaning |
|---|---|
| Build | Before, after, or before with `-march=native` |
| Order | Whether this build ran first or second |
| Threads | All, or one CPU |
| Items | All 487, with scenes and formulas apart |
| Seconds | From the monotonic clock |
| Quiet | The process check's result for that run |

**Rules**

- A run that overlaps another render is repeated and not recorded (FR-025).
- There is no slowdown limit, except that a gate run stays under 30 minutes (FR-026).

## Relationships

```text
Prohibited Option ──used by──> Build-File Search ──reads──> Build File
                  └─used by──> Flag Check ───────reads──> Gated Build's generated file
Seeded Failure ──proves──> Build-File Search, Flag Check
Before and After builds ──compared in──> Output Record ──picks──> reviewed items ──> Review Verdict
Diagnostic build ──gives──> Guard Count ──explains──> Review Verdict
Guards-Off Build ──isolates──> what the guards change ──proves──> Review Verdict
Before, After, and Before host-tuned builds ──timed in──> Cost Record
```
