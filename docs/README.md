# Glennergy-ESP documentation

| Metadata | Value |
| --- | --- |
| Status | Current navigation index; migration from older documents is in progress |
| Audience | Newcomers, developers, maintainers, operators, and evaluators |
| Applies to | Authoritative `dev` documentation campaign |
| Last verified | Glennergy-ESP `b5a502a`; Glennergy `42798be` |

Start here to find the current source for a topic. Some older documents remain
available as evidence or planning history while they are consolidated. A file
being present in the repository does not by itself make it current.

## Start here

- [System context](system-context.md) — how Glennergy and Glennergy-ESP form one
  system, where their boundaries are, and what is current versus planned.
- [Current limitations](current-limitations.md) — temporary, partial, unresolved
  and planned behavior that should not be mistaken for completion.
- [Shared glossary](glossary.md) — canonical project terms, identifiers, units
  and deliberately unresolved meanings.
- [Cross-project interface contract](interface-contract.md) — exact current
  HTTP routes, schemas, errors, compatibility limitations and planned direction.

## Firmware guides

- [Firmware architecture](architecture.md) — startup, tasks, queues, shared
  state, data ownership and recovery boundaries.
- [Development guide](development.md) — ESP-IDF setup, build, validation and
  authorized flash/monitor boundaries.
- [Hardware guide](hardware.md) — source-derived board, display/touch, I2C and
  BME280 configuration with unverified hardware behavior labeled.
- `configuration.md` — planned canonical NVS, Wi-Fi and interval reference.
- [UART command reference](UART_COMMANDS.md) — existing reference; persistence
  and command details are being reconciled with current code.
- `connectivity.md` — planned canonical Wi-Fi, LEOP, cache and health-state guide.
- `ui-guide.md` — planned screen, status and Settings-field guide.
- `troubleshooting.md` — planned read-only-first diagnostic guide.

Paths shown without links are approved campaign artifacts that have not yet
been accepted. Older files should not be used as substitutes for missing
canonical guides without checking current code.

## Server-side architecture

The detailed five-process, IPC, scheduling and server-recovery architecture is
maintained in Glennergy `Docs/architecture.md`. During branch review, open that
file in the sibling Glennergy checkout on
`docs/documentation-campaign-2026-07-26`. A stable GitHub link will replace this
branch-review instruction when the Glennergy documentation is published.

## Existing documents under review

| Document | Current classification | Intended action |
| --- | --- | --- |
| [API_ENDPOINTS.md](API_ENDPOINTS.md) | Detailed but stale API draft; contains old error, queue, timing and deployment-address assumptions | Replace with the canonical interface contract; retain/banner or archive after review |
| [easy_api_doc.md](easy_api_doc.md) | Swedish duplicate overview with stale navigation/encoding | Redirect/banner or archive after unique information is checked |
| [UART_COMMANDS.md](UART_COMMANDS.md) | Useful reference with stale persistence and command details | Update in place from current parser/NVS evidence |
| [ESP-IDF build workflow plan](ESP_IDF_BUILD_WORKFLOW_PLAN.md) | Roadmap containing planned CI/test capabilities | Keep clearly labelled as a plan; move implemented workflow into `development.md` |
| Presentation assets | Rendered architecture/pin/queue/LEOP snapshots | Preserve as presentation snapshots until canonical Mermaid diagrams replace or validate them |

No document in this table is deleted merely because a replacement is planned.

## Decisions, plans, and history

Project planning files and architecture roadmaps exist at the repository root
and in local working copies. They are valuable historical or owner-intent
evidence, but they may describe older module ownership, fake producers or
proposed behavior. The campaign’s
[document disposition register](documentation_campaign/DOCUMENT_DISPOSITION.md)
tracks whether each should be retained, updated, archived or redirected.

## API reference and contributor tooling

- [Doxygen standard](Doxygen_Standard.md) — source-comment conventions.
- [Doxygen workflow guide](Doxygen_Workflow_Guide.md) — automated/manual source
  documentation workflow.
- [Doxygen TODO](Doxygen_TODO.md) — tooling backlog, not product behavior.
- `template_C.c`, `template_H.h`, and `template_Main.c` — comment templates.
- Generated Doxygen output is derived from source comments and is not the
  canonical narrative architecture.

Narrative documentation and automated Doxygen updates must not edit the same
C/C++ comments concurrently. Automated output still requires code review.

## Documentation campaign records

The [campaign workspace](documentation_campaign/README.md) contains the
baseline, snapshots, evidence, claim ledger, coverage matrix, cleanup candidates,
validation gates and audit records. These governance files support accuracy and
review; they are not the primary onboarding path.

## Status labels used here

- **Current:** verified against the recorded authoritative source snapshot.
- **Partial:** working behavior exists but is incomplete.
- **Temporary:** implemented for testing or transition, not final design.
- **Planned:** accepted direction that is not implemented.
- **Unknown:** evidence or intent is insufficient; documentation must not guess.
- **Historical/cleanup candidate:** useful context or suspected obsolete material
  requiring separate review before archival or deletion.

## Maintenance

Update this index whenever a canonical guide is added, replaced, archived or
renamed. Before publication, every canonical document must be reachable from
this page or the root README, and every older competing document must have an
explicit disposition.
