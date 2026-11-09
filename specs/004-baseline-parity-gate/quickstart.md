# Quickstart: Validating Baseline 1 and the Parity Gate

Runnable checks that prove this feature works end to end. The contracts hold the details: [math-path.md](contracts/math-path.md), [capture-baseline.md](contracts/capture-baseline.md), [parity-gate.md](contracts/parity-gate.md), and [baseline-change.md](contracts/baseline-change.md). Every result goes into `evidence.md`, as [evidence-record.md](contracts/evidence-record.md) says.

## Prerequisites

- **Checkout**: `C:\GIT\mandelbulber2` on `deterministic-regression`, clean, with `gh` signed in as `Coastrnaut`.
- **WSL**: The `Ubuntu-24.04` distro, with scratch scripts in `~/verify/kd5`. Run each script as `CLAUDE.md` says, through a script file with its carriage returns stripped.
- **Fresh builds**: `~/verify/kd5/build_fresh.sh` is a copy of `~/verify/kd1/build_fresh.sh`. It fetches the Windows repository, checks out a ref, applies an optional patch, and builds from clean:

```text
./build_fresh.sh ~/mandelbulber2 windows/deterministic-regression
```

- **Go-aheads**: Every push, every pull request, the protection change, every branch deletion on GitHub, every rerun of a workflow, and each merge need the maintainer's go-ahead.
- **Records**: Before the first change, save the repository's settings, protected branches, and rulesets, with the three `gh api` calls in [parity-gate.md](contracts/parity-gate.md).

## 1. The forced path (User Story 1)

On a fresh build of the commit that holds the harness change:

```text
python3 tests/test_regression_runner.py
python tests/test_regression_runner.py
```

Run the first command in WSL and the second on Windows. Both must pass, with the new tests in [math-path.md](contracts/math-path.md).

Then, in the WSL clone:

```text
python3 tests/regression_runner.py
```

With no references, this must print the forced math path line, then stop as a setup error because no reference set exists, with exit 3. The math path line shows that the probe passed.

Make three scratch copies of the harness: one that leaves the setting out of `clean_environment()`, one that misspells it, and one that changes one of the probe's plain values. With the first two, the same command must stop with "the forced math path did not take effect", and with the third, with "glibc's plain math results changed". Each exits 3 before any render.

## 2. Forcing is CPU-proof (SC-004 and FR-005)

Collect three times in the WSL clone, into new folders under `~/verify/kd5`: with the committed harness, with a scratch copy that uses the hidden setting in [math-path.md](contracts/math-path.md), and with one that leaves the setting out.

```text
python3 tests/regression_runner.py --collect ~/verify/kd5/collect-forced
```

A scratch comparison of the collections must find that the forced and hidden collections match for all 487 items. It then counts the items and buffers that differ between the natural and forced collections. Both go into the evidence. These runs check output, so they can run while FractalEvaluator renders.

## 3. Collect and cross-check still work (FR-004)

Collect the committed harness's build in WSL and a fresh MSVC build on Windows, then cross-check them on Windows:

```text
python3 tests/regression_runner.py --collect <WSL folder>
python tests/regression_runner.py --collect <Windows folder>
python tests/regression_runner.py --cross-check <WSL folder> <Windows folder>
```

The cross-check must exit 0 with functional agreement, and the evidence records its count of identical items. Only after this does the harness change land, because a push to `deterministic-regression` doesn't run Cross-Build Check.

## 4. Capture Baseline 1 (User Story 2)

1. With the go-ahead, push the harness change and the capture workflow to `deterministic-regression`. Parity Gate and C++ CI Build (Qt6) must pass on the push.
2. Start capture and confirm that the run started, as [capture-baseline.md](contracts/capture-baseline.md) says. If it didn't, restart it with the empty commit. If it fails for a reason outside the source, rerun it from its page, with the go-ahead.
3. Wait for the run without polling. It must pass, its log must show the forced math path line and "wrote 487 references", and it must upload `baseline-references`.
4. Download the artifact, check its contents and size, commit it on `baseline/1`, compare the blob IDs, and push the branch.

## 5. Land Baseline 1 (User Stories 3 and 4)

1. When you can review and merge the pull request in the same sitting, and with the go-ahead, push the gate steps to `deterministic-regression`. Its `parity-gate` run must fail at "Run Parity Gate", as a setup error because no reference set exists.
2. Open the pull request at once, as [baseline-change.md](contracts/baseline-change.md) says. Its `parity-gate` run must pass on 487 of 487 items, with the gate step under 30 minutes, and Cross-Build Check must pass. If its gate fails, revert the gate steps at once, close the pull request, and start again from capture once the cause is known.
3. The maintainer merges it. The push run on the merge commit must pass. Delete `baseline/1` on GitHub.
4. Record the window, the times, and the CPU lines in the evidence.

## 6. Require the gate (User Story 5)

With the go-ahead, set the protection as [parity-gate.md](contracts/parity-gate.md) says, then read it back:

```text
gh api repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection
```

`parity-gate` must be the only required check, with `strict` false and `enforce_admins` false. The required name must match the check name on the Baseline 1 pull request. The next direct push, such as the evidence commit, must land, and the push output names the bypassed rule.

## 7. Prove the gate catches drift (User Story 6)

Follow the drift proof in [parity-gate.md](contracts/parity-gate.md): first the local compare, then the pull request. The run must fail on `formula bristorbrot` alone, and the merge state must be `BLOCKED`. Then close the pull request and delete the branch.

## 8. Close KD-4 and KD-5 (User Story 7)

1. Compare fresh WSL builds of the amendment's branch and of `deterministic-regression`. The compare must find no difference, which is Workflow 2's local pass. Then, with the go-ahead, open the amendment's pull request from that branch. It edits only `.specify/memory/constitution.md`: KD-4 and KD-5 closed, with the evidence linked, version 1.1.1, and the last amended date.
2. `parity-gate` must pass on it. The maintainer merges it, and the branch is deleted on GitHub.
3. Push the corrections to `MIGRATION_PLAYBOOK.md`, `CLAUDE.md`, and `README.md` that [research.md](research.md) R12 lists. Phase 4's two workflow listings in the playbook must match the committed files byte for byte.

## 9. Final checks

```text
git diff --stat <S0 commit> deterministic-regression -- . ':!specs' ':!tests/references'
git ls-remote --heads origin
```

The first must list only the two workflows, the two harness files, the constitution, the playbook, `CLAUDE.md`, `README.md`, and any fix FR-010 needed (SC-008). The second must list no `baseline/`, `seed/`, or amendment branch (SC-009).

Then save the repository's settings, protected branches, and rulesets again, and compare them with the records from before the first change (SC-006).
