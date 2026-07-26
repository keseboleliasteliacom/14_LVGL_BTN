# Final Documentation Audit Report

| Metadata | Value |
| --- | --- |
| Status | Accepted locally; tracked-documentation critical/high gate passed |
| Audit scope | Canonical and changed tracked Markdown in Glennergy-ESP and Glennergy |
| ESP source snapshot | `b5a502afd9ca2ae374b3131b0031b8390f93b348` |
| Glennergy source snapshot | `42798bee227fcd621cbcb0b37c2b5da771210086` |
| Audit date | 2026-07-26 |

## Outcome

The final focused retest found zero unresolved critical or high factual,
safety, secret-handling, or publication findings in the tracked documentation
set. This satisfies the campaign's critical/high audit gate. The project owner
accepted deferring actual Mermaid rendering to GitHub PR review and authorized
sanitizing the excluded untracked decision document.

## Review method

The campaign used separate discovery, drafting, and fresh-context review passes
across both repositories. Material findings were checked against the pinned
authoritative `dev` source rather than accepted from earlier prose. Review
covered:

- current versus partial, temporary, planned, and unknown behavior;
- routes, schemas, statuses, parsing and producer/consumer compatibility;
- tasks, queues, callbacks, server processes, IPC and failure behavior;
- NVS keys/defaults/persistence, UI fields, UART commands and hardware claims;
- identity, registration, authorization and recommendation unknowns;
- production, hardware, destructive-action and secret-handling gates;
- document ownership, navigation, dispositions and maintenance triggers;
- Markdown relative links, whitespace, JSON examples and Mermaid source
  structure/semantics.

## Material findings resolved

| Severity | Finding | Resolution |
| --- | --- | --- |
| High | UART and Sensor runtime task names were documented as valid despite pointer-to-pointer arguments | Architecture and limitations now distinguish intended metadata from the current defect |
| High | Wi-Fi UI access was overgeneralized as LVGL-lock protected | Architecture/UI/limitations identify the unlocked connected-result path |
| High | Server property-parser failures were described as propagated normally | Property and troubleshooting guides document unsigned conversion, counted zero slots and zero-property success paths |
| High | ID `0` was treated like an ordinary absent ID | Interface and troubleshooting docs identify its unsafe zero-slot behavior |
| High | A legacy tracked API document still published a concrete VPS URL | Numeric deployment URLs were replaced with `<LEOP_BASE_URL>` without copying the address into reports |
| Medium | Algorithm IPC was said to validate the advertised payload size | Architecture/troubleshooting now state `data_size` is ignored |
| Medium | Wi-Fi callback and LEOP-wake context was assigned to the Wi-Fi task | Architecture/connectivity diagrams now show ESP event-loop callback context |
| Medium | Planned identity was over-specified as server-assigned | Documentation keeps UUID source/allocation, property mapping and trust model unresolved |
| Medium | Operations commands lacked consistent preflight gates | Exact-host, authorization, revision/artifact, backup and downtime checks precede live mutations |
| Medium | Navigation, coverage and disposition registers retained campaign-stage claims | Indexes and registers now reflect the current artifact set and explicit deferred gaps |
| Medium | Several Mermaid edges or labels were misleading or renderer-fragile | Event/state edges were corrected and server labels made more robust |

## Validation results

| Check | Result |
| --- | --- |
| Independent whole-set factual/safety review | Pass after corrections |
| Critical/high tracked-documentation gate | Pass: zero unresolved findings |
| Canonical relative local links | Pass |
| `git diff --check` | Pass in both repositories; line-ending conversion notices only |
| Targeted tracked-doc numeric VPS URL scan | Pass; intentional loopback examples remain |
| Private-key/credential-value scan of canonical docs | Pass |
| JSON example parsing | Pass for four detected JSON blocks |
| Mermaid source semantic/fence review | Pass by static inspection |
| Mermaid rendered output | Deferred by owner to GitHub PR review; renderer unavailable locally |
| Firmware/server builds and runtime fixtures | Not executed in this documentation-only campaign |
| Hardware, live endpoint and production-host tests | Not executed and not implied |

## Known exclusions and deferred work

- An unrelated untracked, user-owned ESP planning document was sanitized with
  owner approval without printing the address. It remains outside the campaign
  commits pending a separate content/inclusion review.
- Mermaid diagrams have not been rendered by a local CLI. Static syntax and
  semantic review passed; the owner accepted visual review through GitHub's PR
  rendering instead.
- Stable `origin/main` remains the production code reference; no live
  production deployment was inspected.
- Registration, UUID-like identity, authentication, authorization, endpoint
  migration and recommendation intent remain product-design work.
- Source defects and cleanup candidates are documented but were not fixed.
- A credential-like literal in ESP source was not copied into documentation;
  removal and possible rotation require separate approved code work.

## Post-acceptance HTTP documentation amendment

After the initial campaign acceptance, the owner changed HTTP documentation
ownership so both repositories are independently useful:

- Glennergy now owns the complete server-side `Docs/http-api.md` reference.
- Glennergy-ESP retains every endpoint and behavior relevant to the firmware in
  `docs/interface-contract.md`, plus parser, cache, retry and compatibility
  consequences.
- Shared route, method, status, header, schema and error facts must be updated
  in both documents together.

The amendment reused the reviewed contract rather than reconstructing it. The
shared wire sections were compared for exact synchronization; relative links,
seven JSON examples, whitespace and targeted address/key scans passed. No
source, endpoint, hardware or production system was changed or contacted.

## Acceptance rule

This audit supports draft-PR publication and GitHub visual review. It does not authorize
source cleanup, production operations, hardware access, deletion, direct `dev`
changes, or disclosure of excluded values.
