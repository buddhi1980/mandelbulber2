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
| S1 | `4b60fcdf9` |
| S2 | `2679e892a` |
| S3 | `2ba3b8021` |
| S4 | Protection set at 03:39 UTC on 2026-11-09, on `2ba3b8021` |
| S5 | `60beeac34` |
| S6 | `5a2888039` |

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

S1 is `4b60fcdf9`, pushed on 2026-11-09. Its runs both passed:

| Run | Result | Time |
|---|---|---|
| Parity Gate, [42546386319](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42546386319) | Passed: the build-file search and the flag check | 6 minutes 22 seconds |
| C++ CI Build (Qt6), [42546386321](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42546386321) | Passed | 13 minutes 41 seconds |

## 4. Capture

**The start**: `git push origin deterministic-regression:refs/heads/baseline/1`, at `4b60fcdf9`, started Capture Baseline run [42549364843](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42549364843) with its job running. So a push that only creates a branch does start a workflow, and the empty-commit fallback wasn't needed.

**The run** passed in 31 minutes 9 seconds, far inside its 350-minute limit (SC-003):

| Step | Result | Time |
|---|---|---|
| Compile Engine | The search printed "searched 74 build files" and "passed", and the flag check printed "passed" after the build | 4 minutes 44 seconds |
| Test the Harness | "Ran 242 tests", OK | 1 minute 19 seconds |
| Capture References | Self-Parity passed, then "note: wrote 487 references to tests/references/linux-x86_64-gcc" and "result: passed (0 of 487 items failed)" | 24 minutes 48 seconds |
| Upload References | `baseline-references` uploaded | 3 seconds |

The probe's line in the run: "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 9V45 96-Core Processor, flags fma avx2" (FR-005). The run's toolchain line: "toolchain: gcc 13.3.0, glibc 2.39".

**The artifact**, downloaded to `C:\GIT\mb2-kd5-baseline-1-artifact`:

| Check | Result |
|---|---|
| Files | 26 scene zips, 461 formula zips, `toolchain.txt`, and `settings.txt`: 489 files, and nothing else |
| Size | 83,059,454 bytes, under the harness's limit of 104,857,600 bytes (SC-001) |
| `settings.txt` | Five lines, the fifth `math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4` (FR-003) |
| `toolchain.txt` | `compiler gcc 13.3.0`, `math_library glibc 2.39`, and package lines for the installed `gcc-12`, `gcc-13`, `gcc-14`, `g++-12`, `g++-13`, and `g++-14` packages, `libc6 2.39-0ubuntu8.9`, `libstdc++6`, `libgcc-s1`, and `libgomp1` `14.2.0-4ubuntu2~24.04.1` (FR-009). The runner's glibc is the same build as WSL's |

**The commit**: `465dc07a3` on `baseline/1`, "test: add Baseline 1 for linux-x86_64-gcc", holds exactly those 489 files. For every one, the blob ID in `git ls-tree` equals `git hash-object --no-filters` of the downloaded copy: 489 checked, 0 different (FR-015). Its push started Capture Baseline run 42552875153 with its job skipped (FR-007).

## 5. Baseline 1

**The window**: S2 is `2679e892a`, "ci: add the gate steps to Parity Gate", pushed at 02:49:17 UTC on 2026-11-09. Its Parity Gate run, [42553013294](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42553013294), failed at "Run Parity Gate" as a setup error, as planned: "setup error: no reference set for linux-x86_64-gcc: tests/references/linux-x86_64-gcc does not exist", then "result: setup error". Its probe line was "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 7763 64-Core Processor, flags fma avx2". The job took 11 minutes 15 seconds. Its C++ CI Build (Qt6) run, 42553013223, passed. The window lasted 35 minutes 17 seconds, from that push to the merge at 03:24:34 UTC. Nothing was reverted.

**The pull request**: [Coastrnaut/mandelbulber2#9](https://github.com/Coastrnaut/mandelbulber2/pull/9), "Add Baseline 1 for linux-x86_64-gcc", opened at 02:49:22 UTC from `baseline/1` into `deterministic-regression`. `gh pr view 9 --json files` listed the 489 files under `tests/references/linux-x86_64-gcc/` and nothing else.

| Check | Result | Time |
|---|---|---|
| `parity-gate`, [42553024969](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42553024969) | "result: passed (0 of 487 items failed)", after the probe line "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 7763 64-Core Processor, flags fma avx2". The check reported as `parity-gate` | "Run Parity Gate" 5 minutes 25 seconds, under the 30-minute limit. The job 16 minutes 25 seconds |
| Cross-Build Check, 42553025001 | `cross-check`: "identical: 448 of 487 items matched exactly in every buffer and statistic" and "result: functional agreement (0 of 487 items failed)". `linux-qmake` and `windows-msvc` passed, each "result: passed (0 of 487 items failed)" | `linux-qmake` 20 minutes 31 seconds, `windows-msvc` 28 minutes 53 seconds, `cross-check` 20 seconds |
| C++ CI Build (Qt6) | Both `Build & Test (Linux)` jobs passed | 14 minutes 4 seconds and 8 minutes 38 seconds |
| Azure Pipelines, `Coastrnaut.mandelbulber2` | Passed | 35 minutes 41 seconds |
| CircleCI, `ci/circleci: build-and-pack` | Passed | |
| Capture Baseline | Skipped, because the push didn't create the branch (FR-007) | |

**The merge**: Pull request #9 merged with a merge commit at 03:24:34 UTC, as `2ba3b8021`, which is S3. `git log --first-parent --format=%H 2679e892a..2ba3b8021` lists only `2ba3b80218a95d600acc54d19d40caffb8c191f5`, so nothing else landed in the window (SC-002). Its Parity Gate push run, [42556244528](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42556244528), passed: "result: passed (0 of 487 items failed)", after the probe line "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 9V74 80-Core Processor, flags fma avx2". "Run Parity Gate" took 4 minutes 29 seconds and the job 12 minutes 46 seconds. Its C++ CI Build (Qt6) run, 42556244605, passed. `baseline/1` is deleted on GitHub with `git push origin --delete baseline/1`, and locally.

## 6. The required check

**The protection** (T028): `gh api -X PUT repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection --input protection.json`, with the JSON in [contracts/parity-gate.md](contracts/parity-gate.md), set classic branch protection at 03:39 UTC on 2026-11-09. This reached S4. Reading it back gave:

| Field | Value |
|---|---|
| `required_status_checks.contexts` | `["parity-gate"]`, the name pull request #9 reported |
| `required_status_checks.strict` | `false` |
| `enforce_admins.enabled` | `false` |
| `required_pull_request_reviews` | none |
| `restrictions` | none |
| `allow_force_pushes.enabled`, `allow_deletions.enabled` | `false`, `false` |

**The first direct push** (T029): `git push origin deterministic-regression` at 03:40:01 UTC landed `2ba3b8021..16401caa6`, which carries T027's commit, and GitHub reported the rule it bypassed:

```text
remote: Bypassed rule violations for refs/heads/deterministic-regression:
remote:
remote: - Required status check "parity-gate" is expected.
```

Its Parity Gate push run, [42557650628](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42557650628), passed: "result: passed (0 of 487 items failed)", after "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 7763 64-Core Processor, flags fma avx2". "Run Parity Gate" took 5 minutes 25 seconds and the job 16 minutes 25 seconds (FR-020, SC-006).

The settings, protected branches, and rulesets at the end, compared with the header's, are in section 9.

## 7. The drift proof

**The local check** (T030): fresh WSL builds of `2ba3b8021`, one as it is in `~/mandelbulber2` and one in `~/mandelbulber2-seed` with `drift.patch`, which changes `2.0 * z.x - z.z` to `2.001 * z.x - z.z` on line 36 of `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`. `git diff --stat` in the seeded tree listed that one file, "1 insertion(+), 1 deletion(-)". `--compare` of the seeded binary exited 1 with "result: parity failure (1 of 487 items failed)", and every differing line names `formula bristorbrot`: `raw_render`, `post_effect`, and `final_image` 3 of 2,304 samples, `alpha` 1 of 768, `depth` 61 of 768, `normal` and `normal_world` 183 of 2,304, and 5 statistics values, the first `total_iterations` 17541 against 17481.

**The pull request**: [Coastrnaut/mandelbulber2#10](https://github.com/Coastrnaut/mandelbulber2/pull/10), "Do not merge: drift proof for the parity gate", from `seed/drift-proof` at `faadf9a1f`, one commit on `16401caa6`, "test: change one constant in bristorbrot to prove the gate". `gh pr view 10 --json files` listed only `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`, with 1 addition and 1 deletion.

| Check | Result |
|---|---|
| `parity-gate`, [42557683515](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42557683515) | Failed at "Run Parity Gate" with "result: parity failure (1 of 487 items failed)" and "Process completed with exit code 1", after "math path: forced, glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4; this CPU's own path: fma; CPU: AMD EPYC 9V74 80-Core Processor, flags fma avx2". All 13 differing lines name `formula bristorbrot`. The gate step took 5 minutes 33 seconds and the job 16 minutes 10 seconds |
| Uploaded outputs | `render-regression-snapshots`, artifact 13648960746, 29,163 bytes |
| `mergeStateStatus` | `BLOCKED` (SC-005) |
| Other checks | CircleCI and one C++ CI Build (Qt6) job had passed. Cross-Build Check, Azure Pipelines, and the pull request's C++ CI Build (Qt6) run were still running when it closed, and nothing waited for them |

**The close**: `gh pr close 10` closed it unmerged at 03:57:17 UTC, `git push origin --delete seed/drift-proof` deleted the branch on GitHub, and `git branch -D seed/drift-proof` deleted it locally. The seeded change never reached `deterministic-regression` (FR-023).

## 8. Closing

**The base** (T034): `git push origin deterministic-regression` landed `16401caa6..bf78f8b7f`, which carries T029's and T033's commits, with the same bypass notice as section 6.

**The amendment** (T035): `amend/kd-4-kd-5`, from `bf78f8b7f`, holds one commit, `c9996ba58`, "docs: close KD-4 and KD-5". It edits only `.specify/memory/constitution.md`, 3 insertions and 3 deletions: KD-4's and KD-5's closing cells gain "**Closed 2026-11-09**, see `specs/004-baseline-parity-gate/evidence.md`", and the version line becomes "**Version**: 1.1.1 | **Ratified**: 2026-11-04 | **Last Amended**: 2026-11-09".

**The local compare** (Workflow 2): fresh WSL builds of `c9996ba58` in `~/mandelbulber2-amend` and `bf78f8b7f` in `~/mandelbulber2`. `--compare` exited 0, "result: passed (0 of 487 items failed)". The branch was pushed before this compare ran, out of T035's order, and the pull request was opened only after it passed.

**The pull request**: [Coastrnaut/mandelbulber2#11](https://github.com/Coastrnaut/mandelbulber2/pull/11), "Close KD-4 and KD-5 (constitution 1.1.1)", opened at 04:02:05 UTC. Its body states the reason, the version change, and the dates, names the plans of features 001 to 004 and the gates Workflow 2 and Principles I and II, links this evidence, and confirms compliance (FR-024).

| Check | Result | Time |
|---|---|---|
| `parity-gate`, [42559545224](https://github.com/Coastrnaut/mandelbulber2/actions/runs/42559545224) | Passed | "Run Parity Gate" 5 minutes 27 seconds, the job 16 minutes 34 seconds |
| Cross-Build Check | `linux-qmake`, `windows-msvc`, and `cross-check` passed | 20 minutes 24 seconds, 27 minutes 9 seconds, and 25 seconds |
| C++ CI Build (Qt6) | Both `Build & Test (Linux)` jobs passed | 8 minutes 57 seconds and 10 minutes 37 seconds |
| CircleCI | Passed | |
| Azure Pipelines | Still pending at the merge. On 2026-11-09 the maintainer said not to wait for it any more, so it was left out | |

**The merge**: Pull request #11 merged with a merge commit at 05:20:42 UTC, as `60beeac34`, which is S5. `amend/kd-4-kd-5` is deleted on GitHub and locally.

**The documents** (T037 to T039), in the commit "docs: correct the playbook, CLAUDE.md, and README for the parity gate", which is S6:

- `MIGRATION_PLAYBOOK.md`: Phase 4's six steps describe what this feature built, and Phase 5's steps 1, 4, and 5 land work on `deterministic-regression` by direct push. A script extracted Phase 4's two listings and compared them with `.github/workflows/parity-gate.yml` and `.github/workflows/capture-baseline.yml`: both are identical (FR-025, SC-007). The appendix matches the README's new section.
- `CLAUDE.md`: the auto-merge rule names the required check, the CI section names Capture Baseline and the required check, and the rules against pull requests and side branches name the Baseline Change, amendment, and drift proof exceptions (FR-026). It also gains the maintainer's rule not to wait for Azure Pipelines.
- `README.md`: it gains the section on continuous integration and regression testing. It says the gate runs on pull requests and on pushes to `deterministic-regression` and is required, and that a contributor compares a change before each push (FR-027).

## 9. Success criteria

**The final checks** (T041), on `5a2888039`:

| Check | Result |
|---|---|
| Files | `git diff --stat d505cee69 deterministic-regression -- . ':!specs' ':!tests/references'` lists 8 files: the two workflows, `tests/regression_runner.py`, `tests/test_regression_runner.py`, the constitution, `MIGRATION_PLAYBOOK.md`, `CLAUDE.md`, and `README.md`. No FR-010 fix was needed |
| Master | `origin/master` is still `33965a498cd8a046b87d0cc5c417432fa79c81db`, T001's value |
| Branches | `git ls-remote --heads origin` lists no `baseline/`, `seed/`, or `amend/` branch |
| Settings | Saved again into [settings/after/](settings/after/). Leaving out timestamps, counts, and commit IDs, `repository.json` and `rulesets.json` are the same as in `settings/before/`. `protected-branches.json` goes from none to `deterministic-regression` alone, requiring `parity-gate`, with `enforcement_level` `non_admins` |
| Messages | `git log d505cee69.. --format=%B` holds no attribution line |
| Tests | `python tests/test_regression_runner.py` on Windows: "Ran 242 tests in 276.605s", OK |

| Criterion | Result | Source | Not covered |
|---|---|---|---|
| SC-001 | Met. Baseline 1 holds 487 references, `toolchain.txt`, and `settings.txt` with the `math_path` line, 83,059,454 bytes, under 104,857,600. All 489 blob IDs match the artifact of capture run 42549364843, whose Self-Parity passed at one thread and at all of the runner's threads | Section 4, `465dc07a3`, `2ba3b8021` | |
| SC-002 | Met. Pull request #9 changed only the 489 files under `tests/references/linux-x86_64-gcc/`, and its description has the nine parts. Its gate run 42553024969 and the merge's push run 42556244528 passed on 487 of 487 items. The first-parent log from `2679e892a` to `2ba3b8021` holds only the merge | Section 5 | |
| SC-003 | Met. Gate steps: 5 minutes 25 seconds on pull request #9, 4 minutes 29 seconds on the merge's push run, 5 minutes 25 seconds on the first push after protection, 5 minutes 33 seconds on the drift proof, and 5 minutes 27 seconds on the amendment, all under 30 minutes. The capture run took 31 minutes 9 seconds, within 350 | Sections 4 to 8 | The runners were hosted 4-vCPU `ubuntu-24.04` machines with AMD EPYC 7763, 9V45, and 9V74 CPUs |
| SC-004 | Met. Forced and hidden renders matched for 487 of 487 items on this computer, and scratch harnesses whose forcing didn't take effect stopped with exit 3 before any render. The capture run's CPU was an AMD EPYC 9V45 and the Baseline 1 gate run's an AMD EPYC 7763, each with "flags fma avx2" and its own path `fma`. Five items differ between glibc's FMA path and the forced path | Sections 2, 4, and 5 | |
| SC-005 | Met. Pull request #10's `parity-gate` run 42557683515 failed with "result: parity failure (1 of 487 items failed)" on `formula bristorbrot`, uploaded `render-regression-snapshots`, and showed `BLOCKED`. It was closed unmerged | Section 7 | |
| SC-006 | Met. The protection requires `parity-gate` alone, the name pull request #9 reported. The maintainer's direct pushes after it landed with GitHub's bypass notice. The settings records show no other change, and pull requests #9, #10, and #11 used no auto-merge | Sections 6 and 9 | |
| SC-007 | Met. The constitution shows KD-4 and KD-5 closed at 1.1.1. The playbook's Phase 4 matches what was built, and its two listings are identical to the committed workflows. `CLAUDE.md` names the required check and the three pull request exceptions, and `README.md` holds the regression section | Section 8, `60beeac34`, `5a2888039` | |
| SC-008 | Met. Only the files listed above changed, besides `specs/` and the references, and `master` is unchanged. Both workflows set `contents: read` and use no secrets. Every harness test passes, and Cross-Build Check passed on pull request #9 | Sections 5 and 9 | |
| SC-009 | Met. `baseline/1`, `seed/drift-proof`, and `amend/kd-4-kd-5` are deleted on GitHub and locally | Sections 5, 7, and 8 | |
| SC-010 | Met. Before the harness change landed, 242 tests passed on WSL and on Windows, and the cross-check of fresh WSL and MSVC collections of `72a898b75` exited 0 with functional agreement, 448 of 487 items identical | Section 2 | |
