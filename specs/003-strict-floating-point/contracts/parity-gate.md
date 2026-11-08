# Contract: `parity-gate` and `compile-engine`

The new workflow and reusable action. At this stage `parity-gate` compiles the engine and checks its flags. Playbook Phase 4 adds the gate steps.

## `.github/actions/compile-engine/action.yml`

A composite action, named `Compile Engine`, with a `description`, which GitHub's action metadata requires. Its steps, in order:

| Step | What it does | On failure |
|---|---|---|
| Search the Build Files | Runs `check-flags.sh search` | The job stops before it builds |
| Build and Install to a Local Tree | Uses `./.github/actions/build-linux`, which does not change | The job stops |
| Reject Prohibited Compile Flags | Runs `check-flags.sh flags mandelbulber2/build/Makefile` | The job fails |

Each script step runs with `shell: bash`, and finds the script through `$GITHUB_ACTION_PATH`.

## `.github/workflows/parity-gate.yml`

| Setting | Value |
|---|---|
| Name | `Parity Gate` |
| Triggers | `pull_request`, with no branch filter, and `push` to `ministic-regression` |
| Access | `permissions: contents: read` at the top of the file, and no secrets |
| Job | `parity-gate` |
| Runner | `ubuntu-24.04` |
| Time limit | `timeout-minutes: 120` |
| Steps | `actions/checkout@v7`, then `./.github/actions/compile-engine` |

## Rules

- **Full checkout not needed**: `actions/checkout` keeps its default depth, because `git grep` reads only the current tree.
- **Time**: About 7 minutes on a clean run, from the last KD-1 pull request's checkout and build steps. A bad build file fails in seconds.
- **Status**: Not a required check. Phase 4 makes it one.
- **Other workflows**: Nothing else changes (FR-016). Cross-Build Check still builds through `build-linux` and runs no flag check.
- **Reuse**: Phase 4's `capture-baseline.yml` uses `compile-engine`, so a capture gets the search and the flag check too.
- **Pull requests from this branch**: Each pull request commit on an `upgrade/` branch also starts Cross-Build Check for the push, and `parity-gate` runs only for the pull request.

## Closing Conditions

| Deviation | Closed when | Proof |
|---|---|---|
| KD-2 | The CI flag check passes on all gated builds | `parity-gate` passes on the pull request (SC-001) |
| KD-3 | The CI flag check rejects host-tuned flags on gated builds | The second run under GitHub Actions in [flag-check.md](flag-check.md) (SC-003) |
