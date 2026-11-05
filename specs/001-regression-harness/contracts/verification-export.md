# Contract: Verification Export Format

**Version**: 1

The application writes one directory per render. The harness is the only consumer. Any change to this format increases the version number, and the harness rejects versions it does not know.

## Directory

```text
<dir>/
├── raw_render.bin
├── post_effect.bin
├── final_image.bin
├── alpha.bin
├── depth.bin
├── normal.bin
├── normal_world.bin
├── statistics.txt
└── manifest.txt
```

## Buffer Files

- Raw samples with no header.
- Little-endian byte order.
- Rows run from the top of the image, and pixels run left to right.
- Multi-channel samples are interleaved, in R, G, B order.
- A buffer holds the engine's exact values, with no scaling, normalization, gamma, or quantization added.

| File | Sample | Channels | Source |
|---|---|---|---|
| `raw_render.bin` | float32 | 3 | `cImage::GetImageFloat()` |
| `post_effect.bin` | float32 | 3 | `cImage::GetPostImageFloat()` |
| `final_image.bin` | uint16 | 3 | `cImage::GetImage16()` |
| `alpha.bin` | uint16 | 1 | `cImage::GetAlphaBuf()` |
| `depth.bin` | float32 | 1 | `cImage::GetZBuffer()` |
| `normal.bin` | float32 | 3 | `cImage::GetPixelNormal()`, row by row |
| `normal_world.bin` | float32 | 3 | `cImage::GetPixelNormalWorld()`, row by row |

`normal.bin` and `normal_world.bin` are written only when their buffers are enabled. A gated render enables the normal buffer, and the engine currently enables the world normal buffer on every render, so a missing file fails the scene.

## `statistics.txt`

UTF-8 text with LF line endings. Each line is `name value`, in exactly this order:

```text
total_iterations 316706000
raymarch_count 76800
missed_de 12
iteration_histogram.size 50
iteration_histogram.0 0
iteration_histogram.1 41
...
iteration_histogram.50 7
step_histogram.size 1000
step_histogram.0 0
...
step_histogram.1000 3
```

- Every value is a whole number, with no separators or exponents.
- A histogram of size `n` has buckets `0` to `n`, where bucket `n` is the overflow bucket.
- The file contains no time, rate, process, host, or path values.

## `manifest.txt`

UTF-8 text with LF line endings, written last, using an atomic rename:

```text
mandelbulber-verification-export 1
width 320
height 240
compiler gcc 13.3.0
math_library glibc 2.39
buffer raw_render f32 3
buffer post_effect f32 3
buffer final_image u16 3
buffer alpha u16 1
buffer depth f32 1
buffer normal f32 3
buffer normal_world f32 3
statistics statistics.txt
end
```

- The `end` line proves the manifest is complete.
- `compiler` and `math_library` are identity values. They are never part of a parity comparison.
- Unknown values use the word `unknown`.

## Write Order

1. Create the directory if needed.
2. Delete any existing `manifest.txt`.
3. Write the buffer files.
4. Write `statistics.txt`.
5. Write `manifest.txt`.

If any write fails, the application reports the error and does not write the manifest.
