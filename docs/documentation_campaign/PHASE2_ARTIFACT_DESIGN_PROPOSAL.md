# Phase 2 Artifact Design Proposal

Status: Draft design updated with owner decisions
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
- declare applicability (`dev`, stable `origin/main`, or both), document status,
  canonical owner, last-verified SHAs, and update triggers;
- avoid calling `dev` behavior deployed unless stable-production evidence proves it.

## Cross-project canonical artifacts

Subject to owner approval of repository placement, the campaign requires these
single-owner artifacts:

| Artifact | Audience | Required content | Acceptance boundary |
| --- | --- | --- | --- |
| System context | All | Product purpose, repository relationship, system boundary, external providers, current one-way interaction, deployment context and evidence boundaries | One canonical source; both READMEs summarize and link; current/planned boundary explicit |
| Shared glossary | All | Preferred English terms, exact code identifiers, unresolved terms, casing and units | Every cross-project term has one definition or is explicitly unresolved |
| Current limitations | All | Partial UI, temporary property, API/security/type/timestamp/capacity/testing limitations and planned completion work | Every accepted deferred finding has an owner/location and is not described as implemented |
| Documentation maintenance map | Contributors | Canonical ownership, code-to-doc update triggers, validation commands, generated/history policy | Each canonical artifact has an update trigger and reviewer expectation |
| Final coverage/audit report | Maintainers/evaluators | Objective-by-objective coverage, validation evidence, deferred gaps, severity/disposition log | No unresolved critical/high factual or safety finding |

Canonical cross-project system context, glossary and interface documentation
will live in Glennergy-ESP `docs/`, per owner decision. Glennergy owns its
server-specific architecture, property configuration, operations and security
details and links to the canonical cross-project documents.

## Proposed information architecture

### Glennergy repository

| Artifact | Primary audience | Purpose | Proposed location | Evidence | Validation | Acceptance criteria |
| --- | --- | --- | --- | --- | --- | --- |
| README | New developers, operators, evaluators | Explain the server, its relation to ESP, quick build/deploy/operate path, and documentation navigation | `README.md` | Current README, Makefiles, systemd, deploy/verify scripts | Link check; safe command review; newcomer review | Approachable without hiding production model; links to exact details; no stale tmux/cron guidance |
| Documentation index | All documentation users | Provide one map of current, reference, historical and generated docs | `Docs/README.md` | Documentation inventory | Link and classification audit | Every canonical document reachable; plans/archives labelled |
| Server architecture | Server developers and maintainers | Describe five processes, data ownership, IPC, schedules, concurrency and failure behavior | `Docs/architecture.md` | EV-001 through EV-004 and server discovery | Code-backed review; Mermaid render | All processes/IPC paths and ownership shown; ABI sensitivity stated |
| Interface contract link/summary | Server and embedded developers | Summarize server ownership and link to the canonical ESP-hosted interface contract | `Docs/interface-contract.md` or a README/index section | Canonical ESP contract and server route implementation | Cross-repo link and duplication audit | No independent competing schema copy; server-specific implementation links remain useful |
| Property configuration | Server developers/operators | Define production/example paths, schema, validation and current capacity | `Docs/property-configuration.md` | EV-023 through EV-025 | Parser/schema cross-check; safe example validation | Does not claim invalid seed entries work; ID/capacity decision reflected |
| Operations and deployment | Operators/maintainers | Consolidate build, install, update, verify, rollback, logs, uninstall and security boundary | `Docs/operations.md` | README, Makefiles, scripts, systemd | Shell review; Linux command verification where available | Deployment and verification distinguished; destructive commands clearly marked |
| Server troubleshooting | Operators/developers | Diagnose service, timer, IPC, data freshness and API failures | `Docs/troubleshooting.md` | Failure paths, journald and verify script | Scenario review | Starts read-only; escalation/recovery ownership clear |
| Developer setup | Server developers | Reproducible local prerequisites, build, safe checks, known lack of root test target and separation from deployment | `Docs/development.md` | Makefiles and safe validation targets | Clean/disposable Linux workflow record where available | Commands include cwd/prerequisites/effects; no deployment claim from build success |
| Security boundary | Developers/operators | Document loopback/Nginx boundary, service identity, permissions, unauthenticated API, external TLS evidence gap and future state-changing prerequisites | `Docs/security.md` or explicitly owned sections | Code, units, deploy scripts and interface evidence | Trust-boundary review and secret/address scan | Current controls and unverified external controls distinguished; no secret values |

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
| Developer setup | Embedded developers | ESP-IDF version/environment, target, configuration, build and boundaries around flash/monitor/hardware | `docs/development.md` | README, CMake, sdkconfig defaults and workflow | Safe build record where environment exists; static command review otherwise | Build is not represented as hardware/runtime proof; cwd and prerequisites explicit |
| UI and status guide | General users, evaluators and developers | Explain screens, visible status meanings, five Settings fields and known Wi-Fi/placeholder behavior | `docs/ui-guide.md` or an owned README section | UI code, EV-019 and EV-020 | Field/status inventory and static/runtime-evidence labels | Every visible current field/status is covered or explicitly deferred |

### Cross-project presentation

Glennergy-ESP owns the canonical interface contract and shared system context.
Glennergy links to those artifacts and keeps only server-specific implementation,
property-configuration and operational details canonical locally.

The shared system context has one canonical detailed artifact. Both READMEs may
contain audience-specific summaries, but exact schemas, route semantics and
system facts must not have independent canonical copies.

Property-file schema, validation and capacity are canonical in the property
configuration document. HTTP routes, request/response representations and
errors are canonical in the interface contract. ESP fetch timing, cache and
connectivity state transitions are canonical in the firmware connectivity
document and only summarized/link-referenced from the interface contract.

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
| Sensor-to-display flow | Mermaid sequence | Firmware architecture/UI guide | Sensor task, queue and UI trace | Producer/consumer/order review |
| Configuration persistence | Mermaid sequence | Firmware configuration | UART/config/NVS trace | Validation, write and reload review |
| Connection loss and cache recovery | Mermaid sequence | Connectivity guide | Wi-Fi/LEOP/cache state trace | Timing/transition review |
| Production deployment topology | Mermaid flowchart | System context/operations | Repository server boundary plus explicit external Nginx gap | Current-production applicability and evidence-boundary review |

Planned endpoint migration and registration diagrams should live in a dedicated
future-design/ADR artifact until their authentication, compatibility, ID,
validation, idempotency and persistence decisions exist. The implemented
interface contract contains only a compact, clearly boxed planned-direction note.

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
- Every existing narrative document receives an explicit disposition in
  `DOCUMENT_DISPOSITION.md`: retain, update, replace, archive, redirect or
  delete-later. Superseded direct links receive a banner or link action so stale
  files do not remain competing sources of truth.

## Artifact dependency scope

Unresolved product decisions do not block the entire campaign:

- Recommendation semantics block only the final field definition and affected
  explanatory claims; other documents label the current behavior and unknown intent.
- Property capacity and ID direction block future-registration and final schema
  design claims; current limits and parser behavior can be documented now.
- Canonical repository ownership blocks final placement and cross-repository
  link design, not artifact outlines or evidence preparation.
- Public-address policy blocks publication examples; the safe working default
  remains placeholders.

## Decisions required before affected artifacts are finalized

1. `recommendation[].type` remains explicitly unresolved; current behavior must
   be documented without asserting intended semantics.
2. Five properties is a temporary test limit.
3. Future identity direction is UUID-like and unique per ESP32-S3 unit; exact
   identity/auth/registration design remains open.
4. Glennergy-ESP owns canonical cross-project documentation.
5. Public examples use placeholders and omit the real VPS address.

No drafting task may add specificity beyond these decisions without new evidence
or owner approval. The open UUID/auth/registration questions block only final
future-protocol claims, not documentation of the implemented system.
