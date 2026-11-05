#!/usr/bin/env python3
"""Tests for tests/regression_runner.py, driven by the stand-in renderer in tests/stub_renderer.py.

Run it directly from any folder: python3 tests/test_regression_runner.py
Every fixture lives in a temporary folder, so no test touches tests/references/.
"""

import contextlib
import io
import json
import os
import shutil
import stat
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

TESTS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(TESTS_DIR))

import regression_runner  # noqa: E402

BUFFER_NAMES = ("raw_render", "post_effect", "final_image", "alpha", "depth", "normal", "normal_world")
FLOAT_BUFFERS = ("raw_render", "post_effect", "depth", "normal", "normal_world")
SCENE_TEXT = "# Mandelbulber settings file\n# version 2.35\n[main_parameters]\nN 50;\nmat1_is_defined true;\n"
FIXED_COMMIT = "0123456789abcdef0123456789abcdef01234567"
FIXED_COMMIT_TIME = 1000


class HarnessTestCase(unittest.TestCase):
    """Runs the harness in a temporary root with a stand-in binary."""

    def setUp(self):
        # A stand-in that timed out can still hold files open on Windows.
        self.directory = tempfile.TemporaryDirectory(ignore_cleanup_errors=True)
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name).resolve()
        self.tests_dir = self.root / "tests"
        self.tests_dir.mkdir()
        self.write_scenes("alpha", "beta scene")
        (self.tests_dir / "formula_sweep_base.fract").write_text(SCENE_TEXT, encoding="utf-8")
        (self.tests_dir / "formula_exclusions.txt").write_text(
            "# name reason\ncustom Custom OpenCL formula has an empty CPU implementation\n", encoding="utf-8"
        )
        self.binary = self.make_launcher("build")
        self.addCleanup(os.chdir, os.getcwd())
        os.chdir(self.root)

        environment = {
            "MANDELBULBER_BINARY": str(self.binary),
            "PARITY_TARGET": "test-target",
            "CI": "true",
        }
        patchers = (
            mock.patch.dict(os.environ, environment),
            mock.patch.object(regression_runner, "gate_valid", return_value=True),
            mock.patch.object(regression_runner, "SCENE_TIMEOUT", 20),
            mock.patch.object(regression_runner, "FORMULA_TIMEOUT", 20),
            mock.patch.object(regression_runner, "SCENE_RESOLUTION", "8x6"),
            mock.patch.object(regression_runner, "FORMULA_RESOLUTION", "4x3"),
            mock.patch.object(
                regression_runner,
                "source_state",
                return_value=(FIXED_COMMIT, FIXED_COMMIT_TIME, False),
                create=True,
            ),
        )
        for patcher in patchers:
            patcher.start()
            self.addCleanup(patcher.stop)

    def make_launcher(self, folder):
        """Creates a stand-in binary in its own folder, and returns its path."""
        directory = self.root / folder
        directory.mkdir()
        stub = shutil.copy(TESTS_DIR / "stub_renderer.py", directory / "stub_renderer.py")
        if os.name == "nt":
            launcher = directory / "mandelbulber2.cmd"
            launcher.write_text(f'@"{sys.executable}" "{stub}" %*\n', encoding="utf-8")
        else:
            launcher = directory / "mandelbulber2"
            launcher.write_text(f'#!/bin/sh\nexec "{sys.executable}" "{stub}" "$@"\n', encoding="utf-8")
            launcher.chmod(launcher.stat().st_mode | stat.S_IXUSR)
        return launcher

    def write_scenes(self, *names):
        """Creates scene files and a scene list that names them."""
        lines = ["# Standard Scene Suite"]
        scene_dir = self.tests_dir / "scenes"
        scene_dir.mkdir(exist_ok=True)
        for name in names:
            path = scene_dir / f"{name}.fract"
            path.write_text(SCENE_TEXT, encoding="utf-8")
            lines.append(f"tests/scenes/{name}.fract")
        (self.tests_dir / "scenes.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")

    def set_mode(self, mode, binary=None):
        """Sets the stand-in's mode for one binary."""
        folder = Path(binary or self.binary).parent
        (folder / "stub_mode.txt").write_text(mode, encoding="utf-8")

    def calls(self, binary=None):
        """Returns every call the stand-in recorded, oldest first."""
        path = Path(binary or self.binary).parent / "stub_calls.jsonl"
        if not path.exists():
            return []
        return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines()]

    def render_calls(self, binary=None):
        return [call for call in self.calls(binary) if "--nogui" in call["args"]]

    def run_main(self, *arguments):
        """Runs the harness in-process, and returns its exit code and console output."""
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            try:
                code = regression_runner.main(list(arguments))
            except SystemExit as error:
                code = error.code
        return code, output.getvalue()

    def report_text(self):
        return (self.root / "tests" / "outputs" / "report.txt").read_text(encoding="utf-8")

    def tree_bytes(self, path):
        """Returns every file below a path with its content, for byte-for-byte checks."""
        path = Path(path)
        return {str(file.relative_to(path)): file.read_bytes() for file in sorted(path.rglob("*")) if file.is_file()}


class ScaffoldingTests(HarnessTestCase):
    def test_stand_in_answers_the_version_query(self):
        with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as home:
            result = regression_runner.subprocess.run(
                [str(self.binary), "--version"],
                cwd=self.binary.parent,
                env=regression_runner.clean_environment(home),
                capture_output=True,
                text=True,
                timeout=20,
            )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("Mandelbulber 2.35-test", result.stdout)
        self.assertEqual(len(self.calls()), 1)


if __name__ == "__main__":
    unittest.main()
