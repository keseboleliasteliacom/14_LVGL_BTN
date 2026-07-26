# Phase 2 Artifact Design Proposal

Status: Draft design; owner decisions pending  
Purpose: Define document boundaries before any user-facing rewrite

## Design rules

Every artifact must:

- name its primary audience and purpose;
- distinguish implemented, partial, temporary and planned behavior;
- cite the active campaign snapshot as its implementation basis;
- derive technical claims from the evidence register and current code;
- use placeholders for secrets and deployment-specific values unless the owner
  explicitly approves publication;
- identify a single canonical owner for cross-project facts;
- avoid duplicating generated Doxygen symbol reference;
- retain editable diagram source and pass rendering validation;
- record commands as verified, environment-dependent or unverified;
- link to deeper detail instead of overloading READMEs.

## Proposed information architecture

### Glennergy repository

| Artifact | Primary audience | Purpose | Proposed location | Evidence | Validation | Acceptance criteria |
| --- | --- | --- | --- | --- | --- | --- |
| README | New developers, operators, evaluators | Explain the server, its relation to ESP, quick build/deploy/operate path, and documentation navigation | `README.md` | Current README, Makefiles, systemd, deploy/verify scripts | Link check; safe command review; newcomer review | Approachable without hiding production model; links to exact details; no stale tmux/cron guidance |
| Documentation index | All documentation users | Provide one map of current, reference, historical and generated docs | `Docs/README.md` | Documentation inventory | Link and classification audit | Every canonical document reachable; plans/archives labelled |
| Server architecture | Server developers and maintainers | Describe five processes, data ownership, IPC, schedules, concurrency and failure behavior | `Docs/architecture.md` | EV-001 through EV-004 and server discovery | Code-backed review; Mermaid render | All processes/IPC paths and ownership shown; ABI sensitivity stated |
| Interface contract | Server and embedded developers | Define current routes, schemas, types, limits, errors, timing and planned transition | Proposed `Docs/interface-contract.md` | EV-005 through EV-016 | Cross-repo producer/consumer review; example validation | Current and planned APIs visibly separated; exact keys/types/errors documented; unresolved semantics labelled |
| Property configuration | Server developers/operators | Define production/example paths, schema, validation and current capacity | `Docs/property-configuration.md` | EV-023 through EV-025 | Parser/schema cross-check; safe example validation | Does not claim invalid seed entries work; ID/capacity decision reflected |
| Operations and deployment | Operators/maintainers | Consolidate build, install, update, verify, rollback, logs, uninstall and security boundary | `Docs/operations.md` | README, Makefiles, scripts, systemd | Shell review; Linux command verification where available | Deployment and verification distinguished; destructive commands clearly marked |
| Server troubleshooting | Operators/developers | Diagnose service, timer, IPC, data freshness and API failures | `Docs/troubleshooting.md` | Failure paths, journald and verify script | Scenario review | Starts read-only; escalation/recovery ownership clear |

### Glennergy-ESP repository

| Artifact | Primary audience | Purpose | Proposed location | Evidence | Validation | Acceptance criteria |
| --- | --- | --- | --- | --- | --- | --- |
| README | New and embedded developers, evaluators | Replace vendor boilerplate with product purpose, hardware, quick start, limitations and navigation | `README.md` | EV-017 through EV-022, build files, board config | Link check; build-command review; newcomer review | Explains Glennergy relationship and hardware without requiring architecture expertise |
| Documentation index | All documentation users | Separate current guides, technical reference, decisions, plans and generated docs | `docs/README.md` | Documentation inventory | Link/classification audit | No stale planning document appears canonical by accident |
| Firmware architecture | Embedded developers | Describe startup, five tasks, state, queues, synchronization limits and module ownership | `docs/architecture.md` | EV-017 through EV-022 | Code-backed review; Mermaid render | Task/queue/state ownership exact; direct shared-state limitations visible |
| Hardware and sensor | Embedded developers | Describe board, display/touch, shared I2C and BME280 V2 assumptions/recovery | `docs/hardware.md` | Sensor/HAL code and board config | Pin/config cross-check; hardware claims marked static or observed | Active V2 path distinguished from cleanup candidates |
| Firmware configuration | Embedded developers/operators | Describe NVS keys/defaults, Wi-Fi persistence, intervals and safe reset implications | `docs/configuration.md` | Config, NVS, Wi-Fi and UART code | Default/range cross-check | No secret values; persistence behavior correct |
| UART reference | Embedded developers/testers | Replace stale command behavior with exact commands, arguments, ranges and persistence | `docs/UART_COMMANDS.md` | UART implementation and EV-022 | Parser/command table cross-check | Undocumented testing commands handled deliberately; stale runtime-only claim removed |
| Connectivity/cache behavior | Embedded developers | Describe LEOP state machine, fetch cadence, retries, cache and recovery | `docs/connectivity.md` | EV-015 through EV-017 | State-transition and timing review; Mermaid render | Latest-value/cache semantics and limitations match code |
| Firmware troubleshooting | Embedded developers | Diagnose Wi-Fi, LEOP, cache, sensor, UI and UART problems | `docs/troubleshooting.md` | Error/recovery paths | Scenario review; no hardware mutation without approval | Static diagnostics first; hardware-sensitive steps clearly gated |

### Cross-project presentation

The proposed default is for Glennergy to own the canonical interface contract
because the server owns the routes and response schemas. Glennergy-ESP would
contain a short client-specific page or section that links to the canonical
contract and documents only firmware-specific consumption/cache behavior.

This remains unapproved until the owner chooses canonical ownership.

The shared system context may be expressed in both READMEs at different depth,
but exact schemas and route semantics must not have two independent canonical
copies.

## Diagram plan

| Diagram | Format | Canonical artifact | Evidence basis | Validation |
| --- | --- | --- | --- | --- |
| Whole-system context | Mermaid flowchart | Shared/server architecture | Both discovery reports | Render plus cross-repo review |
| Glennergy process/IPC topology | Mermaid flowchart | Server architecture | EV-001 through EV-004 | Node/edge evidence check |
| Server scheduled refresh | Mermaid sequence | Server architecture/operations | systemd timers, producers, cache, algorithm | Timing/order review |
| ESP task and queue topology | Mermaid flowchart | Firmware architecture | EV-017 and EV-018 | Task/queue ownership review |
| ESP startup | Mermaid sequence | Firmware architecture | `app_main` trace | Initialization-order review |
| Current LEOP polling | Mermaid sequence | Interface/connectivity docs | EV-005 through EV-016 | Producer/consumer and retry review |
| LEOP connectivity states | Mermaid state diagram | Connectivity doc | Fetcher state machine | Transition-condition review |
| Planned endpoint migration | Mermaid sequence | Interface contract, clearly planned | Owner direction plus compatibility decision | Must not imply implementation |
| Planned property registration | Mermaid sequence, clearly conceptual | Interface contract/future work | Owner intent only | Auth/ID/persistence unknowns labelled |

## Current versus planned API design boundary

The current contract can be documented fully now. The planned contract must be
limited to accepted direction until design decisions exist.

Current documentation may state:

- the exact current GET routes and response schemas;
- hard-coded property ID 2 in the ESP implementation;
- current status, timing, cache and error behavior;
- current transport/authentication limitations;
- known type/timestamp/configuration inconsistencies.

Planned documentation may state:

- route shape should move to `/recommendation?id=<id>`;
- ESP property registration and server persistence are desired;
- a compatibility, identity, authentication, authorization, validation,
  idempotency and atomic-persistence design is required.

It must not invent a final registration payload or claim the desired route works.

## Historical and cleanup policy

- Planning files remain evidence/history until Phase 2 assigns archive or ADR
  status; they are not deleted during documentation drafting.
- Generated Doxygen output is derived, not canonical narrative documentation.
- Existing SVG/PNG assets are treated as snapshots unless editable source and
  code alignment are verified.
- Cleanup candidates stay in the separate register and require a separate code
  change authorization and review.

## Decisions that prevent design approval

1. Intended meaning of `recommendation[].type` or approval to leave it explicitly unresolved.
2. Whether the five-property limit is intentional or temporary.
3. Future property-ID ownership/allocation direction.
4. Canonical repository for the cross-project interface contract.
5. Approval of placeholder-only public endpoint examples.

No drafting task should silently choose these answers.
