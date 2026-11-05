# Contract: Application Command Line

Two new options for `mandelbulber2`. Neither changes behavior when it is not used.

## `--verification-export <dir>`

Writes a [verification export](verification-export.md) to `<dir>` after a still render.

```bash
mandelbulber2 --nogui --res 320x240 --format tiff \
  --override 'opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4' \
  --output /tmp/run/image --verification-export /tmp/run/export scene.fract
```

| Condition | Result |
|---|---|
| Settings loaded and the render succeeded | Export is written |
| Settings failed to load | No manifest is written |
| The render reported failure | No manifest is written |
| A write failed | An error is printed and no manifest is written |
| Used without `--nogui`, or with animation, queue, voxel, network, test, or benchmark modes | An error is printed and the program exits with a non-zero status |

**Rules**

- The export runs after the render and the normal image save. It adds no work to the render path.
- The exit status is not part of this contract. The manifest is the proof of success.

## `--list-formulas`

Prints every formula the binary offers, then exits with status 0.

```text
mandelbulber-formula-list 1
2	mandelbulb
3	mandelbulbPower2
...
end
```

**Rules**

- Normal startup messages may appear first, so readers start at the `mandelbulber-formula-list` line.
- Each entry is the numeric ID, a tab, and the internal name.
- The list comes from `newFractalList`, without the `none` entry.
- A reader must fail if the `end` line is missing.

## `--version`

This option already exists and does not change. The harness reads it to record the application version. Startup messages may appear first, so readers take the first line that starts with `Mandelbulber `.
