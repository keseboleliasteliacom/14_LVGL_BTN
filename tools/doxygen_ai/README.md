# Doxygen AI Workflow

This folder contains the OpenAI-powered Doxygen updater used by the GitHub Actions workflow for Glennergy-ESP.

## What It Does

- selects changed or manually chosen C/C++ files
- expands matching `.h` and `.c/.cpp` pairs automatically
- records whether each file was selected directly or through pair expansion
- loads the embedded Doxygen standard and target-style examples
- asks the OpenAI API to return the full updated file
- rejects output if non-comment code changed
- writes a GitHub Actions summary with token usage and estimated cost
- uploads rejected outputs and run manifests as workflow artifacts
- creates or updates a draft PR branch with successful documentation changes
- provides routine `normal` and opt-in cross-module `audit` modes

## Current Behavior

- GitHub workflow UI is defined from the default branch, currently `main`
- workflow runs can still check out and process `dev`
- default everyday model is `gpt-5.4-mini`
- manual file selection supports comma-separated paths
- selecting `Module.h` also processes the matching `Module.c` or `Module.cpp`, and vice versa
- automatic diff selection uses the same pair expansion as manual selection
- file limits are soft and never split a discovered header/source pair
- normal mode preserves ordinary comments exactly
- audit mode adds bounded caller context and a second semantic review
- a run can continue processing files even if one later file is rejected
- reruns can target only previously rejected or deferred files

## Manual Workflow Inputs

Use the `Run workflow` form in GitHub Actions.

| Input | Purpose |
| --- | --- |
| `target_ref` | Branch to check out and process, usually `dev` |
| `model` | OpenAI model to use for the run |
| `reasoning_effort` | Primary request reasoning: `none`, `low`, `medium`, `high`, or `xhigh`; default `low` |
| `retry_reasoning_effort` | Targeted retry reasoning using the same values; default `medium` |
| `test_label` | Optional label for comparison runs and separate PR branches |
| `rerun_mode` | `all` for a normal run, `remaining_only` for rejected/deferred files from an earlier run |
| `file_paths` | Comma-separated file paths for `all` mode; leave empty for `remaining_only` |
| `mode` | `normal` for routine generation; `audit` for caller context and semantic review |
| `ordinary_comments` | `preserve` by default; `audit` only for an intentional ordinary-comment cleanup |

## Recommended Manual Flow

### Normal successful run

1. Open `Actions -> Doxygen AI -> Run workflow`
2. Set:
   - `target_ref = dev`
   - `model = gpt-5.4-mini` or another allowed model
   - `rerun_mode = all`
   - `file_paths = main/WiFi.h, main/WiFi.c, main/HTTP.h, main/HTTP.c`
3. Start the run
4. Review:
   - the `Summary` tab
   - any draft PR that was created or updated
   - any rejected-output artifact if the run failed

### Partial failure, then rerun only remaining files

Example:
- first run processes several files
- some files succeed
- one or more files are rejected or deferred

Then:
1. Fix the prompt/standard/workflow if needed
2. Push the fix
3. Start a new manual run
4. Set:
   - `target_ref = dev`
   - same `model` as before unless you intentionally want to compare models
   - same `test_label` if you used one before
   - `rerun_mode = remaining_only`
   - leave `file_paths` empty
5. Review whether the rerun restored the already successful files and processed only the remaining files

Important:
- use the workflow's manual `remaining_only` mode for this
- do not use GitHub's built-in `Re-run failed jobs` button for this purpose
- GitHub's rerun button repeats the old job; it does not use the workflow's custom rerun logic

## Modes

`normal` is the automatic `dev` workflow mode. It uses target and paired-file evidence, deterministic code and ordinary-comment guards, coverage checks, and churn reporting. It performs one generation request per selected file, excluding targeted retries.

`audit` is opt-in. It also discovers bounded exact-identifier caller snippets and reviews the proposed documentation with a second model request. Clear semantic contradictions reject the file; ambiguity is reported as a warning.

Ordinary-comment cleanup is independent of semantic audit depth. Keep `ordinary_comments=preserve` unless the run is specifically intended to audit TODOs, developer notes, or disabled code.

## Local validation

Resolve selection without an API call:

```text
python tools/doxygen_ai/update_docs.py --base <sha> --head <sha> --dry-run
```

Run deterministic regressions:

```text
python -m unittest discover -s tools/doxygen_ai/tests -v
```
