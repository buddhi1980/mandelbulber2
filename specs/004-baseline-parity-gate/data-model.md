# Data Model: Baseline 1 and the Parity Gate

The things this feature creates or changes, their fields, and the rules that check them. [research.md](research.md) gives the reasons.

## Reference Set

The folder `tests/references/linux-x86_64-gcc/`, written whole by capture and changed only by a Baseline Change.

| Entry | Holds | Count |
|---|---|---|
| `scenes/<name>.zip` | One scene's seven buffers and statistics, in the harness's export format | 26 |
| `formulas/<name>.zip` | One swept formula's seven buffers and statistics | 461 |
| `toolchain.txt` | `compiler`, `math_library`, and one `package` line for each of `gcc`, `g++`, `libc6`, `libstdc++6`, `libgcc-s1`, and `libgomp1` | 1 |
| `settings.txt` | The render settings the set depends on | 1 |

`settings.txt` after this feature, with LF line endings:

```text
export_format 1
scene_resolution 320x240
formula_resolution 32x24
overrides opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4
math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4
```

**Rules**

- Capture writes the whole set or nothing, and refuses one of 104,857,600 bytes or more.
- Every file in the repository is byte-identical to the capture artifact's copy (FR-015).
- The gate refuses a set whose `settings.txt` differs from the harness's lines in any way, so a set without the `math_path` line is refused (FR-003).
- A `toolchain.txt` that differs from the runner's is reported, and a difference in output then counts as a reference target mismatch, not a parity failure.
- An item with no reference, or a reference with no item, fails the gate.

## Math Path Probe

The check in [contracts/math-path.md](contracts/math-path.md), run on Linux at the start of every gate and capture run.

| Field | Meaning |
|---|---|
| Function | One of `sin`, `cos`, `exp`, `log`, `atan`, and `asin` |
| Input | The input's bits |
| Plain result | The bits glibc 2.39's plain path gives |
| FMA result | The bits its FMA path gives |

| Outcome | When | Effect |
|---|---|---|
| Forced | Every result under the forced setting is the plain result | The run goes on, and its report names the CPU's own path |
| Missed a path | A forced result isn't plain, and differs from the hidden result | Setup error, exit 3, before anything renders |
| Plain results changed | A forced result isn't plain, and matches the hidden result | Setup error, exit 3, with the new results |
| Can't run | The child process fails or prints something else | Setup error, exit 3 |
| Not Linux | The harness runs on Windows | No probe. The gate runs only for debugging, says the path isn't forced, and can't pass |

The CPU's own path is the probe's result with no setting: `fma` when every result is the FMA one, `plain` when every result is the plain one, and `unknown` otherwise.

## Capture Run

One run of `.github/workflows/capture-baseline.yml`.

| State | Next |
|---|---|
| Not started | A push that creates `baseline/<number>` starts the job. Any other push to that branch skips it |
| Testing | The harness's own tests pass, or the job fails |
| Building | The build-file search, the build, and the flag check pass, or the job fails |
| Rendering | The probe passes, and Self-Parity passes at one thread and at four, or the job fails and writes nothing |
| Written | 487 references, `toolchain.txt`, and `settings.txt` are in `tests/references/linux-x86_64-gcc/` |
| Uploaded | The `baseline-references` artifact holds the set, kept for 90 days |
| Failed | `capture-failure-outputs` holds `tests/outputs`, kept for 7 days |

A run that failed outside the source may be rerun from GitHub. Any other repeat deletes the branch and pushes it again (FR-007).

## Gate Run

One `parity-gate` job, on a pull request or a push to `deterministic-regression`.

| Harness result | Exit | Job |
|---|---|---|
| Passed | 0 | Passes |
| Parity failure | 1 | Fails, and `render-regression-snapshots` holds `tests/outputs` for 7 days |
| Reference target mismatch, or setup error | 3 | Fails, with the same upload |

## Baseline 1 Pull Request

| Field | Value |
|---|---|
| Head | `baseline/1` |
| Base | `deterministic-regression` |
| Files | Only `tests/references/linux-x86_64-gcc/**`, 489 files, and at most one empty commit (FR-007) |
| Description | What [contracts/baseline-change.md](contracts/baseline-change.md) lists |
| Checks | `parity-gate` passes on 487 of 487 items, and the others run as usual |
| Merge | By the maintainer, never by auto-merge |
| After merge | The push run passes, and `baseline/1` is deleted on GitHub |

## Required Check

| Setting | Value |
|---|---|
| Branch | `deterministic-regression` |
| Required check | `parity-gate` |
| Up to date before merging | No |
| Applies to admins | No, so the maintainer's direct pushes land |
| Required reviews | None |
| Push restrictions | None |
| Changed by | The maintainer alone, who also corrects or removes it if it blocks wrongly |

## Drift Proof

| Field | Value |
|---|---|
| Branch | `seed/drift-proof`, from the tip of `deterministic-regression` |
| Seed | Line 36 of `fractal_bristorbrot.cpp`, `2.0 * z.x - z.z` changed to `2.001 * z.x - z.z` |
| Local check | `--compare` of fresh builds finds `formula bristorbrot` alone |
| CI result | `parity-gate` fails on `formula bristorbrot` alone, and uploads its outputs |
| Merge state | `BLOCKED` |
| End | Closed unmerged, and the branch deleted on GitHub |

## State of `deterministic-regression`

| State | Reached by | `parity-gate` |
|---|---|---|
| S0, today | `specs/003-strict-floating-point` | Builds and checks flags |
| S1 | A direct push of the forced path, its probe, its record, its tests, and the capture workflow | Builds and checks flags |
| S2 | A direct push of the gate steps, after `baseline/1` holds the references | Fails, for lack of references |
| S3 | The merge of Baseline 1 | Passes |
| S4 | The required check | Passes, and blocks pull requests that fail it |
| S5 | The merge of the amendment, version 1.1.1 | Passes |
| S6 | Direct pushes of the playbook, `CLAUDE.md`, and `README.md` corrections | Passes |

Nothing else lands between S2 and S3 (FR-013), except a revert of the gate steps if the Baseline 1 pull request's gate fails. That returns the branch to S1 until capture repeats.

## Evidence Record

`specs/004-baseline-parity-gate/evidence.md`, whose sections [contracts/evidence-record.md](contracts/evidence-record.md) lists.
