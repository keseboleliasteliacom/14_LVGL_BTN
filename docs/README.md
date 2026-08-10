# Glennergy-ESP documentation

Start here to find the current source for a topic. Some older documents remain
available as evidence or planning history while they are consolidated. A file
being present in the repository does not by itself make it current.

## Choose your path

| You are looking forâ€¦ | Start with | Then continue to |
| --- | --- | --- |
| A simple guide to using the touchscreen | [Visual user guide](user-guide.md) | [UI guide](ui-guide.md) if you need technical details |
| A quick project introduction | [Project README](../README.md) | [System context](system-context.md) and [current limitations](current-limitations.md) |
| Firmware implementation details | [Development guide](development.md) | [Architecture](architecture.md), [hardware](hardware.md), and [configuration](configuration.md) |
| HTTP integration details | [ESP interface contract](interface-contract.md) | [Connectivity](connectivity.md) and the [server API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md) |
| UI or serial-console behavior | [UI guide](ui-guide.md) | [UART command reference](UART_COMMANDS.md) |
| A problem to diagnose | [Troubleshooting](troubleshooting.md) | The relevant implementation guide above |
| Coding defects, cleanup, or unresolved design work | [Implementation backlog](implementation-backlog.md) | The linked current guides and server documentation |

You do not need to read every document in order. Each detailed guide begins
with its normal-use model before moving into limitations and source evidence.

## Start here

- [System context](system-context.md) — how Glennergy and Glennergy-ESP form one
  system, where their boundaries are, and what is current versus planned.
- [Current limitations](current-limitations.md) — temporary, partial, unresolved
  and planned behavior that should not be mistaken for completion.
- [Shared glossary](glossary.md) — canonical project terms, identifiers, units
  and deliberately unresolved meanings.
- [ESP HTTP interface contract](interface-contract.md) — every current endpoint
  and behavior relevant to the firmware, plus parser, cache, retry and
  compatibility consequences.
- [Glennergy HTTP API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md)
  — the complete server-side route, method, status, header and schema reference.

## Firmware guides

- [Visual user guide](user-guide.md) — an illustrated, non-technical tour of
  every touchscreen tab and its normal, failed, and offline behavior.
- [Firmware architecture](architecture.md) — startup, tasks, queues, shared
  state, data ownership and recovery boundaries.
- [Development guide](development.md) — ESP-IDF setup, build, validation and
  authorized flash/monitor boundaries.
- [Hardware guide](hardware.md) — source-derived board, display/touch, I2C and
  BME280 configuration with unverified hardware behavior labeled.
- [Configuration](configuration.md) — NVS namespaces, keys, defaults, Wi-Fi
  credential storage and UART persistence behavior.
- [UART command reference](UART_COMMANDS.md) — exact current commands,
  validation, persistence, output intent and safety boundaries.
- [Connectivity](connectivity.md) — Wi-Fi, LEOP, cache, health-state and retry
  lifecycles.
- [UI guide](ui-guide.md) — current tabs, status indicators, Settings fields and
  known display/locking limitations.
- [Troubleshooting](troubleshooting.md) — read-only-first firmware diagnosis and
  gates for hardware-affecting tests.
- [Implementation backlog](implementation-backlog.md) — consolidated current
  defects, decisions, verification work, and cleanup status across both projects.

Paths shown without links are approved campaign artifacts that have not yet
been accepted. Older files should not be used as substitutes for missing
canonical guides without checking current code.

## Server-side architecture

The detailed five-process, IPC, scheduling and server-recovery architecture is
maintained in Glennergy's
[server architecture guide](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/architecture.md).
An offline checkout can use the sibling repository's `Docs/architecture.md`.

## Existing documents under review

| Document | Current classification | Intended action |
| --- | --- | --- |
| [API_ENDPOINTS.md](API_ENDPOINTS.md) | Compatibility pointer for the former detailed duplicate | Keep as a short redirect to the ESP contract and server API reference |
| [easy_api_doc.md](easy_api_doc.md) | Lightweight English integration overview | Keep schema-free and direct readers to the canonical references |
| [UART_COMMANDS.md](UART_COMMANDS.md) | Current canonical UART reference | Update when parser, output, persistence or safety behavior changes |
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
- [Doxygen audit mode](Doxygen_Audit_Mode.md) — deeper semantic/caller-context
  review behavior and its interpretation.
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

<details>
<summary>Index verification metadata</summary>

| Item | Value |
| --- | --- |
| Status | Current navigation index; migration from older documents is in progress |
| Audience | Newcomers, developers, maintainers, operators, and evaluators |
| Applies to | Authoritative `dev` documentation campaign |
| Last verified | Glennergy-ESP `baf9b58d04e827f024c8975b140f7a417e462370`; Glennergy `0048c08ed01fa385d114cd3461e2cad9d7aceb73` |

</details>

Update this index whenever a canonical guide is added, replaced, archived or
renamed. Before publication, every canonical document must be reachable from
this page or the root README, and every older competing document must have an
explicit disposition.
