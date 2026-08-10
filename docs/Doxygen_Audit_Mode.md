# Doxygen Semantic Audit Mode

Audit mode is an opt-in extension of the normal Doxygen updater. It is intended for release checks, documentation campaigns, and modules whose contract depends on callers or state transitions outside the target/header pair.

## Additional Evidence

For callable identifiers in the target file, the updater searches tracked C/C++ files and includes a bounded set of exact-identifier snippets. Paths and line numbers are included. The context is read-only and prioritizes evidence such as task creation, initialization, callbacks, and direct UI update calls.

Context is deliberately capped. Audit mode does not claim whole-program static analysis, indirect-call resolution, or proof of concurrency behavior.

## Semantic Review

After deterministic code and ordinary-comment checks pass, a second Responses API request compares the original and proposed target with paired and caller evidence. It returns:

- `pass`: no material contradiction found
- `warning`: ambiguous evidence or likely unnecessary churn requiring human attention
- `reject`: a clear contradiction with supplied evidence, or a failed/unparseable review

The reviewer focuses on task arguments, active versus legacy/test status, connectivity meaning, ownership, units, blocking behavior, state transitions, and header/source/caller agreement.

## Ordinary Comments

Semantic audit mode still preserves ordinary comments by default. Ordinary-comment cleanup requires the independent `ordinary_comments=audit` choice. This prevents a Doxygen accuracy review from silently deleting TODOs, developer notes, or disabled code.

## Cost Control

Deterministic tests, selection, pairing, code guards, caller discovery, and churn reporting do not use model tokens. Caller context increases generation input size, and the semantic review adds a second request for each eligible proposal. Use small representative batches before broader campaigns.
