# Cross-project implementation backlog

> **Purpose:** This is the consolidated coding and design worklist for
> Glennergy and Glennergy-ESP. It gathers actionable defects, contradictions,
> temporary limits, unresolved decisions and verification gaps that are
> otherwise distributed across both documentation sets.

This page is a planning aid, not evidence that every item should be implemented
exactly as phrased. Items in **Decisions required before implementation** need
an agreed contract before coding. Cleanup candidates require confirmation
before deletion. The
linked source documents remain authoritative for detailed current behavior.

## How to use this backlog

1. Select one item and confirm its dependencies.
2. For a **Decision first** item, record the decision before changing code.
3. Implement and test both repositories together when an item crosses the HTTP
   or identity boundary.
4. Update the linked documentation in the same change.
5. Mark the item complete only after its completion check is satisfied.

Suggested status values are `Open`, `In progress`, `Blocked by decision`,
`Ready for verification`, and `Complete`.

Issues discovered while implementing this backlog are tracked separately in
[Implementation follow-up backlog](implementation-followup-backlog.md). Review
that list after completing the initial backlog, unless a follow-up blocks or
invalidates the item currently being implemented.

## Recommended work order

| Phase | Focus | Why it comes here |
| --- | --- | --- |
| 1 | Recommendation meaning, registration/identity, route migration and final capacity | These decisions shape schemas and compatibility work |
| 2 | Property loading, error propagation and seed-data correctness | Establishes trustworthy server inputs before adding writes |
| 3 | Registration security, validation and atomic persistence | State-changing behavior must not inherit the current unauthenticated trust model |
| 4 | HTTP/JSON compatibility and cache correctness | Producer and consumer should agree before UI behavior is finalized |
| 5 | ESP concurrency, status and Settings completion | Fixes concrete firmware correctness and presentation defects |
| 6 | Tests, runtime/hardware verification and cleanup | Confirms behavior and makes obsolete-code removal safer |

## Decisions required before implementation

| ID | Priority | Decision | Scope | Completion check | Status |
| --- | --- | --- | --- | --- | --- |
| D-01 | High | Define the semantic meaning, range and presentation of recommendation `type`; decide whether to publish the calculated category, `average_WindowLow_percent`, or a redesigned field | Both | One documented schema and matching algorithm, serializer, parser, UI and tests | Open |
| D-02 | High | Design device identity and property registration: UUID source, provisioning, device/property mapping, enrollment, ownership transfer, rotation and recovery | Both | Approved identity lifecycle and data model | Open |
| D-03 | High | Define registration route/method, request and response schemas, validation, duplicate/update behavior, idempotency, retry and conflict rules | Both | Versioned contract with success and error examples | Open |
| D-04 | High | Define authentication, authorization and transport protection before exposing state-changing registration | Both/deployment | Threat-reviewed trust model; no reliance on secret endpoint addresses | Open |
| D-05 | High | Agree the resource-first route names and migration window from `/id=2?recommendation` to `/recommendation?id=2` and equivalents | Both | Compatibility/versioning plan covering old and new clients | Open |
| D-06 | Medium | Decide the intended property capacity beyond the temporary limit of five | Server/contract | Capacity requirement documented with memory, IPC and response-size implications | Open |
| D-07 | Medium | Define timestamp format, timezone/offset policy and validation | Both | One wire format preserved end to end | Open |
| D-08 | Medium | Decide API/schema versioning, structured error format, unknown-property behavior and whether property ID zero is valid | Both | Documented status/error/version contract | Open |

## Confirmed server defects and contradictions

| ID | Priority | Work item | Completion check | Status |
| --- | --- | --- | --- | --- |
| S-01 | High | Fix `homesystem_LoadAllCount()` failure propagation so `-1` cannot become unsigned `SIZE_MAX` in InputCache | Missing, invalid and wrong-root JSON cause a clear startup failure | Open |
| S-02 | High | Make property counts represent successfully parsed entries rather than the truncated source-array length | Rejected entries cannot remain counted as zero-filled properties | Open |
| S-03 | High | Make Meteo reject an empty or entirely invalid property set instead of reporting a successful no-op | Process success proves at least one usable property was processed | Open |
| S-04 | High | Reconcile the 17-entry seed with the implemented schema; address string IDs, missing `city`, and the nonauthoritative nested JSON copy | One canonical valid seed with explicit example/test intent | Open |
| S-05 | High | Implement validated, atomic and concurrency-safe persistence before registration can update property configuration | Interrupted or concurrent writes cannot corrupt the active file; failures return stable errors | Blocked by D-02 to D-04 |
| S-06 | High | Validate `CacheResponse.data_size` before Algorithm reads a payload | Short, oversized and incompatible payloads are rejected without blocking or misinterpretation | Open |
| S-07 | Medium | Add an IPC compatibility strategy for raw native structures, such as versioned framing/serialization or strict release validation | Mixed layouts fail explicitly; partial deployment risk is documented and tested | Open |
| S-08 | Medium | Normalize UV representation so the server does not convert upstream data to integer and then emit a conflicting JSON real | Producer schema and consumer parser use the same JSON type and tested value behavior | Open |
| S-09 | Medium | Stop treating unpopulated result slots as meaningful 96-entry data, or expose validity explicitly | Clients can distinguish populated current values from zero-filled slots | Open |
| S-10 | Medium | Add freshness/readiness information for provider data, algorithm snapshots and the HTTP surface | A listening port and `200` response can be distinguished from ready/current data | Open |
| S-11 | Medium | Decide and implement restart recovery for dated Meteo/Spotpris caches, or explicitly keep them as records only | Restart behavior is tested and matches documented policy | Open |
| S-12 | Medium | Return stable HTTP errors for malformed input, unknown property, internal failure and oversized output | Status and structured body match D-08 and route tests | Blocked by D-08 |
| S-13 | Low | Add application-level readiness checks instead of relying only on systemd process ordering | Dependencies can distinguish process start from usable IPC/data readiness | Open |

## Confirmed ESP defects and contradictions

| ID | Priority | Work item | Completion check | Status |
| --- | --- | --- | --- | --- |
| E-01 | High | Add bounds checks before writing server arrays into fixed 96-entry storage | Empty, 96-entry and oversized arrays are tested without out-of-bounds writes | Open |
| E-02 | High | Validate every required JSON member and type, including safe timestamp handling | Missing/wrong-type members fail cleanly instead of becoming zero or unsafe strings | Open |
| E-03 | High | Require acceptable HTTP status before parsing or caching normal data responses | Non-2xx bodies cannot be treated as successful datasets | Open |
| E-04 | High | Validate a response before replacing the previous cache | Malformed/incompatible online data cannot destroy the last known-good cache | Open |
| E-05 | Medium | Define and implement cache fallback for malformed online responses, not only offline/no-Wi-Fi periods | Controlled tests demonstrate the intended online-failure fallback policy | Open |
| E-06 | High | Fix UART and Sensor `xTaskCreate` name arguments, which currently pass the address of a pointer | Runtime task names are reliably `UART` and `Sensor` | Open |
| E-07 | High | Put the Wi-Fi connected-result LVGL mutations under the required LVGL lock | Every widget mutation uses the same serialization rule | Open |
| E-08 | High | Make Wi-Fi UI state consume and display later disconnect/loss events | Label, color and SSID return to an accurate disconnected state | Open |
| E-09 | Medium | Define synchronization/ownership for shared `app_state_t` access | Shared reads/writes are protected or replaced by clear snapshot/message ownership | Open |
| E-10 | Medium | Complete Settings `System status` and `Last data update` instead of returning constant placeholders | All five Settings fields reflect live, defined state | Open |
| E-11 | Medium | Update or remove UART `sensor_ok` and `update_counter` placeholders | `status` reports maintained fields or clearly omits them | Open |
| E-12 | Medium | Resolve the fetch-interval contradiction: default/fetcher accept `1`, while UART currently accepts only `2-1440` despite saying `1-1440` | Code, validation text, default and docs agree on one range | Open |
| E-13 | Medium | Define rollback when an accepted runtime configuration change fails to persist to NVS | Runtime and reboot-restored values cannot silently diverge, or divergence is reported explicitly | Open |
| E-14 | Medium | Preserve the agreed timestamp format without truncating offsets at 19 characters | Full valid timestamps survive server-to-ESP transfer | Blocked by D-07 |
| E-15 | Medium | Parse `uv_index` using the producer's agreed JSON type | Nonzero and fractional fixtures behave as defined | Blocked by S-08 |
| E-16 | Medium | Move the hard-coded property ID and concrete server address into an appropriate configuration/identity mechanism | Builds are environment-portable and do not require source edits for endpoint/property selection | Blocked by D-02 and D-04 |
| E-17 | Low | Check and report task-creation and LEOP-initialization failures in startup orchestration | Startup cannot silently continue after a required task/module fails | Open |
| E-18 | Low | Decide whether dropped depth-one queue sends need counters, replacement behavior or diagnostics | Important state loss is observable and tested | Open |
| E-19 | Low | Distinguish cached/live data and data age in the UI where useful | Users can tell connectivity from freshness and cached from current snapshots | Blocked by S-10 |

## Cross-project integration and test work

| ID | Priority | Work item | Completion check | Status |
| --- | --- | --- | --- | --- |
| T-01 | High | Add coordinated contract fixtures for all three datasets | Producer and ESP tests share valid, empty, malformed, wrong-type, oversized and unknown-property cases | Open |
| T-02 | High | Implement and test route migration in both repositories | Old/new behavior follows D-05 without an undocumented breaking window | Blocked by D-05 |
| T-03 | High | Implement two-way registration end to end after the design/security gates | Authorized device can safely create/update intended property data; failure and retry paths are tested | Blocked by D-02 to D-04 |
| T-04 | Medium | Add a unified Glennergy root test target or documented equivalent | One local command runs the supported server test suite and reports failures | Open |
| T-05 | Medium | Expand ESP automated coverage beyond existing parser/unit sources | CI covers build plus practical host/simulator tests; hardware-only claims remain separate | Open |
| T-06 | Medium | Add startup, restart, stale-data and partial-provider integration tests | Readiness/freshness and cache behavior are reproducible | Open |
| T-07 | Medium | Verify stable-production compatibility separately from `dev` | Installed revisions and observed behavior are recorded without secrets | Open |

## Documented constraints to review, not assumed defects

These behaviors may be acceptable for the intended product. Keep them visible
until they are deliberately accepted or replaced.

| ID | Current constraint | Review question | Status |
| --- | --- | --- | --- |
| R-01 | ESP fetches recommendation, weather and price synchronously without an individual-request retry | Is the current full-cycle retry/health cadence sufficient? | Open review |
| R-02 | The health probe reuses the recommendation URL rather than a dedicated readiness endpoint | Should health include server readiness and data freshness? | Open review |
| R-03 | Offline caches are loaded once per offline period | Should cache reload or age policy change during long outages? | Open review |
| R-04 | The server recognizes `OPTIONS`, but non-root targets still pass through normal route parsing | Is dependable CORS/preflight behavior required at the application layer or only at the reverse proxy? | Open review |
| R-05 | UART input longer than 127 characters is truncated and its output is human-oriented | Are stricter error reporting or a stable machine-readable diagnostic protocol needed? | Open review |
| R-06 | External reverse-proxy, TLS, DNS, firewall and public-host configuration live outside the repositories | Where should deployment configuration and its verification evidence be maintained? | Open review |

## Hardware and runtime verification

These are evidence tasks, not necessarily code defects.

| ID | Verification task | Status |
| --- | --- | --- |
| V-01 | Record exact board revision and fitted controller markings | Unverified |
| V-02 | Verify physical pin routing against an authoritative schematic | Unverified |
| V-03 | Verify display timing, color order, backlight and visual output | Unverified |
| V-04 | Verify touch orientation, accuracy, reset, interrupts and multi-touch | Unverified |
| V-05 | Verify shared-I2C clock, pull-ups and electrical margins | Unverified |
| V-06 | Verify BME280 presence, wiring, accuracy and recovery | Unverified |
| V-07 | Verify flash/PSRAM identity and behavior under load | Unverified |
| V-08 | Exercise Wi-Fi loss/recovery, cache fallback and UI state on authorized hardware | Unverified |
| V-09 | Verify the repository-defined systemd topology and readiness on an authorized non-production or controlled host | Unverified |

## Cleanup candidates requiring confirmation

Do not delete these merely because static searches found no active call. Confirm
build inclusion, generated-code ownership, teaching/test intent and replacement
history first.

| ID | Repository/path | Candidate action | Gate before change |
| --- | --- | --- | --- |
| C-01 | ESP `main/LEOP/leop.cpp`, `fake_leop.*` | Remove or archive legacy/fake LEOP path | Confirm no teaching/test or registration use |
| C-02 | ESP `main/fake/*`, `main/sensor/fake_sensor.*` | Remove, archive or formalize simulation support | Confirm intended simulation strategy |
| C-03 | ESP `main/hal/bme280_sensor.*` | Remove/archive V1 while V2 is active | Confirm no fallback requirement |
| C-04 | ESP `main/main.c` sample JSON | Remove unused example data | Confirm no generated/debug consumer |
| C-05 | ESP `main/CMakeLists.txt` | Remove duplicate/legacy source entries | Validate full build and link map |
| C-06 | ESP generated UI/comment blocks | Remove or archive commented legacy UI | Respect SquareLine regeneration ownership |
| C-07 | ESP board-support components | Review apparently unused components | Confirm component discovery and indirect dependencies |
| C-08 | Glennergy `API/Meteo/` | Archive/remove older excluded C implementation | Confirm historical/course value |
| C-09 | Glennergy `Client-CPP/` | Document, archive or remove standalone client | Identify manual/demo purpose |
| C-10 | Glennergy legacy HTTP/cache helpers | Remove or consolidate alternate paths | Complete build/reference audit |
| C-11 | Glennergy commented implementations/stubs | Remove after preserving useful design history | Review old TCP, Homesystem writes and Crontab stubs |
| C-12 | Glennergy tracked executables | Stop tracking reproducible binaries if appropriate | Confirm release/course requirements |
| C-13 | Both repositories' stale comments/plans | Correct, classify or archive | Do not mix proposed and implemented behavior |
| C-14 | Glennergy property examples | Remove duplicate/invalid examples | Coordinate with D-02 and D-06 |
| C-15 | ESP committed credential-like Wi-Fi comment | Remove and rotate if ever real/used | Do not reproduce the value in issues or docs |
| C-16 | ESP compiled deployment endpoint | Move into configuration | Coordinate with E-16; keep real address out of public docs |

## Documentation updates required when closing items

At minimum, update the following when relevant:

- [Current limitations](current-limitations.md)
- [ESP interface contract](interface-contract.md)
- [Connectivity](connectivity.md)
- [Firmware architecture](architecture.md)
- [UI guide](ui-guide.md)
- [Configuration](configuration.md)
- Glennergy [HTTP API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md)
- Glennergy [server architecture](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/architecture.md)
- Glennergy [property configuration](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/property-configuration.md)
- Glennergy [security boundaries](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/security.md)

When an item is complete, remove contradictory current statements, preserve
important migration/history notes, update tests and examples, and revalidate
both sides of every changed interface.

<details>
<summary>Source and verification scope</summary>

This backlog consolidates the canonical documentation verified against
Glennergy-ESP `dev` snapshot `b5a502a` and Glennergy `dev` snapshot `42798be`,
plus the campaign cleanup register. It is based primarily on static repository
inspection. Hardware, production deployment and live endpoint behavior remain
unverified unless an item explicitly records later observed evidence.

</details>
