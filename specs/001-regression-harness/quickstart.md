# Quickstart: Validating the Regression Harness

These scenarios prove the feature works end to end. Commands are defined in [contracts/runner-cli.md](contracts/runner-cli.md).

## Prerequisites

- Python 3.10 or later. On Windows, use `python` in place of `python3`.
- For scenarios 2 to 6, the WSL `Ubuntu-24.04` distro or another Linux machine, with the Qt6 and library packages from the playbook. In WSL, build from a Linux clone, not `/mnt/c`, because the Windows checkout has CRLF line endings.
- For scenario 7, a Windows build.
- For scenario 8, both builds of one commit on one computer.
- For scenario 9, a branch pushed to GitHub.

Build the installed tree in WSL or on Linux:

```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
```

Build on Windows in PowerShell, from the repository root. These are the same restore and build steps as `.appveyor.yml`:

```powershell
nuget restore mandelbulber2/msvc/mandelbulber2.sln
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" mandelbulber2/msvc/mandelbulber2.sln /m /p:Configuration=Release /p:Platform=x64
```

## Scenario 1: Harness Logic With the Stand-In Renderer

No build is needed. Run this in WSL, and with `python` on Windows.

```bash
python3 tests/test_regression_runner.py
```

**Expected**: All tests pass in both places. Together they cover:

- Each failure condition, seeded one at a time, fails and names the cause.
- One flipped bit in any buffer fails, and so does a change of 1 in any statistic.
- Identical output passes in gate and compare modes.
- A toolchain difference passes with a note when output matches, and exits 3 as a Reference Target mismatch when it differs.
- The render command uses the fixed settings, and each render gets an isolated home and a clean environment.
- An unstable render blocks capture, and references stay unchanged.
- Capture refuses outside CI, and an interrupted capture leaves references byte-for-byte unchanged.
- A cross-check fails on each functional disagreement, and it records a flipped bit without failing on it.

## Scenario 2: Application Contract

```bash
B=mandelbulber2/build/appdir/usr/bin/mandelbulber2
mkdir -p /tmp/parity
$B --list-formulas | grep -c $'\t'
```

**Expected**: The count equals the number of formulas the binary offers, 462 at the time of writing.

```bash
$B --nogui --res 320x240 --format tiff \
  --override 'opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4' \
  --output /tmp/parity/image --verification-export /tmp/parity/export \
  mandelbulber2/deploy/share/mandelbulber2/examples/IFS_001.fract
ls -l /tmp/parity/export
```

**Expected**:

- `manifest.txt` ends with `end`.
- `raw_render.bin`, `post_effect.bin`, `normal.bin`, and `normal_world.bin` are 921,600 bytes each.
- `final_image.bin` is 460,800 bytes.
- `alpha.bin` is 153,600 bytes.
- `depth.bin` is 307,200 bytes.
- `statistics.txt` holds `total_iterations`, `raymarch_count`, `missed_de`, and every bucket of both histograms, all as whole numbers.

## Scenario 3: A Failed Load Writes No Export

```bash
printf 'not a scene\n' > /tmp/parity/bad.fract
rm -rf /tmp/parity/bad_export
$B --nogui --res 320x240 --output /tmp/parity/bad --verification-export /tmp/parity/bad_export /tmp/parity/bad.fract
test -f /tmp/parity/bad_export/manifest.txt && echo FOUND || echo MISSING
```

**Expected**: `MISSING`, even though the application exits 0.

## Scenario 4: Self-Parity Reports KD-1

```bash
python3 tests/regression_runner.py --self-parity
echo "exit: $?"
```

**Expected on the current build**: A non-zero exit, sample differences reported per item and buffer, and a statement that thread coverage is incomplete. This is the correct result until playbook Phase 3.

**Expected after playbook Phase 3**: Exit 0 with both thread counts enabled.

## Scenario 5: Capture and Gate Refuse Safely

```bash
python3 tests/regression_runner.py --capture
echo "exit: $?"
python3 tests/regression_runner.py
echo "exit: $?"
```

**Expected on the current build**: Both exit 3. Capture refuses before rendering and explains why, because it runs only in CI and needs a second thread count. The gate reports that no reference set exists. Nothing appears under `tests/references/`.

## Scenario 6: Compare Mode

```bash
python3 tests/regression_runner.py --compare mandelbulber2/build/appdir/usr/bin/mandelbulber2
echo "exit: $?"
```

**Expected on the current build**: Exit 1, because renders do not reproduce yet.

**Expected after playbook Phase 3**: Exit 0 when a build is compared with itself, and a report that this is a local check.

## Scenario 7: Windows Run

From the repository root, the harness finds the Windows build by default:

```bash
python tests/regression_runner.py --self-parity
```

**Expected**: The report names the `windows` environment and states that the run is not gate-valid, the run does not pass, and `--capture` refuses to run. After playbook Phase 3, `--compare` can pass on Windows, labeled as not gate-valid.

## Scenario 8: Cross-Check Two Builds on One Computer

Commit your work on Windows, because WSL can only see commits. Then bring that commit into the WSL clone and build it, replacing `upgrade/regression-harness` with your branch:

```bash
cd ~/mandelbulber2
git fetch /mnt/c/GIT/mandelbulber2 upgrade/regression-harness
git checkout --detach FETCH_HEAD
cd mandelbulber2/build
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
python3 tests/regression_runner.py --collect /mnt/c/GIT/mandelbulber2/tests/outputs/collection/qmake
```

Run `qmake6` again first if a source file was added. Next, on Windows, build the same commit, then collect and cross-check:

```bash
python tests/regression_runner.py --collect tests/outputs/collection/msvc
python tests/regression_runner.py --cross-check tests/outputs/collection/qmake tests/outputs/collection/msvc
```

**Expected on the current build**:

- Both collections exit 0, and the Windows one is labeled as not isolated.
- The cross-check exits 0 with `result: functional agreement`.
- It reports that almost no items matched exactly, because renders do not reproduce yet. On 2026-11-05 that was 0 of 487.
- It states that it does not replace the gate.
- `tests/outputs/cross-check-details.txt` lists every differing statistic.

If a scene fails on one build, that `--collect` exits 3, and the cross-check then exits 1 and names the item and the build.

## Scenario 9: Cross-Build Check in GitHub Actions

Push the branch, then open the **Cross-Build Check** run in the repository's **Actions** tab. [contracts/ci-pipeline.md](contracts/ci-pipeline.md) defines the pipeline.

**Expected**:

- `linux-qmake`, `windows-msvc`, and `cross-check` all pass.
- The run summary shows the cross-check report, and the report names the commit and both toolchains.
- The `collection-qmake`, `collection-msvc`, and `cross-check-report` artifacts exist, and the last one holds the report and the details file.
- The pipeline's Application Tests step passes on both jobs, with 8 tests passing on each.

Then confirm that the pipeline changed nothing else:

```bash
git diff --exit-code master -- .github/workflows/github-actions-ci.yml .github/workflows/codeql.yml .circleci .appveyor.yml azure-pipelines.yml .travis.yml
```

**Expected**: No output, and the existing `C++ CI Build (Qt6)` workflow still passes on the same commit.

To see a failure, use a throwaway branch and never merge it. In `printFormulasAndExit`, skip `mandelbulbPower2` under `#ifdef _WIN32`, then push.

**Expected**: `windows-msvc` still builds, `cross-check` fails, and the run summary states that `mandelbulbPower2` is offered by the first collection only.

## After Playbook Phase 3

Before Baseline 1, confirm these outcomes:

| Check | Criterion |
|---|---|
| `--self-parity` passes at two thread counts | SC-003 |
| The final image matches with the normal buffer and export on or off | SC-007 |
| Two builds of the same source report no differences, and a deliberate change to one formula is reported against that formula | SC-010 |
