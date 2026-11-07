# Research: Deterministic CPU Rendering

Every decision below comes from reading the code on 2026-11-06, and from measurements on fresh WSL builds of commit `50bade564` the same day: GCC 13.3.0, Qt 6.4.2, 32 threads. Nothing was changed in the repository, and the one-line depth of field variant in R6 was a local change that has been reverted.

## R1. How `--threads` reaches OpenMP

- **Decision**: Each OpenMP region gets a `num_threads(systemData.GetOpenMpThreads())` clause. The function returns the `--threads` value when it was given, and `omp_get_max_threads()` otherwise.
- **Rationale**: `omp_set_num_threads()` only sets the calling thread's value, and some stages run on other threads. A test program set it to 2 on the main thread, then started a thread: that thread's region still had 32 threads, or 5 with `OMP_NUM_THREADS=5`. A still render runs on the main thread, but the queue runs on a `QThread` and network rendering moves its work to another. The clause gave exactly 3 and exactly 64 in both cases, so it beats the environment and goes above the CPU count (FR-002, FR-003). It is in OpenMP 2.0, so MSVC supports it.
- **The fallback matters**: Without `--threads`, using `numberOfThreads` in the clause would make `limit_CPU_cores` start limiting OpenMP regions in the GUI, and FR-004 keeps that as it is.
- **Buffers**: `voxel_export.cpp:228` and `marchingcubes.cpp:107` size per-thread buffers with `omp_get_max_threads()`, then index them with `omp_get_thread_num()`. Both must use the new function too, or a count above the default would write past the end.
- **Alternatives considered**: Calling `omp_set_num_threads()` at the start of each render thread. It works only if every entry point remembers to, and voxel export, mesh export, and audio loading each have their own.

## R2. Where `--threads` is checked

- **Decision**: In the `cCommandLineInterface` constructor, right after the options are read, before `QApplication` exists.
- **Rationale**: `main()` decides between `QApplication` and `QCoreApplication` from `isNoGUI()`, just after the constructor. `ReadCLI` runs later, and in a GUI run `cErrorMessage::showMessage` opens a modal message box there, which never returns on a computer with no display. The constructor is the only place a refusal can't hang (SC-006). At that point `gApplication` is null, so `showMessage` takes its plain text path.
- **Headless test**: A run is headless if it has `--nogui`, `--queue`, `--test`, `--benchmark`, `--keyframe`, `--flight`, `--voxel`, `--server`, or `--host`, or if the arguments name a queue: several scenes, a `.fractlist`, or a folder. Those last three repeat the test in `handleArgs`.
- **`--help` and `--version`**: Qt handles both inside `parser.process()`, before any application code runs, so they print and exit 0 even with `--threads`. Refusing them would need a second parse just to withhold the help text, so the spec records this as the one exception.
- **QTest**: `runTestCasesAndExit` and `runBenchmarksAndExit` pass the raw arguments to QTest, which stops on options it doesn't know. They already strip `--output` and `--logfilepath`, so they strip `--threads` too, in both the `--threads N` and `--threads=N` forms.

## R3. The random number generator

- **Decision**: A new header-only class, `cPositionRandom`, built on SplitMix64. A hash of the stage, seed, frame, eye, and position gives a 64-bit starting state, and each draw adds a constant and mixes. [random-values.md](contracts/random-values.md) has the exact definition.
- **Rationale**:
  - **Pure function**: The state comes from the seed and position alone, so no thread can affect it (FR-012, FR-014).
  - **Same on every build**: It uses unsigned 64-bit integers only, with no `long`, floating point, or standard library distribution (FR-018). `std::mt19937` output is portable, but `std::uniform_int_distribution` is not.
  - **Fast**: One draw took 1.09 ns, against 2.72 ns for today's generator on one thread. Setting a position and drawing once took 3.29 ns. A position is set once per sample, and a raymarch step costs microseconds.
  - **Scales**: With 100 million draws per thread at 32 threads, today's shared generator took 4.17 s and one generator per thread took 0.40 s. At 4 threads, it was 0.51 s against 0.11 s.
  - **No visible pattern**: Across a 1920x1080 grid of first draws, the correlation with the pixel to the right was -0.00175, with the pixel below +0.00136, with the diagonal -0.00091, and between seeds 1 and 2 at the same pixel -0.00117. All four are inside the noise floor of 0.00208, three standard deviations for that many samples. A chi-square test over 10 buckets gave 7.85 against a 1% limit of 21.67. SC-010 still checks real images (FR-019).
- **Alternatives considered**:
  - **One Park-Miller state per pixel**: It keeps today's multiplier, but its low bits are weak, every caller reduces with `%`, and similar starting states give similar first values, so nearby pixels would share noise.
  - **`cRandom`**: It wraps GSL's Mersenne Twister. Seeding it means filling 624 words, which is far too slow to do once per sample, and it is an allocated object.
  - **`thread_local` state**: It leaves values depending on which thread rendered which pixel.
  - **A hash for each draw, with no running state**: It costs the full 3.29 ns per draw, for no gain over a counter.

## R4. Where the generator lives in the render worker

- **Decision**: `cRenderWorker` gets a `mutable cPositionRandom random`, set at the top of the sample loop in `doWork`. `cLight::CalculateLightVector` and `cLight::CalculateBeam` take a `cPositionRandom *`.
- **Rationale**: Most shaders are `const` member functions, and `objectStack` is already `mutable` for the same reason: it is per-thread scratch state. Twelve of the 27 calls are in the worker itself, and 10 more are in its shaders, so a member needs no signature changes there. `cLight` is shared by all threads, so it can't hold a generator. Passing a pointer draws only for a beam light, as today. The callers are `render_worker.cpp:1740`, `shader_light_shading.cpp:47`, and `shader_volumetric.cpp:409, 592`.
- **Two-pass stereo**: `cRenderJob` renders the same pixels twice, once per eye, so the seed, frame, pixel, and sample would repeat and both eyes would get one noise pattern. Today they differ, so the key includes the forced eye. `GetForcedEye()` needs to become `const`. Other stereo modes already give each eye different pixels or samples.
- **Sequential stages**: `dof.cpp` keeps its shuffle sequential. It builds a generator for the render and sets the pass and entry for each entry of the shuffle, as FR-012 says. The caller passes the generator to `Render`, because the five call sites know the render's settings and `cPostRenderingDOF` doesn't. Those sites are `render_image.cpp:328, 333, 339`, `render_job.cpp:1325`, and `opencl_engine_render_dof.cpp:120`.

## R5. The seed setting

- **Decision**: `render_random_seed`, default 1, range 0 to 2147483647, `morphNone`, `paramStandard`.
- **Rationale**: The name follows `clouds_random_seed` and `random_lights_random_seed`. 1 is the value `gRandomSeed` starts from today, and the choice doesn't matter otherwise. `paramStandard` sends it to network clients with the rest of the scene, so every client draws the same values. `morphNone` keeps it from interpolating, and the frame number already varies noise between frames.
- **Principle II**: Its seed rule names the scene file or the harness. No suite scene names this seed, so FR-034 adds a clarification that a setting's default counts as fixed in the scene file. `clouds_random_seed` and `random_lights_random_seed` already depend on the same reading.
- **Frame number**: `frame_no` already reaches `sParamRender::frameNo`. Every animation path sets it, it is never saved in a scene file, and still renders stay at 0. `--override frame_no=5` sets it on a still render, which gives tests a simple way to render a frame's noise.

## R6. The depth of field blend

- **Decision**: Remove the `pragma` on phase II, so the loop runs in `temp_sort` order on the calling thread. Phase I keeps its region and gets the new clause.
- **Rationale**: The blend at `dof.cpp:319` reads and rewrites pixels, so overlapping discs depend on order, and two threads can update a pixel at once. With one thread today, `schedule(dynamic, 1)` hands out `j` in ascending order, so removing the `pragma` gives exactly the order of a one-thread run.
- **Measured**: The time of the whole stage on `IFS_002.fract` at 1920x1080, in seconds. OpenMP threads were set with `OMP_NUM_THREADS`, because the thread control doesn't exist yet, and render workers stayed at 32.

| Blur radius | Build | 1 thread | 4 threads | 16 threads | 32 threads |
|---|---|---|---|---|---|
| 19.72, the scene's own | Today | 2.9 | 1.0 | 0.8 | 24.8 |
| 19.72, the scene's own | Blend on one thread | 2.4 | 1.7 | 1.5 | 10.0 |
| 100 | Today | 34.3 | 10.2 | 4.3 | 25.0 |
| 100 | Blend on one thread | 33.7 | 22.4 | 20.3 | 27.5 |

- **What this shows**: With the scene's own blur, the fix is slower at 4 to 16 threads and faster at 32, as the spec expected. With a large blur, it is slower at every count above 1, because phase II does most of the work. The spec's statement that one-thread times are an upper limit is wrong, and this plan corrects it: 10.0 seconds at 32 threads is above the 2.4 at one thread, because phase I also loses time at 32 threads. Each of phase I's 1,080 lines opens its own parallel region, phase II had one for each of its 1,920 columns, and the cost looks tied to the number of regions: about 24 seconds for 3,000 regions today, and about 8.5 seconds for 1,080 after the fix, which is about 8 ms a region both times. Why it happens is still not proven, and it may be specific to WSL.
- **Effect on the gate**: Small. The harness renders at 320x240, where that whole scene took 0.3 seconds.
- **Alternatives considered**: One parallel region around the outer loop of phase I would likely remove most of that cost, and it can't change a value. It stays out of this feature, because FR-036 keeps the engine as it is apart from what closes KD-1. It is a good first change after Baseline 1, when the gate can prove it.

## R7. The other OpenMP regions

- **Decision**: Phase I of depth of field and HDR blur stay parallel. Self-Parity checks both once random values are fixed.

| Region | Reads | Writes | Reached by a still render |
|---|---|---|---|
| `dof.cpp:101`, phase I | The post-effect image and z-buffer, unchanged during the region | Its own entry of `temp_image` | Yes |
| `dof.cpp:284`, phase II | The post-effect image, which it changes | Neighboring pixels of the post-effect image and alpha | Yes |
| `post_effect_hdr_blur.cpp:78` | A snapshot of the image | Its own pixel | Yes |
| `voxel_export.cpp:239` | Scene data | Its own slice cells | No, voxel export replaces the render |
| `marchingcubes.cpp:267` | Scene data | Its own voxel cells | No, mesh export replaces the render |
| `audio_track.cpp:302` | Audio samples | Its own frame's entry, and two shared maximums with no lock | Only while loading a scene with audio tracks |

- **Rationale**: FR-021's three regions are the first three rows. The other three get the thread clause (FR-002, SC-005), and they are outside the gate. `opencl_engine_render_dof.cpp:177` runs only on the OpenCL path, and it gets the clause too, so the rule has no exception and a search can check it. The audio region's shared maximums can lose an update, which could only reach a render through an audio-driven animation. No suite scene has audio tracks, and FR-036 leaves it as it is.

## R8. Exact statistics

- **Decision**: Each worker counts into its own `cStatistics`, adds it to the shared one under a mutex at the end of each line, and then clears it. `InitFirstLine` takes the scheduler's lock and claims a line only if it is free. A worker that gives up on a line clears its counts without adding them.
- **Rationale**:
  - **Lost updates**: All threads add to one object on every raymarch step, which loses updates. Atomics would fix that, but one contended cache line per step would slow every render. Local counters cost nothing, and they should speed up all-thread renders, because today's counters and random seed are shared between all threads on the hottest path.
  - **Double counting**: `NextLine` only ever claims a free line under its lock, so `InitFirstLine` is the only way two threads can own one line. Closing it means each line has one owner in a headless still render. That is simpler and cheaper than storing a result per line, and it also stops the wasted second render of a line.
  - **Late start**: A thread whose first line is taken still works: `ShouldIBreak` is true on its first pixel, and `NextLine` gives it a free line. With more threads than lines, every thread starts on line one, and the first to claim it keeps it.
  - **Order**: Integer sums don't depend on the order lines finish. `totalNoise` is a `double`, so its last bits can vary, and it isn't in the export (FR-027 only asks that updates don't collide).
  - **Cost**: Merging takes one pass over two histograms per line, under a lock, which is small against rendering a line. `AddWeighted` already does this for OpenCL tiles.
- **Readers**: `render_image.cpp` copies `data->statistics` once a second while workers write to it. FR-027 only forbids two writers, but a copy of a changing vector is still a data race, so `cRenderer` copies it under the mutex.
- **Alternatives considered**:
  - **One result per line, stored and summed at the end**: It handles double rendering without touching the scheduler, but it holds a pair of histograms per line.
  - **Merging only when a worker finishes**: The dock and `--stats` would stop updating during a render (FR-029).

## R9. Total iterations

- **Decision**: Default-initialize `sDistanceOut::iters` and `totalIters`, and add `nodeOut.totalIters` to `out->totalIters` in the fractal and hybrid node cases of `CalculateDistanceFromObjectsTree`.
- **Rationale**: `CalculateDistanceSimple` adds each fractal evaluation to `out->totalIters`, but the object tree passes it a local `nodeOut` and copies only `nodeOut.iters`, and that is why every total reads 0. `totalIters` has no initial value, so the local must start at 0 first. Delta DE then counts four evaluations per distance, which is the real count (FR-026). Nothing reads `totalIters` except the statistics, so that change can't reach a buffer.
- **A second problem found**: The limit box exit at `calculate_distance.cpp:305` never sets `out->iters`, and its callers put that value in the histogram and in `stepBuff`. That reads an old stack value, which can depend on what the thread rendered before. No suite scene turns on `limits_enabled`, so Self-Parity can't show it. The default initializer closes it. Iteration fog reads that value too, so a scene with both can change, which is the one case outside the suite in FR-028.

## R10. The evidence step

- **Decision**: A scratch script imports `tests/regression_runner.py`, renders the 13 scenes in [evidence-record.md](contracts/evidence-record.md) through the harness's own `render_item`, and compares with `compare_exports`.
- **Rationale**: `--self-parity` alone can't answer FR-007. It compares single-thread runs with the first all-thread run, so on today's code it can't say whether one-thread renders match each other. The script needs only a few lines, and it reuses the harness's isolation and comparison exactly. Three builds separate the causes: thread control alone, then constant random values, then constant values with the blend on one thread.
- **If a cause remains unexplained**: A ThreadSanitizer build lists data races, and `valgrind` finds reads of uninitialized values, such as the one in R9. Both run on the small harness resolution. They are tools for FR-023, and neither is part of a pass.

## R11. Callers outside the render

- **Decision**: Each gets its own local state, and `Random()` is removed.
- **Rationale**: FR-015 names two such callers, but a search of `mandelbulber2/qt` finds two more files, so the spec is corrected. The gradient editor has 4 calls in two functions, and the second function already has a local `cRandom`. The thumbnail widget has 5 calls that spread out timers, and `instanceIndex` makes each widget's sequence different with no shared state. The loop in `opencl_engine_render_dof.cpp:177` runs in parallel and calls `Random()` today, so one generator per entry also removes that race.

## R12. Pull request and commit order

- **Decision**: One branch and one pull request, with each step in its own commit: thread control and the harness setting, then the evidence, the random values, the blend, the statistics, and the closing work.
- **Rationale**: Workflow rule 1 asks that closing KD-1 lands as its own reviewed change, and nothing here is a refactor, so rule 4 doesn't split it. SC-004 needs two builds on either side of the statistics work, which separate commits give. Reviewers can follow the same order.

## Spec changes from this plan

| Requirement | Change | Why |
|---|---|---|
| FR-005 | Adds that `--help` and `--version` still print and exit 0 | R2 |
| FR-012, Key Entities | Adds the eye to a value's position | R4 |
| FR-014 | Limits the rule to a render's random values | The GUI already seeds `cRandom` from the time in `dock_effects.cpp`, `gradient_edit_widget.cpp`, and `randomizer_dialog.cpp`, none of it in a render |
| FR-015 | Lists all four callers outside the gated path | R11 |
| Assumptions | Replaces the depth of field timing with measured results, and removes the upper limit claim | R6 |
| Assumptions | Adds the limit box exit | R9 |
| SC-012 | No longer fixes the test count at 8 | The known-answer test makes 9 |
