# Contract: The Baseline 1 Pull Request

The Baseline Change that brings the first references, for FR-013 and FR-015 to FR-018. The constitution's Baseline Change rules apply in full.

## Order

1. `baseline/1` holds the commit of 489 reference files, pushed and checked as [capture-baseline.md](capture-baseline.md) says.
2. With the go-ahead, the gate steps land on `deterministic-regression` by direct push. Their push run fails for lack of references, as expected.
3. The pull request opens straight away:

```text
gh pr create --repo Coastrnaut/mandelbulber2 --base deterministic-regression --head baseline/1 --title "Add Baseline 1 for linux-x86_64-gcc" --body-file <file>
```

4. Its `parity-gate` run must pass on 487 of 487 items.
5. The maintainer merges it. Auto-merge is never turned on.
6. The push run of `parity-gate` on the merge commit must pass, and `baseline/1` is deleted on GitHub.

Nothing else lands on `deterministic-regression` between steps 2 and 5. After step 6, the evidence records the failing push run from step 2 and how long the window lasted.

## Files

- Only the 489 files under `tests/references/linux-x86_64-gcc/`: 26 scene zips, 461 formula zips, `toolchain.txt`, and `settings.txt`.
- Each one is byte-identical to the artifact's copy, which the blob ID check in [capture-baseline.md](capture-baseline.md) proves.
- No other file. The gate steps reach the base by direct push, not through this pull request.

## Description

In this order, with no attribution lines:

1. **What this is**: Baseline 1, the first reference set for the First Reference Target.
2. **Why output is recorded**: KD-1, KD-2, and KD-3 are closed, so renders reproduce and floating-point behavior is locked. Baseline 1 records what the gated build renders now, so later changes can be compared with it.
3. **No earlier references**: None exist, which replaces the usual before-and-after evidence.
4. **The capture**: A link to the capture run, which passed Self-Parity at one thread and at four, the run's CPU line, and the size of the set.
5. **The toolchain**: The lines of `toolchain.txt`.
6. **The math path**: The `math_path` line, and the fact that every Linux render takes glibc's plain x86-64 path, so the runner's CPU doesn't change the results.
7. **The check**: How the files were shown to match the artifact.
8. **Not covered**: Anything the evidence leaves out, including KD-4 and KD-5, which close in the amendment that follows.

## Approval

GitHub doesn't let a pull request's author approve it, so the maintainer's merge is the approval, as for pull request 7. Nobody else merges it, and no bypass is used to merge it while a check fails.
