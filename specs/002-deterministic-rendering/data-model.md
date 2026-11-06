# Data Model: Deterministic CPU Rendering

## Thread Count

The number of threads a render uses at every parallel stage.

| Field | Type | Meaning |
|---|---|---|
| `sSystem::numberOfThreads` | `int` | Render workers and screen-space ambient occlusion. Exists today. |
| `sSystem::threadsFromCommandLine` | `int` | New. The value of `--threads`, or 0 if the option was not used. |

**Rules**

- `--threads N` sets both fields once, before settings load.
- With the option set, `limit_CPU_cores` does not overwrite `numberOfThreads` in the queue (FR-004).
- `sSystem::GetOpenMpThreads()` is new. It returns `threadsFromCommandLine` if that is above 0, and otherwise `omp_get_max_threads()`, or 1 in a build without OpenMP. Every OpenMP region uses it, and so does each per-thread buffer sized for one, so a thread index always fits its buffer.
- The GUI path does not change, because the option is refused there.

## Render Seed

| Field | Type | Meaning |
|---|---|---|
| `render_random_seed` | Scene setting, `int` | The number every random value in a render derives from. Default 1. |
| `sParamRender::renderRandomSeed` | `int` | The same value, read when the render starts. |

**Rules**

- The setting is `morphNone`, so it does not interpolate between keyframes.
- It is `paramStandard`, so it reaches network clients with the rest of the scene.
- `clouds_random_seed`, `random_lights_random_seed`, and `coloring_random_seed` stay separate (FR-020).

## Random Position

What identifies one random value. [random-values.md](contracts/random-values.md) defines the arithmetic.

| Part | Source | Meaning |
|---|---|---|
| Stage | The caller | Render workers, screen-space ambient occlusion, or depth of field |
| Seed | `renderRandomSeed` | The Render Seed |
| Frame | `sParamRender::frameNo` | The animation frame, or 0 for a still render |
| Eye | `cStereo::GetForcedEye()` | Which pass of a two-pass stereo render, or 0 for any other render |
| `a`, `b`, `c` | The caller | The pixel and sample, or the post effect's own position |
| Place in sequence | The generator | How many draws came since the position was set |

**Rules**

- The first four parts make a key that stays fixed for a render. The next three restart the sequence.
- Output buffers and statistics can't shift a value, because no optional output draws one.
- A second pass of two-pass stereo renders the same pixels again, so the eye keeps the two eyes' noise separate, as it is today.

## `cPositionRandom`

A new class in `mandelbulber2/src/position_random.hpp`. It is header-only, because `Random` sits on the hottest path in the renderer.

| Member | Meaning |
|---|---|
| `cPositionRandom(seed, frame, eye, stage)` | Builds the key |
| `SetPosition(a, b, c)` | Restarts the sequence for one position |
| `Random(max)` | Returns the next value, from 0 to `max` |

**Ownership**

| Owner | Lifetime | Thread |
|---|---|---|
| `cRenderWorker` | One worker | Its own thread only. The member is `mutable`, like `objectStack`, because most shaders are `const`. |
| `cSSAOWorker::doWork` | One worker | Its own thread only |
| `cPostRenderingDOF::Render` | One call | The calling thread, because the shuffle is sequential |
| `cOpenClEngineRenderDOF` shuffle loop | One loop entry | One OpenMP thread |

No instance is shared between threads.

## Line ownership

`cScheduler` hands each image line to one thread. This feature closes the one gap in that rule.

| State | Meaning |
|---|---|
| `linePendingThreadId[y] == 0` | Free |
| `linePendingThreadId[y] == id` | Claimed by thread `id` |
| `linePendingThreadId[y] == 9999` | Rendered by the network server |
| `lineDone[y]` | Finished |

| Transition | Today | With this feature |
|---|---|---|
| Thread starts on its first line | `InitFirstLine` writes the owner with no lock and no check | `InitFirstLine` takes the lock and claims the line only if it is free |
| Thread finishes a line | `NextLine` marks it done under the lock | No change |
| Thread needs another line | `NextLine` claims a free line under the lock | No change |
| Thread finds its line owned by another | `ShouldIBreak` makes it give up, and the line stays with its owner | No change. It now happens only at the start of a worker, or for a line the network server finished. |

**Result**: In a headless still render, one thread renders each line, once. A thread that finds its first line taken moves on to a free line before it renders a pixel.

## Render Statistics

| Object | Owner | Updated by | Read by |
|---|---|---|---|
| `cRenderWorker::statistics` | One worker | That worker, for the line it is rendering | That worker |
| `sRenderData::statistics` | The render job | Workers, at the end of each line, under `sRenderData::statisticsMutex` | `cRenderer` and `cRenderJob` |

| Value | Type | Exact at every thread count | In the verification export |
|---|---|---|---|
| `totalNumberOfIterations` | `long long` | Yes | Yes |
| `numberOfRaymarchings` | `int` | Yes | Yes |
| `missedDE` | `int` | Yes | Yes |
| `histogramIterations` | `cHistogram` | Yes | Yes |
| `histogramStepCount` | `cHistogram` | Yes | Yes |
| `numberOfRenderedPixels` | `size_t` | Yes | No |
| `totalNumberOfDOFRepeats` | `long long` | Yes | No |
| `totalNoise` | `double` | No, because the sum of floating-point values depends on the order of the lines | No |

**Rules**

- A worker resizes its histograms to match the shared ones when `doWork` starts. `cRenderWorker` is also built for OpenCL with no thread data, so the constructor can't do it.
- When a line finishes, the worker adds its values to the shared object under the mutex, then clears them. This is what keeps the GUI's dock and `--stats` updating (FR-029).
- When a worker gives up on a line, it clears its values without adding them (FR-025).
- `cRenderer` copies the shared object under the mutex before it emits `updateStatistics`.
- `cStatistics::Add` sums every value. Integer sums do not depend on order, which is why the totals match at every thread count (FR-024).
- `cStatistics::Reset` also clears `totalNoise`, which it misses today.

## Iteration count

`sDistanceOut` carries each distance result.

| Field | Today | With this feature |
|---|---|---|
| `iters` | No initial value, and the limit box exit never sets it | Starts at 0 |
| `totalIters` | Set to 0, and the object tree never adds to it | Starts at 0, and the object tree adds each node's count |

**Rules**

- `CalculateDistanceSimple` already adds each fractal evaluation to its own `out->totalIters`. The object tree passes it a local `nodeOut`, so the count stops there. The fix adds `nodeOut.totalIters` to `out->totalIters` in the fractal and hybrid node cases.
- Nothing else reads `totalIters`, so the fix can't reach a buffer (FR-028).
- Reading `iters` on the limit box exit reads an old stack value today. No suite scene turns on `limits_enabled`, so Self-Parity can't show it, and the fix closes it for the scenes that do.

## Cause Evidence and Order Audit

[evidence-record.md](contracts/evidence-record.md) defines both. They are one file, `evidence.md`, because each audit verdict needs the same builds and scenes as the causes.
