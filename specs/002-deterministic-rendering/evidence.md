# Evidence: Deterministic CPU Rendering

This record says why CPU renders differed, and how each OpenMP region behaves. [contracts/evidence-record.md](contracts/evidence-record.md) defines it. Every number here comes from a fresh build of the commit named beside it.

## Header

| Field | Value |
|---|---|
| Date | 2026-11-07 |
| Computer | Intel Core i9-14900K, 32 logical CPUs |
| Environment | WSL `Ubuntu-24.04`, kernel 6.18.54.1 |
| Toolchain | GCC 13.3.0, glibc 2.39, Qt 6.4.2, `mandelbulber-opencl.pro` in release mode |
| Thread counts | 1, and all 32 |
| Commit of step 1 | `0bd701a0469a91895ce173c0c4c2bbb2289ba77b` |

## Builds

| Name | Source | Purpose |
|---|---|---|
| Thread control | Commit `0bd701a04`, which adds `--threads` | Whether one-thread renders reproduce today |
| Constant values | The same commit, with a local change that makes `Random(max)` return `max / 2` | What still differs at all threads when random values can't depend on thread order |
| Constant values, one-thread blend | The constant values build, with the `pragma` on depth of field phase II also removed | Whether the blend explains what is left in the post-effect buffers |

The two local changes stayed in a throwaway clone and were never committed.

## Scenes

The record covers these scenes at the harness's resolution and overrides.

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

## Summary

| Build | Thread count | Scenes with a buffer difference | Scenes with a statistics difference |
|---|---|---|---|
| Thread control | 1 | 0 of 13 | 0 of 13 |
| Thread control | All | 13 of 13 | 13 of 13 |
| Constant values | 1 | 0 of 13 | 0 of 13 |
| Constant values | All | 3 of 13: `IFS_002`, `amazing_surf 002`, `menger octo 001` | 13 of 13 |
| Constant values, one-thread blend | All | 0 of 13 | 13 of 13 |

**One-thread renders reproduced before the fix.** All 13 scenes matched in every buffer and statistic across three renders, on both builds.

**The 30 percent figure does not repeat.** Total iterations read 0 in all 13 scenes, at both thread counts, so it had no spread at all. The widest spread of any statistic was 13.3%, the iteration histogram total of `beam_light` on the constant values build, from 994,587 to 1,146,763.

## Results

Each buffer count is the most samples that differed between the first render and a later one. Spread is the lowest and highest value across the renders.

| Scene | Build | Threads | Buffers that differ | Statistics that differ | Spread |
|---|---|---|---|---|---|
| IFS_001 | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,150,042; step histogram total 76,800 |
| Makin3D-Julia_001 | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,460; iteration histogram total 19,882,324; step histogram total 76,800 |
| chromatic_aberration | Thread control | 1 | none | none | total iterations 0; raymarch count 2,874,023; missed DE 81,225; iteration histogram total 142,071,682; step histogram total 2,874,023 |
| difs_tree | Thread control | 1 | none | none | total iterations 0; raymarch count 545,076; missed DE 84,837; iteration histogram total 44,331,237; step histogram total 545,076 |
| IFS_002 | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,908,647; step histogram total 76,800 |
| amazing_surf 002 | Thread control | 1 | none | none | total iterations 0; raymarch count 88,048; missed DE 99; iteration histogram total 6,702,987; step histogram total 88,048 |
| EXR example | Thread control | 1 | none | none | total iterations 0; raymarch count 843,720; missed DE 0; iteration histogram total 26,207,454; step histogram total 843,720 |
| benchmark-GPU | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,890; iteration histogram total 24,474,281; step histogram total 76,800 |
| gradients - specular highlights | Thread control | 1 | none | none | total iterations 0; raymarch count 82,154; missed DE 0; iteration histogram total 1,549,107; step histogram total 82,154 |
| menger octo 001 | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,280; step histogram total 76,800 |
| clouds 005 | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,584,385; step histogram total 76,800 |
| beam_light | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,218; step histogram total 76,800 |
| hdr_blur | Thread control | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,386; step histogram total 76,800 |
| IFS_001 | Thread control | All | `raw_render` 230,396, `post_effect` 230,396, `final_image` 214,769, `depth` 75,685, `normal` 228,922, `normal_world` 227,709 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,609 to 76,651; missed DE 0; iteration histogram total 2,149,781 to 2,150,525; step histogram total 76,798 to 76,800 |
| Makin3D-Julia_001 | Thread control | All | `raw_render` 210,798, `post_effect` 210,796, `final_image` 203,644, `alpha` 30,494, `depth` 77, `normal` 263, `normal_world` 405 | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,255 to 76,269; missed DE 19,378 to 19,399; iteration histogram total 19,316,036 to 19,378,264; step histogram total 76,798 to 76,800 |
| chromatic_aberration | Thread control | All | `raw_render` 230,363, `post_effect` 230,363, `final_image` 228,185, `depth` 76,800, `normal` 229,010, `normal_world` 229,350 | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 2,833,310 to 2,836,255; missed DE 81,129 to 81,142; iteration histogram total 135,877,137 to 136,882,381; step histogram total 2,868,239 to 2,871,667 |
| difs_tree | Thread control | All | `raw_render` 230,333, `post_effect` 230,333, `final_image` 220,826, `alpha` 33,067, `depth` 43,410, `normal` 130,230, `normal_world` 130,230 | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 533,712 to 533,839; missed DE 84,256 to 84,487; iteration histogram total 41,821,397 to 41,957,846; step histogram total 544,860 to 544,930 |
| IFS_002 | Thread control | All | `raw_render` 230,396, `post_effect` 230,398, `final_image` 226,734, `alpha` 16,106, `depth` 75,742, `normal` 229,212, `normal_world` 229,075 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,039 to 76,148; missed DE 0; iteration histogram total 1,893,686 to 1,896,569; step histogram total 76,756 to 76,763 |
| amazing_surf 002 | Thread control | All | `raw_render` 230,395, `post_effect` 230,399, `final_image` 229,397, `alpha` 16,698, `depth` 72,489, `normal` 218,019, `normal_world` 218,499 | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 87,070 to 87,204; missed DE 83 to 90; iteration histogram total 6,683,799 to 6,685,366; step histogram total 88,041 to 88,044 |
| EXR example | Thread control | All | `raw_render` 230,396, `post_effect` 230,396, `final_image` 222,755, `alpha` 4, `depth` 76,456, `normal` 115,892, `normal_world` 187,816 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 733,086 to 734,164; missed DE 0; iteration histogram total 24,431,050 to 25,159,786; step histogram total 841,878 to 841,990 |
| benchmark-GPU | Thread control | All | `raw_render` 230,396, `post_effect` 230,396, `final_image` 228,727, `alpha` 56,553, `depth` 921, `normal` 2,953, `normal_world` 3,567 | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,202 to 76,315; missed DE 19,863 to 19,873; iteration histogram total 24,274,018 to 24,289,080; step histogram total 76,797 to 76,800 |
| gradients - specular highlights | Thread control | All | `raw_render` 190,447, `post_effect` 190,447, `final_image` 156,637, `depth` 75,550, `normal` 212,986, `normal_world` 201,667 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,003 to 75,328; missed DE 0; iteration histogram total 1,481,192 to 1,496,992; step histogram total 81,288 to 81,414 |
| menger octo 001 | Thread control | All | `raw_render` 230,396, `post_effect` 230,400, `final_image` 229,620, `alpha` 13,647, `depth` 75,671, `normal` 229,531, `normal_world` 229,403 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,248 to 76,372; missed DE 0; iteration histogram total 1,836,761 to 1,837,183; step histogram total 76,795 to 76,800 |
| clouds 005 | Thread control | All | `raw_render` 230,398, `post_effect` 230,398, `final_image` 194,220, `depth` 75,790, `normal` 49,646, `normal_world` 45,760 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,310 to 76,346; missed DE 0; iteration histogram total 2,572,921 to 2,573,824; step histogram total 76,786 to 76,789 |
| beam_light | Thread control | All | `raw_render` 230,400, `post_effect` 230,400, `final_image` 210,494, `alpha` 72,161, `depth` 4,450, `normal` 13,390, `normal_world` 13,414 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 61,888 to 62,220; missed DE 0; iteration histogram total 1,095,670 to 1,187,819; step histogram total 74,241 to 74,448 |
| hdr_blur | Thread control | All | `raw_render` 19,991, `post_effect` 129,110, `final_image` 34,432, `alpha` 5,497, `depth` 4,445, `normal` 13,386, `normal_world` 13,407 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 65,985 to 66,878; missed DE 0; iteration histogram total 918,778 to 978,234; step histogram total 76,266 to 76,333 |
| IFS_001 | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,150,982; step histogram total 76,800 |
| Makin3D-Julia_001 | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,473; iteration histogram total 19,881,667; step histogram total 76,800 |
| chromatic_aberration | Constant values | 1 | none | none | total iterations 0; raymarch count 2,278,512; missed DE 61,452; iteration histogram total 111,811,302; step histogram total 2,278,512 |
| difs_tree | Constant values | 1 | none | none | total iterations 0; raymarch count 460,800; missed DE 63,744; iteration histogram total 40,830,864; step histogram total 460,800 |
| IFS_002 | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,909,746; step histogram total 76,800 |
| amazing_surf 002 | Constant values | 1 | none | none | total iterations 0; raymarch count 88,050; missed DE 88; iteration histogram total 6,705,000; step histogram total 88,050 |
| EXR example | Constant values | 1 | none | none | total iterations 0; raymarch count 843,706; missed DE 0; iteration histogram total 26,208,343; step histogram total 843,706 |
| benchmark-GPU | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,900; iteration histogram total 24,473,013; step histogram total 76,800 |
| gradients - specular highlights | Constant values | 1 | none | none | total iterations 0; raymarch count 82,154; missed DE 0; iteration histogram total 1,546,131; step histogram total 82,154 |
| menger octo 001 | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,332; step histogram total 76,800 |
| clouds 005 | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,584,210; step histogram total 76,800 |
| beam_light | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,373; step histogram total 76,800 |
| hdr_blur | Constant values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,373; step histogram total 76,800 |
| IFS_001 | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,617 to 76,624; missed DE 0; iteration histogram total 2,149,364 to 2,149,850; step histogram total 76,799 to 76,800 |
| Makin3D-Julia_001 | Constant values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,165 to 75,226; missed DE 19,361 to 19,370; iteration histogram total 19,034,213 to 19,117,430; step histogram total 76,796 to 76,798 |
| chromatic_aberration | Constant values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 2,079,228 to 2,080,965; missed DE 61,322 to 61,325; iteration histogram total 101,918,599 to 102,642,717; step histogram total 2,269,763 to 2,270,118 |
| difs_tree | Constant values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 449,131 to 450,181; missed DE 63,569 to 63,594; iteration histogram total 37,739,163 to 38,171,051; step histogram total 460,591 to 460,648 |
| IFS_002 | Constant values | All | `post_effect` 45,012, `final_image` 39,062, `alpha` 352 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,756 to 74,193; missed DE 0; iteration histogram total 1,869,941 to 1,873,533; step histogram total 76,717 to 76,725 |
| amazing_surf 002 | Constant values | All | `post_effect` 98,964, `final_image` 94,371, `alpha` 2,447 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 87,837 to 87,858; missed DE 88; iteration histogram total 6,693,588 to 6,694,953; step histogram total 88,047 to 88,049 |
| EXR example | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 796,641 to 799,962; missed DE 0; iteration histogram total 25,805,256 to 25,849,275; step histogram total 841,711 to 841,911 |
| benchmark-GPU | Constant values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,218 to 76,332; missed DE 19,869 to 19,878; iteration histogram total 24,285,375 to 24,301,510; step histogram total 76,798 to 76,799 |
| gradients - specular highlights | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,343 to 75,687; missed DE 0; iteration histogram total 1,496,637 to 1,502,282; step histogram total 81,347 to 81,437 |
| menger octo 001 | Constant values | All | `post_effect` 136,799, `final_image` 129,692, `alpha` 690 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,497 to 76,612; missed DE 0; iteration histogram total 1,836,126 to 1,836,352; step histogram total 76,794 to 76,796 |
| clouds 005 | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,663 to 76,688; missed DE 0; iteration histogram total 2,580,570 to 2,581,281; step histogram total 76,790 to 76,797 |
| beam_light | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 59,917 to 60,313; missed DE 0; iteration histogram total 994,587 to 1,146,763; step histogram total 74,453 to 74,688 |
| hdr_blur | Constant values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 63,242 to 63,870; missed DE 0; iteration histogram total 1,279,208 to 1,294,787; step histogram total 74,011 to 74,146 |
| IFS_001 | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,614 to 76,638; missed DE 0; iteration histogram total 2,149,132 to 2,149,742; step histogram total 76,799 to 76,800 |
| Makin3D-Julia_001 | Constant values, one-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,261 to 75,280; missed DE 19,335 to 19,372; iteration histogram total 19,070,670 to 19,085,822; step histogram total 76,795 to 76,797 |
| chromatic_aberration | Constant values, one-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 2,087,752 to 2,090,662; missed DE 61,328 to 61,336; iteration histogram total 102,137,988 to 102,542,439; step histogram total 2,270,102 to 2,270,661 |
| difs_tree | Constant values, one-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 448,772 to 449,224; missed DE 63,523 to 63,577; iteration histogram total 37,656,833 to 37,952,784; step histogram total 460,576 to 460,606 |
| IFS_002 | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,746 to 74,124; missed DE 0; iteration histogram total 1,869,830 to 1,874,025; step histogram total 76,714 to 76,716 |
| amazing_surf 002 | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 87,855 to 87,869; missed DE 88; iteration histogram total 6,694,074 to 6,694,689; step histogram total 88,047 to 88,049 |
| EXR example | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 800,153 to 803,845; missed DE 0; iteration histogram total 25,760,262 to 25,872,950; step histogram total 841,935 to 841,997 |
| benchmark-GPU | Constant values, one-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,236 to 76,325; missed DE 19,869 to 19,881; iteration histogram total 24,261,338 to 24,290,252; step histogram total 76,799 to 76,800 |
| gradients - specular highlights | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 74,805 to 75,844; missed DE 0; iteration histogram total 1,490,363 to 1,505,487; step histogram total 81,338 to 81,521 |
| menger octo 001 | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,493 to 76,651; missed DE 0; iteration histogram total 1,836,057 to 1,836,263; step histogram total 76,792 to 76,800 |
| clouds 005 | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,677 to 76,688; missed DE 0; iteration histogram total 2,580,454 to 2,580,524; step histogram total 76,791 to 76,798 |
| beam_light | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 59,775 to 60,220; missed DE 0; iteration histogram total 1,110,709 to 1,113,351; step histogram total 74,447 to 74,580 |
| hdr_blur | Constant values, one-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 63,456 to 64,362; missed DE 0; iteration histogram total 1,286,653 to 1,306,511; step histogram total 74,037 to 74,067 |

## Causes

| Cause | Scenes | Effect | Proof | Fix |
|---|---|---|---|---|
| All threads share one random state. `RandomInt` reads and writes `gRandomSeed` at `common_math.cpp:47` with no lock, and every worker draws from it, including the raymarch step jitter at `render_worker.cpp:822` and `:826`. | All 13, at all threads | Every buffer except sometimes `alpha`: up to all 230,400 samples of `raw_render` | Every scene differs on the thread control build. With constant values, 10 of the 13 match in every buffer, and the other 3 differ only in the buffers the next cause explains. | FR-012 to FR-015 |
| The depth of field blend runs in thread order. The parallel loop at `dof.cpp:284` blends overlapping discs into the shared image, and a blend gives a different result in a different order. | `IFS_002`, `amazing_surf 002`, `menger octo 001`, at all threads | `post_effect`, `final_image`, and `alpha` | These 3 scenes still differ with constant values, and no scene differs in any buffer once that `pragma` is removed. | FR-022 |
| All threads update one statistics object with no synchronization, at `render_worker.cpp:803`, `:810`, `:918`, `:954`, and `:961` and in the shaders. Updates get lost. | All 13, at all threads | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | Statistics match at one thread, and still differ when no random value and no blend depends on thread order. At all threads the raymarch count falls below the exact one-thread count: `IFS_001` reads 76,609 to 76,651 against 76,800, and `beam_light` 61,888 to 62,220 against 76,800. | FR-024, FR-027 |
| Two threads can render one line, because `cScheduler::InitFirstLine` claims a line with no lock and no check. | Not isolated by these runs | The statistics of that line count twice | These runs can't separate it from lost updates, which pull the counts the other way. A ThreadSanitizer run at 32x24 on 2026-11-07 reported races in `cImage::PutPixelImage`, which only happen when two threads write one pixel. The statistics work must prove it gone. | FR-025 |
| Total iterations stays at 0. `CalculateDistanceFromObjectsTree` passes a local `nodeOut` to `CalculateDistanceSimple` and copies only `nodeOut.iters`. | All 13, at both thread counts | `total_iterations` reads 0, so it hides any difference | The value is 0 in every render of every scene and build. | FR-026 |

Every difference in Results traces to one of these causes. The limit box exit in the spec's Assumptions stays unmeasured, because no suite scene turns on `limits_enabled`.

## Success criteria

| Criterion | Result | Commit | Not covered |
|---|---|---|---|
| SC-005 | At `--threads 1`, the highest one-second sample was 0.97 to 0.99 CPUs in every row: render workers, ambient occlusion, depth of field, HDR blur, keyframe, flight, voxel, mesh, queue, `--test`, and `--benchmark`. The post effects added 2.84, 9.15, and 3.00 seconds of CPU time. At `--threads 4`, each stage peaked between 3.73 and 3.89 CPUs. A network client ran one `RenderWorker` thread and never more. | `0bd701a04` | Repeated on the final commit in T084 |
| SC-006 | All 17 refusal rows exited with the listed status, named `--threads`, and wrote no image | `0bd701a04` | Repeated on the final commit in T084 |
