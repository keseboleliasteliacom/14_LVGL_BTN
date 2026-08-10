#!/usr/bin/env python3
"""
Update Doxygen comments in changed C/C++ files using the OpenAI Responses API.

The script is intentionally conservative:
- Only changed files are processed
- Only selected source/header extensions are allowed
- A file-level documentation heuristic decides between "update" and "document"
- The generated output is rejected if non-comment code changed
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import urllib.error
import urllib.request
from dataclasses import dataclass
from difflib import unified_diff
from pathlib import Path
from typing import Iterable

try:
    from .workflow_policy import (
        SelectedFile,
        caller_context,
        documentation_churn,
        expand_selections,
        limit_pair_groups,
        ordinary_comments_changed,
    )
except ImportError:  # Direct script execution.
    from workflow_policy import (
        SelectedFile,
        caller_context,
        documentation_churn,
        expand_selections,
        limit_pair_groups,
        ordinary_comments_changed,
    )


REPO_ROOT = Path(__file__).resolve().parents[2]
DOCS_DIR = REPO_ROOT / "docs"
REJECTED_OUTPUT_DIR = REPO_ROOT / "tools" / "doxygen_ai" / "rejected"
STATE_DIR = REPO_ROOT / "tools" / "doxygen_ai" / "state"
DEFAULT_MANIFEST_PATH = STATE_DIR / "run_manifest.json"
ALLOWED_SUFFIXES = {".c", ".h", ".cpp", ".hpp", ".cc", ".hh"}
HEADER_SUFFIXES = {".h", ".hpp", ".hh"}
SOURCE_SUFFIXES = {".c", ".cpp", ".cc"}
DEFAULT_MODEL = os.getenv("OPENAI_MODEL", "gpt-5.4-mini")
DEFAULT_REASONING_EFFORT = os.getenv("OPENAI_REASONING_EFFORT", "low")
DEFAULT_RETRY_REASONING_EFFORT = os.getenv("OPENAI_RETRY_REASONING_EFFORT", "medium")
REASONING_EFFORTS = ("none", "low", "medium", "high", "xhigh")
DEFAULT_MAX_FILES = int(os.getenv("MAX_FILES_PER_RUN", "2"))
DEFAULT_MAX_CHARS = int(os.getenv("MAX_CHARS_PER_FILE", "18000"))
DEFAULT_MAX_OUTPUT_TOKENS = int(os.getenv("OPENAI_MAX_OUTPUT_TOKENS", "12000"))
DEFAULT_INPUT_COST_PER_MILLION = float(os.getenv("OPENAI_INPUT_COST_PER_MILLION", "0.75"))
DEFAULT_OUTPUT_COST_PER_MILLION = float(os.getenv("OPENAI_OUTPUT_COST_PER_MILLION", "4.50"))
DEFAULT_USD_TO_SEK = float(os.getenv("OPENAI_USD_TO_SEK", "9.19981"))
DEFAULT_TEST_LABEL = os.getenv("DOXYGEN_AI_TEST_LABEL", "").strip()
DEFAULT_MODE = os.getenv("DOXYGEN_AI_MODE", "normal").strip() or "normal"
MODES = ("normal", "audit")


@dataclass
class Usage:
    input_tokens: int = 0
    output_tokens: int = 0
    total_tokens: int = 0


@dataclass
class OpenAIResult:
    text: str
    usage: Usage
    model: str


@dataclass
class FileResult:
    path: str
    status: str
    model: str = ""
    reasoning_effort: str = ""
    input_tokens: int = 0
    output_tokens: int = 0
    total_tokens: int = 0
    estimated_cost_usd: float = 0.0
    estimated_cost_sek: float = 0.0
    details: str = ""
    selection: str = "direct"
    semantic_verdict: str = "not_run"
    semantic_findings: tuple[str, ...] = ()
    documentation_changed_lines: int = 0
    documentation_churn_ratio: float = 0.0


@dataclass(frozen=True)
class SemanticReview:
    verdict: str
    findings: tuple[str, ...]


def combine_openai_results(previous: OpenAIResult, latest: OpenAIResult) -> OpenAIResult:
    return OpenAIResult(
        text=latest.text,
        usage=Usage(
            input_tokens=previous.usage.input_tokens + latest.usage.input_tokens,
            output_tokens=previous.usage.output_tokens + latest.usage.output_tokens,
            total_tokens=previous.usage.total_tokens + latest.usage.total_tokens,
        ),
        model=latest.model,
    )


def estimate_cost_usd(usage: Usage, input_cost_per_million: float, output_cost_per_million: float) -> float:
    return (
        (usage.input_tokens / 1_000_000) * input_cost_per_million
        + (usage.output_tokens / 1_000_000) * output_cost_per_million
    )


def convert_usd_to_sek(usd_amount: float, usd_to_sek: float) -> float:
    return usd_amount * usd_to_sek


def run_git(args: list[str]) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    return result.stdout.strip()


def load_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def resolve_diff_range(base: str | None, head: str | None) -> tuple[str, str]:
    if base and head:
        return base, head
    if base:
        return base, head or "HEAD"

    event_name = os.getenv("GITHUB_EVENT_NAME", "")
    if event_name == "push":
        before = os.getenv("GITHUB_EVENT_BEFORE", "")
        sha = os.getenv("GITHUB_SHA", "") or "HEAD"
        if before and before != "0000000000000000000000000000000000000000":
            return before, sha
    if event_name == "pull_request":
        base_ref = os.getenv("GITHUB_BASE_REF", "")
        sha = os.getenv("GITHUB_SHA", "") or "HEAD"
        if base_ref:
            return f"origin/{base_ref}", sha

    return "HEAD~1", "HEAD"


def changed_files(base: str, head: str) -> list[Path]:
    names = run_git(["diff", "--name-only", "--diff-filter=ACMR", base, head]).splitlines()
    files: list[Path] = []
    for name in names:
        rel = Path(name)
        if rel.suffix.lower() not in ALLOWED_SUFFIXES:
            continue
        abs_path = (REPO_ROOT / rel).resolve()
        try:
            abs_path.relative_to(REPO_ROOT.resolve())
        except ValueError:
            continue
        if abs_path.is_file():
            files.append(abs_path)
    return files


def parse_explicit_files(file_paths_raw: str) -> list[Path]:
    files: list[Path] = []
    seen: set[Path] = set()

    for part in file_paths_raw.split(","):
        raw = part.strip()
        if not raw:
            continue

        rel = Path(raw)
        abs_path = (REPO_ROOT / rel).resolve()

        try:
            abs_path.relative_to(REPO_ROOT.resolve())
        except ValueError as exc:
            raise RuntimeError(f"Explicit file path is outside the repository: {raw}") from exc

        if not abs_path.exists():
            raise RuntimeError(f"Explicit file path does not exist: {raw}")

        if abs_path.suffix.lower() not in ALLOWED_SUFFIXES:
            raise RuntimeError(f"Explicit file path has unsupported extension: {raw}")

        if abs_path not in seen:
            seen.add(abs_path)
            files.append(abs_path)

    return files


def expand_module_files(files: list[Path]) -> list[Path]:
    """Compatibility wrapper for callers that only need expanded paths."""
    return [item.path for item in expand_selections(files)]


def has_doxygen(text: str) -> bool:
    return bool(re.search(r"/\*\*[\s\S]*?@(file|brief|param|return|defgroup|ingroup)\b", text))


def is_entrypoint_or_test(path: Path, text: str) -> bool:
    lowered = path.name.lower()
    return lowered.startswith("main.") or "int main(" in text or "int main(void" in text or "void app_main(" in text or "app_main(" in text


def find_paired_file(path: Path) -> Path | None:
    if path.suffix.lower() in HEADER_SUFFIXES:
        candidate_suffixes = SOURCE_SUFFIXES
    elif path.suffix.lower() in SOURCE_SUFFIXES:
        candidate_suffixes = HEADER_SUFFIXES
    else:
        return None

    for suffix in candidate_suffixes:
        candidate = path.with_suffix(suffix)
        if candidate.exists():
            return candidate
    return None


def build_prompt(
    path: Path,
    code: str,
    docs_rules: str,
    header_template: str,
    source_template: str,
    main_template: str,
    paired_file_context: str,
    caller_file_context: str,
    workflow_mode: str,
    ordinary_comment_policy: str,
) -> tuple[str, str]:
    file_kind = "header" if path.suffix.lower() in HEADER_SUFFIXES else "source"
    documentation_mode = "update_existing_docs" if has_doxygen(code) else "document_full_file"
    template = header_template if file_kind == "header" else main_template if is_entrypoint_or_test(path, code) else source_template
    ordinary_rule = (
        "Existing ordinary comments are immutable. Do not add, remove, move, or rewrite them."
        if ordinary_comment_policy == "preserve"
        else "Ordinary comments may change only when the explicit comment-audit policy identifies a stale, "
        "inaccurate, redundant, or disabled-code comment; keep unrelated ordinary comments unchanged."
    )
    system_prompt = f"""You are a documentation-only refactoring assistant for an embedded C/C++ repository.

LAYER 1 — IMMUTABLE TRANSFORMATION CONTRACT
- Only Doxygen and ordinary comments may change. Every non-comment code token must remain unchanged and in the same order.
- Preserve logging, strings, identifiers, declarations, signatures, initialization, control flow, formatting intent, and line endings.
- Do not add, remove, move, or rewrite any non-comment code, including declarations, prototypes, includes, macros, or runtime diagnostics.
- Existing Doxygen may change when needed for accuracy, required coverage, or the repository's target style. Avoid synonym-only and formatting-only churn.
- {ordinary_rule}
- Documentation must describe only behavior supported by the code and must be written in English.
- Return the complete updated target file only, without Markdown fences or explanations.
"""

    user_prompt = f"""LAYER 2 — CANONICAL DOCUMENTATION POLICY

The following repository standard is authoritative. Apply each rule once; do not infer extra requirements from repetition.

{docs_rules}

Relevant target-style template:
{template}

LAYER 3 — CURRENT FILE TASK

Mode: {documentation_mode}
Path: {path.relative_to(REPO_ROOT).as_posix()}

Mode behavior:
- update_existing_docs: correct incomplete, stale, inaccurate, or unnecessarily verbose Doxygen while preserving accurate documentation.
- document_full_file: add all documentation required by the canonical policy.

Paired module context (read-only):
{paired_file_context}

Caller and integration context (read-only; {workflow_mode} mode):
{caller_file_context}

Evidence rules:
- A function or variable name containing "test", "legacy", or similar wording does not prove that it is inactive.
- Use callers and state assignments when describing runtime role, task arguments, connectivity, ownership, or integration status.
- If supporting evidence is absent or ambiguous, use narrower wording instead of inventing a guarantee.

Final self-check:
1. Required coverage is complete.
2. Documentation agrees with signatures, reachable behavior, side effects, and relevant embedded constraints.
3. Accurate existing documentation is not cosmetically rewritten.
4. Every non-comment code token is unchanged.

Return only the complete updated target file.

Target file contents:
{code}
"""
    return system_prompt, user_prompt


def extract_output_text(data: dict) -> str:
    if isinstance(data.get("output_text"), str) and data["output_text"].strip():
        return data["output_text"]

    outputs = data.get("output", [])
    parts: list[str] = []
    for item in outputs:
        for content in item.get("content", []):
            if content.get("type") == "output_text":
                parts.append(content.get("text", ""))
    return "".join(parts)


def strip_markdown_fences(text: str) -> str:
    stripped = text.strip()
    if stripped.startswith("```") and stripped.endswith("```"):
        lines = stripped.splitlines()
        if len(lines) >= 3:
            return "\n".join(lines[1:-1]).strip() + "\n"
    return text


def normalize_code_without_comments(text: str) -> str:
    result: list[str] = []
    i = 0
    n = len(text)
    in_line_comment = False
    in_block_comment = False
    in_string = False
    in_char = False

    while i < n:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < n else ""

        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
                result.append("\n")
            i += 1
            continue

        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
            else:
                if ch == "\n":
                    result.append("\n")
                i += 1
            continue

        if in_string:
            result.append(ch)
            if ch == "\\" and i + 1 < n:
                result.append(text[i + 1])
                i += 2
                continue
            if ch == '"':
                in_string = False
            i += 1
            continue

        if in_char:
            result.append(ch)
            if ch == "\\" and i + 1 < n:
                result.append(text[i + 1])
                i += 2
                continue
            if ch == "'":
                in_char = False
            i += 1
            continue

        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            continue

        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            continue

        if ch == '"':
            in_string = True
            result.append(ch)
            i += 1
            continue

        if ch == "'":
            in_char = True
            result.append(ch)
            i += 1
            continue

        result.append(ch)
        i += 1

    lines = [" ".join(line.split()) for line in "".join(result).splitlines()]
    return "\n".join(line for line in lines if line)


def code_changed(original: str, updated: str) -> bool:
    return normalize_code_without_comments(original) != normalize_code_without_comments(updated)


def mask_comments_strings_and_preprocessor(text: str) -> str:
    """Mask non-code regions while preserving offsets and line breaks."""
    result = list(text)
    i = 0
    n = len(text)
    state = "code"

    while i < n:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < n else ""

        if state == "line_comment":
            if ch == "\n":
                state = "code"
            else:
                result[i] = " "
            i += 1
            continue

        if state == "block_comment":
            if ch == "*" and nxt == "/":
                result[i] = " "
                result[i + 1] = " "
                state = "code"
                i += 2
                continue
            if ch != "\n":
                result[i] = " "
            i += 1
            continue

        if state in {"string", "char"}:
            terminator = '"' if state == "string" else "'"
            if ch == "\\" and i + 1 < n:
                result[i] = " "
                if text[i + 1] != "\n":
                    result[i + 1] = " "
                i += 2
                continue
            if ch == terminator:
                result[i] = " "
                state = "code"
            elif ch != "\n":
                result[i] = " "
            i += 1
            continue

        if ch == "/" and nxt == "/":
            result[i] = " "
            result[i + 1] = " "
            state = "line_comment"
            i += 2
            continue
        if ch == "/" and nxt == "*":
            result[i] = " "
            result[i + 1] = " "
            state = "block_comment"
            i += 2
            continue
        if ch == '"':
            result[i] = " "
            state = "string"
            i += 1
            continue
        if ch == "'":
            result[i] = " "
            state = "char"
            i += 1
            continue
        i += 1

    masked = "".join(result)
    masked_lines = []
    for line in masked.splitlines(keepends=True):
        content = line[:-1] if line.endswith("\n") else line
        newline = "\n" if line.endswith("\n") else ""
        if content.lstrip().startswith("#"):
            masked_lines.append(" " * len(content) + newline)
        else:
            masked_lines.append(line)
    return "".join(masked_lines)


def has_direct_doxygen_block(text: str, definition_start: int) -> bool:
    prefix = text[:definition_start].rstrip()
    if not prefix.endswith("*/"):
        return False

    block_start = prefix.rfind("/**")
    if block_start < 0:
        return False
    return prefix.find("*/", block_start) == len(prefix) - 2


def find_undocumented_public_function_definitions(text: str) -> list[str]:
    """Find top-level non-static function definitions lacking direct Doxygen."""
    masked = mask_comments_strings_and_preprocessor(text)
    missing: list[str] = []
    segment_start = 0
    brace_depth = 0

    for index, ch in enumerate(masked):
        if ch == "{" and brace_depth == 0:
            raw_segment = masked[segment_start:index]
            leading = len(raw_segment) - len(raw_segment.lstrip())
            signature = raw_segment.strip()

            if signature and ")" in signature:
                close_paren = signature.rfind(")")
                depth = 1
                open_paren = close_paren - 1
                while open_paren >= 0 and depth > 0:
                    if signature[open_paren] == ")":
                        depth += 1
                    elif signature[open_paren] == "(":
                        depth -= 1
                    open_paren -= 1

                if depth == 0:
                    open_paren += 1
                    before_params = signature[:open_paren].rstrip()
                    name_match = re.search(
                        r"([A-Za-z_~][A-Za-z0-9_~]*(?:::[A-Za-z_~][A-Za-z0-9_~]*)*)$",
                        before_params,
                    )
                    if name_match is not None:
                        name = name_match.group(1)
                        prefix = before_params[:name_match.start()]
                        is_static = re.search(r"\bstatic\b", prefix) is not None
                        if not is_static and name not in {"if", "for", "while", "switch"}:
                            definition_start = segment_start + leading
                            if not has_direct_doxygen_block(text, definition_start):
                                missing.append(name)

            brace_depth += 1
            continue

        if ch == "{" and brace_depth > 0:
            brace_depth += 1
        elif ch == "}" and brace_depth > 0:
            brace_depth -= 1
            if brace_depth == 0:
                segment_start = index + 1
        elif ch == ";" and brace_depth == 0:
            segment_start = index + 1

    return missing


def code_diff_excerpt(original: str, updated: str, max_lines: int = 12) -> str:
    original_lines = normalize_code_without_comments(original).splitlines()
    updated_lines = normalize_code_without_comments(updated).splitlines()
    diff_lines = list(
        unified_diff(
            original_lines,
            updated_lines,
            fromfile="original",
            tofile="updated",
            lineterm="",
        )
    )
    if not diff_lines:
        return "No normalized code diff available."
    return "\n".join(diff_lines[:max_lines])


def call_openai(
    system_prompt: str,
    user_prompt: str,
    model: str,
    reasoning_effort: str,
    max_output_tokens: int,
) -> OpenAIResult:
    api_key = os.getenv("OPENAI_API_KEY")
    if not api_key:
        raise RuntimeError("OPENAI_API_KEY is required")

    payload = {
        "model": model,
        "input": [
            {
                "role": "system",
                "content": [{"type": "input_text", "text": system_prompt}],
            },
            {
                "role": "user",
                "content": [{"type": "input_text", "text": user_prompt}],
            },
        ],
        "reasoning": {"effort": reasoning_effort},
        "max_output_tokens": max_output_tokens,
    }

    request = urllib.request.Request(
        "https://api.openai.com/v1/responses",
        data=json.dumps(payload).encode("utf-8"),
        headers={
            "Content-Type": "application/json",
            "Authorization": f"Bearer {api_key}",
        },
        method="POST",
    )

    with urllib.request.urlopen(request, timeout=180) as response:
        data = json.loads(response.read().decode("utf-8"))

    text = strip_markdown_fences(extract_output_text(data))
    usage_data = data.get("usage", {}) or {}
    usage = Usage(
        input_tokens=int(usage_data.get("input_tokens") or 0),
        output_tokens=int(usage_data.get("output_tokens") or 0),
        total_tokens=int(usage_data.get("total_tokens") or 0),
    )
    actual_model = str(data.get("model") or model)
    return OpenAIResult(text=text, usage=usage, model=actual_model)


def call_openai_with_retry(
    system_prompt: str,
    user_prompt: str,
    model: str,
    reasoning_effort: str,
    retry_reasoning_effort: str,
    max_output_tokens: int,
    original: str,
) -> tuple[OpenAIResult, bool]:
    first_result = call_openai(system_prompt, user_prompt, model, reasoning_effort, max_output_tokens)
    if not code_changed(original, first_result.text):
        return first_result, False

    diff_excerpt = code_diff_excerpt(original, first_result.text)
    retry_user_prompt = f"""{user_prompt}

Previous attempt was rejected because non-comment code changed.

You must regenerate the file with comments/documentation changes only.
Do not alter any code tokens, including:
- string literals
- JSON keys
- identifiers
- constants
- URLs
- macro names
- forward declarations
- helper prototypes
- any other non-comment lines

Normalized code diff excerpt from the rejected attempt:
{diff_excerpt}
"""
    retry_result = call_openai(system_prompt, retry_user_prompt, model, retry_reasoning_effort, max_output_tokens)
    return combine_openai_results(first_result, retry_result), True


def parse_semantic_review(text: str) -> SemanticReview:
    try:
        payload = json.loads(strip_markdown_fences(text))
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"Semantic reviewer returned invalid JSON: {exc}") from exc
    verdict = str(payload.get("verdict") or "").lower()
    if verdict not in {"pass", "warning", "reject"}:
        raise RuntimeError(f"Semantic reviewer returned unsupported verdict: {verdict or '(empty)'}")
    findings = tuple(str(item) for item in payload.get("findings", []) if str(item).strip())
    return SemanticReview(verdict=verdict, findings=findings)


def review_semantics(
    path: Path,
    original: str,
    updated: str,
    paired_context: str,
    callers: str,
    model: str,
    reasoning_effort: str,
) -> tuple[SemanticReview, OpenAIResult]:
    system_prompt = """You review documentation-only C/C++ changes for semantic accuracy.
Return JSON only: {"verdict":"pass|warning|reject","findings":["concise evidence-based finding"]}.
Reject only a clear contradiction with supplied code evidence. Use warning for ambiguity or likely churn.
Check task arguments, active versus test/legacy status, connectivity meaning, ownership, units, blocking,
state transitions, and header/source/caller agreement. Do not request code changes."""
    user_prompt = f"""Target: {path.relative_to(REPO_ROOT).as_posix()}

Original target:
{original}

Proposed target:
{updated}

Paired context:
{paired_context}

Caller/integration context:
{callers}
"""
    result = call_openai(system_prompt, user_prompt, model, reasoning_effort, min(DEFAULT_MAX_OUTPUT_TOKENS, 2500))
    return parse_semantic_review(result.text), result


def retry_missing_function_documentation(
    system_prompt: str,
    user_prompt: str,
    model: str,
    retry_reasoning_effort: str,
    max_output_tokens: int,
    previous_result: OpenAIResult,
    missing_functions: list[str],
) -> OpenAIResult:
    missing_list = "\n".join(f"- {name}" for name in missing_functions)
    retry_user_prompt = f"""{user_prompt}

Your previous output was rejected by a deterministic documentation coverage check.

The following public function definitions are still missing a directly preceding Doxygen block:
{missing_list}

Regenerate the full file and add concise repository-standard Doxygen documentation directly above every listed function definition.
For public source functions whose contract is already documented in the paired header, prefer the brief implementation + see-header pattern.
Preserve every code token and all existing developer comments exactly.
Do not make synonym-only, formatting-only, or unrelated documentation changes.
"""
    retry_result = call_openai(system_prompt, retry_user_prompt, model, retry_reasoning_effort, max_output_tokens)
    return combine_openai_results(previous_result, retry_result)


def normalize_file_text(text: str) -> str:
    normalized = text.replace("\r\n", "\n").replace("\r", "\n")
    return normalized if normalized.endswith("\n") else f"{normalized}\n"


def write_file(path: Path, text: str) -> None:
    path.write_text(normalize_file_text(text), encoding="utf-8", newline="\n")


def write_rejected_output(path: Path, text: str) -> Path:
    REJECTED_OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    relative_name = path.relative_to(REPO_ROOT).as_posix().replace("/", "__")
    output_path = REJECTED_OUTPUT_DIR / f"{relative_name}.rejected.txt"
    output_path.write_text(text if text.endswith("\n") else f"{text}\n", encoding="utf-8", newline="\n")
    return output_path


def append_github_summary(
    file_results: list[FileResult],
    total_usage: Usage,
    total_estimated_cost_usd: float,
    total_estimated_cost_sek: float,
    requested_model: str,
    reasoning_effort: str,
    retry_reasoning_effort: str,
    test_label: str,
    workflow_mode: str,
) -> None:
    summary_path = os.getenv("GITHUB_STEP_SUMMARY", "").strip()
    if not summary_path:
        return

    updated_count = sum(1 for result in file_results if result.status == "updated")
    no_change_count = sum(1 for result in file_results if result.status == "no_change")
    skipped_count = sum(1 for result in file_results if result.status == "skipped")
    rejected_count = sum(1 for result in file_results if result.status == "rejected")

    lines = [
        "## Doxygen AI Summary",
        "",
        f"- Requested model: {requested_model}",
        f"- Initial reasoning effort: {reasoning_effort}",
        f"- Targeted retry reasoning effort: {retry_reasoning_effort}",
        f"- Test label: {test_label or '(none)'}",
        f"- Workflow mode: {workflow_mode}",
        f"- Direct files: {sum(1 for result in file_results if result.selection == 'direct')}",
        f"- Pair-expanded files: {sum(1 for result in file_results if result.selection == 'paired')}",
        f"- Files considered: {len(file_results)}",
        f"- Files updated: {updated_count}",
        f"- Files unchanged: {no_change_count}",
        f"- Files skipped: {skipped_count}",
        f"- Files rejected: {rejected_count}",
        f"- Input tokens: {total_usage.input_tokens}",
        f"- Output tokens: {total_usage.output_tokens}",
        f"- Total tokens: {total_usage.total_tokens}",
        f"- Estimated cost (USD): ${total_estimated_cost_usd:.6f}",
        f"- Estimated cost (SEK): {total_estimated_cost_sek:.6f} kr",
        "",
        "| File | Selected | Status | Semantic | Model | Reasoning | Input | Output | Total | USD | SEK | Details |",
        "| --- | --- | --- | --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- |",
    ]

    for result in file_results:
        lines.append(
            f"| {result.path} | {result.selection} | {result.status} | {result.semantic_verdict} | "
            f"{result.model or '-'} | {result.reasoning_effort or '-'} | "
            f"{result.input_tokens} | {result.output_tokens} | {result.total_tokens} | "
            f"${result.estimated_cost_usd:.6f} | {result.estimated_cost_sek:.6f} kr | {result.details or '-'} |"
        )

    with open(summary_path, "a", encoding="utf-8", newline="\n") as handle:
        handle.write("\n".join(lines) + "\n")


def write_manifest(
    manifest_path: Path,
    requested_model: str,
    reasoning_effort: str,
    retry_reasoning_effort: str,
    test_label: str,
    file_results: list[FileResult],
    workflow_mode: str,
) -> None:
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "requested_model": requested_model,
        "reasoning_effort": reasoning_effort,
        "retry_reasoning_effort": retry_reasoning_effort,
        "test_label": test_label,
        "workflow_mode": workflow_mode,
        "base_sha": os.getenv("DOXYGEN_AI_BASE_SHA", ""),
        "head_sha": os.getenv("DOXYGEN_AI_HEAD_SHA", ""),
        "files": [
            {
                "path": result.path,
                "status": result.status,
                "model": result.model,
                "reasoning_effort": result.reasoning_effort,
                "input_tokens": result.input_tokens,
                "output_tokens": result.output_tokens,
                "total_tokens": result.total_tokens,
                "estimated_cost_usd": result.estimated_cost_usd,
                "estimated_cost_sek": result.estimated_cost_sek,
                "details": result.details,
                "selection": result.selection,
                "semantic_verdict": result.semantic_verdict,
                "semantic_findings": list(result.semantic_findings),
                "documentation_changed_lines": result.documentation_changed_lines,
                "documentation_churn_ratio": result.documentation_churn_ratio,
            }
            for result in file_results
        ],
    }
    manifest_path.write_text(json.dumps(payload, indent=2), encoding="utf-8", newline="\n")


def load_remaining_files_from_manifest(manifest_path: Path) -> list[Path]:
    if not manifest_path.exists():
        raise RuntimeError(f"Manifest file does not exist: {manifest_path}")

    payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    files: list[Path] = []
    seen: set[Path] = set()

    for entry in payload.get("files", []):
        status = str(entry.get("status") or "")
        if status not in {"rejected", "deferred"}:
            continue

        rel = Path(str(entry.get("path") or "")).as_posix()
        abs_path = (REPO_ROOT / rel).resolve()
        try:
            abs_path.relative_to(REPO_ROOT.resolve())
        except ValueError as exc:
            raise RuntimeError(f"Manifest file path is outside the repository: {rel}") from exc
        if not abs_path.is_file():
            continue
        if abs_path.suffix.lower() not in ALLOWED_SUFFIXES:
            raise RuntimeError(f"Manifest file path has unsupported extension: {rel}")
        if abs_path not in seen:
            seen.add(abs_path)
            files.append(abs_path)

    return files


def process_files(
    files: Iterable[SelectedFile],
    model: str,
    reasoning_effort: str,
    retry_reasoning_effort: str,
    max_chars: int,
    max_output_tokens: int,
    input_cost_per_million: float,
    output_cost_per_million: float,
    usd_to_sek: float,
    deferred_files: list[SelectedFile],
    manifest_path: Path,
    workflow_mode: str,
    ordinary_comment_policy: str,
) -> int:
    docs_rules = load_text(DOCS_DIR / "Doxygen_Standard.md")
    header_template = load_text(DOCS_DIR / "template_H.h")
    source_template = load_text(DOCS_DIR / "template_C.c")
    main_template = load_text(DOCS_DIR / "template_Main.c")
    total_usage = Usage()
    changed_count = 0
    file_results: list[FileResult] = []
    rejection_messages: list[str] = []

    tracked_context_files = [
        (REPO_ROOT / name).resolve()
        for name in run_git(["ls-files", "*.c", "*.h", "*.cpp", "*.hpp", "*.cc", "*.hh"]).splitlines()
    ]

    for selected in deferred_files:
        path = selected.path
        rel = path.relative_to(REPO_ROOT).as_posix()
        file_results.append(
            FileResult(
                path=rel,
                status="deferred",
                details="Deferred by MAX_FILES_PER_RUN",
                selection=selected.selection,
            )
        )

    for selected in files:
        path = selected.path
        original = load_text(path)
        rel = path.relative_to(REPO_ROOT).as_posix()
        paired_file = find_paired_file(path)
        paired_file_context = "No paired header/source file was found."
        caller_file_context = "Caller-context discovery is disabled in normal mode."

        if not original.strip():
            print(f"No changes for {rel}: file is empty or whitespace-only")
            file_results.append(
                FileResult(
                    path=rel,
                    status="no_change",
                    details="Empty or whitespace-only file; no documentation required",
                    selection=selected.selection,
                )
            )
            continue

        if len(original) > max_chars:
            print(f"Skipping {rel}: file exceeds MAX_CHARS_PER_FILE ({len(original)} > {max_chars})")
            file_results.append(
                FileResult(
                    path=rel,
                    status="skipped",
                    details="File exceeds MAX_CHARS_PER_FILE",
                    selection=selected.selection,
                )
            )
            continue

        if paired_file is not None:
            paired_text = load_text(paired_file)
            paired_rel = paired_file.relative_to(REPO_ROOT).as_posix()
            if len(paired_text) <= max_chars:
                paired_file_context = (
                    f"Use this only as supporting module context. Do not modify it.\n"
                    f"Paired path: {paired_rel}\n"
                    f"Paired file contents:\n{paired_text}"
                )
            else:
                paired_file_context = (
                    f"Paired path: {paired_rel}\n"
                    f"Paired file exists but was omitted because it exceeds MAX_CHARS_PER_FILE."
                )

        if workflow_mode == "audit":
            caller_file_context = caller_context(
                target=path,
                target_text=original,
                candidates=tracked_context_files,
                repo_root=REPO_ROOT,
            )

        system_prompt, user_prompt = build_prompt(
            path,
            original,
            docs_rules,
            header_template,
            source_template,
            main_template,
            paired_file_context,
            caller_file_context,
            workflow_mode,
            ordinary_comment_policy,
        )

        print(
            f"Processing {rel} with model {model}, initial reasoning effort {reasoning_effort}, "
            f"and targeted retry effort {retry_reasoning_effort}"
        )
        coverage_retried = False
        try:
            result, retried = call_openai_with_retry(
                system_prompt=system_prompt,
                user_prompt=user_prompt,
                model=model,
                reasoning_effort=reasoning_effort,
                retry_reasoning_effort=retry_reasoning_effort,
                max_output_tokens=max_output_tokens,
                original=original,
            )

            if (
                path.suffix.lower() in SOURCE_SUFFIXES
                and result.text.strip()
                and not code_changed(original, result.text)
            ):
                missing_function_docs = find_undocumented_public_function_definitions(result.text)
                if missing_function_docs:
                    print(
                        f"Retrying {rel}: missing direct Doxygen documentation for "
                        f"{', '.join(missing_function_docs)}"
                    )
                    result = retry_missing_function_documentation(
                        system_prompt=system_prompt,
                        user_prompt=user_prompt,
                        model=model,
                        retry_reasoning_effort=retry_reasoning_effort,
                        max_output_tokens=max_output_tokens,
                        previous_result=result,
                        missing_functions=missing_function_docs,
                    )
                    coverage_retried = True
        except urllib.error.HTTPError as exc:
            body = exc.read().decode("utf-8", errors="replace")
            file_results.append(
                FileResult(path=rel, status="rejected", details=f"HTTP {exc.code}", selection=selected.selection)
            )
            rejection_messages.append(f"{rel}: OpenAI API request failed with HTTP {exc.code}: {body}")
            print(f"Rejected {rel}: OpenAI API request failed with HTTP {exc.code}")
            continue
        except urllib.error.URLError as exc:
            file_results.append(
                FileResult(
                    path=rel,
                    status="rejected",
                    details="Network/API request error",
                    selection=selected.selection,
                )
            )
            rejection_messages.append(f"{rel}: OpenAI API request failed: {exc}")
            print(f"Rejected {rel}: OpenAI API request failed: {exc}")
            continue

        updated = result.text
        semantic_review = SemanticReview("not_run", ())
        if (
            workflow_mode == "audit"
            and updated.strip()
            and normalize_file_text(original) != normalize_file_text(updated)
            and not code_changed(original, updated)
            and (
                ordinary_comment_policy == "audit"
                or not ordinary_comments_changed(original, updated)
            )
        ):
            try:
                semantic_review, semantic_result = review_semantics(
                    path=path,
                    original=original,
                    updated=updated,
                    paired_context=paired_file_context,
                    callers=caller_file_context,
                    model=model,
                    reasoning_effort=retry_reasoning_effort,
                )
                result = combine_openai_results(result, semantic_result)
            except (RuntimeError, urllib.error.HTTPError, urllib.error.URLError) as exc:
                semantic_review = SemanticReview("reject", (f"Semantic review failed closed: {exc}",))
        usage = result.usage
        actual_model = result.model
        retry_count = int(retried) + int(coverage_retried)
        effort_path = (
            reasoning_effort
            if retry_count == 0
            else f"{reasoning_effort} -> {retry_reasoning_effort} ({retry_count} targeted retry{'s' if retry_count != 1 else ''})"
        )

        total_usage.input_tokens += usage.input_tokens
        total_usage.output_tokens += usage.output_tokens
        total_usage.total_tokens += usage.total_tokens
        estimated_cost = estimate_cost_usd(usage, input_cost_per_million, output_cost_per_million)
        estimated_cost_sek = convert_usd_to_sek(estimated_cost, usd_to_sek)

        if not updated.strip():
            print(f"Skipping {rel}: model returned empty output")
            file_results.append(
                FileResult(
                    path=rel,
                    status="skipped",
                    model=actual_model,
                    reasoning_effort=effort_path,
                    input_tokens=usage.input_tokens,
                    output_tokens=usage.output_tokens,
                    total_tokens=usage.total_tokens,
                    estimated_cost_usd=estimated_cost,
                    estimated_cost_sek=estimated_cost_sek,
                    details="Model returned empty output",
                    selection=selected.selection,
                )
            )
            continue

        if code_changed(original, updated):
            rejected_path = write_rejected_output(path, updated)
            details = f"Non-comment code changed after retry={retried}; saved to {rejected_path.relative_to(REPO_ROOT).as_posix()}"
            file_results.append(
                FileResult(
                    path=rel,
                    status="rejected",
                    model=actual_model,
                    reasoning_effort=effort_path,
                    input_tokens=usage.input_tokens,
                    output_tokens=usage.output_tokens,
                    total_tokens=usage.total_tokens,
                    estimated_cost_usd=estimated_cost,
                    estimated_cost_sek=estimated_cost_sek,
                    details=details,
                    selection=selected.selection,
                )
            )
            rejection_messages.append(f"{rel}: {details}")
            print(f"Rejected {rel}: {details}")
            continue

        if ordinary_comment_policy == "preserve" and ordinary_comments_changed(original, updated):
            rejected_path = write_rejected_output(path, updated)
            details = (
                "Ordinary developer comments changed; normal and audit Doxygen modes preserve them exactly; "
                f"saved to {rejected_path.relative_to(REPO_ROOT).as_posix()}"
            )
            file_results.append(
                FileResult(
                    path=rel,
                    status="rejected",
                    model=actual_model,
                    reasoning_effort=effort_path,
                    input_tokens=usage.input_tokens,
                    output_tokens=usage.output_tokens,
                    total_tokens=usage.total_tokens,
                    estimated_cost_usd=estimated_cost,
                    estimated_cost_sek=estimated_cost_sek,
                    details=details,
                    selection=selected.selection,
                )
            )
            rejection_messages.append(f"{rel}: {details}")
            print(f"Rejected {rel}: {details}")
            continue

        if path.suffix.lower() in SOURCE_SUFFIXES:
            missing_function_docs = find_undocumented_public_function_definitions(updated)
            if missing_function_docs:
                rejected_path = write_rejected_output(path, updated)
                missing_names = ", ".join(missing_function_docs)
                details = (
                    "Missing direct Doxygen documentation for public function "
                    f"definition(s) after coverage_retry={coverage_retried}: {missing_names}; saved to "
                    f"{rejected_path.relative_to(REPO_ROOT).as_posix()}"
                )
                file_results.append(
                    FileResult(
                        path=rel,
                        status="rejected",
                        model=actual_model,
                        reasoning_effort=effort_path,
                        input_tokens=usage.input_tokens,
                        output_tokens=usage.output_tokens,
                        total_tokens=usage.total_tokens,
                        estimated_cost_usd=estimated_cost,
                        estimated_cost_sek=estimated_cost_sek,
                        details=details,
                        selection=selected.selection,
                    )
                )
                rejection_messages.append(f"{rel}: {details}")
                print(f"Rejected {rel}: {details}")
                continue

        if semantic_review.verdict == "reject":
            rejected_path = write_rejected_output(path, updated)
            details = (
                "Semantic review rejected the proposed documentation: "
                + "; ".join(semantic_review.findings)
                + f"; saved to {rejected_path.relative_to(REPO_ROOT).as_posix()}"
            )
            file_results.append(
                FileResult(
                    path=rel,
                    status="rejected",
                    model=actual_model,
                    reasoning_effort=effort_path,
                    input_tokens=usage.input_tokens,
                    output_tokens=usage.output_tokens,
                    total_tokens=usage.total_tokens,
                    estimated_cost_usd=estimated_cost,
                    estimated_cost_sek=estimated_cost_sek,
                    details=details,
                    selection=selected.selection,
                    semantic_verdict=semantic_review.verdict,
                    semantic_findings=semantic_review.findings,
                )
            )
            rejection_messages.append(f"{rel}: {details}")
            print(f"Rejected {rel}: {details}")
            continue

        original_normalized = normalize_file_text(original)
        updated_normalized = normalize_file_text(updated)
        churn = documentation_churn(original_normalized, updated_normalized)
        if original_normalized == updated_normalized:
            print(f"No documentation changes needed for {rel}")
            file_results.append(
                FileResult(
                    path=rel,
                    status="no_change",
                    model=actual_model,
                    reasoning_effort=effort_path,
                    input_tokens=usage.input_tokens,
                    output_tokens=usage.output_tokens,
                    total_tokens=usage.total_tokens,
                    estimated_cost_usd=estimated_cost,
                    estimated_cost_sek=estimated_cost_sek,
                    details="No changes were necessary",
                    selection=selected.selection,
                    semantic_verdict=semantic_review.verdict,
                    semantic_findings=semantic_review.findings,
                )
            )
            continue

        details = "Documentation updated"
        if semantic_review.verdict == "warning" and semantic_review.findings:
            details += "; semantic warning: " + "; ".join(semantic_review.findings)
        if churn.changed_lines > 80 and churn.ratio > 1.5:
            details += (
                f"; high documentation churn ({churn.changed_lines} changed comment lines, "
                f"ratio={churn.ratio:.2f})"
            )

        write_file(path, updated_normalized)
        changed_count += 1
        file_results.append(
            FileResult(
                path=rel,
                status="updated",
                model=actual_model,
                reasoning_effort=effort_path,
                input_tokens=usage.input_tokens,
                output_tokens=usage.output_tokens,
                total_tokens=usage.total_tokens,
                estimated_cost_usd=estimated_cost,
                estimated_cost_sek=estimated_cost_sek,
                details=details,
                selection=selected.selection,
                semantic_verdict=semantic_review.verdict,
                semantic_findings=semantic_review.findings,
                documentation_changed_lines=churn.changed_lines,
                documentation_churn_ratio=churn.ratio,
            )
        )
        print(
            f"Updated {rel} "
            f"(model={actual_model}, input_tokens={usage.input_tokens}, output_tokens={usage.output_tokens}, "
            f"total_tokens={usage.total_tokens}, code_retried={retried}, coverage_retried={coverage_retried}, "
            f"estimated_cost_usd=${estimated_cost:.6f}, "
            f"estimated_cost_sek={estimated_cost_sek:.6f} kr)"
        )

    total_estimated_cost = estimate_cost_usd(
        total_usage,
        input_cost_per_million=input_cost_per_million,
        output_cost_per_million=output_cost_per_million,
    )
    total_estimated_cost_sek = convert_usd_to_sek(total_estimated_cost, usd_to_sek)
    print(
        "Run summary: "
        f"files_updated={changed_count}, "
        f"input_tokens={total_usage.input_tokens}, "
        f"output_tokens={total_usage.output_tokens}, "
        f"total_tokens={total_usage.total_tokens}, "
        f"estimated_cost_usd=${total_estimated_cost:.6f}, "
        f"estimated_cost_sek={total_estimated_cost_sek:.6f} kr"
    )
    append_github_summary(
        file_results=file_results,
        total_usage=total_usage,
        total_estimated_cost_usd=total_estimated_cost,
        total_estimated_cost_sek=total_estimated_cost_sek,
        requested_model=model,
        reasoning_effort=reasoning_effort,
        retry_reasoning_effort=retry_reasoning_effort,
        test_label=os.getenv("DOXYGEN_AI_TEST_LABEL", "").strip(),
        workflow_mode=workflow_mode,
    )
    write_manifest(
        manifest_path=manifest_path,
        requested_model=model,
        reasoning_effort=reasoning_effort,
        retry_reasoning_effort=retry_reasoning_effort,
        test_label=os.getenv("DOXYGEN_AI_TEST_LABEL", "").strip(),
        file_results=file_results,
        workflow_mode=workflow_mode,
    )

    if rejection_messages:
        raise RuntimeError(
            "One or more files were rejected during documentation generation:\n"
            + "\n".join(rejection_messages)
        )
    return changed_count


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", default=None)
    parser.add_argument("--head", default=None)
    parser.add_argument("--files", default="")
    parser.add_argument("--manifest", default=str(DEFAULT_MANIFEST_PATH))
    parser.add_argument("--remaining-only", action="store_true")
    parser.add_argument("--dry-run", action="store_true", help="Resolve and report file selection without API calls")
    parser.add_argument(
        "--mode",
        choices=MODES,
        default=DEFAULT_MODE,
        help="normal for routine updates; audit adds caller context and semantic review",
    )
    parser.add_argument(
        "--ordinary-comments",
        choices=("preserve", "audit"),
        default="preserve",
        help="preserve exactly by default; audit explicitly allows evidence-based cleanup",
    )
    parser.add_argument("--max-files", type=int, default=DEFAULT_MAX_FILES)
    parser.add_argument("--max-chars", type=int, default=DEFAULT_MAX_CHARS)
    parser.add_argument("--max-output-tokens", type=int, default=DEFAULT_MAX_OUTPUT_TOKENS)
    parser.add_argument("--model", default=DEFAULT_MODEL)
    parser.add_argument(
        "--reasoning-effort",
        choices=REASONING_EFFORTS,
        default=DEFAULT_REASONING_EFFORT,
        help="Responses API reasoning effort (default: %(default)s)",
    )
    parser.add_argument(
        "--retry-reasoning-effort",
        choices=REASONING_EFFORTS,
        default=DEFAULT_RETRY_REASONING_EFFORT,
        help="Reasoning effort for targeted retries (default: %(default)s)",
    )
    parser.add_argument("--test-label", default=DEFAULT_TEST_LABEL)
    parser.add_argument("--input-cost-per-million", type=float, default=DEFAULT_INPUT_COST_PER_MILLION)
    parser.add_argument("--output-cost-per-million", type=float, default=DEFAULT_OUTPUT_COST_PER_MILLION)
    parser.add_argument("--usd-to-sek", type=float, default=DEFAULT_USD_TO_SEK)
    args = parser.parse_args()
    if args.ordinary_comments == "audit" and args.mode != "audit":
        parser.error("--ordinary-comments audit requires --mode audit")
    os.environ["DOXYGEN_AI_TEST_LABEL"] = args.test_label.strip()
    manifest_path = Path(args.manifest).resolve()
    try:
        manifest_path.relative_to(REPO_ROOT.resolve())
    except ValueError as exc:
        raise RuntimeError(f"Manifest path is outside the repository: {manifest_path}") from exc

    if args.remaining_only:
        remaining_paths = load_remaining_files_from_manifest(manifest_path)
        selections = [
            SelectedFile(path.resolve(), "remaining", str(path.with_suffix("").resolve()))
            for path in remaining_paths
        ]
        print(f"Remaining-only mode enabled for {len(selections)} file(s)")
        explicit_files = ""
        base = os.getenv("DOXYGEN_AI_BASE_SHA", "")
        head = os.getenv("DOXYGEN_AI_HEAD_SHA", "")
    else:
        explicit_files = args.files.strip()
        if explicit_files:
            direct_files = parse_explicit_files(explicit_files)
            selections = expand_selections(direct_files)
            base = ""
            head = run_git(["rev-parse", "HEAD"])
            print(f"Manual file selection enabled for {len(selections)} file(s) after module expansion")
        else:
            base, head = resolve_diff_range(args.base, args.head)
            direct_files = changed_files(base, head)
            selections = expand_selections(direct_files)
            print(
                f"Automatic diff selection found {len(direct_files)} direct file(s) and "
                f"{len(selections) - len(direct_files)} paired file(s)"
            )

    os.environ["DOXYGEN_AI_BASE_SHA"] = base
    os.environ["DOXYGEN_AI_HEAD_SHA"] = head

    if not selections:
        if args.remaining_only:
            print("No rejected or deferred files found in the manifest")
        elif explicit_files:
            print("No valid explicit files were provided")
        else:
            print(f"No changed C/C++ files found in diff {base}..{head}")
        return 0

    limited_files, deferred_files = limit_pair_groups(selections, args.max_files)
    if len(selections) > len(limited_files):
        print(
            f"Pair-aware limit selected {len(limited_files)} file(s) and deferred "
            f"{len(deferred_files)} file(s); configured soft limit={args.max_files}"
        )

    for selected in limited_files:
        print(f"SELECTED {selected.selection}: {selected.path.relative_to(REPO_ROOT).as_posix()}")
    for selected in deferred_files:
        print(f"DEFERRED {selected.selection}: {selected.path.relative_to(REPO_ROOT).as_posix()}")

    if args.dry_run:
        return 0

    changed_count = process_files(
        limited_files,
        model=args.model,
        reasoning_effort=args.reasoning_effort,
        retry_reasoning_effort=args.retry_reasoning_effort,
        max_chars=args.max_chars,
        max_output_tokens=args.max_output_tokens,
        input_cost_per_million=args.input_cost_per_million,
        output_cost_per_million=args.output_cost_per_million,
        usd_to_sek=args.usd_to_sek,
        deferred_files=deferred_files,
        manifest_path=manifest_path,
        workflow_mode=args.mode,
        ordinary_comment_policy=args.ordinary_comments,
    )
    return 0 if changed_count >= 0 else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # noqa: BLE001
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)



