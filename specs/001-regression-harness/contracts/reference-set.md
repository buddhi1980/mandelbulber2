# Contract: Reference Set

Only `--capture` writes this layout, and only on success.

## Layout

```text
tests/references/<target>/
├── toolchain.txt
├── settings.txt
├── scenes/
│   └── <scene name>.zip
└── formulas/
    └── <internal name>.zip
```

## `toolchain.txt`

```text
compiler gcc 13.3.0
math_library glibc 2.39
package libc6 2.39-0ubuntu8.9
package gcc-13 13.3.0-6ubuntu2~24.04
```

`package` lines come from `dpkg-query` for `gcc-[0-9]*`, `g++-[0-9]*`, `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1`, in sorted order, skipping entries with no version. They are left out on platforms without `dpkg-query`. Versions shown here are examples.

## `settings.txt`

The fixed render settings used for the capture:

```text
export_format 1
scene_resolution 320x240
formula_resolution 32x24
overrides opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4
```

A difference from the harness's current settings is a setup error (exit code 3), because the comparison would otherwise report drift that isn't real.

## Zip Entries

Each zip holds one render, in this order:

```text
manifest.txt
statistics.txt
raw_render.planes
post_effect.planes
final_image.planes
depth.planes
normal.planes
```

- `manifest.txt` and `statistics.txt` are stored as written by the application.
- A `.planes` entry holds a buffer split into byte planes. All first bytes come first, then all second bytes, and so on. Plane count equals the sample size: 4 for `f32`, 2 for `u16`.
- Entries use deflate at level 9.
- Every entry uses the fixed timestamp 1980-01-01 00:00:00, so a recapture of unchanged data is byte-identical.

Reading a reference reverses the plane split and verifies each entry's checksum.

## Replacement

1. Build the new set in a staging directory.
2. Check that it is complete and under 100 MB.
3. Move any existing `tests/references/<target>/` aside, move the new set into place, then delete the old set.

If the new set cannot be moved into place, the old set is moved back. Any failure before step 3 leaves the existing set untouched.
