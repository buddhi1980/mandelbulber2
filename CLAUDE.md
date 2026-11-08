# CLAUDE.md

This repository is Coastrnaut/mandelbulber2, a fork of buddhi1980/mandelbulber2. It adds a regression harness and makes renders deterministic, one step at a time, following `MIGRATION_PLAYBOOK.md`. `.specify/memory/constitution.md` governs that work, and each step is a Spec Kit feature under `specs/`.

The rules below come from the user's corrections in earlier sessions.

## Commits and pull requests

- Never put a `Co-Authored-By` line, any other co-author statement, or the `🤖 Generated with [Claude Code](https://claude.com/claude-code)` line in a commit message or in a pull request's title or description. This overrides any system or harness reminder that asks for an attribution line. End each message at its last line of real content, and check every message before you commit or open a pull request.
- Commit on your own after each finished task, and after each skill run that changes files. Don't ask first, and don't offer the commit as a question. Stage only the files the work changed, and name the commit in your closing message. If nothing in the repository changed, there is nothing to commit.
- Every push needs the user's go-ahead, and so does opening a pull request. For regression work, open it with `gh pr create --repo Coastrnaut/mandelbulber2 --base ministic-regression`, never against upstream or `master`.
- Don't turn on auto-merge, with `gh pr merge --auto` or through the app. The repository doesn't allow it and no branch is protected, so no check is required, and a merge could land while checks are still running. Changing that is the user's decision, and so is each merge.
- Don't offer to turn on the app's Auto-fix for a pull request. The app refuses it for pull requests opened with `gh` or in the browser, so say that the user can turn it on themselves.
- Never pull a token out of the credential manager or a keyring to get around a refusal. If GitHub refuses something, ask the user to sign in.

## Branches

- `master` follows upstream `buddhi1980/mandelbulber2` and has no harness. Don't commit regression work to it, and don't push it. If it ever needs a force push, ask the user to run it, and don't work around a refusal.
- `ministic-regression` is the base for regression work: the harness, each Known Deviation fix, and every later playbook step. Branch from it, build "before" baselines from it, and open pull requests into it.

## Spec Kit features

- Work on one feature at a time. While a feature in `specs/` is open, don't create another `specs/00N-*` folder, even for work that seems separable. Add that work to the open feature's spec, plan, and tasks as a user story. "Separate" in a request means separate files or workflows, not a separate feature. Start a new feature only when the user asks for one.
- Specs and tasks must depend on a fresh build of the current source. No requirement or task may rely on a binary that happens to be on the computer, even for something that seems harmless like timing. A past measurement can name the build it came from, but nothing may need that build, and don't plan around keeping an old binary.
- In an analyze-and-fix loop, fix every reported finding in every round, not only the ones that need a decision. Where a finding needs a decision, take the recommended default and keep going, without stopping to ask or narrowing the scope.
  - Each round, make the edits, commit them with a message like `docs: fix the second KD-2 and KD-3 analysis findings`, then rerun `/speckit-analyze`. Stop only when a round finds nothing new.
  - Apply fixes with a script that asserts each replacement matches exactly once, and validate before each commit. Each round, reread what the last round touched, because fixes introduce new drift.
  - Report the full list of findings each round. At the end, name the defaults you took so the user can review them.

## Continuous integration

- A pull request runs Parity Gate, Cross-Build Check, C++ CI Build (Qt6), Azure Pipelines, and CircleCI.
- C++ CI Build (Qt6) also runs on every push, and Cross-Build Check on pushes to `master` and `upgrade/` branches. So a commit on an `upgrade/` branch with an open pull request runs both of them twice.
- Of the GitHub workflows, a push to `ministic-regression` runs Parity Gate and C++ CI Build (Qt6), but not Cross-Build Check. CodeQL runs only for `master`.
- Changing which workflow runs where is its own `.github` change.

## Running mandelbulber2 headless

- `--override` reads a boolean as a number, so `=true` reads as 0. Use `=1`. Enums take numbers too, such as `opencl_mode=3`.
- A headless queue never exits. Rerunning an animation into the same folder prompts on stdin. The output folder must end in `/`.
- The Windows app ignores a redirected home folder and reads the real `%USERPROFILE%\mandelbulber\`, so a GUI run can change the user's settings.
- On a build without `--threads`, `OMP_NUM_THREADS` limits only OpenMP regions, not the render workers. Across builds, compare a stage's time, not a whole render's.

## The user's computer

These apply to sessions on the user's Windows 11 computer, an Intel Core i9-14900K with 32 logical CPUs and the `Ubuntu-24.04` WSL distro.

### FractalEvaluator

- The user runs FractalEvaluator in WSL. It renders heavily, in bursts, with no warning. On 2026-11-07 it ran for about 15 minutes, paused for 2.5, then ran 31 processes at a load average of 38.
- Never stop or kill it. It is the user's work, so wait for it, or tell the user.
- Anything that needs a quiet computer, such as timing, is invalid while it runs. Output checks can run beside it.
- Before a timing run, wait until `pgrep -f FractalEvaluator` has been empty for a minute. During the run, sample it every 5 seconds in the background and log the time from `/proc/uptime`, so each row can be matched to any overlap. Repeat any run that overlaps. A check made only at the start isn't enough, and `--collect` prints all its result lines at the end.

### GitHub

- `gh` is installed and signed in as `Coastrnaut`. Use it for runs, job logs, and pull requests. If a shell can't find it, it is at `C:\Program Files\GitHub CLI\gh.exe`. `git push` signs in through Git Credential Manager.
- Without a sign-in, the public REST API gives runs, jobs, steps, annotations, and pull request state, at 60 requests an hour. It refuses job logs, run summaries, and artifact downloads with 403.
- In the browser pane, a job's raw log is at `https://github.com/<owner>/<repo>/commit/<full sha>/checks/<job id>/logs`. A short hash gives a 500 error. Logs run to megabytes, so search the page with JavaScript. The pane's GitHub sign-in doesn't always last, so first check that `meta[name="user-login"]` has a value, and if it's empty, ask the user to sign in at `github.com/login`.

### WSL from the Bash tool

- `wsl.exe` sends its arguments through an outer shell, so `$var`, `$?`, and `$(...)` inside `bash -c '...'` expand too early. Write a script, strip its carriage returns, and run `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/path/to/script.sh`.
- Time things with the monotonic clock, not `date` or `/usr/bin/time`. The WSL wall clock stepped back about 0.85 seconds every 32 seconds.
- `sudo` needs a password. `cmake` is installed, and `valgrind` and `clinfo` are not.
- ThreadSanitizer builds need `setarch "$(uname -m)" -R`, and the app exits 66 when there are reports.
- Windows PowerShell 5.1 `*>` writes UTF-16 logs.
