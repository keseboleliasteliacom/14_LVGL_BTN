# Final coverage report

| Metadata | Value |
| --- | --- |
| Status | Coverage assessment complete; owner acceptance and Mermaid rendering decision pending |
| Campaign target | Approximately 90% final documentation for approximately 95% complete code |
| Authoritative source | Glennergy-ESP `b5a502a`; Glennergy `42798be` |
| Stable comparison | ESP `origin/main` `daf35c5`; Glennergy `origin/main` `61761b5` |
| Canonical cross-project owner | Glennergy-ESP |

This report measures documentation coverage against the approved campaign
baseline. It does not declare the campaign complete. Final acceptance still
requires an independent final audit, disposition of its findings, retesting,
and project-owner review.

## Outcome summary

The campaign now has a coherent, English, layered documentation set covering
the shared system, current interface, server internals and lifecycle, firmware
internals and operation, configuration, UI, hardware, safety, and known gaps.
Material cross-project claims are tied to pinned source snapshots and separate
implemented, temporary, partial, planned, and unknown behavior.

Coverage is strongest for static implementation truth. It is intentionally
limited for live production, physical hardware, external reverse-proxy/TLS
configuration, and runtime fault behavior because those environments were not
accessed. Mermaid sources exist but were not rendered in the campaign
environment. These are visible validation gaps, not implied passes.

## Baseline objective coverage

| Baseline objective | Primary artifacts | Assessment | Remaining work |
| --- | --- | --- | --- |
| Accurate system overview and repository relationship | Root README, [system context](../system-context.md), [glossary](../glossary.md), both documentation indexes | Covered and independently reviewed | Final whole-set audit and owner acceptance remain |
| Server architecture and process/data flow | Glennergy `Docs/architecture.md` and `Docs/README.md` | Covered; five processes, schedules, IPC, readiness, ABI and restart/cache behavior reviewed | Mermaid rendering and runtime IPC observation pending |
| Firmware architecture and data flow | [Firmware architecture](../architecture.md) | Covered; startup, five tasks, queues, direct state, callback contexts, LVGL boundary and defects reviewed | Mermaid rendering and runtime concurrency behavior pending |
| Exact cross-project interface | [Interface contract](../interface-contract.md), [connectivity](../connectivity.md) | Covered at source level, including ID-zero, schemas, errors, cadence, cache and incompatibilities | Runtime/schema fixtures and stable end-to-end compatibility remain unverified |
| Setup and development | [ESP development](../development.md), Glennergy `Docs/development.md` | Covered with build-versus-deploy/flash boundaries | Campaign environment did not execute all documented toolchains/commands |
| Configuration and persistence | [ESP configuration](../configuration.md), [UART reference](../UART_COMMANDS.md), Glennergy `Docs/property-configuration.md` | Covered, including defaults, keys, ranges, parser disagreement and error propagation | Registration/property-write design remains unresolved |
| Deployment and operations | Glennergy `Docs/operations.md`, `Docs/security.md` | Covered from scripts, Makefiles and units with destructive actions gated | No production host, Nginx, TLS, DNS, firewall, backup or rollback was exercised |
| Runtime flow and sequence diagrams | System context, both architecture guides, connectivity and configuration | Mermaid sources cover the major confirmed current flows | Mermaid CLI unavailable; render/readability acceptance pending |
| Troubleshooting | [ESP troubleshooting](../troubleshooting.md), Glennergy `Docs/troubleshooting.md` | Covered with read-only-first diagnosis and mutation gates | Runtime exercises remain intentionally unperformed |
| Current limitations and future direction | [Current limitations](../current-limitations.md), [interface contract](../interface-contract.md), relevant guides | Covered and current/planned distinction independently reviewed | Recommendation meaning, identity, registration, auth and route migration remain design work |
| Navigation and source-of-truth control | [ESP documentation index](../README.md), Glennergy `Docs/README.md`, [disposition register](DOCUMENT_DISPOSITION.md) | Canonical locations defined and most guides linked | Competing older documents still need final banners/archive decisions; disposition register status must be finalized |
| Maintenance handoff | [Maintenance map](MAINTENANCE_MAP.md), validation/evidence/claim registers | Drafted | Automation and owner acceptance pending |
| Independent final verification | [Final audit report](FINAL_AUDIT_REPORT.md) plus claim-level and artifact-specific reviews | Critical/high tracked-documentation gate passed after focused retest | Owner acceptance remains pending |

## Audience journeys

| Audience | Recommended journey | Coverage |
| --- | --- | --- |
| New reader or evaluator | ESP root README → [system context](../system-context.md) → [limitations](../current-limitations.md) | Approachable entry point and deep guides are present; final acceptance pending |
| Embedded developer | [Development](../development.md) → [hardware](../hardware.md) → [architecture](../architecture.md) → [configuration](../configuration.md) → [connectivity](../connectivity.md) → [UART](../UART_COMMANDS.md) | Strong static coverage; build/hardware observation not comprehensive |
| Server developer | Glennergy README → `Docs/development.md` → `Docs/architecture.md` → `Docs/property-configuration.md` | Strong source/workflow coverage; final audit pending |
| Operator | Glennergy README → `Docs/operations.md` → `Docs/security.md` → `Docs/troubleshooting.md` | Safe lifecycle and diagnosis documented; live production verification absent |
| UI/product reviewer | [UI guide](../ui-guide.md) → [limitations](../current-limitations.md) → [connectivity](../connectivity.md) | All current screens/fields described; no physical visual acceptance performed |
| Interface maintainer | [Interface contract](../interface-contract.md) → [glossary](../glossary.md) → both architecture/connectivity guides | Exact static contract and incompatibilities covered; integration fixtures pending |
| Documentation maintainer | Both indexes → [maintenance map](MAINTENANCE_MAP.md) → [evidence register](EVIDENCE_REGISTER.md) → [claim ledger](CLAIM_LEDGER.md) → [validation matrix](VALIDATION_MATRIX.md) | Governance and triggers covered; automation remains optional/pending |

## Runtime-area coverage

| Runtime area | Documented in | Evidence and status |
| --- | --- | --- |
| Server five-process topology | System context; Glennergy architecture/README | Static Makefile, entry-point and systemd evidence; reviewed |
| Meteo/Spotpris schedules and providers | Glennergy architecture, operations, troubleshooting | Static timers/services/source; external services not contacted |
| InputCache/FIFO/socket ownership | Glennergy architecture and troubleshooting | Static source/protocol evidence; runtime IPC not observed |
| Algorithm calculation/publication | Glennergy architecture; interface contract; limitations | Current emitted percentage traced; intended recommendation semantics unknown |
| SHM/semaphore HTTP publication | Glennergy architecture/troubleshooting | Static source evidence; ABI and freshness limits documented |
| HTTP routes/statuses/schemas | Interface contract | Producer/consumer static comparison; examples parse; runtime fixtures pending |
| ESP startup and FreeRTOS tasks | Firmware architecture | Static `app_main` and worker evidence; runtime names/concurrency not observed |
| Queue and shared-state ownership | Firmware architecture/connectivity | Static producer/consumer trace; mixed semantics and lack of common mutex documented |
| Wi-Fi credentials/events/reconnect/SNTP | Configuration/connectivity/UI/troubleshooting | Static NVS/event/task trace; no live network or credentials used |
| LEOP fetch/health/state/cache | Connectivity/interface contract/troubleshooting | Static cadence, threshold and cache trace; no real endpoint contacted |
| JSON parsing and compatibility | Interface contract/limitations | Static producer-consumer comparison; timestamp, UV and bounds defects documented |
| NVS/UART persistence and reboot | Configuration/UART/troubleshooting | Exact static keys/ranges/commands; no device mutation performed |
| Display/touch/LVGL UI | Hardware/architecture/UI guide | Source/config and generated-project evidence; physical behavior unverified |
| BME280 shared I2C and recovery | Hardware/architecture/troubleshooting | Active V2 source path traced; sensor presence/electrical/runtime behavior unverified |
| Server deploy/verify/rollback/purge | Glennergy development/operations/security/troubleshooting | Scripts and units inspected; host operations not executed |

## Evidence-register disposition

Every evidence-register item is represented below. “Covered” means the claim
has a reader-facing home; it does not upgrade static evidence to runtime proof.

| Evidence IDs | Reader-facing coverage | Status or deferred boundary |
| --- | --- | --- |
| EV-001–EV-004 | System context; Glennergy architecture, README, operations | Covered statically; production/runtime observation deferred |
| EV-005–EV-010 | Interface contract; system context; connectivity | Covered, including transitional routes and one-way GET; runtime fixtures deferred |
| EV-011–EV-016 | Interface contract; limitations; connectivity/troubleshooting | Covered incompatibilities and cache/health behavior; fixes separate |
| EV-017–EV-022 | Firmware architecture, UI, hardware, configuration, UART, limitations | Covered; concurrency/UI/hardware runtime tests deferred |
| EV-023–EV-026 | Glennergy property configuration/architecture; interface contract; limitations | Covered; five-property limit temporary and recommendation intent unresolved |
| EV-027 | ESP root README | Replaced with current product entry point; final audit pending |
| EV-028 | Glennergy README | Integrated as current server entry point; final audit pending |
| EV-029–EV-030 | ESP index, disposition register; Glennergy index/architecture | Canonical replacements exist; old-doc banners/archive classification remains |
| EV-031 | Both development guides; Glennergy operations | Covered; no root server test target is clearly stated |
| EV-032 | Cleanup register and document disposition | Deferred to separately authorized cleanup/archival work |
| EV-033–EV-035 | System context, glossary, interface contract, limitations, public examples | Owner direction recorded; identity/auth details unresolved; real VPS omitted |
| EV-036–EV-037 | Firmware architecture, limitations, UI guide/troubleshooting | Defects documented; source fixes separate |
| EV-038–EV-039 | Glennergy architecture/property/troubleshooting | Limitations documented; source fixes separate |
| EV-040–EV-041 | Cleanup/security/configuration records and placeholder-only public docs | Sensitive value/address not reproduced; source cleanup/config mechanism separate |

EV-014’s broad unknown-ID statement is qualified in the canonical interface:
positive unknown IDs normally return an empty array, while ID zero can match
zero-filled slots. The evidence register itself should be narrowed during final
governance cleanup so it cannot be read without that qualification.

## Validation performed

The drafting campaign performed, as applicable:

- pinned-source and fetched-remote comparison;
- repeated independent factual/semantic reviews;
- producer/consumer comparison for all three HTTP categories;
- relative-link existence checks;
- Markdown whitespace checks;
- targeted scans for real endpoint addresses, credential/key patterns, and
  private-key material;
- JSON parsing of contract examples;
- inspection of build, deployment, verification, systemd, NVS, SPIFFS, UART,
  generated UI, hardware configuration, and Doxygen workflows.

Validation results belong to the recorded source snapshots. Later source or
workflow changes require revalidation through the maintenance map.

## Deferred validation and rationale

| Deferred check | Reason | Acceptance effect | Future trigger |
| --- | --- | --- | --- |
| Mermaid rendering and visual inspection | Mermaid CLI/runtime unavailable locally | Diagram source exists but render gate is not passed | Install/enable renderer, render every Mermaid block, inspect labels/layout |
| Complete ESP-IDF clean build/test execution | Toolchain availability and campaign scope | Static build instructions are not execution proof | CI/local ESP-IDF environment available; run clean build and focused tests |
| Server build and full command execution | Cross-platform/tool availability and no production mutation | Command text/source reviewed; not every command observed | Linux CI/dev host available; run safe build/check targets |
| Interface runtime/schema fixtures | No controlled integrated server/device harness | Static compatibility coverage only | Add server/client fixture tests for routes, statuses, bounds and schemas |
| Physical ESP32-S3/display/touch/BME280 tests | No approved hardware interaction in campaign | Hardware behavior remains explicitly unverified | Authorized board, test plan and reproducible observation available |
| Live Wi-Fi, NVS, cache, reconnect and fault injection | Would use/mutate hardware or credentials | Recovery timing and UI behavior not runtime-proven | Authorized development device and redacted test network available |
| Production deployment, verifier, rollback and purge | Production access and disruptive/destructive risk | Operations are source-derived, not production-proven | Authorized operator, maintenance window, backup/recovery plan |
| Nginx/TLS/DNS/firewall/VPS verification | Configuration lies outside repositories and real address is intentionally omitted | External exposure/security remains unverified | Owner supplies approved redacted configuration or authorizes inspection |
| Stable end-to-end compatibility | Stable branches contain differing generations; no deployed pair inspected | No claim that all current docs apply to production | Identify deployed SHAs and run compatibility verification |

## Deferred product and cleanup work

Documentation records, but cannot resolve, these implementation/design gaps:

- final recommendation semantics;
- command-first endpoint migration and compatibility strategy;
- UUID-like identity source and provisioning;
- device-to-property mapping;
- two-way property registration schema and persistence;
- API authentication, authorization and transport security;
- five-property temporary capacity;
- incomplete Settings fields and Wi-Fi disconnect display;
- parser, bounds, timestamp, UV, IPC size, concurrency and LVGL locking defects.

Legacy/fake modules, obsolete comments, credential-like source comments,
compiled endpoint configuration, stale narrative files, generated artifacts,
and historical plans remain cleanup/disposition work. Documentation coverage is
not authorization to delete, rewrite, rotate, deploy, or modify `dev` source.

## Remaining acceptance work

Before campaign acceptance:

1. Render and inspect Mermaid diagrams, or obtain explicit owner acceptance of
   the documented rendering deferral.
2. Confirm navigation, secret/address scans, whitespace and links on the final
   committed set.
3. Obtain project-owner acceptance before publication/PR completion.

Until those steps are complete, this report is a draft assessment rather than
a completion certificate.
