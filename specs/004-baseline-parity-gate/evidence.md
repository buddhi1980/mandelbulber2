# Evidence: Baseline 1 and the Parity Gate

The results of feature 004, in the sections [contracts/evidence-record.md](contracts/evidence-record.md) lists. Each result names the commit, run, or pull request it came from.

## 1. Header

| Item | Value |
|---|---|
| Date started | 2026-11-08 |
| Computer | Intel Core i9-14900K, 32 logical CPUs, Windows 11 Pro 10.0.26300 |
| WSL toolchain | `Ubuntu-24.04`: GCC `13.3.0-6ubuntu2~24.04.1`, glibc `2.39-0ubuntu8.9`, Qt 6.4.2, Python 3.12.3, 32 CPUs |
| Windows Python | 3.10.11 |
| S0 | `d505cee69e149566b58a814cd6eda476c88f502c`, the tip of `deterministic-regression` when the work started |
| `origin/master` at S0 | `33965a498cd8a046b87d0cc5c417432fa79c81db` |
| Harness tests at S0 | 224 passed on Windows, in 239.6 seconds |
| FractalEvaluator | Every local result here checks output, not time, so it may run while FractalEvaluator renders. It wasn't running when the work started |

**Settings before the first change** (FR-022), saved from Git Bash in [settings/before/](settings/before/):

- `repository.json`: the repository's settings, with `temp_clone_token` removed. The default branch is `master`, the repository is public, and `allow_auto_merge` and `delete_branch_on_merge` are both false.
- `protected-branches.json`: no protected branch.
- `rulesets.json`: one ruleset, "protect default", with enforcement disabled. It targets `~DEFAULT_BRANCH`, which is `master`, with the `deletion` and `non_fast_forward` rules and no bypass actors. It doesn't apply to `deterministic-regression`, and this feature doesn't change it.

**States of `deterministic-regression`**, as [data-model.md](data-model.md) defines them:

| State | Commit |
|---|---|
| S0 | `d505cee69` |
| S1 | Not reached yet |
| S2 | Not reached yet |
| S3 | Not reached yet |
| S4 | Not reached yet |
| S5 | Not reached yet |
| S6 | Not reached yet |

## 2. The forced path, locally

Every result here comes from fresh builds of `72a898b75`, the harness commit. T007's first commit, `64b4ac04f`, set the forced setting itself in the probe's first child, so a harness that forgot to give renders the setting would still have passed. `72a898b75` runs that child with the render environment itself. Its new test, `test_probe_fails_when_renders_get_no_setting`, fails on `64b4ac04f`.

| Check | Result |
|---|---|
| Harness tests, Windows | 242 passed in 254.9 seconds in `C:\GIT\mandelbulber2`, and 242 passed in 279.2 seconds in the MSVC clone |
| Harness tests, WSL | 242 passed in 119.8 seconds, on a fresh build of `72a898b75` that took 60.1 seconds |
| The probe on this computer (T009) | "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: Intel(R) Core(TM) i9-14900K, flags fma avx2", then "setup error: no reference set for linux-x86_64-gcc: tests/references/linux-x86_64-gcc does not exist", with exit 3 |
| Renders get no setting (T010) | Exit 3, no render: "setup error: the forced math path did not take effect: sin gave 3fe10e8ae531c9f8, and with every optional feature hidden it gives 3fe10e8ae531c9f9" |
| The setting misspelled as `glibc.cpu.hwcap` (T010) | Exit 3, no render, with the same message |
| One plain value changed (T010) | Exit 3, no render: "setup error: glibc's plain math results changed: sin gives 3fe10e8ae531c9f9 with the forcing and with every optional feature hidden, and the probe expects 3fe10e8ae531c9fa", after a line that lists all six results |
| Forced against hidden (T011, SC-004) | 487 of 487 items identical in every buffer and statistic. The forced and hidden collections took 60.4 and 59.6 seconds |
| Natural against forced (T011, FR-005) | 482 of 487 identical, and 5 differ, as the table below shows. The natural collection took 59.3 seconds |
| Collect and cross-check (T012, FR-004, SC-010) | The MSVC Release build of `72a898b75`, in `C:\GIT\mb2-kd5-72a898b75`, took 177 seconds, and its collect passed on 487 of 487 items. `--cross-check` of the WSL forced collection and the MSVC collection exited 0: "identical: 448 of 487 items matched exactly in every buffer and statistic" and "result: functional agreement (0 of 487 items failed)". Feature 003 found the same 448 on glibc's FMA path |

**What forcing changes** (FR-005): these items differ between glibc's FMA path, which this CPU takes by itself, and the forced path. The scratch harness `runner-unforced.py` made the natural collection, and the committed harness made the forced one.

| Item | What differs |
|---|---|
| `formula riemann_sphere_msltoe` | `normal_world`: 1 of 2,304 samples |
| `scene EXR example` | `raw_render` 20, `post_effect` 20, and `final_image` 5, of 230,400 samples each |
| `scene difs_tree` | `raw_render` 3 and `post_effect` 3, of 230,400 samples each |
| `scene mandelnest pow 5` | `raw_render` 3 and `post_effect` 3 of 230,400 samples, and 3 statistics values, the first `total_iterations` 277045196 against 277045197 |
| `scene riemann_sphere_msltoe_001` | Every buffer: `raw_render` and `post_effect` 146,110, `final_image` 33,601, `normal` 98,834, and `normal_world` 102,684, of 230,400 samples each, `alpha` 784 and `depth` 7,016 of 76,800, and 471 statistics values, the first `total_iterations` 641258594 against 641165986 |

Four items move by a few samples. In `riemann_sphere_msltoe_001`, fractal iteration amplifies the last-bit differences in glibc's `sin` and `cos`, so most of the image changes, as the plan expected. No references exist yet, so this isn't drift. Baseline 1 records the forced output.

FractalEvaluator ran 0 processes during these checks.

## 3. Landing S1

Not run yet.

## 4. Capture

Not run yet.

## 5. Baseline 1

Not run yet.

## 6. The required check

Not run yet.

## 7. The drift proof

Not run yet.

## 8. Closing

Not run yet.

## 9. Success criteria

Not filled in yet.
