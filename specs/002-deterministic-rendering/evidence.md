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
| Random values, commit `539efacad` | 1 | 0 of 13 | 0 of 13 |
| Random values, commit `539efacad` | All | 3 of 13: `IFS_002`, `amazing_surf 002`, `menger octo 001` | 13 of 13 |
| One-thread blend, commit `8ce2ec0b7`, all 26 scenes | 1 | 0 of 26 | 0 of 26 |
| One-thread blend, commit `8ce2ec0b7`, all 26 scenes | All | 0 of 26, and 0 of 26 between 1 thread and all threads | 26 of 26 |
| Exact statistics, commit `ec238daf1`, all 26 scenes | 1 | 0 of 26 | 0 of 26 |
| Exact statistics, commit `ec238daf1`, all 26 scenes | All | 0 of 26, and 0 of 26 between 1 thread and all threads | 0 of 26, and 0 of 26 between 1 thread and all threads |

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
| IFS_001 | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,151,595; step histogram total 76,800 |
| Makin3D-Julia_001 | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,473; iteration histogram total 19,880,974; step histogram total 76,800 |
| chromatic_aberration | Random values | 1 | none | none | total iterations 0; raymarch count 2,875,502; missed DE 80,952; iteration histogram total 142,123,742; step histogram total 2,875,502 |
| difs_tree | Random values | 1 | none | none | total iterations 0; raymarch count 545,089; missed DE 84,678; iteration histogram total 44,330,525; step histogram total 545,089 |
| IFS_002 | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,910,173; step histogram total 76,800 |
| amazing_surf 002 | Random values | 1 | none | none | total iterations 0; raymarch count 88,048; missed DE 97; iteration histogram total 6,702,645; step histogram total 88,048 |
| EXR example | Random values | 1 | none | none | total iterations 0; raymarch count 843,698; missed DE 0; iteration histogram total 26,206,011; step histogram total 843,698 |
| benchmark-GPU | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,905; iteration histogram total 24,472,742; step histogram total 76,800 |
| gradients - specular highlights | Random values | 1 | none | none | total iterations 0; raymarch count 82,154; missed DE 0; iteration histogram total 1,547,857; step histogram total 82,154 |
| menger octo 001 | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,817; step histogram total 76,800 |
| clouds 005 | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,583,511; step histogram total 76,800 |
| beam_light | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| hdr_blur | Random values | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| IFS_001 | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,717 to 76,754; missed DE 0; iteration histogram total 2,150,149 to 2,150,344; step histogram total 76,795 to 76,798 |
| Makin3D-Julia_001 | Random values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,809 to 75,983; missed DE 19,377 to 19,382; iteration histogram total 17,923,764 to 18,162,832; step histogram total 76,799 to 76,800 |
| chromatic_aberration | Random values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 2,790,699 to 2,810,503; missed DE 80,873 to 80,885; iteration histogram total 135,917,308 to 136,271,532; step histogram total 2,872,932 to 2,873,552 |
| difs_tree | Random values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 542,768 to 542,881; missed DE 84,622 to 84,631; iteration histogram total 42,820,095 to 43,136,133; step histogram total 545,054 to 545,068 |
| IFS_002 | Random values | All | `post_effect` 19,422, `final_image` 16,642, `alpha` 94 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,905 to 74,240; missed DE 0; iteration histogram total 1,871,467 to 1,875,051; step histogram total 76,715 to 76,732 |
| amazing_surf 002 | Random values | All | `post_effect` 31,824, `final_image` 28,400, `alpha` 147 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 87,747 to 87,752; missed DE 97; iteration histogram total 6,694,684 to 6,695,481; step histogram total 88,042 to 88,045 |
| EXR example | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 819,981 to 827,312; missed DE 0; iteration histogram total 25,757,318 to 25,810,578; step histogram total 842,732 to 842,808 |
| benchmark-GPU | Random values | All | none | `raymarch_count`, `missed_de`, `iteration_histogram` | total iterations 0; raymarch count 76,561 to 76,609; missed DE 19,891 to 19,900; iteration histogram total 24,331,017 to 24,341,774; step histogram total 76,800 |
| gradients - specular highlights | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,297 to 75,491; missed DE 0; iteration histogram total 1,492,850 to 1,500,531; step histogram total 81,405 to 81,484 |
| menger octo 001 | Random values | All | `post_effect` 106,458, `final_image` 92,142, `alpha` 245 | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,571 to 76,662; missed DE 0; iteration histogram total 1,836,545 to 1,836,652; step histogram total 76,794 to 76,796 |
| clouds 005 | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,270 to 76,319; missed DE 0; iteration histogram total 2,570,678 to 2,571,360; step histogram total 76,788 to 76,793 |
| beam_light | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 60,248 to 60,445; missed DE 0; iteration histogram total 1,126,856 to 1,138,940; step histogram total 74,503 to 74,582 |
| hdr_blur | Random values | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 63,279 to 63,952; missed DE 0; iteration histogram total 846,354 to 906,917; step histogram total 76,368 to 76,409 |
| IFS_001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,151,595; step histogram total 76,800 |
| Makin3D-Julia_001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,473; iteration histogram total 19,880,974; step histogram total 76,800 |
| IFS_002 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,910,173; step histogram total 76,800 |
| amazing_surf 002 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 88,048; missed DE 97; iteration histogram total 6,702,645; step histogram total 88,048 |
| EXR example | One-thread blend | 1 | none | none | total iterations 0; raymarch count 843,698; missed DE 0; iteration histogram total 26,206,011; step histogram total 843,698 |
| benchmark-GPU | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 19,905; iteration histogram total 24,472,742; step histogram total 76,800 |
| menger octo 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,817; step histogram total 76,800 |
| clouds 005 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 2,583,511; step histogram total 76,800 |
| gradients - specular highlights | One-thread blend | 1 | none | none | total iterations 0; raymarch count 82,154; missed DE 0; iteration histogram total 1,547,857; step histogram total 82,154 |
| hybrid007 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 229; iteration histogram total 3,334,627; step histogram total 76,800 |
| hybrid008 - collatz | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 140; iteration histogram total 3,343,467; step histogram total 76,800 |
| mandelbox_menger | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 39; iteration histogram total 2,575,645; step histogram total 76,800 |
| mandelbulb_eye | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 14,627; iteration histogram total 3,096,687; step histogram total 76,800 |
| menger cross mod1 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 9,042; iteration histogram total 1,596,727; step histogram total 76,800 |
| menger prism shape2 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 822; iteration histogram total 2,001,895; step histogram total 76,800 |
| pseudo kleinian std de 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 5,388,951; step histogram total 76,800 |
| voxel_export_with_Bristorbrot4D | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 2; iteration histogram total 2,823,286; step histogram total 76,800 |
| beam_light | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| hdr_blur | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| jos_kleinian | One-thread blend | 1 | none | none | total iterations 0; raymarch count 76,800; missed DE 16; iteration histogram total 7,137,329; step histogram total 76,800 |
| chromatic_aberration | One-thread blend | 1 | none | none | total iterations 0; raymarch count 2,875,502; missed DE 80,952; iteration histogram total 142,123,742; step histogram total 2,875,502 |
| difs_tree | One-thread blend | 1 | none | none | total iterations 0; raymarch count 545,089; missed DE 84,678; iteration histogram total 44,330,525; step histogram total 545,089 |
| hex grid 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 595,846; missed DE 5,066; iteration histogram total 64,417,166; step histogram total 595,846 |
| mandelnest pow 5 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 580,904; missed DE 100; iteration histogram total 16,312,243; step histogram total 580,904 |
| monte carlo DOF 001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 609,015; missed DE 0; iteration histogram total 12,282,022; step histogram total 609,015 |
| riemann_sphere_msltoe_001 | One-thread blend | 1 | none | none | total iterations 0; raymarch count 539,928; missed DE 1,702; iteration histogram total 63,152,683; step histogram total 539,928 |
| IFS_001 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,714 to 76,732; missed DE 0; iteration histogram total 2,150,148 to 2,150,353; step histogram total 76,795 to 76,799 |
| Makin3D-Julia_001 | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,796 to 75,934; missed DE 19,370 to 19,379; iteration histogram total 17,840,554 to 17,921,344; step histogram total 76,798 to 76,799 |
| IFS_002 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,861 to 74,077; missed DE 0; iteration histogram total 1,870,734 to 1,873,657; step histogram total 76,714 to 76,721 |
| amazing_surf 002 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 87,723 to 87,757; missed DE 97; iteration histogram total 6,694,878 to 6,695,684; step histogram total 88,041 to 88,045 |
| EXR example | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 821,291 to 828,014; missed DE 0; iteration histogram total 25,647,801 to 25,853,845; step histogram total 842,779 to 842,815 |
| benchmark-GPU | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram` | total iterations 0; raymarch count 76,583 to 76,612; missed DE 19,890 to 19,894; iteration histogram total 24,340,053 to 24,340,786; step histogram total 76,800 |
| menger octo 001 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,529 to 76,586; missed DE 0; iteration histogram total 1,836,422 to 1,836,886; step histogram total 76,795 to 76,797 |
| clouds 005 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 76,287 to 76,330; missed DE 0; iteration histogram total 2,570,250 to 2,571,333; step histogram total 76,791 to 76,795 |
| gradients - specular highlights | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,351 to 75,587; missed DE 0; iteration histogram total 1,492,655 to 1,503,904; step histogram total 81,346 to 81,387 |
| hybrid007 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,915 to 74,244; missed DE 229; iteration histogram total 3,258,750 to 3,263,805; step histogram total 76,663 to 76,677 |
| hybrid008 - collatz | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,416 to 75,463; missed DE 140; iteration histogram total 3,326,263 to 3,327,962; step histogram total 76,778 to 76,787 |
| mandelbox_menger | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 72,394 to 73,400; missed DE 39; iteration histogram total 2,060,991 to 2,121,267; step histogram total 76,762 to 76,768 |
| mandelbulb_eye | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 67,100 to 67,305; missed DE 14,295 to 14,352; iteration histogram total 2,676,266 to 2,704,934; step histogram total 76,444 to 76,499 |
| menger cross mod1 001 | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 75,335 to 75,368; missed DE 9,026 to 9,034; iteration histogram total 1,590,723 to 1,592,056; step histogram total 76,729 to 76,741 |
| menger prism shape2 001 | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 73,467 to 73,935; missed DE 821 to 822; iteration histogram total 1,959,865 to 1,963,717; step histogram total 76,519 to 76,578 |
| pseudo kleinian std de 001 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 74,799 to 74,852; missed DE 0; iteration histogram total 4,932,940 to 4,985,999; step histogram total 76,626 to 76,650 |
| voxel_export_with_Bristorbrot4D | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 65,833 to 66,027; missed DE 2; iteration histogram total 2,390,611 to 2,493,886; step histogram total 76,651 to 76,675 |
| beam_light | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 59,820 to 60,345; missed DE 0; iteration histogram total 1,111,425 to 1,132,267; step histogram total 74,383 to 74,596 |
| hdr_blur | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 63,412 to 64,813; missed DE 0; iteration histogram total 846,816 to 854,795; step histogram total 76,360 to 76,394 |
| jos_kleinian | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 72,365 to 72,520; missed DE 16; iteration histogram total 6,426,602 to 6,501,472; step histogram total 76,781 to 76,785 |
| chromatic_aberration | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 2,784,000 to 2,793,169; missed DE 80,853 to 80,868; iteration histogram total 135,431,945 to 136,173,622; step histogram total 2,873,245 to 2,873,293 |
| difs_tree | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 542,672 to 542,769; missed DE 84,626 to 84,636; iteration histogram total 42,670,030 to 42,735,388; step histogram total 545,045 to 545,055 |
| hex grid 001 | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 589,216 to 589,572; missed DE 5,064 to 5,066; iteration histogram total 61,058,905 to 61,358,866; step histogram total 595,825 to 595,835 |
| mandelnest pow 5 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 576,305 to 576,631; missed DE 100; iteration histogram total 16,178,139 to 16,205,071; step histogram total 580,847 to 580,861 |
| monte carlo DOF 001 | One-thread blend | All | none | `raymarch_count`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 558,744 to 564,635; missed DE 0; iteration histogram total 11,942,603 to 12,017,579; step histogram total 604,407 to 604,736 |
| riemann_sphere_msltoe_001 | One-thread blend | All | none | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | total iterations 0; raymarch count 536,702 to 537,534; missed DE 1,701 to 1,702; iteration histogram total 59,850,789 to 60,229,524; step histogram total 539,875 to 539,885 |
| IFS_001 | Exact statistics | 1 | none | none | total iterations 776,608,889; raymarch count 76,800; missed DE 0; iteration histogram total 2,151,595; step histogram total 76,800 |
| Makin3D-Julia_001 | Exact statistics | 1 | none | none | total iterations 192,226,038; raymarch count 76,800; missed DE 19,473; iteration histogram total 19,880,974; step histogram total 76,800 |
| IFS_002 | Exact statistics | 1 | none | none | total iterations 120,032,867; raymarch count 76,800; missed DE 0; iteration histogram total 1,910,173; step histogram total 76,800 |
| amazing_surf 002 | Exact statistics | 1 | none | none | total iterations 548,994,432; raymarch count 88,048; missed DE 97; iteration histogram total 6,702,645; step histogram total 88,048 |
| EXR example | Exact statistics | 1 | none | none | total iterations 346,917,048; raymarch count 843,698; missed DE 0; iteration histogram total 26,206,011; step histogram total 843,698 |
| benchmark-GPU | Exact statistics | 1 | none | none | total iterations 1,528,427,187; raymarch count 76,800; missed DE 19,905; iteration histogram total 24,472,742; step histogram total 76,800 |
| menger octo 001 | Exact statistics | 1 | none | none | total iterations 187,424,425; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,817; step histogram total 76,800 |
| clouds 005 | Exact statistics | 1 | none | none | total iterations 12,867,858; raymarch count 76,800; missed DE 0; iteration histogram total 2,583,511; step histogram total 76,800 |
| gradients - specular highlights | Exact statistics | 1 | none | none | total iterations 15,149,503; raymarch count 82,154; missed DE 0; iteration histogram total 1,547,857; step histogram total 82,154 |
| hybrid007 | Exact statistics | 1 | none | none | total iterations 67,875,780; raymarch count 76,800; missed DE 229; iteration histogram total 3,334,627; step histogram total 76,800 |
| hybrid008 - collatz | Exact statistics | 1 | none | none | total iterations 251,672,458; raymarch count 76,800; missed DE 140; iteration histogram total 3,343,467; step histogram total 76,800 |
| mandelbox_menger | Exact statistics | 1 | none | none | total iterations 20,613,096; raymarch count 76,800; missed DE 39; iteration histogram total 2,575,645; step histogram total 76,800 |
| mandelbulb_eye | Exact statistics | 1 | none | none | total iterations 10,810,976; raymarch count 76,800; missed DE 14,627; iteration histogram total 3,096,687; step histogram total 76,800 |
| menger cross mod1 001 | Exact statistics | 1 | none | none | total iterations 184,331,872; raymarch count 76,800; missed DE 9,042; iteration histogram total 1,596,727; step histogram total 76,800 |
| menger prism shape2 001 | Exact statistics | 1 | none | none | total iterations 25,315,362; raymarch count 76,800; missed DE 822; iteration histogram total 2,001,895; step histogram total 76,800 |
| pseudo kleinian std de 001 | Exact statistics | 1 | none | none | total iterations 154,552,620; raymarch count 76,800; missed DE 0; iteration histogram total 5,388,951; step histogram total 76,800 |
| voxel_export_with_Bristorbrot4D | Exact statistics | 1 | none | none | total iterations 17,801,010; raymarch count 76,800; missed DE 2; iteration histogram total 2,823,286; step histogram total 76,800 |
| beam_light | Exact statistics | 1 | none | none | total iterations 1,705,711; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| hdr_blur | Exact statistics | 1 | none | none | total iterations 1,632,271; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| jos_kleinian | Exact statistics | 1 | none | none | total iterations 34,280,567; raymarch count 76,800; missed DE 16; iteration histogram total 7,137,329; step histogram total 76,800 |
| chromatic_aberration | Exact statistics | 1 | none | none | total iterations 4,192,032,600; raymarch count 2,875,502; missed DE 80,952; iteration histogram total 142,123,742; step histogram total 2,875,502 |
| difs_tree | Exact statistics | 1 | none | none | total iterations 169,820,519; raymarch count 545,089; missed DE 84,678; iteration histogram total 44,330,525; step histogram total 545,089 |
| hex grid 001 | Exact statistics | 1 | none | none | total iterations 908,958,716; raymarch count 595,846; missed DE 5,066; iteration histogram total 64,417,166; step histogram total 595,846 |
| mandelnest pow 5 | Exact statistics | 1 | none | none | total iterations 277,045,196; raymarch count 580,904; missed DE 100; iteration histogram total 16,312,243; step histogram total 580,904 |
| monte carlo DOF 001 | Exact statistics | 1 | none | none | total iterations 159,662,705; raymarch count 609,015; missed DE 0; iteration histogram total 12,282,022; step histogram total 609,015 |
| riemann_sphere_msltoe_001 | Exact statistics | 1 | none | none | total iterations 641,217,000; raymarch count 539,928; missed DE 1,702; iteration histogram total 63,152,683; step histogram total 539,928 |
| IFS_001 | Exact statistics | All | none | none | total iterations 776,608,889; raymarch count 76,800; missed DE 0; iteration histogram total 2,151,595; step histogram total 76,800 |
| Makin3D-Julia_001 | Exact statistics | All | none | none | total iterations 192,226,038; raymarch count 76,800; missed DE 19,473; iteration histogram total 19,880,974; step histogram total 76,800 |
| IFS_002 | Exact statistics | All | none | none | total iterations 120,032,867; raymarch count 76,800; missed DE 0; iteration histogram total 1,910,173; step histogram total 76,800 |
| amazing_surf 002 | Exact statistics | All | none | none | total iterations 548,994,432; raymarch count 88,048; missed DE 97; iteration histogram total 6,702,645; step histogram total 88,048 |
| EXR example | Exact statistics | All | none | none | total iterations 346,917,048; raymarch count 843,698; missed DE 0; iteration histogram total 26,206,011; step histogram total 843,698 |
| benchmark-GPU | Exact statistics | All | none | none | total iterations 1,528,427,187; raymarch count 76,800; missed DE 19,905; iteration histogram total 24,472,742; step histogram total 76,800 |
| menger octo 001 | Exact statistics | All | none | none | total iterations 187,424,425; raymarch count 76,800; missed DE 0; iteration histogram total 1,839,817; step histogram total 76,800 |
| clouds 005 | Exact statistics | All | none | none | total iterations 12,867,858; raymarch count 76,800; missed DE 0; iteration histogram total 2,583,511; step histogram total 76,800 |
| gradients - specular highlights | Exact statistics | All | none | none | total iterations 15,149,503; raymarch count 82,154; missed DE 0; iteration histogram total 1,547,857; step histogram total 82,154 |
| hybrid007 | Exact statistics | All | none | none | total iterations 67,875,780; raymarch count 76,800; missed DE 229; iteration histogram total 3,334,627; step histogram total 76,800 |
| hybrid008 - collatz | Exact statistics | All | none | none | total iterations 251,672,458; raymarch count 76,800; missed DE 140; iteration histogram total 3,343,467; step histogram total 76,800 |
| mandelbox_menger | Exact statistics | All | none | none | total iterations 20,613,096; raymarch count 76,800; missed DE 39; iteration histogram total 2,575,645; step histogram total 76,800 |
| mandelbulb_eye | Exact statistics | All | none | none | total iterations 10,810,976; raymarch count 76,800; missed DE 14,627; iteration histogram total 3,096,687; step histogram total 76,800 |
| menger cross mod1 001 | Exact statistics | All | none | none | total iterations 184,331,872; raymarch count 76,800; missed DE 9,042; iteration histogram total 1,596,727; step histogram total 76,800 |
| menger prism shape2 001 | Exact statistics | All | none | none | total iterations 25,315,362; raymarch count 76,800; missed DE 822; iteration histogram total 2,001,895; step histogram total 76,800 |
| pseudo kleinian std de 001 | Exact statistics | All | none | none | total iterations 154,552,620; raymarch count 76,800; missed DE 0; iteration histogram total 5,388,951; step histogram total 76,800 |
| voxel_export_with_Bristorbrot4D | Exact statistics | All | none | none | total iterations 17,801,010; raymarch count 76,800; missed DE 2; iteration histogram total 2,823,286; step histogram total 76,800 |
| beam_light | Exact statistics | All | none | none | total iterations 1,705,711; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| hdr_blur | Exact statistics | All | none | none | total iterations 1,632,271; raymarch count 76,800; missed DE 0; iteration histogram total 1,478,359; step histogram total 76,800 |
| jos_kleinian | Exact statistics | All | none | none | total iterations 34,280,567; raymarch count 76,800; missed DE 16; iteration histogram total 7,137,329; step histogram total 76,800 |
| chromatic_aberration | Exact statistics | All | none | none | total iterations 4,192,032,600; raymarch count 2,875,502; missed DE 80,952; iteration histogram total 142,123,742; step histogram total 2,875,502 |
| difs_tree | Exact statistics | All | none | none | total iterations 169,820,519; raymarch count 545,089; missed DE 84,678; iteration histogram total 44,330,525; step histogram total 545,089 |
| hex grid 001 | Exact statistics | All | none | none | total iterations 908,958,716; raymarch count 595,846; missed DE 5,066; iteration histogram total 64,417,166; step histogram total 595,846 |
| mandelnest pow 5 | Exact statistics | All | none | none | total iterations 277,045,196; raymarch count 580,904; missed DE 100; iteration histogram total 16,312,243; step histogram total 580,904 |
| monte carlo DOF 001 | Exact statistics | All | none | none | total iterations 159,662,705; raymarch count 609,015; missed DE 0; iteration histogram total 12,282,022; step histogram total 609,015 |
| riemann_sphere_msltoe_001 | Exact statistics | All | none | none | total iterations 641,217,000; raymarch count 539,928; missed DE 1,702; iteration histogram total 63,152,683; step histogram total 539,928 |

## Causes

| Cause | Scenes | Effect | Proof | Fix |
|---|---|---|---|---|
| All threads share one random state. `RandomInt` reads and writes `gRandomSeed` at `common_math.cpp:47` with no lock, and every worker draws from it, including the raymarch step jitter at `render_worker.cpp:822` and `:826`. | All 13, at all threads | Every buffer except sometimes `alpha`: up to all 230,400 samples of `raw_render` | Every scene differs on the thread control build. With constant values, 10 of the 13 match in every buffer, and the other 3 differ only in the buffers the next cause explains. | FR-012 to FR-015 |
| The depth of field blend runs in thread order. The parallel loop at `dof.cpp:284` blends overlapping discs into the shared image, and a blend gives a different result in a different order. | `IFS_002`, `amazing_surf 002`, `menger octo 001`, at all threads | `post_effect`, `final_image`, and `alpha` | These 3 scenes still differ with constant values, and no scene differs in any buffer once that `pragma` is removed. | FR-022 |
| All threads update one statistics object with no synchronization, at `render_worker.cpp:803`, `:810`, `:918`, `:954`, and `:961` and in the shaders. Updates get lost. | All 13, at all threads | `raymarch_count`, `missed_de`, `iteration_histogram`, `step_histogram` | Statistics match at one thread, and still differ when no random value and no blend depends on thread order. At all threads the raymarch count falls below the exact one-thread count: `IFS_001` reads 76,609 to 76,651 against 76,800, and `beam_light` 61,888 to 62,220 against 76,800. | FR-024, FR-027 |
| Two threads can render one line, because `cScheduler::InitFirstLine` claims a line with no lock and no check. | Not isolated by these runs | The statistics of that line count twice | These runs can't separate it from lost updates, which pull the counts the other way. A ThreadSanitizer run at 32x24 on 2026-11-07 reported races in `cImage::PutPixelImage`, which only happen when two threads write one pixel. The statistics work must prove it gone. | FR-025 |
| Total iterations stays at 0. `CalculateDistanceFromObjectsTree` passes a local `nodeOut` to `CalculateDistanceSimple` and copies only `nodeOut.iters`. | All 13, at both thread counts | `total_iterations` reads 0, so it hides any difference | The value is 0 in every render of every scene and build. | FR-026 |

Every difference in Results traces to one of these causes. The limit box exit in the spec's Assumptions stays unmeasured, because no suite scene turns on `limits_enabled`.

## Order audit

Each region is one a gated render can reach. The proof for each comes from commit `8ce2ec0b7`, where all 26 scenes matched in every buffer across three renders at each thread count, and between 1 thread and all 32.

| Region | Reads | Writes | Verdict | Scene | Proof | Result |
|---|---|---|---|---|---|---|
| `dof.cpp:101`, depth of field phase I | The post-effect image and the z-buffer, which do not change during the region | Its own entry of `temp_image`, which no other thread reaches | Does not depend on order | `IFS_002.fract` | Every buffer matched at 1 thread and at all threads | Stays parallel |
| `dof.cpp:284`, depth of field phase II | The post-effect image, which it changes | Neighboring pixels of the post-effect image and alpha, which two threads can reach at once | Depends on order | `IFS_002.fract` | It differed at all threads with constant values, and every buffer matched once the loop ran on one thread | Moves to one thread |
| `post_effect_hdr_blur.cpp:78`, HDR blur | A snapshot of the post-effect image in `tempImage`, which does not change during the region | Its own pixel, which no other thread reaches | Does not depend on order | `tests/scenes/hdr_blur.fract` | Every buffer matched at 1 thread and at all threads | Stays parallel |

`audio_track.cpp:302` is outside the gate, because it needs a scene with audio tracks, and no suite scene has them.

## Other checks

| Check | Result | Commit |
|---|---|---|
| `--stats` keeps updating during a render (FR-029) | A 1920x1080 render of `IFS_001.fract` printed 43 different totals, from 144,771,000 to 27,265,100,000 iterations | `ec238daf1` |
| Colliding updates (FR-027) | ThreadSanitizer gave 7 reports on `IFS_001.fract` and 1 on `chromatic_aberration.fract`, at 32x24 with `--threads 4`. None names `cStatistics`, `cHistogram`, or `cPositionRandom`. Six are inside Qt, which isn't built for ThreadSanitizer, and two are the scheduler's unlocked reads of `lineDone`, which the plan expects. | `ec238daf1` |

## Success criteria

| Criterion | Result | Commit | Not covered |
|---|---|---|---|
| SC-005 | At `--threads 1`, the highest one-second sample was 0.97 to 0.99 CPUs in every row: render workers, ambient occlusion, depth of field, HDR blur, keyframe, flight, voxel, mesh, queue, `--test`, and `--benchmark`. The post effects added 2.84, 9.15, and 3.00 seconds of CPU time. At `--threads 4`, each stage peaked between 3.73 and 3.89 CPUs. A network client ran one `RenderWorker` thread and never more. | `0bd701a04` | Repeated on the final commit in T084 |
| SC-006 | All 17 refusal rows exited with the listed status, named `--threads`, and wrote no image | `0bd701a04` | Repeated on the final commit in T084 |
| SC-004 | `--compare` between `71a20a6da` and `ec238daf1` reported 5,126 differences across all 487 items, all of them statistics, and 0 in any buffer. It exits 1, because total iterations changes from 0 to the real count. | `ec238daf1` | None |
