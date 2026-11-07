# Contract: Random Values

How every random value in a CPU render is made. The generator uses only unsigned 64-bit integer arithmetic, so one seed and position give the same value on every build (FR-018).

## Scene setting

| Name | Type | Default | Range | Interpolation | Saved in scene files |
|---|---|---|---|---|---|
| `render_random_seed` | int | 1 | 0 to 2147483647 | None | Only when it differs from the default |

A scene that names no seed uses the default, with no edit to the scene file (FR-016). The default of 1 is the value `gRandomSeed` starts from today. The GUI gets no control for it.

## Algorithm

All arithmetic wraps at 64 bits.

```text
golden = 0x9E3779B97F4A7C15

mix(z):
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9
    z = (z ^ (z >> 27)) * 0x94D049BB133111EB
    return z ^ (z >> 31)

absorb(hash, value):
    return mix(hash + value + golden)

key(stage, seed, frame, eye):
    hash = 0
    for value in stage, seed, frame, eye:
        hash = absorb(hash, value)
    return hash

set_position(key, a, b, c):
    state = key
    for value in a, b, c:
        state = absorb(state, value)

random(max):
    state = state + golden
    return (mix(state) >> 32) % (max + 1)
```

**Rules**

- `mix` is the output function of SplitMix64.
- Each input is converted to an unsigned 32-bit value, then widened to 64 bits.
- `max` is at least 0, and the result is from 0 to `max`, the same range as today's `Random(max)` (FR-017).
- A position restarts the sequence, so a draw's place in its sequence is the number of draws since `set_position`.
- Nothing else feeds the state (FR-014): no thread, line order, time, process ID, or address.

## Positions

| Stage | `stage` | `a` | `b` | `c` | Set |
|---|---|---|---|---|---|
| Render workers | 1 | Pixel x | Pixel y | Sample | At the start of each sample |
| Screen-space ambient occlusion | 2 | Pixel x | Pixel y | 0 | At the start of each pixel |
| Depth of field | 3 | Pass | Entry | 0 | At the start of each entry of the shuffle |
| OpenCL depth of field | 3 | Pass | Entry | 0 | At the start of each entry of the shuffle |
| Outside a render | 4 | Caller's choice | 0 | 0 | See below |

`frame` is `sParamRender::frameNo`, which is 0 in a still render. `eye` is 0 unless a two-pass stereo render forces an eye: 1 for the left pass and 2 for the right.

A draw inside a shader takes the next value of the sample's sequence, however deep the reflection or refraction that asked for it.

## Callers

| File | Calls | Where the generator comes from |
|---|---|---|
| `render_worker.cpp` | 12 | The worker's own generator |
| `shader_ambient_occlusion.cpp` | 1 | The worker's own generator |
| `shader_aux_shadow.cpp` | 4 | The worker's own generator |
| `shader_global_illumination.cpp` | 3 | The worker's own generator |
| `shader_specular_highlight.cpp` | 1 | The worker's own generator |
| `shader_volumetric.cpp` | 1 | The worker's own generator |
| `light.cpp` | 1 | Passed in by the worker through `CalculateLightVector` and `CalculateBeam` |
| `ssao_worker.cpp` | 2 | A local generator in `doWork` |
| `dof.cpp` | 2 | A local generator in `Render`, built by the caller from the render's settings |

That is the 27 calls of FR-013. These callers are outside a render, so they only need their own local state (FR-015):

| File | Calls | Replacement |
|---|---|---|
| `opencl_engine_render_dof.cpp` | 2 | One generator per loop entry, because that loop runs in parallel |
| `interface.cpp` | 3 | A local generator in the view reset, with stage 4 |
| `gradient_edit_widget.cpp` | 4 | The time-seeded local `cRandom` that function's neighbor already uses |
| `thumbnail_widget.cpp` | 5 | A generator per widget, with stage 4 and the widget's `instanceIndex` as `a` |

`gRandomSeed`, `RandomInt()`, and `Random()` are then removed from `common_math` (FR-015, SC-015). `cRandom` stays, because random lights and the GUI use it with their own seeds (FR-020).

## Known answers

The values below come from the algorithm above, written with plain Python integers. Each row is the first four draws after `set_position`, with `max` of 1000, 1000, 65536, and 2147483646.

| Stage | Seed | Frame | Eye | `a`, `b`, `c` | Draws |
|---|---|---|---|---|---|
| 1 | 1 | 0 | 0 | 0, 0, 0 | 390, 845, 14177, 1529457532 |
| 1 | 1 | 0 | 0 | 1, 0, 0 | 339, 248, 50895, 334660501 |
| 1 | 1 | 0 | 0 | 0, 1, 0 | 352, 735, 26035, 1005037598 |
| 1 | 1 | 0 | 0 | 0, 0, 1 | 620, 118, 33057, 1233439096 |
| 1 | 2 | 0 | 0 | 0, 0, 0 | 774, 588, 16964, 183562170 |
| 1 | 1 | 1 | 0 | 0, 0, 0 | 307, 134, 6, 679786783 |
| 1 | 1 | 0 | 2 | 0, 0, 0 | 98, 450, 20952, 965693168 |
| 2 | 1 | 0 | 0 | 0, 0, 0 | 6, 204, 55716, 330138314 |
| 3 | 1 | 0 | 0 | 0, 0, 0 | 647, 39, 35239, 1092441823 |

A new `--test` test checks these values, so qmake and MSVC builds each prove FR-018 on every run of the suite.
