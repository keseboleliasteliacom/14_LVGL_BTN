"""Deterministic selection, context, and comment-safety policy helpers."""

from __future__ import annotations

import re
from dataclasses import dataclass
from difflib import SequenceMatcher
from pathlib import Path
from typing import Iterable

HEADER_SUFFIXES = (".h", ".hpp", ".hh")
SOURCE_SUFFIXES = (".c", ".cpp", ".cc")
FUNCTION_NAME_RE = re.compile(
    r"^[ \t]*(?:(?:static|inline|extern|const|volatile)\s+)*"
    r"(?:[A-Za-z_][A-Za-z0-9_:<>]*[ \t*]+)+"
    r"([A-Za-z_][A-Za-z0-9_]*)[ \t]*\(",
    re.MULTILINE,
)


@dataclass(frozen=True)
class SelectedFile:
    path: Path
    selection: str
    pair_key: str


@dataclass(frozen=True)
class ChurnReport:
    changed_lines: int
    original_comment_lines: int
    ratio: float


def find_pair(path: Path) -> Path | None:
    suffixes = SOURCE_SUFFIXES if path.suffix.lower() in HEADER_SUFFIXES else HEADER_SUFFIXES
    for suffix in suffixes:
        candidate = path.with_suffix(suffix)
        if candidate.is_file():
            return candidate.resolve()
    return None


def expand_selections(paths: Iterable[Path]) -> list[SelectedFile]:
    """Expand same-stem pairs while retaining deterministic direct/pair provenance."""
    direct = [path.resolve() for path in paths]
    direct_set = set(direct)
    result: list[SelectedFile] = []
    seen: set[Path] = set()

    for path in direct:
        pair = find_pair(path)
        pair_key = str(path.with_suffix("").resolve())
        if path not in seen:
            result.append(SelectedFile(path, "direct", pair_key))
            seen.add(path)
        if pair is not None and pair not in seen:
            selection = "direct" if pair in direct_set else "paired"
            result.append(SelectedFile(pair, selection, pair_key))
            seen.add(pair)
    return result


def limit_pair_groups(
    selections: list[SelectedFile], max_files: int
) -> tuple[list[SelectedFile], list[SelectedFile]]:
    """Limit direct selections while allowing their pairs beyond the soft cap."""
    if max_files < 1:
        return [], selections[:]

    groups: list[list[SelectedFile]] = []
    indexes: dict[str, int] = {}
    for item in selections:
        if item.pair_key not in indexes:
            indexes[item.pair_key] = len(groups)
            groups.append([])
        groups[indexes[item.pair_key]].append(item)

    accepted: list[SelectedFile] = []
    deferred: list[SelectedFile] = []
    accepted_direct = 0
    for group in groups:
        direct_count = sum(item.selection != "paired" for item in group)
        if not accepted or accepted_direct + direct_count <= max_files:
            accepted.extend(group)
            accepted_direct += direct_count
        else:
            deferred.extend(group)
    return accepted, deferred


def _comment_blocks(text: str) -> list[tuple[str, bool]]:
    """Return comment text and whether each block is Doxygen."""
    blocks: list[tuple[str, bool]] = []
    i = 0
    while i < len(text):
        if text.startswith("//", i):
            end = text.find("\n", i)
            end = len(text) if end < 0 else end
            block = text[i:end]
            blocks.append((block, block.startswith("///") or block.startswith("//!")))
            i = end
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = len(text) if end < 0 else end + 2
            block = text[i:end]
            blocks.append((block, block.startswith("/**") or block.startswith("/*!")))
            i = end
        elif text[i] in {'"', "'"}:
            quote = text[i]
            i += 1
            while i < len(text):
                if text[i] == "\\":
                    i += 2
                elif text[i] == quote:
                    i += 1
                    break
                else:
                    i += 1
        else:
            i += 1
    return blocks


def ordinary_comments(text: str) -> list[str]:
    return [block for block, is_doxygen in _comment_blocks(text) if not is_doxygen]


def ordinary_comments_changed(original: str, updated: str) -> bool:
    return ordinary_comments(original) != ordinary_comments(updated)


def doxygen_comments(text: str) -> list[str]:
    return [block for block, is_doxygen in _comment_blocks(text) if is_doxygen]


def documentation_churn(original: str, updated: str) -> ChurnReport:
    before = "\n".join(doxygen_comments(original)).splitlines()
    after = "\n".join(doxygen_comments(updated)).splitlines()
    matcher = SequenceMatcher(a=before, b=after, autojunk=False)
    unchanged = sum(block.size for block in matcher.get_matching_blocks())
    changed = (len(before) - unchanged) + (len(after) - unchanged)
    denominator = max(1, len(before))
    return ChurnReport(changed, len(before), changed / denominator)


def function_names(text: str) -> list[str]:
    ignored = {"if", "for", "while", "switch", "return", "sizeof"}
    names: list[str] = []
    for match in FUNCTION_NAME_RE.finditer(text):
        line_start = text.rfind("\n", 0, match.start()) + 1
        line_end = text.find("\n", match.start())
        line_end = len(text) if line_end < 0 else line_end
        if text[line_start:line_end].lstrip().startswith("typedef"):
            continue
        name = match.group(1)
        if name not in ignored and name not in names:
            names.append(name)
    return names


def caller_context(
    target: Path,
    target_text: str,
    candidates: Iterable[Path],
    repo_root: Path,
    max_snippets: int = 8,
    max_chars: int = 6000,
    radius: int = 2,
) -> str:
    """Collect bounded exact-identifier caller snippets as read-only evidence."""
    names = function_names(target_text)
    if not names:
        return "No callable identifiers were found for caller-context discovery."
    pattern = re.compile(r"\b(?:" + "|".join(map(re.escape, names)) + r")\b")
    snippets: list[str] = []
    used = 0
    target = target.resolve()
    for path in sorted({candidate.resolve() for candidate in candidates}, key=str):
        if path == target or not path.is_file():
            continue
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except (OSError, UnicodeError):
            continue
        for index, line in enumerate(lines):
            if not pattern.search(line):
                continue
            start = max(0, index - radius)
            end = min(len(lines), index + radius + 1)
            rel = path.relative_to(repo_root).as_posix()
            body = "\n".join(f"{number + 1}: {lines[number]}" for number in range(start, end))
            snippet = f"Caller candidate: {rel}:{index + 1}\n{body}"
            if snippets and used + len(snippet) > max_chars:
                return "\n\n".join(snippets)
            snippets.append(snippet)
            used += len(snippet)
            if len(snippets) >= max_snippets:
                return "\n\n".join(snippets)
    return "\n\n".join(snippets) or "No external caller references were found."
