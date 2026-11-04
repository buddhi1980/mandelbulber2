# Quickstart: Validating the Regression Harness

These scenarios prove the feature works end to end. Commands are defined in [contracts/runner-cli.md](contracts/runner-cli.md).

## Prerequisites

- Python 3.10 or later.
- For scenarios 2 to 6, a Linux machine with the Qt6 and library packages from the playbook.
- For scenario 7, a Windows build.

Build the installed tree:

```bash
mkdir -p mandelbulber2/build && cd mandelbulber2/build
qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
make -j$(nproc)
make INSTALL_ROOT=appdir install
cd ../..
```

## Scenario 1: Harness Logic With the Stand-In Renderer

No build is needed.

```bash
python3 tests/test_regression_runner.py
```

**Expected**: All tests pass. Together they cover:

- Each failure condition, seeded one at a time, fails and names the cause.
- One flipped bit in any buffer fails, and so does a change of 1 in any statistic.
- Identical output passes in gate and compare modes.
- A toolchain difference passes with a note when output matches, and exits 3 as a Reference Target mismatch when it differs.
- The render command uses the fixed settings, and each render gets an isolated home and a clean environment.
- An unstable render blocks capture, and references stay unchanged.
- Capture refuses outside CI, and an interrupted capture leaves references byte-for-byte unchanged.

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
- `raw_render.bin`, `post_effect.bin`, and `normal.bin` are 921,600 bytes each.
- `final_image.bin` is 460,800 bytes.
- `depth.bin` is 307,200 bytes.

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

## Scenario 7: Windows Debug Run

Run from the repository root, with the path to your build:

```bash
MANDELBULBER_BINARY=build-win/Release/x64/mandelbulber2.exe python tests/regression_runner.py --self-parity
```

**Expected**: The report states that the run is not gate-valid, the run does not pass, and `--capture` refuses to run. After playbook Phase 3, `--compare` can pass on Windows, labeled as not gate-valid.

## After Playbook Phase 3

Before Baseline 1, confirm these outcomes:

| Check | Criterion |
|---|---|
| `--self-parity` passes at two thread counts | SC-003 |
| The final image matches with the normal buffer and export on or off | SC-007 |
| A build compared with itself reports no differences | SC-010 |
