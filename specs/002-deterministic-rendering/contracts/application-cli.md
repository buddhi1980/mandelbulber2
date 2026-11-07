# Contract: Application Command Line

One new option for `mandelbulber2`. Without it, behavior does not change.

## `--threads <N>`

Sets the number of threads for one headless CPU render.

```bash
mandelbulber2 --threads 4 --nogui --format tiff --output /tmp/run/image scene.fract
```

`N` is a whole number of at least 1, given as the next argument or as `--threads=N`. It may be higher than the number of logical CPUs (FR-003).

### Where it applies

Every headless CPU render mode accepts it (FR-001).

| Mode | Started by | Result |
|---|---|---|
| Still render | `--nogui` with one scene | Applies |
| Keyframe animation | `--keyframe` | Applies |
| Flight animation | `--flight` | Applies |
| Queue | `--queue`, a `.fractlist`, a folder, or several scenes | Applies, and takes precedence over `limit_CPU_cores` |
| Voxel export | `--voxel` | Applies |
| Network server | `--server` | Applies |
| Network client | `--host` | Applies, and the client reports `N` workers to the server |
| Test suite | `--test` | Applies |
| Benchmark | `--benchmark` | Applies |

### What it governs

| Stage | Today | With `--threads N` |
|---|---|---|
| Render workers | `systemData.numberOfThreads` | `N` |
| Screen-space ambient occlusion | The same value, capped at the image height | `N`, capped at the image height |
| Depth of field phase I and HDR blur | The OpenMP default | `N` |
| Depth of field phase II | The OpenMP default | One thread, always (FR-022) |
| Voxel export and mesh export | The OpenMP default | `N` |
| Audio analysis in an animation | The OpenMP default | `N` |
| The shuffle in OpenCL depth of field | The OpenMP default | `N` |

`OMP_NUM_THREADS` does not override `N` (FR-002), because each OpenMP region carries its own `num_threads` clause.

### Without the option

| Case | Result |
|---|---|
| Headless render | Every logical CPU, as today |
| GUI | `limit_CPU_cores` sets the render workers, as today |
| Queue | `limit_CPU_cores` sets the render workers, as today |
| OpenMP regions | The OpenMP default, including `OMP_NUM_THREADS`, as today |

### Refusals

The application checks the option right after it reads the command line, before it opens a window, loads settings, or renders. Each refusal prints an error that names `--threads`, prints the help text, and exits with a non-zero status.

| Use | Exit status name | Value |
|---|---|---|
| `N` is 0, negative, text, a fraction, empty, or too large for an `int` | `cliErrorThreadsInvalid` | -81 |
| In the GUI: none of the modes in the table above | `cliErrorThreadsNotAllowed` | -82 |
| With `--gpu` or `--gpuall` | `cliErrorThreadsNotAllowed` | -82 |
| With `--list`, `--list-formulas`, `--touch`, `--help-input`, `--help-examples`, or `--help-opencl` | `cliErrorThreadsNotAllowed` | -82 |

**Rules**

- A process sees the status as a value from 0 to 255 on Linux, so -81 reads as 175 and -82 as 174. Readers check for non-zero.
- With no argument after `--threads`, Qt prints an error that names the option and exits 1.
- `--help` and `--version` still print and exit 0. Qt handles them before the application reads any option.
- The refusal comes first because the GUI's error path opens a message box, and that path needs a display. The check runs before the window system starts, so a refusal never waits for a click.
- `--test` and `--benchmark` remove `--threads` and its value before they start QTest, because QTest stops on options it does not know.
- A scene that turns on `opencl_enabled` in its own settings renders through OpenCL, and `--threads` then only sets the CPU stages that still run. The option can't see this before settings load, and the harness always passes `opencl_enabled=0`.

## `--help`

The help text describes `--threads` (FR-006):

```text
--threads <N>  Sets the number of threads for a headless CPU render. The
               default is every logical CPU. Takes precedence over
               limit_CPU_cores and OMP_NUM_THREADS.
```

## Scene setting

`render_random_seed` is an ordinary scene setting, so `--override` can set it. See [random-values.md](random-values.md).

```bash
mandelbulber2 --nogui --override 'render_random_seed=2' scene.fract
```

The same route sets the frame number for a still render, which is how a test renders one frame of noise without an animation:

```bash
mandelbulber2 --nogui --override 'frame_no=5' scene.fract
```
