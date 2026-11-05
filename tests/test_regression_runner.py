#!/usr/bin/env python3
"""Tests for tests/regression_runner.py, driven by the stand-in renderer in tests/stub_renderer.py.

Run it directly from any folder: python3 tests/test_regression_runner.py
Every fixture lives in a temporary folder, so no test touches tests/references/.
"""

import contextlib
import io
import json
import os
import re
import shutil
import stat
import sys
import tempfile
import unittest
import zipfile
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
        # A stand-in that timed out can still hold files open on Windows. TemporaryDirectory's
        # ignore_cleanup_errors recurses until it fails there on Python 3.10.
        self.root = Path(tempfile.mkdtemp(prefix="mandelbulber-test-")).resolve()
        self.addCleanup(shutil.rmtree, self.root, ignore_errors=True)
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

    def use_one_formula(self):
        """Excludes all but one formula, for tests that only need one of each kind of item."""
        (self.tests_dir / "formula_exclusions.txt").write_text(
            "custom reason\nmandelbulbPower2 reason\naexion reason\n", encoding="utf-8"
        )

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
        with regression_runner.temporary_folder() as home:
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


class ScenesListTests(HarnessTestCase):
    """T023: a bad scene list fails before any render runs."""

    def set_scene_list(self, text):
        (self.tests_dir / "scenes.txt").write_text(text, encoding="utf-8")

    def assert_scene_failure(self, pattern):
        with self.assertRaisesRegex(regression_runner.SetupError, pattern):
            regression_runner.build_work_items(self.binary)
        self.assertEqual(self.render_calls(), [])

    def test_empty_list_fails(self):
        self.set_scene_list("")
        self.assert_scene_failure("no scenes")

    def test_comment_only_list_fails(self):
        self.set_scene_list("# Standard Scene Suite\n\n   \n# nothing here\n")
        self.assert_scene_failure("no scenes")

    def test_missing_scene_fails_and_names_it(self):
        self.set_scene_list("tests/scenes/alpha.fract\ntests/scenes/gone.fract\n")
        self.assert_scene_failure("gone.fract")

    def test_two_scenes_with_one_file_name_fail(self):
        other = self.tests_dir / "other"
        other.mkdir()
        (other / "alpha.fract").write_text(SCENE_TEXT, encoding="utf-8")
        self.set_scene_list("tests/scenes/alpha.fract\ntests/other/alpha.fract\n")
        self.assert_scene_failure("same file name.*alpha")

    def write_scene_line(self, line):
        text = SCENE_TEXT + line + "\n"
        (self.tests_dir / "scenes" / "alpha.fract").write_text(text, encoding="utf-8")

    def test_url_asset_fails(self):
        self.write_scene_line("file_background https://example.com/sky.jpg;")
        self.assert_scene_failure("alpha.fract.*URL or absolute path")

    def test_quoted_url_asset_fails(self):
        self.write_scene_line('mat1_file_color_texture "http://example.com/a.png";')
        self.assert_scene_failure("alpha.fract.*URL or absolute path")

    def test_absolute_path_assets_fail(self):
        for value in ("/home/user/sky.jpg", "C:/textures/sky.jpg", "C:\\textures\\sky.jpg"):
            with self.subTest(value=value):
                self.write_scene_line(f"file_envmap {value};")
                self.assert_scene_failure("alpha.fract.*URL or absolute path")

    def test_relative_asset_is_allowed(self):
        self.write_scene_line("file_envmap textures/sky.jpg;")
        names = [item.name for item in regression_runner.build_work_items(self.binary)]
        self.assertIn("alpha", names)

    def test_nebula_mode_fails(self):
        self.write_scene_line("nebula_mode true;")
        self.assert_scene_failure("alpha.fract.*nebula")

    def test_nebula_mode_off_is_allowed(self):
        self.write_scene_line("nebula_mode false;")
        self.assertTrue(regression_runner.build_work_items(self.binary))

    def test_scene_list_order_and_comments(self):
        self.set_scene_list("\n# comment\ntests/scenes/beta scene.fract\n\ntests/scenes/alpha.fract\n")
        items = regression_runner.build_work_items(self.binary)
        scenes = [item.name for item in items if item.kind == "scene"]
        self.assertEqual(scenes, ["beta scene", "alpha"])


class FormulaListTests(HarnessTestCase):
    """T024: the formula list comes from the binary, and every exclusion is justified."""

    def set_exclusions(self, text):
        (self.tests_dir / "formula_exclusions.txt").write_text(text, encoding="utf-8")

    def test_missing_binary_names_the_path(self):
        missing = self.root / "nowhere" / "mandelbulber2"
        with self.assertRaisesRegex(regression_runner.SetupError, "nowhere"):
            regression_runner.load_formulas(missing)

    def test_list_problems_name_the_binary(self):
        for mode in ("no-list-end", "no-list-start", "empty-list", "exit-3"):
            with self.subTest(mode=mode):
                self.set_mode(mode)
                with self.assertRaisesRegex(regression_runner.SetupError, self.binary.name):
                    regression_runner.load_formulas(self.binary)

    def test_timeout_names_the_binary(self):
        self.set_mode("hang")
        with mock.patch.object(regression_runner, "FORMULA_TIMEOUT", 1):
            with self.assertRaisesRegex(regression_runner.SetupError, self.binary.name):
                regression_runner.load_formulas(self.binary)

    def test_list_reads_names_and_ids(self):
        formulas = regression_runner.load_formulas(self.binary)
        self.assertEqual(formulas, {"custom": 10000, "mandelbulb": 2, "mandelbulbPower2": 3, "aexion": 19})

    def test_exclusion_without_a_reason_fails(self):
        self.set_exclusions("custom\n")
        with self.assertRaisesRegex(regression_runner.SetupError, "reason.*custom|custom.*reason"):
            regression_runner.build_work_items(self.binary)

    def test_exclusion_of_an_unknown_formula_fails(self):
        self.set_exclusions("custom reason\nretired no longer offered\n")
        with self.assertRaisesRegex(regression_runner.SetupError, "retired"):
            regression_runner.build_work_items(self.binary)

    def test_excluded_formulas_are_not_work_items(self):
        self.set_exclusions("# comment\n\ncustom reason\naexion another reason\n")
        items = regression_runner.build_work_items(self.binary)
        formulas = [item.name for item in items if item.kind == "formula"]
        self.assertEqual(formulas, ["mandelbulb", "mandelbulbPower2"])

    def test_work_item_fields(self):
        items = regression_runner.build_work_items(self.binary)
        scene = items[0]
        self.assertEqual((scene.kind, scene.name), ("scene", "alpha"))
        self.assertEqual(scene.source, Path("tests/scenes/alpha.fract"))
        self.assertIsNone(scene.formula_id)
        self.assertEqual(scene.resolution, regression_runner.SCENE_RESOLUTION)
        self.assertEqual(scene.time_limit, regression_runner.SCENE_TIMEOUT)
        formula = next(item for item in items if item.kind == "formula")
        self.assertEqual((formula.name, formula.formula_id), ("mandelbulb", 2))
        self.assertEqual(formula.source, regression_runner.FORMULA_BASE)
        self.assertEqual(formula.resolution, regression_runner.FORMULA_RESOLUTION)
        self.assertEqual(formula.time_limit, regression_runner.FORMULA_TIMEOUT)

    def test_missing_base_scene_fails(self):
        (self.tests_dir / "formula_sweep_base.fract").unlink()
        with self.assertRaisesRegex(regression_runner.SetupError, "formula_sweep_base"):
            regression_runner.build_work_items(self.binary)


class RenderTestCase(HarnessTestCase):
    """Helpers for tests that render one item at a time."""

    def scene_item(self, name="alpha", time_limit=20):
        return regression_runner.WorkItem(
            "scene", name, Path(f"tests/scenes/{name}.fract"), None, regression_runner.SCENE_RESOLUTION, time_limit
        )

    def output_dir(self, name="alpha"):
        return regression_runner.OUT_DIR / "renders" / name

    def new_report(self):
        with contextlib.redirect_stdout(io.StringIO()):
            report = regression_runner.Report("test", "linux")
        self.addCleanup(report.close)
        return report

    def render(self, item=None, neutral=False):
        """Renders one item, and returns its result, the report, and the console output."""
        report = self.new_report()
        output = io.StringIO()
        item = item or self.scene_item()
        with contextlib.redirect_stdout(output):
            result = regression_runner.render_and_validate(
                item, self.binary, self.output_dir(item.name), report, neutral=neutral
            )
        return result, report, output.getvalue()


class RenderFailureTests(RenderTestCase):
    """T025: a zero exit status is never enough."""

    def assert_item_fails(self, mode, cause):
        self.render()  # Leaves a valid export behind, which must not satisfy the check.
        self.set_mode(mode)
        result, report, console = self.render()
        self.assertIsNone(result, mode)
        self.assertEqual(report.item_results, {"scene alpha": "failed"})
        self.assertEqual(report.exit_code(), regression_runner.EXIT_SETUP)
        self.assertIn("scene alpha", console)
        self.assertIn(cause, console)
        self.assertIn("render.log", console)

    def test_failed_load(self):
        self.assert_item_fails("bad-settings-text", "not valid Mandelbulber settings file")

    def test_settings_line_error_with_a_valid_export(self):
        self.assert_item_fails("settings-line-error", "Error in settings file")

    def test_texture_error_with_a_valid_export(self):
        self.assert_item_fails("texture-error", "Can't load texture")

    def test_alternate_lookup_with_a_valid_export(self):
        self.assert_item_fails("alternate-lookup", "Looking for the file at")

    def test_system_data_folder_with_a_valid_export(self):
        self.assert_item_fails("system-data-dir", "Trying to use /usr/share/mandelbulber2")

    def test_non_zero_exit(self):
        self.assert_item_fails("exit-3", "exit status 3")

    def test_timeout(self):
        self.render()
        self.set_mode("hang")
        result, report, console = self.render(self.scene_item(time_limit=1))
        self.assertIsNone(result)
        self.assertEqual(report.item_results, {"scene alpha": "failed"})
        self.assertIn("timed out after 1 seconds", console)

    def test_every_failure_message_is_detected(self):
        messages = (
            "not valid Mandelbulber settings file",
            "No materials defined",
            "Error in settings file",
            "Too many errors in settings file",
            "Can't load texture",
            "Looking for the file at",
            "Trying to use /usr/share/mandelbulber2",
        )
        for message in messages:
            with self.subTest(message=message):
                self.set_mode(f"print:{message}")
                result, report, console = self.render()
                self.assertIsNone(result)
                self.assertIn(message, console)

    def test_other_output_does_not_fail(self):
        self.set_mode("print:cannot move folder a to b!")
        result, report, _ = self.render()
        self.assertIsNotNone(result)
        self.assertEqual(report.item_results, {})


class ExportValidationTests(RenderTestCase):
    """T026: only a complete, valid export counts."""

    def assert_invalid(self, mode, cause):
        self.set_mode(mode)
        result, report, console = self.render()
        self.assertIsNone(result, mode)
        self.assertEqual(report.item_results, {"scene alpha": "failed"})
        self.assertEqual(report.exit_code(), regression_runner.EXIT_SETUP)
        self.assertIn("scene alpha", console)
        self.assertIn(cause, console)

    def test_valid_export_is_returned(self):
        result, report, _ = self.render()
        self.assertEqual((result.width, result.height), (8, 6))
        self.assertEqual(result.compiler, "gcc 13.3.0")
        self.assertEqual(result.math_library, "glibc 2.39")
        self.assertEqual(tuple(result.buffers), BUFFER_NAMES)
        self.assertEqual(len(result.buffers["raw_render"]), 8 * 6 * 12)
        self.assertEqual(len(result.buffers["alpha"]), 8 * 6 * 2)
        self.assertEqual(result.statistics["iteration_histogram.size"], 50)

    def test_no_export(self):
        self.assert_invalid("no-export", "manifest")

    def test_no_end_line(self):
        self.assert_invalid("no-end", "end line")

    def test_unknown_version(self):
        self.assert_invalid("bad-version", "format version 99")

    def test_wrong_size(self):
        for buffer in BUFFER_NAMES:
            with self.subTest(buffer=buffer):
                self.assert_invalid(f"wrong-size:{buffer}", buffer)

    def test_wrong_type(self):
        for buffer in BUFFER_NAMES:
            with self.subTest(buffer=buffer):
                self.assert_invalid(f"wrong-type:{buffer}", buffer)

    def test_missing_buffer(self):
        for buffer in BUFFER_NAMES:
            with self.subTest(buffer=buffer):
                self.assert_invalid(f"missing-buffer:{buffer}", f"missing buffer {buffer}")

    def test_extra_buffer(self):
        self.assert_invalid("extra-buffer", "unexpected buffer specular")

    def test_bad_statistics(self):
        self.assert_invalid("bad-statistics", "whole number")

    def test_no_statistics(self):
        self.assert_invalid("no-statistics", "statistics")

    def test_buffer_file_missing_from_the_folder(self):
        result, _, _ = self.render()
        self.assertIsNotNone(result)
        export = self.output_dir() / "export"
        (export / "depth.bin").unlink()
        with self.assertRaisesRegex(regression_runner.RenderError, "depth"):
            regression_runner.read_export(export)

    def test_one_failed_item_leaves_the_rest_running(self):
        items = regression_runner.build_work_items(self.binary)
        for mode in ("no-end", "bad-version", "wrong-size:depth", "extra-buffer", "bad-statistics"):
            with self.subTest(mode=mode):
                self.set_mode(f"only:alpha:{mode}")
                report = self.new_report()
                results = {}
                with contextlib.redirect_stdout(io.StringIO()):
                    for item in items:
                        results[item.name] = regression_runner.render_and_validate(
                            item, self.binary, self.output_dir(item.name), report
                        )
                self.assertIsNone(results.pop("alpha"))
                self.assertTrue(all(result is not None for result in results.values()), results)
                self.assertEqual(report.item_results, {"scene alpha": "failed"})

    def test_formula_identity_for_only_mode(self):
        items = regression_runner.build_work_items(self.binary)
        formula = next(item for item in items if item.name == "mandelbulbPower2")
        self.set_mode(f"only:{formula.formula_id}:no-end")
        result, _, console = self.render(formula)
        self.assertIsNone(result)
        self.assertIn("formula mandelbulbPower2", console)


class CommandTests(RenderTestCase):
    """T027: the render command and its isolation."""

    def option(self, call, name):
        return call["args"][call["args"].index(name) + 1]

    def test_scene_command(self):
        self.render()
        call = self.render_calls()[-1]
        self.assertIn("--nogui", call["args"])
        self.assertEqual(self.option(call, "--res"), regression_runner.SCENE_RESOLUTION)
        self.assertEqual(self.option(call, "--format"), "tiff")
        self.assertEqual(self.option(call, "--override"), regression_runner.RENDER_OVERRIDES)
        self.assertEqual(call["args"].count("--override"), 1)

    def test_fixed_settings(self):
        self.assertEqual(
            regression_runner.RENDER_OVERRIDES,
            "opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4",
        )

    def test_formula_command(self):
        items = regression_runner.build_work_items(self.binary)
        formula = next(item for item in items if item.name == "mandelbulb")
        self.render(formula)
        call = self.render_calls()[-1]
        self.assertEqual(self.option(call, "--res"), regression_runner.FORMULA_RESOLUTION)
        self.assertEqual(self.option(call, "--override"), regression_runner.RENDER_OVERRIDES + "#fractal1_formula=2")
        self.assertEqual(call["args"].count("--override"), 1)

    def test_every_path_is_absolute(self):
        self.render()
        call = self.render_calls()[-1]
        self.assertTrue(Path(self.option(call, "--output")).is_absolute())
        self.assertTrue(Path(self.option(call, "--verification-export")).is_absolute())
        self.assertTrue(Path(call["args"][-1]).is_absolute())
        self.assertTrue(Path(call["args"][-1]).exists())

    def test_runs_in_the_binary_folder(self):
        self.render()
        self.assertEqual(Path(self.render_calls()[-1]["cwd"]).resolve(), self.binary.parent.resolve())

    def test_output_folder_exists_before_the_render(self):
        self.render()
        self.assertTrue(self.render_calls()[-1]["output_folder_exists"])

    def test_each_render_gets_its_own_home(self):
        self.render()
        self.render()
        homes = [call["env"]["HOME"] for call in self.render_calls()]
        self.assertEqual(len(set(homes)), 2)

    def assert_clean(self, call):
        self.assertEqual(call["env"]["LC_ALL"], "C.UTF-8")
        self.assertEqual(call["env"]["LANG"], "C.UTF-8")
        self.assertIn("HOME", call["env"])
        for name in ("LD_LIBRARY_PATH", "OMP_NUM_THREADS", "MANDELBULBER_BINARY", "HARNESS_TEST_VARIABLE"):
            self.assertNotIn(name, call["env"])

    def test_host_variables_do_not_reach_the_render(self):
        polluted = {"LD_LIBRARY_PATH": "/tmp", "OMP_NUM_THREADS": "1", "HARNESS_TEST_VARIABLE": "x", "LC_ALL": "de_DE"}
        with mock.patch.dict(os.environ, polluted):
            self.render()
        self.assert_clean(self.render_calls()[-1])

    def test_host_variables_do_not_reach_the_formula_list(self):
        polluted = {"LD_LIBRARY_PATH": "/tmp", "OMP_NUM_THREADS": "1", "HARNESS_TEST_VARIABLE": "x", "LC_ALL": "de_DE"}
        with mock.patch.dict(os.environ, polluted):
            regression_runner.load_formulas(self.binary)
        call = self.calls()[-1]
        self.assertEqual(call["args"], ["--list-formulas"])
        self.assert_clean(call)
        self.assertEqual(Path(call["cwd"]).resolve(), self.binary.parent.resolve())

    def test_earlier_output_is_removed(self):
        self.render()
        leftover = self.output_dir() / "leftover.txt"
        leftover.write_text("old", encoding="utf-8")
        self.render()
        self.assertFalse(leftover.exists())


class ModeTestCase(HarnessTestCase):
    """Helpers for tests that run a whole mode."""

    def enable_threads(self):
        patcher = mock.patch.object(regression_runner, "THREAD_OPTION", "--threads")
        patcher.start()
        self.addCleanup(patcher.stop)

    def thread_counts(self):
        count = os.cpu_count() or 1
        return ("1", str(count if count > 1 else 2))

    def calls_by_item(self, binary=None):
        """Groups render calls by scene name or formula ID."""
        groups = {}
        for call in self.render_calls(binary):
            arguments = call["args"]
            overrides = arguments[arguments.index("--override") + 1].split("#")
            formula = [part.partition("=")[2] for part in overrides if part.startswith("fractal1_formula=")]
            key = formula[0] if formula else Path(arguments[-1]).stem
            groups.setdefault(key, []).append(call)
        return groups


class SelfParityTests(ModeTestCase):
    """T035: repeated renders must match each other."""

    def test_identical_output_passes(self):
        self.enable_threads()
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PASSED, output)
        self.assertIn("[+] scene alpha: exact match", output)
        self.assertIn("[+] formula mandelbulb: exact match", output)
        self.assertIn("result: passed", output)
        groups = self.calls_by_item()
        self.assertEqual(sorted(groups), sorted(["alpha", "beta scene", "2", "3", "19"]))
        one, many = self.thread_counts()
        for key, calls in groups.items():
            exported = [call["args"] for call in calls if "--verification-export" in call["args"]]
            threads = [arguments[arguments.index("--threads") + 1] for arguments in exported]
            self.assertEqual(threads.count(one), 3, key)
            self.assertEqual(threads.count(many), 3, key)
            self.assertEqual(len(calls), 7, key)

    def test_each_items_runs_stay_consecutive(self):
        self.enable_threads()
        self.write_scenes("alpha")
        self.run_main("--self-parity")
        one, _ = self.thread_counts()
        order = [
            call["args"][-1] + call["args"][call["args"].index("--override") + 1]
            for call in self.render_calls()
            if call["args"][call["args"].index("--threads") + 1] == one and "--verification-export" in call["args"]
        ]
        self.assertEqual(len(order), 12)
        with mock.patch.object(regression_runner.os, "cpu_count", return_value=1):
            self.calls_path = self.binary.parent / "stub_calls.jsonl"
            self.calls_path.unlink()
            self.run_main("--self-parity")
        order = [
            call["args"][-1] + call["args"][call["args"].index("--override") + 1]
            for call in self.render_calls()
            if call["args"][call["args"].index("--threads") + 1] == "1" and "--verification-export" in call["args"]
        ]
        self.assertEqual(order[:3], [order[0]] * 3)
        self.assertEqual(order[3:6], [order[3]] * 3)
        self.assertNotEqual(order[0], order[3])

    def test_unstable_output_fails_per_item_and_buffer(self):
        self.enable_threads()
        self.set_mode("unstable")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        for buffer in BUFFER_NAMES:
            self.assertRegex(output, rf"\[-\] scene alpha {buffer}: [12] of \d+ samples differ")
            self.assertRegex(output, rf"\[-\] formula mandelbulb {buffer}: [12] of \d+ samples differ")
        self.assertIn("result: parity failure (5 of 5 items failed)", output)

    def test_every_buffer_is_compared(self):
        self.enable_threads()
        for buffer in BUFFER_NAMES:
            with self.subTest(buffer=buffer):
                self.set_mode(f"unstable:{buffer}")
                code, output = self.run_main("--self-parity")
                self.assertEqual(code, regression_runner.EXIT_PARITY, output)
                self.assertRegex(output, rf"\[-\] scene alpha {buffer}: [12] of \d+ samples differ")

    def test_references_stay_unchanged(self):
        self.enable_threads()
        reference = self.tests_dir / "references" / "test-target"
        reference.mkdir(parents=True)
        (reference / "toolchain.txt").write_text("compiler gcc\n", encoding="utf-8")
        before = self.tree_bytes(self.tests_dir / "references")
        self.run_main("--self-parity")
        self.assertEqual(self.tree_bytes(self.tests_dir / "references"), before)

    def test_failing_outputs_stay_and_passing_outputs_go(self):
        self.enable_threads()
        self.set_mode("only:alpha:unstable")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        renders = self.tests_dir / "outputs" / "renders"
        self.assertTrue((renders / "scenes" / "alpha").exists())
        self.assertFalse((renders / "scenes" / "beta scene").exists())
        self.assertFalse((renders / "formulas" / "mandelbulb").exists())

    def test_report_names_the_target_and_reaches_the_file(self):
        self.enable_threads()
        code, output = self.run_main("--self-parity")
        self.assertIn("target: test-target", output)
        report = self.report_text()
        self.assertIn("mode: self-parity", report)
        self.assertIn("target: test-target", report)
        self.assertIn("[+] scene alpha: exact match", report)
        self.assertIn("result: passed", report)

    def test_render_failure_exits_three_and_the_rest_still_run(self):
        self.enable_threads()
        self.set_mode("only:alpha:exit-3")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("scene alpha", output)
        self.assertIn("[+] scene beta scene: exact match", output)
        self.assertIn("result: setup error (1 of 5 items failed)", output)

    def test_parity_failure_wins_over_a_render_failure(self):
        self.enable_threads()
        self.set_mode("only:alpha:unstable")
        real = regression_runner.render_item

        def fail_one(item, *arguments, **options):
            if item.name == "mandelbulb":
                raise regression_runner.RenderError("seeded failure")
            return real(item, *arguments, **options)

        with mock.patch.object(regression_runner, "render_item", fail_one):
            code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("seeded failure", output)


class ThreadCoverageTests(ModeTestCase):
    """T036: a run without a thread control cannot pass."""

    def test_identical_output_exits_three(self):
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("thread coverage is incomplete", output)
        self.assertIn("[+] scene alpha: exact match", output)
        self.assertEqual(len(self.calls_by_item()["alpha"]), 4)

    def test_differing_output_exits_one_and_reports_both(self):
        self.set_mode("unstable")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("thread coverage is incomplete", output)
        self.assertIn("samples differ", output)

    def test_missing_binary_exits_three(self):
        missing = str(self.root / "nowhere" / "mandelbulber2")
        with mock.patch.dict(os.environ, {"MANDELBULBER_BINARY": missing}):
            code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("nowhere", output)
        self.assertIn("result: setup error", output)

    def test_two_modes_exit_two(self):
        code, output = self.run_main("--self-parity", "--capture")
        self.assertEqual(code, 2, output)

    def test_unexpected_error_exits_three(self):
        with mock.patch.object(regression_runner, "build_work_items", side_effect=ValueError("boom")):
            code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("unexpected harness error", output)
        self.assertIn("boom", output)

    def test_thread_settings_on_one_cpu(self):
        with mock.patch.object(regression_runner, "THREAD_OPTION", "--threads"):
            with mock.patch.object(regression_runner.os, "cpu_count", return_value=1):
                settings, complete = regression_runner.thread_settings()
        self.assertEqual(settings, [("--threads", "1"), ("--threads", "2")])
        self.assertTrue(complete)

    def test_thread_settings_with_many_cpus(self):
        with mock.patch.object(regression_runner, "THREAD_OPTION", "--threads"):
            with mock.patch.object(regression_runner.os, "cpu_count", return_value=8):
                settings, complete = regression_runner.thread_settings()
        self.assertEqual(settings, [("--threads", "1"), ("--threads", "8")])
        self.assertTrue(complete)

    def test_thread_settings_without_a_control(self):
        settings, complete = regression_runner.thread_settings()
        self.assertEqual(settings, [()])
        self.assertFalse(complete)


class NeutralRenderTests(ModeTestCase):
    """T037: the options must not change the final image (FR-012)."""

    def neutral_calls(self):
        return [call for call in self.render_calls() if "--verification-export" not in call["args"]]

    def test_matching_neutral_image_passes(self):
        self.enable_threads()
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PASSED, output)
        self.assertEqual(len(self.neutral_calls()), 5)

    def test_neutral_settings(self):
        self.enable_threads()
        self.run_main("--self-parity")
        for call in self.neutral_calls():
            arguments = call["args"]
            overrides = arguments[arguments.index("--override") + 1].split("#")
            self.assertNotIn("normal_enabled=1", overrides)
            self.assertIn("color_quality=1", overrides)
            self.assertIn("opencl_enabled=0", overrides)
            self.assertIn("DOF_samples=8", overrides)
            self.assertNotIn("--verification-export", arguments)

    def test_differing_neutral_image_fails(self):
        self.enable_threads()
        self.set_mode("only:alpha:neutral-differs")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertRegex(output, r"\[-\] scene alpha final_image: 1 of \d+ samples differ.*neutral")
        self.assertIn("[+] scene beta scene: exact match", output)

    def test_missing_neutral_image_fails(self):
        self.enable_threads()
        self.set_mode("only:alpha:no-neutral-image")
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("scene alpha", output)
        self.assertIn("no image", output)

    def test_undecodable_neutral_image_fails(self):
        item = regression_runner.WorkItem("scene", "alpha", Path("tests/scenes/alpha.fract"), None, "8x6", 20)
        with contextlib.redirect_stdout(io.StringIO()):
            report = regression_runner.Report("test", "linux")
        self.addCleanup(report.close)
        real = regression_runner.render_item

        def corrupt(item, binary, output_dir, thread_args=(), neutral=False):
            outcome = real(item, binary, output_dir, thread_args, neutral)
            (output_dir / "image.tiff").write_bytes(b"II*\0broken")
            return outcome

        with mock.patch.object(regression_runner, "render_item", corrupt):
            with contextlib.redirect_stdout(io.StringIO()):
                image = regression_runner.render_and_validate(
                    item, self.binary, regression_runner.OUT_DIR / "x", report, neutral=True
                )
        self.assertIsNone(image)
        self.assertEqual(report.item_results, {"scene alpha": "failed"})


class PlatformTests(ModeTestCase):
    """T038: without isolation, a self-parity run cannot pass."""

    def test_not_gate_valid(self):
        self.enable_threads()
        with mock.patch.object(regression_runner, "gate_valid", return_value=False):
            code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("not gate-valid", output)
        self.assertIn("[+] scene alpha: exact match", output)


class StatisticsComparisonTests(ModeTestCase):
    """T045: a change of 1 in any statistic fails, and names the statistic and both values."""

    NAMES = (
        "total_iterations",
        "raymarch_count",
        "missed_de",
        "iteration_histogram.7",
        "iteration_histogram.50",
        "step_histogram.7",
        "step_histogram.1000",
    )

    def setUp(self):
        super().setUp()
        self.write_scenes("alpha")
        self.use_one_formula()

    def test_every_statistic_is_compared(self):
        for name in self.NAMES:
            with self.subTest(name=name):
                self.set_mode(f"unstable-stat:{name}")
                code, output = self.run_main("--self-parity")
                self.assertEqual(code, regression_runner.EXIT_PARITY, output)
                self.assertRegex(output, rf"\[-\] scene alpha statistics: {re.escape(name)} \d+ vs \d+")

    def test_console_shows_five_and_counts_the_rest(self):
        names = [f"step_histogram.{bucket}" for bucket in range(8)]
        self.set_mode("only:alpha:unstable-stat:" + ",".join(names))
        code, output = self.run_main("--self-parity")
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        lines = [line for line in output.splitlines() if "alpha statistics" in line and "run 2 " in line]
        self.assertEqual(len(lines), 6, output)
        self.assertIn("3 more differ", lines[-1])
        kept = [line for line in self.report_text().splitlines() if "alpha statistics" in line and "run 2 " in line]
        self.assertEqual(len(kept), 8)
        self.assertNotIn("more differ", self.report_text())


class StatisticsFormatTests(unittest.TestCase):
    """T046: the record has one fixed order, and only whole numbers."""

    def record(self, iteration_size=1, step_size=1):
        lines = ["total_iterations 1", "raymarch_count 2", "missed_de 3"]
        for histogram, size in (("iteration_histogram", iteration_size), ("step_histogram", step_size)):
            lines.append(f"{histogram}.size {size}")
            lines += [f"{histogram}.{bucket} {bucket}" for bucket in range(size + 1)]
        return "\n".join(lines) + "\n"

    def test_record_parses_in_a_fixed_order(self):
        statistics = regression_runner.parse_statistics(self.record())
        self.assertEqual(
            list(statistics),
            [
                "total_iterations",
                "raymarch_count",
                "missed_de",
                "iteration_histogram.size",
                "iteration_histogram.0",
                "iteration_histogram.1",
                "step_histogram.size",
                "step_histogram.0",
                "step_histogram.1",
            ],
        )
        self.assertEqual(statistics["step_histogram.1"], 1)

    def assert_rejected(self, text, pattern):
        with self.assertRaisesRegex(regression_runner.RenderError, pattern):
            regression_runner.parse_statistics(text)

    def test_values_must_be_whole_numbers(self):
        for value in ("1.5", "3.16706e+08", "1,000", "-1", "", "0x10", "nan"):
            with self.subTest(value=value):
                self.assert_rejected(self.record().replace("missed_de 3", f"missed_de {value}"), "whole number")

    def test_time_based_names_are_rejected(self):
        self.assert_rejected(self.record() + "time 12\n", "unknown statistic time")
        self.assert_rejected(self.record().replace("missed_de 3", "seconds 3"), "unknown statistic seconds")

    def test_reordered_record_is_rejected(self):
        text = self.record().replace("total_iterations 1\nraymarch_count 2", "raymarch_count 2\ntotal_iterations 1")
        self.assert_rejected(text, "where total_iterations belongs")

    def test_incomplete_record_is_rejected(self):
        self.assert_rejected(self.record().replace("step_histogram.1 1\n", ""), "step_histogram.1 is missing")

    def test_record_must_end_with_a_line_break(self):
        self.assert_rejected(self.record()[:-1], "line break")

    def test_empty_histogram_buckets_follow_the_size(self):
        statistics = regression_runner.parse_statistics(self.record(iteration_size=3))
        self.assertEqual(statistics["iteration_histogram.3"], 3)


class GateTestCase(ModeTestCase):
    """Helpers for tests that need references. Everything stays in the temporary root."""

    def make_references(self, binary=None):
        """Writes a reference set from stand-in exports, and returns its folder."""
        binary = binary or self.binary
        folder = regression_runner.REF_ROOT / "test-target"
        with contextlib.redirect_stdout(io.StringIO()):
            report = regression_runner.Report("test", "linux")
            self.addCleanup(report.close)
            for item in regression_runner.build_work_items(binary):
                output_dir = regression_runner.OUT_DIR / "fixture" / item.name
                export = regression_runner.render_and_validate(item, binary, output_dir, report)
                regression_runner.write_reference(folder / f"{item.kind}s" / f"{item.name}.zip", export)
        toolchain = regression_runner.toolchain_identity(export)
        (folder / "toolchain.txt").write_text("\n".join(toolchain) + "\n", encoding="utf-8", newline="\n")
        settings = regression_runner.settings_lines()
        (folder / "settings.txt").write_text("\n".join(settings) + "\n", encoding="utf-8", newline="\n")
        shutil.rmtree(regression_runner.OUT_DIR / "fixture", ignore_errors=True)
        return folder


class ReferenceFormatTests(GateTestCase):
    """T049: the reference format round-trips exactly, and it is reproducible."""

    def setUp(self):
        super().setUp()
        self.write_scenes("alpha")
        self.use_one_formula()
        self.path = self.root / "reference.zip"
        with contextlib.redirect_stdout(io.StringIO()):
            report = regression_runner.Report("test", "linux")
            self.addCleanup(report.close)
            item = regression_runner.build_work_items(self.binary)[0]
            self.export = regression_runner.render_and_validate(
                item, self.binary, regression_runner.OUT_DIR / "fixture", report
            )
        regression_runner.write_reference(self.path, self.export)

    def test_planes_split_and_join(self):
        data = bytes(range(24))
        for size in (2, 4):
            planes = regression_runner.split_planes(data, size)
            self.assertEqual(planes[:2], bytes([0, size]))
            self.assertEqual(regression_runner.join_planes(planes, size), data)

    def test_every_buffer_round_trips(self):
        loaded = regression_runner.read_reference(self.path)
        self.assertEqual(loaded.buffers, self.export.buffers)
        self.assertEqual(loaded.statistics, self.export.statistics)
        self.assertEqual((loaded.width, loaded.height), (self.export.width, self.export.height))
        self.assertEqual(loaded.compiler, self.export.compiler)
        self.assertEqual(loaded.math_library, self.export.math_library)
        self.assertEqual(loaded.manifest_bytes, self.export.manifest_bytes)
        self.assertEqual(loaded.statistics_bytes, self.export.statistics_bytes)

    def test_entries_follow_the_contract(self):
        expected = ["manifest.txt", "statistics.txt"] + [f"{name}.planes" for name in BUFFER_NAMES]
        with zipfile.ZipFile(self.path) as archive:
            self.assertEqual(archive.namelist(), expected)
            for info in archive.infolist():
                self.assertEqual(info.date_time, (1980, 1, 1, 0, 0, 0))
                self.assertEqual(info.compress_type, zipfile.ZIP_DEFLATED)

    def test_corrupted_entry_is_rejected(self):
        data = bytearray(self.path.read_bytes())
        with zipfile.ZipFile(self.path) as archive:
            info = archive.getinfo("raw_render.planes")
        start = info.header_offset + 30 + len(info.filename) + len(info.extra)
        data[start + info.compress_size // 2] ^= 0xFF
        self.path.write_bytes(data)
        with self.assertRaisesRegex(regression_runner.SetupError, "reference.zip"):
            regression_runner.read_reference(self.path)

    def test_writing_twice_is_byte_identical(self):
        again = self.root / "again.zip"
        regression_runner.write_reference(again, self.export)
        self.assertEqual(again.read_bytes(), self.path.read_bytes())

    def test_missing_entry_is_rejected(self):
        with zipfile.ZipFile(self.path, "r") as archive:
            kept = [(info, archive.read(info.filename)) for info in archive.infolist()[:-1]]
        with zipfile.ZipFile(self.path, "w") as archive:
            for info, data in kept:
                archive.writestr(info, data)
        with self.assertRaisesRegex(regression_runner.SetupError, "reference.zip"):
            regression_runner.read_reference(self.path)


class GateResultTests(GateTestCase):
    """T050: the gate passes on identical output and names every difference."""

    def setUp(self):
        super().setUp()
        self.use_one_formula()
        self.make_references()

    def test_identical_output_passes(self):
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PASSED, output)
        self.assertIn("mode: gate", output)
        self.assertIn("target: test-target", output)
        self.assertIn("toolchain: gcc 13.3.0, glibc 2.39", output)
        self.assertIn("[+] scene alpha: exact match", output)
        self.assertIn("[+] formula mandelbulb: exact match", output)
        self.assertIn("result: passed", output)

    def test_one_render_per_item(self):
        before = len(self.render_calls())
        self.run_main()
        self.assertEqual(len(self.render_calls()) - before, 3)

    def test_flipped_bit_names_the_item_buffer_and_count(self):
        for buffer in BUFFER_NAMES:
            with self.subTest(buffer=buffer):
                self.set_mode(f"flip-bit:{buffer}")
                code, output = self.run_main()
                self.assertEqual(code, regression_runner.EXIT_PARITY, output)
                self.assertRegex(output, rf"\[-\] scene alpha {buffer}: 1 of \d+ samples differ")
                self.assertIn("result: parity failure", output)

    def test_mantissa_change_fails_on_float_buffers(self):
        for buffer in FLOAT_BUFFERS:
            with self.subTest(buffer=buffer):
                self.set_mode(f"flip-mantissa:{buffer}")
                code, output = self.run_main()
                self.assertEqual(code, regression_runner.EXIT_PARITY, output)
                self.assertRegex(output, rf"\[-\] scene alpha {buffer}: 1 of \d+ samples differ")
                self.assertNotIn("final_image:", output)

    def test_changed_statistic_names_both_values(self):
        self.set_mode("stat-plus-one:raymarch_count")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        match = re.search(r"\[-\] scene alpha statistics: raymarch_count (\d+) vs (\d+)", output)
        self.assertIsNotNone(match, output)
        self.assertEqual(int(match.group(1)) + 1, int(match.group(2)))

    def test_resize_reports_a_layout_difference(self):
        self.set_mode("resize")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertRegex(output, r"\[-\] scene alpha layout: 8x6 vs 9x6")

    def test_failing_outputs_stay_and_passing_outputs_go(self):
        self.set_mode("only:alpha:flip-bit:depth")
        self.run_main()
        renders = self.tests_dir / "outputs" / "renders"
        self.assertTrue((renders / "scenes" / "alpha").exists())
        self.assertFalse((renders / "scenes" / "beta scene").exists())


class GateCoverageTests(GateTestCase):
    """T051: every item needs a reference, and every reference needs an item."""

    def setUp(self):
        super().setUp()
        self.use_one_formula()
        self.folder = self.make_references()

    def test_scene_with_no_reference_fails(self):
        (self.folder / "scenes" / "alpha.zip").unlink()
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("[-] scene alpha: no reference", output)

    def test_formula_with_no_reference_fails(self):
        (self.folder / "formulas" / "mandelbulb.zip").unlink()
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("[-] formula mandelbulb: no reference", output)

    def test_leftover_reference_fails(self):
        shutil.copy(self.folder / "scenes" / "alpha.zip", self.folder / "scenes" / "retired.zip")
        shutil.copy(self.folder / "formulas" / "mandelbulb.zip", self.folder / "formulas" / "gone.zip")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("[-] scene retired: reference has no work item", output)
        self.assertIn("[-] formula gone: reference has no work item", output)
        self.assertIn("2 of 5 items failed", output)

    def test_extra_buffer_fails(self):
        self.set_mode("extra-buffer")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("unexpected buffer specular", output)

    def test_drift_in_one_formula_fails_on_that_formula_alone(self):
        self.set_mode("only:2:flip-bit:normal")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertRegex(output, r"\[-\] formula mandelbulb normal: 1 of \d+ samples differ")
        self.assertIn("[+] scene alpha: exact match", output)
        self.assertIn("result: parity failure (1 of 3 items failed)", output)

    def test_missing_reference_set_exits_three(self):
        shutil.rmtree(self.folder)
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("no reference set", output)
        self.assertNotIn("result: passed", output)
        self.assertNotIn("[+]", output)

    def test_changed_settings_exit_three(self):
        changes = (
            ("SCENE_RESOLUTION", "16x12"),
            ("FORMULA_RESOLUTION", "8x6"),
            ("RENDER_OVERRIDES", "opencl_enabled=0"),
            ("EXPORT_VERSION", 2),
        )
        for name, value in changes:
            with self.subTest(name=name):
                with mock.patch.object(regression_runner, name, value):
                    code, output = self.run_main()
                self.assertEqual(code, regression_runner.EXIT_SETUP, output)
                self.assertIn("render settings differ", output)

    def test_corrupted_reference_exits_three(self):
        (self.folder / "scenes" / "alpha.zip").write_bytes(b"not a zip")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("alpha.zip", output)


class GateSafetyTests(GateTestCase):
    """T052: a toolchain difference is not drift, and the gate never writes references."""

    def setUp(self):
        super().setUp()
        self.use_one_formula()
        self.folder = self.make_references()

    def test_other_toolchain_with_matching_output_passes_with_a_note(self):
        self.set_mode("other-toolchain")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PASSED, output)
        self.assertIn("note: the toolchain differs from the reference set", output)
        self.assertIn("gcc 99.1.0", output)

    def test_other_toolchain_with_drift_is_a_mismatch(self):
        self.set_mode("other-toolchain-drift")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("result: reference target mismatch", output)
        self.assertNotIn("result: parity failure", output)
        self.assertRegex(output, r"\[-\] scene alpha raw_render: 1 of \d+ samples differ")

    def test_package_difference_is_a_toolchain_difference(self):
        with mock.patch.object(regression_runner, "package_lines", return_value=["package libc6 9.9"]):
            code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PASSED, output)
        self.assertIn("note: the toolchain differs from the reference set", output)
        self.assertIn("package libc6 9.9", output)
        self.set_mode("flip-bit:depth")
        with mock.patch.object(regression_runner, "package_lines", return_value=["package libc6 9.9"]):
            code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("result: reference target mismatch", output)

    def test_leftover_reference_beside_a_toolchain_difference_exits_one(self):
        shutil.copy(self.folder / "scenes" / "alpha.zip", self.folder / "scenes" / "retired.zip")
        self.set_mode("other-toolchain-drift")
        code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_PARITY, output)
        self.assertIn("[-] scene retired: reference has no work item", output)

    def test_not_gate_valid_exits_three(self):
        with mock.patch.object(regression_runner, "gate_valid", return_value=False):
            code, output = self.run_main()
        self.assertEqual(code, regression_runner.EXIT_SETUP, output)
        self.assertIn("not gate-valid", output)

    def test_references_stay_unchanged(self):
        before = self.tree_bytes(self.folder)
        self.set_mode("flip-bit:depth")
        self.run_main()
        self.set_mode("same")
        self.run_main()
        self.assertEqual(self.tree_bytes(self.folder), before)

    def test_toolchain_identity_without_dpkg(self):
        result = mock.Mock(side_effect=FileNotFoundError("dpkg-query"))
        regression_runner.package_lines.cache_clear()
        self.addCleanup(regression_runner.package_lines.cache_clear)
        with mock.patch.object(regression_runner.subprocess, "run", result):
            self.assertEqual(regression_runner.package_lines(), [])

    def test_toolchain_identity_with_dpkg(self):
        result = mock.Mock(stdout="libgomp1 14.2\nlibc6 2.39\ngcc-13 \ngcc-13 13.3.0\n")
        regression_runner.package_lines.cache_clear()
        self.addCleanup(regression_runner.package_lines.cache_clear)
        with mock.patch.object(regression_runner.subprocess, "run", return_value=result):
            lines = regression_runner.package_lines()
        self.assertEqual(lines, ["package gcc-13 13.3.0", "package libc6 2.39", "package libgomp1 14.2"])


class ExportSourceTests(unittest.TestCase):
    """T048: the export writes only the values the contract lists."""

    def test_no_excluded_statistics_are_read(self):
        source = (TESTS_DIR.parent / "mandelbulber2" / "src" / "verification_export.cpp").read_text(encoding="utf-8")
        for name in ("statistics.time", "totalNoise", "maskedPixels", "tilesDone", "GetNumberOf", "GetTotalNumberOf"):
            self.assertNotIn(name, source)
        for name in ("totalNumberOfIterations", "numberOfRaymarchings", "missedDE"):
            self.assertIn(f"statistics.{name}", source)


if __name__ == "__main__":
    unittest.main()
