# Research: Baseline 1 and the Parity Gate

Each item gives a decision, the reason, and what else was considered. "Tested on 2026-11-08" means a run in WSL `Ubuntu-24.04` on the Intel Core i9-14900K, with glibc `2.39-0ubuntu8.9` and Python 3.12.3. FractalEvaluator wasn't running. Scratch scripts stay in `~/verify/kd5` and out of the repository.

## R1. The setting that forces the path

- **Decision**: `GLIBC_TUNABLES=glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`.
- **Tested on 2026-11-08**: A Python program computed `sin`, `cos`, `tan`, `atan`, `asin`, `exp`, `log`, and `sqrt` at 200,000 fixed inputs each, and `atan2` and `pow` at 100,000 pairs each. It ran with no setting, with this setting, with every optional feature hidden, and with the older names that the KD-5 test of `specs/003-strict-floating-point` also used.
  - With this setting, 648 values changed: 155 for `sin`, 136 for `cos`, 118 for `exp`, 117 for `asin`, 72 for `pow`, 31 for `atan`, 16 for `atan2`, and 3 for `log`. `tan` and `sqrt` didn't change in this sample, although the 2026-11-07 test found `tan` values that do.
  - Hiding every optional feature as well gave exactly the values this setting gave. That hidden setting, which SC-004 and the check in R3 use, is `glibc.cpu.hwcaps=-AVX,-AVX2,-FMA,-FMA4,-SSE3,-SSSE3,-SSE4_1,-SSE4_2,-POPCNT,-LZCNT,-MOVBE,-BMI1,-BMI2,-F16C,-AVX512F,-AVX512CD,-AVX512BW,-AVX512DQ,-AVX512VL,-ERMS,-XSAVE,-XSAVEC`. So these three names reach glibc's plain path for every function tested, and the paths that only need AVX give the plain results.
  - The loader's diagnostics showed the setting take effect: `dl_platform` went from `haswell` to `x86_64`.
  - The older names, `-AVX2_Usable,-FMA_Usable`, changed no value, so glibc 2.39 ignores them, and they are left out.
- **Why**: These are the three names the maintainer's answer gives. Together they are the smallest setting that reaches the plain path, and glibc honors them.
- **Not chosen**: Hiding every optional feature, which gives the same values but also moves glibc's string and memory routines, and is a longer list to keep right. Keeping the older names, which do nothing on glibc 2.39.

## R2. Where the harness sets it

- **Decision**: `clean_environment()` adds the setting on Linux, so every render and every query the harness runs gets it: Self-Parity, capture, the gate, compare, and collect. Nothing changes on Windows.
- **Why**: Every render and query already takes its whole environment from that function, which builds it from scratch, so one change covers every mode, and nothing in CI or a user's shell can bypass it. Windows uses the C runtime's math library, where the setting means nothing. On a Linux computer that isn't x86-64, glibc ignores names it doesn't know, and the check in R3 refuses to gate or capture there, because the forced path exists only for x86-64.
- **Not chosen**: Setting it in the workflows, which `clean_environment()` would strip, and which would leave local runs on another path. Setting it only for the gate and capture, which would leave compare and collect on another path than the references, so a local compare wouldn't predict the gate.

## R3. Confirming the forced path

- **Decision**: On Linux, before the gate or capture renders anything, the harness runs a probe. A child Python process, started with the render environment, computes six functions at inputs where glibc's FMA and plain paths differ, and prints the result bits. If the results under the forced setting are the plain path's, the run goes on. Otherwise the child runs again with the hidden setting from R1, and the run stops with a setup error, exit 3. The error says the forcing missed a path when the forced and hidden results differ, and that glibc's plain results changed when they match. The same probe, run without any setting, shows which path this CPU takes by itself. The run's report prints that, with the CPU model and its `fma`, `fma4`, and `avx2` flags. Self-Parity, compare, and collect don't run the probe. On Windows, the gate runs only for debugging, says the path isn't forced, and can't pass (FR-002).
- **Tested on 2026-11-08**: These inputs came from the run in R1. Bits are big-endian hexadecimal.

| Function | Input | Plain path | FMA path |
|---|---|---|---|
| `sin` | `c023f95182a9930c` | `3fe10e8ae531c9f9` | `3fe10e8ae531c9f8` |
| `cos` | `c023a35a858793de` | `bfed8b7b0b14170a` | `bfed8b7b0b14170b` |
| `exp` | `c033f504816f0069` | `3e227b3381e9a676` | `3e227b3381e9a677` |
| `log` | `3ff2e042927c0e01` | `3fc528a95565bf3c` | `3fc528a95565bf3b` |
| `atan` | `c02bb0cb295e9e1b` | `bff7faa774776c93` | `bff7faa774776c94` |
| `asin` | `bfef0b04ab606b7b` | `bff535bf4d1dfedb` | `bff535bf4d1dfedc` |

- **Why**: The probe tests what matters, the bits glibc's math gives under the render environment. So it catches a renamed setting, a stripped variable, or a glibc that picks another path, and it takes well under a second. A CPU without FMA takes the plain path anyway, so the probe passes there too, which is right. Python's `math` functions call glibc's, and the child sees the same glibc and setting as a render.
- **Not chosen**: Parsing `ld.so --list-diagnostics`, whose format is internal and which shows CPU features, not the path a function takes. Reading `/proc/cpuinfo`, which says what the CPU offers, not what glibc chose. Trusting the variable, which fails silently if glibc renames it. A probe inside `mandelbulber2`, which is an engine change.
- **Limit**: These are glibc 2.39's plain results. A glibc that changes its plain code fails the probe with the message that its plain results changed. The message gives the new results, which the hidden run shows are plain, and they land as a harness change (FR-010). That toolchain then needs a Baseline Change anyway.

## R4. Recording the forced path

- **Decision**: `settings.txt` gains a fifth line, `math_path glibc.cpu.hwcaps=-AVX2,-FMA,-FMA4`. The gate already refuses a set whose settings differ, with "render settings differ from the reference set", so a set made without the forcing is refused before anything renders. Collections don't change.
- **Why**: One line reuses a check that exists and already fails as a setup error. Cross-check compares only the four collection settings, so leaving collections alone keeps Cross-Build Check working, and the MSVC build has no such setting to record. The line holds the setting itself, so changing the setting later makes the old set's line differ, and that needs a Baseline Change, as any change to gated output does.
- **Not chosen**: `toolchain.txt`, where a difference counts as a reference target mismatch, so the gate would still render and compare. A separate file, which is one more file to keep in step. A collection field, which cross-check would have to skip for Windows.

## R5. How capture starts

- **Decision**: `capture-baseline.yml` runs on a push to `baseline/**`, and its job runs only when that push created the branch, `if: github.event.created`. Baseline 1 starts with `git push origin deterministic-regression:refs/heads/baseline/1`.
- **Why**: FR-007 asks for capture on the push that makes the branch and on nothing later. The push event's `created` field says exactly that, with no path filter. Every later push to the branch, such as the references commit, starts a run whose job is skipped.
- **Risk**: GitHub's Actions documentation doesn't say whether a push that only creates a branch, with no new commit, starts a workflow, although the push event includes such a push with `created` set. The quickstart checks that a capture run started. If none did, delete the branch on GitHub and push it again with one empty commit on top. That push both creates the branch and carries a commit.
- **Repeats**: A run that failed outside the source, such as a runner fault, can be rerun from its page on GitHub. A rerun repeats the same push event, so `created` still holds. Any other repeat deletes the branch on GitHub and pushes it again at the commit to capture. Captures on different branches are independent, and only the maintainer pushes `baseline/` branches.
- **Not chosen**: `paths-ignore: tests/references/**`, because GitHub compares a new branch's push against other commits by rules its documentation describes loosely, so the first run could be skipped. A step that reads the last commit's files, which would skip capture on a branch made at a commit that only added references, such as Baseline 1's merge. A tag, and making `deterministic-regression` the default branch, which the Clarifications didn't choose.

## R6. The capture workflow's steps

- **Decision**: Checkout, `compile-engine`, the harness's own tests, capture, an upload of `tests/references` as `baseline-references`, kept for 90 days, the longest GitHub allows for a public repository, with `if-no-files-found: error`, and an upload of `tests/outputs` as `capture-failure-outputs` when the job fails, kept for 7 days. Read-only access and a 350-minute limit, as the playbook lists.
- **Why**: The harness's tests fail in about a minute, not after hours of rendering. The failure outputs keep the run's report, which FR-010 needs to find a cause.
- **Not chosen**: The playbook's listing as it stands, which has neither step.

## R7. The gate steps and the red window

- **Decision**: The gate steps are the playbook's listing. The order of work is:
  1. The harness change and the capture workflow land by direct push.
  2. A push creates `baseline/1`, and capture runs.
  3. The references are committed on `baseline/1` and pushed.
  4. When the maintainer can review and merge Baseline 1 in the same sitting, the gate steps land on `deterministic-regression` by direct push.
  5. The Baseline 1 pull request opens at once.
  6. The maintainer merges it, and the push run passes.

  The failing push run from step 4, and how long the window lasted, are recorded after step 6. If the pull request's gate fails, the gate steps are reverted at once, the pull request closes, and the work goes back to step 2 once the cause is known (FR-013).
- **Why**: A pull request's run uses the merge commit, which takes the gate steps from the base and the references from the branch, so it runs the real gate. Capture and the gate build from the same source, because the gate steps change only a workflow file. The window lasts one pull request run, about 20 minutes.
- **Not chosen**: Landing the gate steps first, which leaves every push red through hours of capture. Putting them in the Baseline 1 pull request, which may hold only reference files.

## R8. References that match the artifact byte for byte

- **Decision**: Download the artifact with `gh run download <run> -n baseline-references -D <folder>`, copy it into `tests/references/` on `baseline/1`, and commit. Then compare each blob ID in `git ls-tree -r HEAD tests/references` with `git hash-object --no-filters` of the downloaded file. Equal IDs mean equal bytes.
- **Size, found on 2026-11-08**: A collection of the KD-1 final commit, which uses the reference set's zip format, takes 83,055,069 bytes: 73,881,296 for the scenes and 9,173,773 for the formulas. So Baseline 1 should take about 79% of the harness's 104,857,600-byte limit. Every zip stays far below GitHub's 50 MB warning size.
- **Why**: A blob ID hashes the exact stored bytes, so the check catches a line-ending conversion of either text file, whichever checkout makes the commit.
- **Not chosen**: A `.gitattributes` rule, which changes a file that SC-008 doesn't allow. Committing from WSL, which has no GitHub sign-in.

## R9. The required check

- **Decision**: Classic branch protection on `deterministic-regression`, set with one call that the maintainer approves:

```text
gh api -X PUT repos/Coastrnaut/mandelbulber2/branches/deterministic-regression/protection --input protection.json
```

`protection.json` requires the `parity-gate` check, with `strict` false, `enforce_admins` false, no required reviews, and no push restrictions. GitHub also refuses force pushes and deletion of a protected branch from anyone who can't bypass the rule. That comes with protecting any branch.
- **Why**: The job reports its check as `parity-gate`. With `enforce_admins` false, the maintainer's direct pushes land. With `strict` false, a pull request doesn't have to be brought up to date with every new commit before it can merge, which would rerun the gate each time. Nothing else is required.
- **Records and undoing it**: Before the first change, the evidence saves `gh api repos/Coastrnaut/mandelbulber2`, the list of protected branches, and the list of rulesets, and it saves them again at the end (FR-022). If the check blocks pull requests it shouldn't, the maintainer corrects it with the same call, or removes it with `gh api -X DELETE` on the same path (FR-019).
- **Not chosen**: A repository ruleset, a second system to keep right for one rule, whose bypass list must name the admins. `strict` true. Pinning the check to the GitHub Actions app's ID, which blocks every pull request if the ID is wrong.

## R10. The drift proof

- **Decision**: On a `seed/drift-proof` branch, change `2.0 * z.x - z.z` to `2.001 * z.x - z.z` on line 36 of `mandelbulber2/formula/definition/fractal_bristorbrot.cpp`. Before the branch is pushed, `--compare` between fresh WSL builds of `deterministic-regression` and of the seeded tree must find exactly one item that differs, `formula bristorbrot`.
- **Found on 2026-11-08**: Bristorbrot is formula 21. The scenes' formula lines name 24 IDs, and 21 isn't one of them: 3, 4, 7, 8, 10, 50, 71, 72, 77, 82, 88, 91, 101, 102, 103, 106, 107, 113, 115, 229, 603, 604, 800, and 1603. The sweep leaves out only `custom`.
- **Why**: One constant in a simple formula changes that formula's output alone. The local compare proves it before CI does, including for any scene that relies on a default formula.
- **Not chosen**: A formula a scene uses, which fails a scene too and blurs what the proof shows. A change that alters only statistics, which is harder to make on purpose.

## R11. What forcing changes (FR-005 and SC-004)

- **Decision**: Build the commit that holds the forcing fresh in WSL, with `~/verify/kd5/build_fresh.sh`, a copy of `~/verify/kd1/build_fresh.sh`. Then run `--collect` three times: with the committed harness, with a scratch copy that leaves the setting out, and with one that hides every optional feature from R1. A scratch script compares the collections item by item and buffer by buffer, as in `specs/003-strict-floating-point`. The forced and hidden collections must match for all 487 items (SC-004), and the evidence counts what differs between the natural and forced ones (FR-005).
- **Why**: Collect renders each item once with the harness's environment, so a one-line scratch change to the setting is the only difference between the runs. These runs check output, not time, so they can run while FractalEvaluator renders. The gate's time comes from CI (FR-014).

## R12. Documents

- **Decision**:
  - **The constitution** (FR-024): KD-4 and KD-5 are marked closed, with the date and a link to `specs/004-baseline-parity-gate/evidence.md`. The version becomes 1.1.1, and the last amended date updates. The amendment names the plans of features 001 to 004, which list KD-4 or KD-5 as open, and the gates Workflow 2 and Principles I and II. Its pull request edits only this file.
  - **The playbook** (FR-025):
    - Phase 4's step 1 gets both workflow listings as committed, and says the gate steps land after capture. Phase 3's listing of `parity-gate.yml` stays as Phase 3 built it.
    - Step 2 records the forced path, its probe, and its settings line.
    - Step 3 starts capture with the push of `baseline/1`, then downloads, checks, and commits the references, pushes the gate steps, and opens the pull request.
    - Step 4 says the maintainer's direct pushes bypass the required check, and that a red push run is a defect to fix at once.
    - Step 5 names the drift proof's formula.
    - Step 6 says 1.1.1, and that the amendment's pull request edits only the constitution.
    - Phase 5's steps 1, 4, and 5 say that work lands on `deterministic-regression` by direct push, which the push run of the required gate checks.
  - **`CLAUDE.md`** (FR-026): Its auto-merge rule loses "no branch is protected, so no check is required", and its CI section names Capture Baseline and the required check. Its rules against pull requests and side branches name the three this feature needs: a Baseline Change, an amendment, and a drift proof.
  - **`README.md`** (FR-027): It gains the playbook appendix's section. The section says that the gate runs on pull requests and on pushes to `deterministic-regression` and is required, and "Before committing changes to feature branches" becomes a comparison before each push.
- **Why**: Each of these statements describes what this feature changes, so each would be wrong once it lands.

## R13. Reviewed changes under direct pushes

- **Decision**: An engine fix that capture needs lands as one direct push that holds only the fix, with its expected effect on output in the commit message and the evidence, after the maintainer reads it and gives the go-ahead (FR-010). A direct push that fails the gate after Baseline 1 is fixed or reverted by the next commit, and the maintainer chooses which (FR-021).
- **Why**: Workflow 1 asks for a reviewed change, not a pull request, and the maintainer reviews each push before the go-ahead. Only a Baseline Change and an amendment must be pull requests.
- **Not chosen**: A pull request for each engine fix, which the maintainer no longer wants where the constitution doesn't require one.

## Changes to the spec

Research settled the exact setting (R1), the probe (R3), where the record lives (R4), the trigger (R5), and the drift proof's formula (R10), and each fit the spec as written. On 2026-11-08, the review checklist in `checklists/review.md` led to these spec changes:

- **Recovery**: How capture repeats (FR-007, FR-010), what happens when the Baseline 1 gate fails (FR-013) or the push after its merge fails (FR-017), and how the required check is undone (FR-019).
- **Clearer terms**: The plain path, the probe's two failures, and the modes it covers (FR-002, FR-003, FR-005), a full gate run (FR-014), pull requests for others (FR-020), the next commit after a failure (FR-021), Phase 5 (FR-025), and the README's statements (FR-027).
- **Records**: The repository's settings before and after (FR-022), and the named plans and gates (FR-024).
- **Measures**: One unit for the size limit, the capture's 350 minutes, every gate run under 30 minutes, the check's name (SC-006), and the local cross-check (SC-010).
