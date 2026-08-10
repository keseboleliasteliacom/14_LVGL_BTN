# Embedded Doxygen Workflow Guide

This document explains the GitHub setup and the intended workflow for the OpenAI-powered Doxygen updater in Glennergy-ESP.

## Main Files

- GitHub Actions workflow: `.github/workflows/doxygen-openai.yml`
- Update script: `tools/doxygen_ai/update_docs.py`
- Workflow usage guide: `tools/doxygen_ai/README.md`
- Active documentation standard: `docs/Doxygen_Standard.md`
- Audit-mode behavior: `docs/Doxygen_Audit_Mode.md`
- Future follow-ups and open questions: `docs/Doxygen_TODO.md`
- AI context used by the updater: `AI_CONTEXT.md`

## First-Time Setup

1. In GitHub, open `Settings -> Secrets and variables -> Actions`
2. Add the repository secret:
   - `OPENAI_API_KEY`
3. Make sure the workflow file in `main` is up to date if you expect new manual-run inputs to appear in the GitHub UI
4. Push or manually run the workflow against `dev`

## Current Safety Controls

- automatic runs are restricted to `dev`
- manual runs support explicit file selection
- only C/C++ source and header files are processed
- matching `.h` and `.c/.cpp` files are paired automatically in both diff and manual selection
- pair-aware soft limits never split a discovered pair
- automatic runs use normal mode; manual runs may select semantic audit mode
- ordinary comments are preserved unless their separate audit policy is explicitly selected
- generated output is rejected if non-comment code changed
- rejected outputs and run manifests are saved as artifacts
- successful results can be restored into a rerun using `remaining_only`

## How The Workflow Runs

1. GitHub shows the workflow UI from the default branch
2. The run checks out the selected `target_ref`, usually `dev`
3. The script selects changed files or manually specified files
4. Matching header/source pairs are added automatically when names match, including unchanged partners in diff mode
5. The script loads the active embedded documentation standard and templates
6. The selected model generates full-file documentation updates
7. Output is rejected if non-comment code changed
8. Successful files are written to the workflow branch and summarized in GitHub Actions
9. Rejected outputs and manifests are uploaded as artifacts for troubleshooting or reruns

In audit mode, bounded caller snippets are added as read-only evidence and a second request reviews changed documentation for cross-module semantic contradictions.

For pre-merge validation, a manually dispatched workflow may target the exact branch from which it was dispatched. Other unrelated target refs remain rejected.

## Manual Run Examples

### Normal run

Use:

- `target_ref = dev`
- `model = gpt-5.4-mini` or another allowed model
- `rerun_mode = all`
- `file_paths = main/WiFi.h, main/WiFi.c, main/HTTP.h, main/HTTP.c`
- `mode = normal` for routine work or `audit` for cross-module review
- `ordinary_comments = preserve` unless this is explicitly a comment-cleanup run

### Rerun only remaining files

Use this after an earlier run where one or more files were rejected or deferred.

Use:

- same `target_ref`
- same `model` unless you intentionally want to compare models
- same `test_label` if you used one earlier
- `rerun_mode = remaining_only`
- leave `file_paths` empty

Do not use GitHub's built-in `Re-run failed jobs` button for this purpose. That repeats the old job and does not trigger the workflow's custom remaining-only logic.

## Recommended Usage

- use `tools/doxygen_ai/README.md` for day-to-day workflow usage
- use `docs/Doxygen_Standard.md` as the source of truth for documentation style
- use `docs/Doxygen_TODO.md` for active concerns and future improvements

## Budget Notes

- the default budget-oriented model is `gpt-5.4-mini`
- stronger models can be selected manually for comparison or quality-sensitive runs
- current workflow cost summaries are estimate-only and do not yet auto-switch pricing by selected model
- audit mode usually costs more because changed proposals receive a second semantic-review request
