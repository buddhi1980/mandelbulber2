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

Not run yet.

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
