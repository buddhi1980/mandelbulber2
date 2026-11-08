# Contract: The Evidence Record

What `specs/004-baseline-parity-gate/evidence.md` holds. Each result names the commit, run, or pull request it came from, and nothing is recorded from a binary older than its commit.

## Sections

1. **Header**: The commits of S1 to S6 in [data-model.md](../data-model.md), the local toolchain, the machine, and how each local result was checked against FractalEvaluator.
2. **The forced path, locally** (FR-001 to FR-005):
   - The harness's tests on WSL and on Windows, with their counts.
   - The probe's line on this computer.
   - The forced and hidden collections, which must match for all 487 items (SC-004).
   - The natural and forced collections: which items and buffers differ, and how many (FR-005).
   - The probe failing for a scratch harness that leaves out the setting, and for one that misspells it, with exit 3 and no render.
   - Collect in WSL and on Windows, then cross-check, with its result and count of identical items (FR-004).
3. **Landing S1**: The push and its Parity Gate and C++ CI Build (Qt6) runs.
4. **Capture** (FR-006 to FR-010):
   - The run, its CPU line and own math path, its Self-Parity result, its step times, and the job's time.
   - The artifact's file counts and size.
   - The blob ID check of every committed file.
5. **Baseline 1** (FR-013 to FR-018):
   - The failing push run of the gate steps, and how long the window lasted.
   - The pull request, its `parity-gate` run with the gate step's time, the job's time, and the CPU line.
   - Cross-Build Check's result and count of identical items, and the other checks.
   - The merge, the passing push run, and the deleted branch.
6. **The required check** (FR-019 to FR-022): The protection read back, the first direct push afterwards with GitHub's bypass notice, and the repository settings left unchanged.
7. **The drift proof** (FR-023): The local compare, the pull request, the failing run and the item it named, the uploaded outputs, `mergeStateStatus`, and the closed pull request and deleted branch.
8. **Closing** (FR-024 to FR-027): The amendment's pull request and merge, and the document commits.
9. **Success criteria**: One row for each of SC-001 to SC-009: the result, the commit or run, and anything not covered.

## Rules

- **Quoting CI**: A CI result quotes the job log's own words, such as the probe's line and the harness's result line, and links the run.
- **Times**: Times come from the job's step list, in minutes and seconds.
- **Not covered**: Anything not covered is named in its row, not left out.
