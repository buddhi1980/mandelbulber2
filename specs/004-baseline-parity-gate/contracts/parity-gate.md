# Contract: The Parity Gate

The gate steps in `.github/workflows/parity-gate.yml`, the required check, and the drift proof, for FR-011 to FR-014 and FR-019 to FR-023. [research.md](../research.md) R7, R9, and R10 give the reasons.

## The workflow after this feature

```yaml
name: Parity Gate

on:
  pull_request:
  push:
    branches:
      - deterministic-regression

permissions:
  contents: read

jobs:
  parity-gate:
    runs-on: ubuntu-24.04
    timeout-minutes: 120
    steps:
      - uses: actions/checkout@v7
      - uses: ./.github/actions/compile-engine
      - name: Test the Harness
        run: python3 tests/test_regression_runner.py
      - name: Run Parity Gate
        run: python3 tests/regression_runner.py
      - name: Upload Failed Outputs
        if: failure()
        uses: actions/upload-artifact@v7
        with:
          name: render-regression-snapshots
          path: tests/outputs
          retention-days: 7
```

**Rules**

- **Changed**: Only the last three steps are new. The name, triggers, access, runner, time limit, and first two steps stay as they are (FR-011).
- **The gate**: The harness's default mode runs the probe, refuses a reference set with other settings, renders every item once, and compares every buffer and statistic exactly (FR-012).
- **Outcomes**: Exit 0 passes the job. Exit 1, a parity failure, and exit 3, a reference target mismatch or a setup error, fail it.
- **Before Baseline 1**: No references exist, so the gate fails as a setup error. The gate steps therefore land only right before Baseline 1's pull request opens (FR-013).
- **Time**: The gate step must take under 30 minutes on the hosted 4-vCPU runner in every gate run the evidence records. The evidence takes each run's step and job times from the job's step list (FR-014).

## The required check

After Baseline 1 merges, and with the maintainer's go-ahead, one call sets classic branch protection:

```text
gh api -X PUT repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection --input protection.json
```

```json
{
  "required_status_checks": { "strict": false, "contexts": ["parity-gate"] },
  "enforce_admins": false,
  "required_pull_request_reviews": null,
  "restrictions": null
}
```

| Effect | Who |
|---|---|
| A pull request can't merge until `parity-gate` passes on it | Anyone who can't bypass the rule |
| A direct push lands, and its push run checks it afterwards | The maintainer, who bypasses as an admin |
| A direct push is refused unless its commit has passed `parity-gate`, which only a pull request can show, so others change the branch through pull requests | Anyone else with write access |
| Force pushes to the branch and deleting it are refused | Anyone who can't bypass, as on every protected branch |

Reading the protection back must show `parity-gate` as the only required check, `strict` false, and `enforce_admins` false (SC-006). The first direct push afterwards must land. GitHub then reports the rule it bypassed, which the evidence quotes.

**Records**: Before the first change of this feature, the evidence saves the repository's settings, its protected branches, and its rulesets. It saves them again at the end, so a reader can see that only this protection changed (FR-022):

```text
gh api repos/Coastrnaut/mandelbulber2
gh api repos/Coastrnaut/mandelbulber2/branches?protected=true
gh api repos/Coastrnaut/mandelbulber2/rulesets
```

**Undoing it**: Only the maintainer changes the protection. If it blocks pull requests it shouldn't, such as when the name it requires never reports, the maintainer corrects it with the same call, or removes it, and the evidence records why (FR-019):

```text
gh api -X DELETE repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection
```

**A failing direct push**: A push run that fails is a defect in that push. The next commit to land fixes or reverts it, the maintainer chooses which, and references are never regenerated to clear it (FR-021).

## The drift proof

After the check is required:

1. In WSL, build `deterministic-regression` and the seeded tree fresh, then run `--compare` between them. It must find one differing item, `formula bristorbrot`.
2. With the go-ahead, push `seed/drift-proof`. It has one commit that changes `2.0 * z.x - z.z` to `2.001 * z.x - z.z` on line 36 of `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`. Then open a pull request into `deterministic-regression`.
3. `parity-gate` must fail on `formula bristorbrot` alone, with exit 1, and upload `render-regression-snapshots`.
4. `gh pr view <number> --json mergeStateStatus` must give `BLOCKED`.
5. Close the pull request unmerged and delete the branch on GitHub. Cross-Build Check and the other checks may run on it, and nothing waits for them.

The seeded change is never merged, and no bypass is used to merge it (FR-023).

## Closing conditions

| Deviation | Closed when | Proof |
|---|---|---|
| KD-4 | Baseline 1 is merged | The Baseline 1 pull request, and the gate passing on it and on the push after it (SC-001, SC-002) |
| KD-5 | The math path is settled and recorded with the references, before capture | The forced path, its probe, and the `math_path` line in Baseline 1 (FR-001 to FR-003, SC-004) |
