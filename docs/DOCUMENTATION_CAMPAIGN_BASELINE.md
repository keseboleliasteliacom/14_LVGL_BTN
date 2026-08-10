# Glennergy Documentation Campaign Baseline

Status: Approved campaign baseline; Phase 0 active  
Created: 2026-07-26  
Scope: `glennergy` and `Glennergy-ESP`

## Purpose

This document defines the agreed starting point for a coordinated documentation
campaign across the Glennergy server and ESP32 firmware repositories. It exists
to prevent scope drift, inconsistent terminology, unsupported architectural
claims, and large documentation rewrites that are difficult to review or undo.

No existing documentation should be treated as replaced merely because this
plan exists. Each later change must be reviewed and accepted independently.

## Repository Baseline

At the time this plan was created:

| Repository | Branch | Commit |
| --- | --- | --- |
| `Glennergy-ESP` | `dev` | `f2974fb64af25495f79ba412c2626c9b77688ee8` |
| `glennergy` | `dev` | `42798bee227fcd621cbcb0b37c2b5da771210086` |

The `Glennergy-ESP` working tree already contained modified and untracked files.
Those changes predate this campaign and must not be discarded, overwritten, or
assumed to be part of the documentation work. The `glennergy` working tree was
clean when inspected.

The commit IDs above identify the tracked-code reference points, but they do not
capture uncommitted files. Before implementation begins, the project owner
should make an intentional checkpoint commit of the existing work, or otherwise
back it up, and then create a dedicated documentation branch in each repository.

Suggested branch name:

```text
docs/documentation-campaign
```

Creating branches or commits is deliberately not part of this baseline step.

## Active Campaign Snapshot

Remote references were fetched and reconciled on 2026-07-26 before campaign
branches were created.

| Repository | Current implementation (`dev`) | Stable production (`origin/main`) | Campaign branch |
| --- | --- | --- | --- |
| `Glennergy-ESP` | `b5a502afd9ca2ae374b3131b0031b8390f93b348` | `daf35c538d84586576f8286c2d543eb1c3c89e6a` | `docs/documentation-campaign-2026-07-26` |
| `glennergy` | `42798bee227fcd621cbcb0b37c2b5da771210086` | `61761b5eda30bee417a0b6e33e10fb061e18db26` | `docs/documentation-campaign-2026-07-26` |

`dev` is authoritative for current implementation and documentation discovery.
`main` represents stable production. The two lines may intentionally diverge;
the campaign must not merge them merely to make their histories appear aligned.
Discoveries and drafts cite the active `dev` snapshot. Later movement of either
branch is recorded in the snapshot/drift log before being incorporated.

Local `main` was stale in both repositories at reconciliation time. Remote
tracking `origin/main`, rather than the stale local branch, records the stable
production snapshot above.

## Objectives

1. Accurately describe the implemented behavior of both projects.
2. Explain how the server and ESP32 firmware interact as one system.
3. Make setup, development, operation, troubleshooting, and onboarding easier.
4. Produce maintainable diagrams whose sources live beside the documentation.
5. Establish consistent terminology and cross-links across both repositories.
6. Record uncertainty and planned behavior without presenting either as fact.
7. Leave behind a repeatable process for keeping documentation current.
8. Reach roughly 90% of the intended final documentation for code the owner
   estimates is roughly 95% complete.

Campaign completion does not mean the product backlog is complete. Success means
the current implementation is accurately documented, remaining limitations and
temporary behavior are visible, and the structure can accept unfinished
features later without a full rewrite.

## Non-Goals

Unless separately approved, this campaign will not:

- change runtime behavior or application architecture;
- refactor source code merely to make it easier to document;
- change production configuration or deployment state;
- replace generated third-party or vendor documentation;
- claim that planned features are already implemented;
- delete older plans before their historical value has been reviewed;
- modify, commit on, push to, merge into, rebase, reset, or otherwise change a
  `dev` branch without explicit owner approval;
- force-push, destructively reset, delete, merge, deploy, or mutate production.

The owner has authorized fetching remote references, creating local
documentation branches, making documentation checkpoint commits, pushing those
documentation branches, and opening documentation pull requests. Creation of a
documentation branch from `dev` does not modify `dev`. Direct action on `dev`
still requires a separate approval.

Source defects discovered during documentation work will be recorded separately.
They will not be silently fixed as part of a documentation edit.

## Working Principles

### Evidence before prose

Every material technical claim must be supported by at least one concrete source:

- source file and symbol;
- build or deployment script;
- configuration declaration and use;
- test or safely executed command;
- producer and consumer definitions for an interface;
- explicit existing design decision, clearly labelled as design intent.

Each finding should be classified as:

- **Confirmed implemented**: directly supported by current code or a verified command.
- **Partially implemented**: working behavior exists but is incomplete.
- **Temporary/example behavior**: used now but not intended as the final design.
- **Planned**: desired future behavior rather than current behavior.
- **Inferred**: strongly suggested but not completely established.
- **Cleanup candidate/deprecated**: possibly obsolete and awaiting safe verification.
- **Unknown**: insufficient evidence; requires owner input or further work.

Owner-reported intent and hardware observations are valuable evidence, but they
must be labelled as owner evidence until verified independently where possible.

### Separate discovery from editing

The first phase is read-only. Documentation should not be broadly rewritten
until the inventory, architecture maps, terminology, conflicts, and gaps have
been reviewed.

### One integrator, independent reviewers

Focused investigators may examine separate parts of the system in parallel.
One lead integrates the results so the final documents use the same terminology
and do not contradict each other. A fresh reviewer then checks the integrated
result against the repositories.

Agents should not concurrently edit the same document. Parallel agent work is
primarily for read-only discovery and independent review unless file ownership
has been assigned explicitly.

### Prefer diagrams as code

Use Mermaid as the default diagram format because it is reviewable as text and
renders in common Markdown tooling. Retain editable sources for any SVG or PNG
exports. Use another format only when Mermaid cannot express the required result
clearly.

### Layer documentation by audience

Documentation is written in English. READMEs should remain approachable for new
developers, project evaluators, team members, and operators: explain purpose,
project relationships, prerequisites, quick start, basic configuration, common
operations, troubleshooting entry points, and where to find more detail.

Detailed documents serve embedded developers, server developers, and maintainers
with exact interfaces, payloads, units, errors, retries, task and queue ownership,
configuration, deployment, and failure/recovery behavior. Generated Doxygen is
the implementation-level symbol reference and should be linked rather than
copied wholesale into narrative Markdown.

### Protect secrets, production, and hardware

Secret locations and mechanisms may be documented. Secret values must never be
retrieved, used, printed, copied, committed, diagrammed, or placed in examples.
This includes GitHub Actions secrets such as `OPENAI_API_KEY`, SSH private keys,
Wi-Fi credentials, API or registration tokens, TLS private keys, `.env` values,
credential contents in NVS/SPIFFS, production credentials, sensitive logs,
property/customer data, and production JSON contents. If a committed credential
is discovered, report its location without reproducing its value.

Use placeholders such as `https://leop.example.com` and `LEOP_BASE_URL` in public
documentation by default. Do not publish production IPs, internal hostnames, SSH
endpoints, usernames, management ports, or administrative paths unless the owner
explicitly decides they are operationally necessary. Security must not rely on
address secrecy.

Production VPS access, service changes, deployments, production logs or
configuration access, live API mutation, uninstall/purge, flashing or erasing an
ESP32, changing NVS/SPIFFS/device Wi-Fi, opening a real device serial port,
sending live commands, fault injection, and hardware soak testing all require
explicit approval.

### Coordinate Doxygen automation

Narrative architecture documents and Doxygen are separate but cross-checked
workstreams. Do not allow campaign work and automated Doxygen runs to edit the
same C/C++ comments concurrently. Automated comments are not architectural truth
until reviewed against code. The automatic Doxygen workflow currently responds
to `dev`, not ordinary narrative documentation branches; workflow triggers must
still be reviewed before each push.

## Planned Deliverables

The final set will be confirmed after discovery. The initial target is:

| Artifact | Primary source of truth | Intended location |
| --- | --- | --- |
| Documentation inventory and coverage matrix | Both repositories | ESP campaign workspace initially |
| Shared glossary | Both repositories | Cross-linked from both projects |
| System context and deployment overview | Both repositories | Both projects or shared canonical copy |
| Improved server README | Server source, build and deployment files | `glennergy/README.md` |
| Improved firmware README | ESP-IDF source and board configuration | `Glennergy-ESP/README.md` |
| Server component architecture | Processes, services and IPC | `glennergy` documentation |
| Firmware component/task architecture | Tasks, queues, state and drivers | ESP documentation |
| Cross-project interface contract | Producers, consumers and payloads | Canonical location chosen in discovery |
| Setup and development guides | Build scripts and verified commands | Relevant repository |
| Configuration references | Declarations, defaults and persistence | Relevant repository |
| Runtime sequence diagrams | Traced end-to-end paths | Relevant architecture documents |
| Troubleshooting and operational guides | Error paths, logs and recovery tools | Relevant repository |
| Accuracy and consistency review report | Completed documentation and code | Campaign workspace |
| Current limitations and planned completion work | Owner notes and verified code | Campaign workspace |
| Owner-notes register | Owner-provided context | Campaign workspace |
| Cleanup-candidate register | Static and reference analysis | Campaign workspace |
| Snapshot/drift log | Git references and campaign decisions | Campaign workspace |

Possible sequence diagrams include startup, sensor-to-display flow,
ESP-to-server publication, server-to-ESP commands, connection loss and recovery,
configuration persistence, and error reporting. Only flows confirmed during
discovery should be documented as implemented.

## Campaign Phases

### Phase 0: Checkpoint and campaign setup

1. Review and approve this baseline.
2. Preserve all pre-existing uncommitted work.
3. Create intentional checkpoint commits or backups.
4. Create a dedicated documentation branch in each repository.
5. Decide where cross-project canonical documents will live.
6. Add a campaign log, evidence register, glossary, and coverage matrix.

Exit criteria:

- both starting states are recoverable;
- campaign-owned files are distinguishable from earlier work;
- the documentation lead and review process are agreed.

### Phase 1: Read-only discovery

1. Inventory first-party source, configuration, scripts, tests, and documents.
2. Identify generated, vendored, obsolete, historical, and authoritative files.
3. Map the high-level server architecture.
4. Map firmware startup, FreeRTOS tasks, queues, state, UI, and hardware modules.
5. Inventory HTTP, MQTT, UART, WebSocket, file, or other interfaces actually used.
6. Trace configuration, secrets, defaults, and persistent state.
7. Trace build, test, installation, deployment, and operational workflows.
8. Compare every cross-project producer with its consumer.
9. Draft the shared glossary.
10. Record contradictions, ambiguity, missing evidence, and suspected stale docs.

Exit criteria:

- a reviewed inventory and coverage matrix exist;
- major runtime components and interfaces have evidence;
- confirmed, inferred, planned, and unknown behavior are separated;
- no broad documentation rewrite has occurred.

### Phase 2: Documentation design

1. Choose the canonical location and owner for every artifact.
2. Define README structures and navigation between documents.
3. Approve terminology and diagram conventions.
4. Prioritize artifacts by user value and confidence of evidence.
5. Define acceptance criteria for each artifact.

Exit criteria:

- every proposed document has a purpose, audience, owner, evidence source, and
  validation method;
- duplicate sources of truth have been avoided or explicitly justified.

### Phase 3: Drafting and integration

Suggested order:

1. Shared glossary and system context.
2. Cross-project communication contract, separating current behavior from
   planned registration and endpoint correction.
3. Server architecture and README.
4. Firmware architecture and README.
5. Build, setup, configuration, deployment, and operation guides.
6. Component, deployment, flow, state, and sequence diagrams.
7. Troubleshooting and known-limitations documentation.

Each artifact follows this loop:

```text
Investigate -> draft -> evidence review -> render or command validation -> integrate
```

Changes should be small enough to review independently. An artifact should not
be marked complete while it contains unlabelled assumptions.

### Phase 4: Independent verification

1. Recheck factual claims against current code.
2. Verify cross-project names, paths, ports, payloads, units, and defaults.
3. Render every diagram and inspect it for correctness and readability.
4. Run documented build or verification commands when safe and practical.
5. Check internal links and navigation.
6. Review from the perspective of a new developer.
7. Produce a final coverage report with remaining unknowns and deferred work.

Exit criteria:

- no known contradiction remains between documents or projects;
- unverifiable statements are removed or labelled;
- commands are verified or explicitly marked unverified;
- diagrams render from committed source;
- remaining gaps are visible rather than silently omitted.

### Phase 5: Maintenance handoff

1. Define which documents must change with particular code areas.
2. Add repository guidance for documentation conventions and validation commands.
3. Consider automated link, Mermaid, spelling, and Doxygen checks.
4. Decide whether the proven workflow should become a reusable Codex skill.
5. Schedule occasional documentation drift reviews if useful.

## Suggested Investigation Roles

The documentation lead coordinates the work and owns the shared vocabulary.
Focused read-only roles may include:

1. **Server investigator**: processes, APIs, configuration, build, deployment,
   service management, storage, and failure behavior.
2. **Firmware investigator**: startup, tasks, queues, shared state, UI, network,
   sensors, persistence, board interfaces, and failure behavior.
3. **Interface investigator**: cross-project endpoints, messages, schemas,
   timing, retries, compatibility, and ownership.
4. **Documentation auditor**: existing-document accuracy, contradictions,
   missing coverage, terminology, navigation, and newcomer usability.
5. **Final verifier**: independently checks integrated documents against code.

The lead should resolve conflicts by returning to evidence, not by choosing the
most polished explanation.

## Required Finding Format

Investigators should return findings in a consistent form:

```text
Claim:
<short technical statement>

Classification:
Confirmed | Inferred | Planned | Unknown

Evidence:
- <repository/path and symbol, setting, or verified command>

Documentation impact:
<artifact to add, update, or question>

Open questions:
<anything that requires more evidence or owner input>
```

## Quality Gates

An artifact is ready only when applicable checks pass:

- **Accuracy**: statements correspond to implementation or are clearly labelled.
- **Traceability**: important statements can be traced back to evidence.
- **Completeness**: happy paths, failure behavior, dependencies, and limitations
  are covered where relevant.
- **Consistency**: names and concepts agree within and across repositories.
- **Audience fit**: prerequisites are not assumed without explanation.
- **Reproducibility**: commands contain required working directory and context.
- **Maintainability**: canonical ownership is clear and duplication is limited.
- **Diagram validity**: sources render and match the described behavior.
- **Safety**: examples do not expose secrets or encourage unsafe production use.
- **Reviewability**: changes are scoped and do not mix unrelated code changes.

## Change and Decision Log

Material changes to this plan should be recorded here rather than silently
rewriting the baseline.

| Date | Decision | Reason | Approved by |
| --- | --- | --- | --- |
| 2026-07-26 | Create a discovery-first, cross-repository campaign baseline | Establish recoverable scope and quality controls before editing | Pending project-owner review |
| 2026-07-26 | Approve `dev` as current authority and `main` as stable production | Keep current and deployed behavior distinguishable | Project owner |
| 2026-07-26 | Authorize fetches, documentation branches, checkpoint commits, documentation pushes and PRs | Enable a recoverable campaign workflow | Project owner |
| 2026-07-26 | Require approval for direct action on `dev` | Protect authoritative development history | Project owner |
| 2026-07-26 | Target roughly 90% final documentation for roughly 95% complete code | Document useful current truth without waiting for backlog completion | Project owner |
| 2026-07-26 | Use English and layered documentation for mixed audiences | Keep entry points accessible while retaining technical depth | Project owner |
| 2026-07-26 | Leave recommendation-result semantics unresolved | Current algorithm behavior appears inconsistent; documentation must describe evidence without inventing intent | Project owner |
| 2026-07-26 | Treat five properties as a temporary test limit | Avoid presenting an implementation constraint as the final product capacity | Project owner |
| 2026-07-26 | Plan a UUID-like identity unique to each ESP32-S3 unit | Replace temporary unauthenticated incrementing integers with a device-linked identity direction; exact design remains open | Project owner |
| 2026-07-26 | Keep canonical cross-project documentation in Glennergy-ESP | Give shared system/interface material one repository owner | Project owner |
| 2026-07-26 | Omit the real VPS address from public documentation | Use placeholders now; publication can be reconsidered later | Project owner |

## Pause and Rollback Rules

Pause the campaign when:

- code and existing documentation provide conflicting sources of truth;
- a change would alter runtime behavior;
- generated or vendored files may be overwritten;
- agents produce incompatible interpretations of a shared interface;
- an operation would require production access, secrets, deployment, deletion,
  or another expansion of authority;
- the scope grows beyond the approved deliverable list.

To roll back campaign work safely:

1. Stop active edits and record the reason.
2. Identify campaign commits and files; do not include pre-existing dirty work.
3. Prefer reverting reviewed campaign commits or restoring individual campaign
   files from the checkpoint branch rather than resetting an entire repository.
4. Re-run `git status` in both repositories and confirm unrelated changes remain.
5. Update the decision log before resuming with a revised plan.

Destructive Git commands are not part of the normal rollback procedure.

## Approval Point

Approval of this file authorizes Phase 0 setup, Phase 1 read-only discovery,
documentation-branch creation, documentation checkpoint commits, documentation
branch pushes, and documentation pull requests. It does not authorize broad
narrative-document drafting before the discovery/design gates, source-code
changes, direct action on `dev`, deployments, production or hardware actions,
force-pushes, destructive history changes, or deletion of existing material.

After discovery, the project owner should review:

- the inventory and coverage matrix;
- confirmed architecture findings;
- conflicting or stale documentation;
- the proposed canonical document structure;
- the prioritized drafting backlog;
- all questions that cannot be resolved from repository evidence.

Only then should Phase 2 and later editing begin.
