# Phase 1 Discovery Review Package

Status: Discovery complete; owner decisions recorded
Source snapshots: see `CHECKPOINT_MANIFEST.md`

## Executive summary

Glennergy is a systemd-managed Linux server stack that gathers weather and
electricity-price data, calculates quarter-hour result arrays, and exposes them
through a loopback HTTP server normally fronted by Nginx. Glennergy-ESP is an
ESP32-S3 firmware application with Wi-Fi, UI, UART, sensor and LEOP tasks. It
polls Glennergy over HTTP for recommendation, weather and price data, publishes
latest-value snapshots to the UI, and caches successful JSON responses in
SPIFFS for offline display.

The implemented cross-project direction is currently request/response from ESP
to server: three unauthenticated GET requests retrieve server data. Property
registration, server-side property mutation and a two-way management protocol
do not exist yet. They remain a major planned design workstream.

The code is documentable now, but several semantics and compatibility issues
must be explicit. The campaign can reach its 90% target without completing the
product backlog if current, partial, temporary and planned behavior remain
visibly separated.

## Confirmed current architecture

### Glennergy

- InputCache owns current weather/price inputs and local IPC.
- Meteo and Spotpris run as timer-triggered producers.
- Algorithm reads InputCache, computes 96 quarter-hour results and publishes a
  POSIX shared-memory snapshot guarded by a named semaphore.
- The HTTP server reads that snapshot and listens only on `127.0.0.1:8080`.
- Nginx is expected to be the external boundary; its configuration is outside
  these repositories.
- Production configuration is `/etc/glennergy/fastigheter.json`; the repository
  includes an installation seed/example.

### Glennergy-ESP

- `app_main` initializes NVS/configuration, display/touch/LVGL, Wi-Fi, SPIFFS
  and the UI, then runs Wi-Fi, UI, UART, Sensor and LEOP tasks.
- Wi-Fi, sensor and LEOP flows use depth-1 latest-value queues where applicable.
- Some shared application state is still read/written directly without mutexes.
- The active sensor is BME280 V2 on the display/touch I2C bus.
- LEOP fetches recommendation, weather and price; SPIFFS cache supports offline
  display, and health/status logic reports connected/degraded/unavailable.
- UART configuration values now persist to NVS despite stale documentation
  claiming they are runtime-only.

## Implemented cross-project contract

Current routes are:

```text
GET <LEOP_BASE_URL>/id=2?recommendation
GET <LEOP_BASE_URL>/id=2?weather
GET <LEOP_BASE_URL>/id=2?price
```

The server accepts the general pattern `/id=<non-negative integer>?<command>`.
The owner’s earlier ID 3 was a valid illustration; the firmware currently
hard-codes ID 2.

Each successful matching dataset normally contains 96 objects. Exact field
schemas, error behavior, timing, cache behavior and limits are supported by
`EV-005` through `EV-016` in the evidence register.

The intended `/recommendation?id=2` form is planned and currently unsupported.
Changing both repositories without a compatibility plan would break existing
clients.

## High-impact limitations and contradictions

1. The recommendation calculation produces a categorical result but discards
   it and publishes the output of `average_WindowLow_percent` instead. The local
   variable is misleadingly named `temp`; intended wire semantics remain unresolved.
2. Property configuration contains 17 examples, but major runtime structures
   process at most five; several later entries also violate the integer-ID/full
   schema expected by code.
3. Server timestamps retain an ISO-8601 offset, while ESP buffers truncate the
   string to 19 characters.
4. Weather UV index is real-valued server-side but stored as integer on ESP.
5. The ESP trusts parseable response bodies rather than HTTP status for normal
   GET success and does not fall back to cache after malformed online responses.
6. Current transport uses plain HTTP and no API authentication. A state-changing
   registration API must not reuse that trust model without deliberate design.
7. Settings UI is exactly three-of-five functional; the Wi-Fi status UI handles
   initial connection but not later disconnection correctly.
8. Existing narrative docs contain stale queue, retry, error, persistence,
   language, endpoint and source-of-truth claims.

## Documentation inventory assessment

### High-priority current entry points

- ESP README: vendor-example boilerplate; needs a Glennergy-specific replacement.
- Glennergy README: useful operational guide; needs newcomer/system context and
  deeper-document navigation.
- Cross-project API contract: no reliable canonical document currently exists.

### Useful but noncanonical inputs

- Existing ESP API, UART, signal-flow, connectivity and presentation documents.
- Current project plans and roadmaps.
- Glennergy systemd migration plan and ignored architecture overview.
- Doxygen standards/workflows as contributor tooling.

These sources contain valuable history and diagrams, but implementation claims
must be revalidated. Planning documents should be labelled as plans, ADRs or
archives instead of being mixed into current user navigation.

## Proposed canonical documentation structure

This remains a Phase 2 design subject, but discovery supports:

1. Approachable README in each repository.
2. Documentation index/navigation in each repository.
3. Shared system context with an explicit canonical owner and mirrored links.
4. One canonical cross-project interface contract that separates current and
   planned APIs.
5. Detailed server architecture and operations documents in Glennergy.
6. Detailed firmware architecture, hardware, configuration and UART documents
   in Glennergy-ESP.
7. Mermaid source for current runtime/sequence diagrams; rendered derivatives
   only where needed.
8. Clearly labelled limitations, planned work, ADR/history and generated API
   documentation.

## Prioritized drafting backlog

1. Resolve the blocking terminology/semantic decisions below.
2. Approve canonical cross-project document ownership.
3. Design the shared glossary, system context and current API contract.
4. Design server and firmware architecture documents.
5. Design and rewrite the two READMEs with layered navigation.
6. Consolidate/correct configuration, UART, operations and troubleshooting docs.
7. Produce current-state component/data-flow/sequence diagrams.
8. Add clearly conceptual planned endpoint-migration and registration diagrams.
9. Independently verify all artifacts and produce a remaining-coverage report.

## Owner decisions recorded for Phase 2

1. `recommendation[].type` intent remains unresolved. Documentation describes
   current evidence and the likely inconsistency without inventing semantics.
2. Five properties is a temporary test limit, not final product capacity.
3. Current incrementing integer IDs are temporary and unauthenticated. Future
   identity should use a UUID or similar identifier unique to each ESP32-S3
   unit. Exact identity source, property relationship, registration and
   authorization remain future design questions.
4. Glennergy-ESP owns canonical cross-project system and interface documentation.
   Glennergy contains server-specific details and links to the canonical copy.
5. Public documentation uses endpoint placeholders and omits the real VPS
   address unless the owner changes this policy later.

The future authentication, UUID derivation/provisioning, idempotency and schema
details for registration can be designed later. They do not block documenting
the implemented system.
