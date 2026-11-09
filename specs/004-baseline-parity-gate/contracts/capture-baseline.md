# Contract: Capture Baseline

The new workflow, `.github/workflows/capture-baseline.yml`, and how a capture becomes a commit, for FR-006 to FR-010 and FR-015. [research.md](../research.md) R5 to R8 give the reasons.

## The workflow

```yaml
name: Capture Baseline

on:
  push:
    branches:
      - 'baseline/**'

permissions:
  contents: read

jobs:
  capture-baseline:
    if: github.event.created
    runs-on: ubuntu-24.04
    timeout-minutes: 350
    steps:
      - uses: actions/checkout@v7
      - uses: ./.github/actions/compile-engine
      - name: Test the Harness
        run: python3 tests/test_regression_runner.py
      - name: Capture References
        run: python3 tests/regression_runner.py --capture
      - name: Upload References
        uses: actions/upload-artifact@v7
        with:
          name: baseline-references
          path: tests/references
          if-no-files-found: error
          retention-days: 90
      - name: Upload Failed Outputs
        if: failure()
        uses: actions/upload-artifact@v7
        with:
          name: capture-failure-outputs
          path: tests/outputs
          retention-days: 7
```

**Rules**

- **Start**: Only a push that creates a branch under `baseline/` runs the job, so the only branches made there are `baseline/<number>`. Every later push to that branch runs the workflow with the job skipped (FR-007). Only the maintainer pushes them, and a capture makes only an artifact.
- **Repeats**: A run that failed outside the source, such as a runner fault, may be rerun from its page on GitHub, which repeats the same push event. Any other repeat deletes the branch on GitHub and pushes it again at the commit to capture. Captures on different branches are independent.
- **Access**: Read-only, with no secrets (FR-006).
- **Build**: Through `compile-engine`, so the build-file search and the flag check run before any render.
- **Harness rules**: Capture refuses outside CI, without thread control, or where renders can't be isolated. It runs the math path probe, passes Self-Parity at one thread and at four, and writes a whole set or nothing (FR-008).
- **Nothing written**: When Self-Parity or the probe fails, the capture step fails, so the references aren't uploaded, and the job keeps the report in `capture-failure-outputs` (FR-010).
- **Time**: Self-Parity renders each item 7 times. The playbook expects about 2 hours, inside the 350-minute limit.
- **Lifetime**: The references artifact is kept for 90 days, the longest GitHub allows for a public repository.

## Starting Baseline 1

With the maintainer's go-ahead, from a clean checkout of `deterministic-regression` that is even with `origin/deterministic-regression`, so the branch carries no unpushed commit:

```text
git push origin deterministic-regression:refs/heads/baseline/1
gh run list --repo Coastrnaut/mandelbulber2 --workflow capture-baseline.yml --branch baseline/1
```

If no run is listed within 2 minutes, delete `baseline/1` on GitHub, then push it again with one empty commit on top. That push creates the branch and carries a commit (R5).

## Checking the artifact

Download it to a new folder outside the repository:

```text
gh run download <run ID> --repo Coastrnaut/mandelbulber2 -n baseline-references -D <folder>
```

The folder must hold exactly:

| Path | Count |
|---|---|
| `linux-x86_64-gcc/scenes/*.zip` | 26 |
| `linux-x86_64-gcc/formulas/*.zip` | 461 |
| `linux-x86_64-gcc/toolchain.txt` | 1 |
| `linux-x86_64-gcc/settings.txt` | 1, whose fifth line is the `math_path` line |

The total must be under 104,857,600 bytes. The run's log must show "wrote 487 references to tests/references/linux-x86_64-gcc" and the forced math path line.

## Committing it

1. Fetch `baseline/1` and switch to it.
2. Copy the folder's `linux-x86_64-gcc` into `tests/references/`.
3. Commit only those 489 files, with a message such as `test: add Baseline 1 for linux-x86_64-gcc`.
4. For every file, compare the blob ID in `git ls-tree -r HEAD tests/references` with `git hash-object --no-filters` of the downloaded copy. Any difference stops the work (FR-015).
5. With the go-ahead, push `baseline/1`. The capture job is skipped.
6. Switch back to `deterministic-regression`.

The pull request that follows is in [baseline-change.md](baseline-change.md).
