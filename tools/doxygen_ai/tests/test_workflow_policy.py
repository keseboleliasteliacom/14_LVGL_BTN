from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from tools.doxygen_ai.workflow_policy import (
    caller_context,
    documentation_churn,
    expand_selections,
    function_names,
    limit_pair_groups,
    ordinary_comments_changed,
    semantic_claim_issues,
)
from tools.doxygen_ai.update_docs import changed_files, code_changed, parse_semantic_review


class WorkflowPolicyTests(unittest.TestCase):
    def test_automatic_selection_expands_unchanged_pair(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "Module.c"
            header = root / "Module.h"
            source.write_text("void Module_Run(void) {}\n", encoding="utf-8")
            header.write_text("void Module_Run(void);\n", encoding="utf-8")
            selected = expand_selections([source])
            self.assertEqual([item.path for item in selected], [source.resolve(), header.resolve()])
            self.assertEqual([item.selection for item in selected], ["direct", "paired"])

    def test_expansion_deduplicates_two_direct_files(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "Module.c"
            header = root / "Module.h"
            source.touch()
            header.touch()
            selected = expand_selections([header, source])
            self.assertEqual(len(selected), 2)
            self.assertTrue(all(item.selection == "direct" for item in selected))

    def test_file_limit_does_not_split_pair(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            paths = []
            for name in ("A", "B"):
                source = root / f"{name}.c"
                header = root / f"{name}.h"
                source.touch()
                header.touch()
                paths.append(source)
            accepted, deferred = limit_pair_groups(expand_selections(paths), 1)
            self.assertEqual(len(accepted), 2)
            self.assertEqual(len(deferred), 2)

    def test_first_pair_may_exceed_tiny_soft_limit(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "A.c"
            header = root / "A.h"
            source.touch()
            header.touch()
            accepted, deferred = limit_pair_groups(expand_selections([source]), 1)
            self.assertEqual(len(accepted), 2)
            self.assertFalse(deferred)

    def test_ordinary_comments_are_immutable(self) -> None:
        original = '/** @brief Old. */\n// developer note\nvoid run(void);\n'
        doxygen_only = '/** @brief Better. */\n// developer note\nvoid run(void);\n'
        ordinary_change = '/** @brief Better. */\n// rewritten note\nvoid run(void);\n'
        self.assertFalse(ordinary_comments_changed(original, doxygen_only))
        self.assertTrue(ordinary_comments_changed(original, ordinary_change))

    def test_comment_scanner_ignores_comment_markers_in_strings(self) -> None:
        original = 'const char *url = "https://example.test";\n// note\n'
        updated = 'const char *url = "https://example.test";\n// note\n'
        self.assertFalse(ordinary_comments_changed(original, updated))

    def test_churn_report_distinguishes_unchanged_docs(self) -> None:
        text = '/**\n * @brief Stable.\n */\nvoid run(void);\n'
        report = documentation_churn(text, text)
        self.assertEqual(report.changed_lines, 0)
        self.assertEqual(report.ratio, 0.0)

    def test_function_name_discovery(self) -> None:
        names = function_names(
            "typedef void (*callback_t)(void *arg);\n"
            "void LEOPFetcher_Work(void *arg);\n"
            "static bool helper(void) { return true; }"
        )
        self.assertEqual(names, ["LEOPFetcher_Work", "helper"])

    def test_real_leop_context_contains_task_creation_argument(self) -> None:
        root = Path(__file__).resolve().parents[3]
        target = root / "main/LEOP/LEOP_Fetcher.h"
        caller = root / "main/main.c"
        context = caller_context(target, target.read_text(encoding="utf-8"), [target, caller], root)
        self.assertIn("xTaskCreate(LEOPFetcher_Work", context)
        self.assertIn("&app", context)

    def test_caller_context_finds_task_argument(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "LEOP_Fetcher.h"
            caller = root / "main.c"
            target.write_text("void LEOPFetcher_Work(void *arg);\n", encoding="utf-8")
            caller.write_text(
                "void app_main(void)\n{\n    xTaskCreate(LEOPFetcher_Work, \"leop\", 4096, &app, 4, 0);\n}\n",
                encoding="utf-8",
            )
            context = caller_context(target, target.read_text(), [target, caller], root)
            self.assertIn("&app", context)
            self.assertIn("main.c:3", context)

    def test_real_weather_context_proves_test_named_function_is_active(self) -> None:
        root = Path(__file__).resolve().parents[3]
        target = root / "ui/Tabs/Weather/Weather_UI.h"
        caller = root / "ui/screens/ui_Screen1.c"
        context = caller_context(target, target.read_text(encoding="utf-8"), [target, caller], root)
        self.assertIn("Weather_UI_Update_test();", context)
        self.assertIn("weather_dashboard_create();", context)

    def test_code_change_guard_allows_doxygen_only_change(self) -> None:
        original = "/** @brief Old. */\nvoid run(void) {}\n"
        updated = "/** @brief Accurate. */\nvoid run(void) {}\n"
        self.assertFalse(code_changed(original, updated))

    def test_code_change_guard_rejects_runtime_change(self) -> None:
        original = "int value = 1;\n"
        updated = "int value = 2;\n"
        self.assertTrue(code_changed(original, updated))

    def test_semantic_review_parser(self) -> None:
        review = parse_semantic_review('{"verdict":"warning","findings":["Caller is ambiguous"]}')
        self.assertEqual(review.verdict, "warning")
        self.assertEqual(review.findings, ("Caller is ambiguous",))

    def test_semantic_review_parser_rejects_unknown_verdict(self) -> None:
        with self.assertRaises(RuntimeError):
            parse_semantic_review('{"verdict":"maybe","findings":[]}')

    def test_august_10_baseline_expands_22_direct_files_to_34_targets(self) -> None:
        direct = changed_files("95277e5", "42430c0")
        selected = expand_selections(direct)
        accepted, deferred = limit_pair_groups(selected, 25)
        self.assertEqual(len(direct), 22)
        self.assertEqual(len(selected), 34)
        self.assertEqual(sum(item.selection == "paired" for item in selected), 12)
        self.assertEqual(len(accepted), 34)
        self.assertFalse(deferred)

    def test_wifi_ip_connectivity_contradiction_is_deterministic(self) -> None:
        root = Path(__file__).resolve().parents[3]
        header = (root / "main/WiFi.h").read_text(encoding="utf-8")
        source = (root / "main/WiFi.c").read_text(encoding="utf-8")
        issues = semantic_claim_issues(header, source, "")
        self.assertTrue(any("IP_EVENT_STA_GOT_IP" in issue for issue in issues))

    def test_active_weather_test_view_is_deterministic(self) -> None:
        root = Path(__file__).resolve().parents[3]
        header_path = root / "ui/Tabs/Weather/Weather_UI.h"
        caller_path = root / "ui/screens/ui_Screen1.c"
        header = header_path.read_text(encoding="utf-8")
        callers = caller_context(header_path, header, [header_path, caller_path], root)
        issues = semantic_claim_issues(header, "", callers)
        self.assertTrue(any("active caller context" in issue for issue in issues))

    def test_leop_task_argument_mismatch_is_deterministic(self) -> None:
        root = Path(__file__).resolve().parents[3]
        header = (root / "main/LEOP/LEOP_Fetcher.h").read_text(encoding="utf-8")
        source = (root / "main/LEOP/LEOP_Fetcher.c").read_text(encoding="utf-8")
        issues = semantic_claim_issues(header, source, "")
        self.assertTrue(any("app_state_t" in issue for issue in issues))


if __name__ == "__main__":
    unittest.main()
