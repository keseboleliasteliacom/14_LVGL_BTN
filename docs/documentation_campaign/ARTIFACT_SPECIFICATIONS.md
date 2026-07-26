# Approved Documentation Artifact Specifications

Status: Phase 2 design baseline

Every canonical artifact records:

- status and intended audience;
- canonical repository owner;
- applicability to authoritative `dev`, stable production `main`, or both;
- last-verified repository SHAs;
- current, partial, temporary, planned and unknown behavior labels;
- update triggers and validation evidence.

## Canonical cross-project artifacts in Glennergy-ESP

### `docs/system-context.md`

Audience: all readers.  
Applicability: current `dev`; production differences explicitly identified.

Required sections:

1. Purpose and scope of the whole Glennergy system.
2. Relationship between Glennergy and Glennergy-ESP.
3. Current system boundary and external dependencies.
4. Current one-way HTTP interaction.
5. Server deployment boundary: Nginx evidence limitation and loopback service.
6. Whole-system component diagram.
7. Current limitations versus planned completion work.
8. Links to canonical interface and repository-specific architecture.

Update triggers:

- new repository/system component;
- communication direction or transport changes;
- deployment boundary changes;
- property registration implementation;
- canonical terminology changes.

### `docs/glossary.md`

Audience: all readers.  
Applicability: both repositories.

Required content:

- preferred English term;
- exact code identifiers or Swedish legacy names where applicable;
- definition, units and status;
- unresolved terms visibly marked;
- distinction between property ID and planned device identity.

Update triggers: public schema, terminology, identifier or unit changes.

### `docs/interface-contract.md`

Audience: embedded and server developers, maintainers.  
Applicability: current `dev`; stable-production differences validated separately.

Required sections:

1. Scope, authority and version metadata.
2. Current transport/security limitations.
3. Exact current route grammar and ESP routes using placeholder base URL.
4. Methods, statuses and error behavior.
5. Exact recommendation, weather and price schemas.
6. Types, units, timestamps, array limits and empty-result behavior.
7. ESP parsing, timeout, retry, health and cache behavior.
8. Known producer/consumer incompatibilities.
9. Clearly separated planned endpoint correction.
10. Clearly separated future UUID-like identity/registration direction without
    an invented payload or authorization design.
11. Compatibility/update requirements.

The recommendation-type intent remains explicitly unresolved. The document may
describe current emitted values and implementation evidence but must not name a
final meaning.

Update triggers: server route/schema/status changes, ESP request/parser changes,
identity/registration implementation, security/transport changes.

### `docs/current-limitations.md`

Audience: all readers.  
Applicability: current implementation.

Required areas:

- temporary example property and property ID 2;
- temporary five-property test limit;
- unresolved recommendation semantics;
- partial Settings UI and Wi-Fi disconnect display;
- current plain HTTP/no API authentication;
- timestamp and UV-index conversion limitations;
- shared-state synchronization limitations;
- incomplete test/runtime/hardware verification;
- planned endpoint and registration work;
- cleanup candidates linked, not mixed with confirmed defects.

## Glennergy canonical artifacts

### `README.md`

Approachable server entry point: purpose, project relationship, components,
prerequisites, safe build, deliberate deployment, routine operation,
troubleshooting entry points and navigation. It summarizes rather than copies
cross-project schemas.

### `Docs/README.md`

Navigation index classifying current guides, reference, generated documentation,
historical plans and contributor tooling.

### `Docs/architecture.md`

Exact five-process architecture; component ownership; FIFO, Unix socket and
shared-memory IPC; schedules; ABI sensitivity; startup/readiness and failure
behavior; process/IPC and refresh diagrams.

### `Docs/property-configuration.md`

Canonical property-file schema and paths; parser behavior; current temporary
capacity; valid/invalid example caveats; current integer ID rules; future
UUID-like device identity linked as planned rather than merged into current
property schema.

### `Docs/development.md`

Local prerequisites and safe build/static checks, including the absence of a
unified root test target. Building must not be represented as deployment proof.

### `Docs/operations.md`

Install, update, service/timer lifecycle, logging, verification, backup,
rollback, uninstall and purge. Production-sensitive and destructive operations
are visibly classified.

### `Docs/security.md`

Loopback/Nginx boundary, service account and permissions, systemd hardening,
current unauthenticated API, unverified external TLS configuration, secret
handling and prerequisites for future state-changing registration.

### `Docs/troubleshooting.md`

Read-only-first diagnosis for services, timers, IPC, data freshness, API and
deployment. It must not normalize destructive recovery.

## Glennergy-ESP canonical artifacts

### `README.md`

Replace the vendor example with product purpose, relationship to Glennergy,
supported hardware, prerequisites, safe quick build, hardware-action boundaries,
current feature summary, limitations and documentation navigation.

### `docs/README.md`

Navigation index separating current guides, reference, ADR/history, plans,
presentation snapshots, contributor tooling and generated Doxygen.

### `docs/architecture.md`

Startup, five tasks, queues, direct shared state, synchronization limitations,
module/data ownership and task/queue diagram.

### `docs/development.md`

ESP-IDF environment/version evidence, target/configuration/build workflow,
available tests, and explicit flash/monitor/hardware approval boundary.

### `docs/hardware.md`

Board/display/touch, pin/configuration evidence, shared I2C assumptions, active
BME280 V2 path, reconnection behavior and static-versus-observed evidence labels.

### `docs/configuration.md`

NVS namespaces/keys/defaults/ranges, Wi-Fi credential mechanism without values,
UART persistence and reset/erase implications.

### `docs/UART_COMMANDS.md`

Exact current commands, arguments, validation, output intent and persistence.
The testing `reboot` command requires an explicit publication decision.

### `docs/connectivity.md`

Wi-Fi/LEOP lifecycle; GET/fetch cadence; latest-value queues; connected,
degraded and unavailable states; cache behavior; timeout/retry/health logic;
connection-loss/cache-recovery diagrams.

### `docs/ui-guide.md`

Approachable screen/status guide covering every current screen, five Settings
fields, LEOP states and the known Wi-Fi disconnect-display limitation.

### `docs/troubleshooting.md`

Read-only/static diagnosis for Wi-Fi, LEOP, cache, BME280, UI and UART. Flash,
erase, serial-device, fault-injection and soak-test actions are explicitly gated.

## Campaign completion artifacts

### `docs/documentation_campaign/MAINTENANCE_MAP.md`

Maps code/workflow areas to affected canonical documents, owners and checks.

### `docs/documentation_campaign/FINAL_COVERAGE_REPORT.md`

Maps every baseline objective, audience journey, runtime area and evidence item
to accepted documentation and validation. Deferred gaps include rationale and
future update trigger.

### `docs/documentation_campaign/FINAL_AUDIT_REPORT.md`

Contains fresh-context findings, severity, disposition and retest evidence.
Completion requires zero unresolved critical/high factual or safety findings.

## Acceptance rule

An artifact is accepted only when its required sections exist, material claims
are in the claim ledger, applicable validation-matrix checks pass or are
explicitly scoped as unverified, and no competing canonical copy remains
unclassified in the disposition register.
