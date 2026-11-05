# Contract: Cross-Build Check Pipeline

A separate GitHub Actions pipeline that builds one commit with both build systems, tests each build, and cross-checks them. It is added beside the existing workflows, and it changes none of them.

## Files

| File | Change |
|---|---|
| `.github/actions/build-linux/action.yml` | New |
| `.github/actions/build-windows/action.yml` | New |
| `.github/workflows/cross-build-check.yml` | New |
| `.gitignore` | Gains `!.github/actions/build-*/`, because `build-*/` would otherwise ignore both action folders |
| `.github/workflows/github-actions-ci.yml` | Must stay byte-identical |
| `.github/workflows/codeql.yml` | Must stay byte-identical |
| `.circleci/config.yml`, `.appveyor.yml`, `azure-pipelines.yml`, `.travis.yml` | Must stay byte-identical |

## Where the Build Steps Come From

The pipeline repeats steps that already work in CI, and it copies nothing else from those files.

| Source | Taken | Left out |
|---|---|---|
| `.appveyor.yml` | `nuget restore` from `before_build`, `msbuild` with `/m` from `build_script`, a Visual Studio 2026 image | The remote desktop `init` step, the `/logger` option, the zip and artifact steps, the `Debug` configuration |
| `.github/workflows/github-actions-ci.yml` | The package lists, `qmake6`, `make`, and `make INSTALL_ROOT=appdir install` | `make-package.sh`, `./install`, the AppImage steps, and the release upload |

The Windows build also runs Inno Setup, because the project does so whenever `ISCC.exe` exists, and the runner image has it. That adds about 2 minutes and makes an installer nothing uses.

## Same Commands in All Three Environments

The pipeline runs the commands a developer runs locally. Only paths and the Python launcher differ.

| Step | WSL | Windows | GitHub Actions |
|---|---|---|---|
| Build | `qmake6`, `make`, `make install` | `nuget restore`, `msbuild` | `build-linux` and `build-windows` run those same commands |
| Stand-in tests | `python3 tests/test_regression_runner.py` | `python tests/test_regression_runner.py` | Both build jobs |
| Application tests | `--test` with a temporary `HOME` | `--test` from the build's output folder | Both build jobs |
| Collect | `python3 tests/regression_runner.py --collect <dir>` | `python tests/regression_runner.py --collect <dir>` | Both build jobs |
| Cross-check | Either side, once both collections are readable | Either side, once both collections are readable | The `cross-check` job |

## `.github/actions/build-linux/action.yml`

```yaml
name: Build Linux
description: Install dependencies, then build and install the release binary with qmake.
runs:
  using: composite
  steps:
    - name: Install Build Dependencies
      shell: bash
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential mesa-common-dev libgl1-mesa-dev \
          libpng-dev libgsl-dev libgomp1 liblzo2-dev libtiff-dev libsndfile1-dev \
          opencl-headers ocl-icd-libopencl1 ocl-icd-opencl-dev \
          qt6-base-dev qt6-tools-dev qt6-multimedia-dev qt6-svg-dev \
          qt6-tools-dev-tools qt6-declarative-dev
    - name: Build and Install to a Local Tree
      shell: bash
      run: |
        mkdir -p mandelbulber2/build
        cd mandelbulber2/build
        qmake6 ../qmake/mandelbulber-opencl.pro CONFIG+=release
        make -j$(nproc)
        make INSTALL_ROOT=appdir install
```

## `.github/actions/build-windows/action.yml`

```yaml
name: Build Windows
description: Restore packages, then build the Release binary with MSVC.
runs:
  using: composite
  steps:
    - name: Add MSBuild to the Path
      uses: microsoft/setup-msbuild@v3
    - name: Restore Packages
      shell: pwsh
      run: nuget restore mandelbulber2/msvc/mandelbulber2.sln
    - name: Build
      shell: pwsh
      run: msbuild mandelbulber2/msvc/mandelbulber2.sln /m /p:Configuration=Release /p:Platform=x64
    - name: Copy OpenCL.dll Beside the Binary
      shell: pwsh
      run: Copy-Item mandelbulber2/msvc/packages/opencl-nuget-msvc-x64.*/build/native/bin/OpenCL.dll build-win/Release/x64/
```

A clean build does not put `OpenCL.dll` beside the binary, and a hosted runner may not have a copy in `System32` (research R17).

## `.github/workflows/cross-build-check.yml`

```yaml
name: Cross-Build Check

on:
  pull_request:
  push:
    branches:
      - master
      - 'upgrade/**'
  workflow_dispatch:

permissions:
  contents: read

concurrency:
  group: cross-build-check-${{ github.ref }}
  cancel-in-progress: true

jobs:
  linux-qmake:
    runs-on: ubuntu-24.04
    timeout-minutes: 90
    steps:
      - uses: actions/checkout@v7
      - id: build
        uses: ./.github/actions/build-linux
      - name: Test the Harness
        run: python3 tests/test_regression_runner.py
      - name: Run the Application Tests
        run: env -i HOME="$(mktemp -d)" PATH="$PATH" LC_ALL=C.UTF-8 QTEST_FUNCTION_TIMEOUT=1800000 mandelbulber2/build/appdir/usr/bin/mandelbulber2 --test
      - name: Collect
        if: ${{ !cancelled() && steps.build.outcome == 'success' }}
        run: python3 tests/regression_runner.py --collect tests/outputs/collection/qmake
      - name: Upload Collection
        if: ${{ always() }}
        uses: actions/upload-artifact@v7
        with:
          name: collection-qmake
          path: tests/outputs/collection/qmake
          if-no-files-found: warn
          overwrite: true
          retention-days: 7

  windows-msvc:
    runs-on: windows-2025-vs2026
    timeout-minutes: 120
    steps:
      - uses: actions/checkout@v7
      - uses: actions/setup-python@v7
        with:
          python-version: '3.12'
      - id: build
        uses: ./.github/actions/build-windows
      - name: Test the Harness
        run: python tests/test_regression_runner.py
      - name: Run the Application Tests
        working-directory: build-win/Release/x64
        env:
          QT_FORCE_STDERR_LOGGING: '1'
          QTEST_FUNCTION_TIMEOUT: '1800000'
        run: .\mandelbulber2.exe --test
      - name: Collect
        if: ${{ !cancelled() && steps.build.outcome == 'success' }}
        run: python tests/regression_runner.py --collect tests/outputs/collection/msvc
      - name: Upload Collection
        if: ${{ always() }}
        uses: actions/upload-artifact@v7
        with:
          name: collection-msvc
          path: tests/outputs/collection/msvc
          if-no-files-found: warn
          overwrite: true
          retention-days: 7

  cross-check:
    needs: [linux-qmake, windows-msvc]
    if: ${{ !cancelled() }}
    runs-on: ubuntu-24.04
    timeout-minutes: 30
    steps:
      - uses: actions/checkout@v7
      - name: Download Collections
        uses: actions/download-artifact@v8
        with:
          path: tests/outputs/collection
      - name: Cross-Check
        run: python3 tests/regression_runner.py --cross-check tests/outputs/collection/collection-qmake tests/outputs/collection/collection-msvc
      - name: Publish the Report
        if: ${{ always() }}
        run: |
          if [ -f tests/outputs/report.txt ]; then
            sed 's/^/    /' tests/outputs/report.txt >> "$GITHUB_STEP_SUMMARY"
          fi
      - name: Upload the Report
        if: ${{ always() }}
        uses: actions/upload-artifact@v7
        with:
          name: cross-check-report
          path: |
            tests/outputs/report.txt
            tests/outputs/cross-check-details.txt
          if-no-files-found: warn
          overwrite: true
          retention-days: 7
```

## Rules

- **Triggers**: Pull requests, pushes to `master` and `upgrade/**`, and manual starts. A manual start is only offered once the workflow is on the default branch.
- **Access**: `contents: read` only. The pipeline uses no secrets, publishes no release, and reads or writes no references (FR-053).
- **Runner labels**: Pinned to `ubuntu-24.04` and `windows-2025-vs2026`, not `-latest`. The hosted image's software can still change over time.
- **Failure handling**: A failed `--test` still lets `Collect` run, because that step depends only on the build. Each collection uploads even after a failure, and `cross-check` runs unless the run was cancelled.
- **Missing collection**: If a build failed, its artifact is missing, and `--cross-check` exits 3 and names it.
- **Status**: This is not the parity gate, and it is not a required check unless a maintainer makes it one (FR-054). The flag check is not included, because it fails until playbook Phase 3 closes KD-2.
- **Run summary**: GitHub limits a summary to 1 MiB per step. The report stays well under that, and every differing statistic is in the `cross-check-report` artifact instead (research R16). Each line is indented four spaces, so it shows as code.
- **Artifacts**: `collection-qmake`, `collection-msvc`, and `cross-check-report`, kept for 7 days. Each collection is about the size of a reference set, about 77 MB. Each upload sets `overwrite: true`, so rerunning a failed job cannot clash with its earlier artifact.
