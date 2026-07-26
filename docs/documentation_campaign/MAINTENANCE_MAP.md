# Documentation maintenance map

| Metadata | Value |
| --- | --- |
| Status | Maintenance handoff complete; project-owner acceptance pending |
| Canonical cross-project owner | Glennergy-ESP |
| Source baseline | Glennergy-ESP `b5a502a`; Glennergy `42798be` |
| Audience | Maintainers, reviewers, and documentation contributors |

This map identifies documentation that must be reviewed when source,
configuration, workflows, or operating assumptions change. It assigns
repository/module ownership rather than naming an individual who may later
leave the project.

The Glennergy-ESP repository owns shared system terminology and the
cross-project contract. Glennergy owns server implementation, deployment, and
operations guides. Each repository owns its root README and source-level
Doxygen comments.

## Change-to-document map

| Changed area | Primary owner | Required document review | Minimum checks |
| --- | --- | --- | --- |
| Server HTTP parser, routes, statuses, or serialization (`Server/`) | Glennergy server maintainer | ESP [interface contract](../interface-contract.md), [system context](../system-context.md), [limitations](../current-limitations.md), [connectivity](../connectivity.md); Glennergy README, `Docs/troubleshooting.md`, `Docs/security.md` | Producer/consumer schema comparison, examples parse, links, secret/address scan |
| ESP HTTP client, response structs, or JSON parser (`main/HTTP.*`, `main/JSONParser/`, `main/LEOP/*`) | Glennergy-ESP connectivity maintainer | [Interface contract](../interface-contract.md), [connectivity](../connectivity.md), [limitations](../current-limitations.md), [troubleshooting](../troubleshooting.md), [glossary](../glossary.md) | Compare all three routes and payloads against server; bounds/types/timestamp review |
| Property identity, registration, authentication, or authorization in either repository | Cross-project maintainers; ESP documentation owner coordinates | [System context](../system-context.md), [interface contract](../interface-contract.md), [limitations](../current-limitations.md), [glossary](../glossary.md), [connectivity](../connectivity.md); Glennergy `Docs/property-configuration.md` and `Docs/security.md` | Threat/compatibility review, migration and failure cases, no invented planned schema |
| Recommendation algorithm or wire meaning (`Algorithm/average.*`, `Algorithm/main.c`) | Glennergy algorithm maintainer | [Interface contract](../interface-contract.md), [glossary](../glossary.md), [limitations](../current-limitations.md); Glennergy README/troubleshooting | Verify emitted value, type, range, units, and ESP interpretation; owner decision if semantics change |
| Server process topology, schedules, IPC structs, paths, or SHM/semaphore ownership | Glennergy runtime maintainer | Glennergy `Docs/architecture.md`, `Docs/operations.md`, `Docs/troubleshooting.md`, `Docs/security.md`; ESP [system context](../system-context.md) and [interface contract](../interface-contract.md) where externally visible | Build all five binaries, ABI/layout review, systemd verification, Mermaid render |
| Server property schema, parser, five-property limit, or seed JSON | Glennergy configuration maintainer | Glennergy `Docs/property-configuration.md`, README, troubleshooting; ESP [limitations](../current-limitations.md), [glossary](../glossary.md), and contract if IDs change | Validate both InputCache and Meteo parsers, seed examples, error propagation, capacity |
| Server Makefiles, deploy/verify/uninstall scripts, systemd units, or permissions | Glennergy operations maintainer | Glennergy `Docs/development.md`, `Docs/operations.md`, `Docs/security.md`, `Docs/troubleshooting.md`, README | Safe local build/checks; shell/static review; installed-host commands only with authorization |
| ESP startup, tasks, queues, callbacks, or shared `app_state_t` | Glennergy-ESP firmware maintainer | [Firmware architecture](../architecture.md), [connectivity](../connectivity.md), [limitations](../current-limitations.md), [troubleshooting](../troubleshooting.md) | Task/queue producer-consumer trace, context/locking review, Mermaid render, ESP-IDF build |
| Wi-Fi lifecycle, credentials, reconnect, SNTP, or queue semantics | Glennergy-ESP connectivity maintainer | [Connectivity](../connectivity.md), [configuration](../configuration.md), [UI guide](../ui-guide.md), [architecture](../architecture.md), [troubleshooting](../troubleshooting.md), limitations | Event/task-context trace, queue-full behavior, NVS path, secret scan, authorized runtime test if needed |
| LEOP cadence, state, health probe, cache, or retry behavior | Glennergy-ESP connectivity maintainer | [Connectivity](../connectivity.md), [interface contract](../interface-contract.md), [UI guide](../ui-guide.md), [troubleshooting](../troubleshooting.md), limitations | Deadline/state trace, offline/online cache cases, status meanings, Mermaid render |
| NVS namespaces, keys, defaults, validation, persistence, erase, or reboot behavior | Glennergy-ESP configuration maintainer | [Configuration](../configuration.md), [UART reference](../UART_COMMANDS.md), [troubleshooting](../troubleshooting.md), limitations | Type/key/default table comparison, setter range tests, persistence-failure and erase safety review |
| UART parser, commands, diagnostics, or output | Glennergy-ESP UART maintainer | [UART reference](../UART_COMMANDS.md), [configuration](../configuration.md), [architecture](../architecture.md), [troubleshooting](../troubleshooting.md) | Exact parser tokens/ranges, mutation classification, output/source comparison, build |
| Board, display, touch, I2C, BME280, pins, or ESP-IDF target/config | Glennergy-ESP hardware maintainer | [Hardware guide](../hardware.md), [development guide](../development.md), [architecture](../architecture.md), [troubleshooting](../troubleshooting.md), root README | Source/config comparison, static-vs-observed labels, ESP-IDF build; hardware test only when authorized |
| LVGL screens, object names, update task, status colors, or Settings fields | Glennergy-ESP UI maintainer | [UI guide](../ui-guide.md), [architecture](../architecture.md), [limitations](../current-limitations.md), [troubleshooting](../troubleshooting.md) | All-screen inventory, queue/state trace, LVGL lock review, authorized visual test |
| SquareLine project or generated `ui/` export | Glennergy-ESP UI maintainer | [Development guide](../development.md), [architecture](../architecture.md), [UI guide](../ui-guide.md) | Preserve custom integration, generated-object reference search, build, reviewed generated diff |
| ESP-IDF version, component manifest/lock, CI build, partitions, flash workflow | Glennergy-ESP build maintainer | [Development guide](../development.md), [hardware guide](../hardware.md), [troubleshooting](../troubleshooting.md), ESP build-workflow plan | Clean build, dependency/config drift review; flashing remains authorization-gated |
| Canonical terminology or document location | ESP documentation owner with both repository maintainers | [Glossary](../glossary.md), [documentation index](../README.md), both root READMEs and Glennergy `Docs/README.md` | Cross-repository link search, duplicate-canonical-source review, disposition update |
| Doxygen workflows, standards, templates, or generated output | Owning repository's documentation/tooling maintainer | Repository Doxygen guide/standard/TODO; this maintenance map | Workflow syntax, source-comment scope, generated output classified noncanonical, secret-safe action review |
| Historical plans, stale API docs, presentation snapshots, or cleanup candidates | Owning repository maintainer; documentation owner classifies | [Document disposition register](DOCUMENT_DISPOSITION.md), [cleanup register](CLEANUP_CANDIDATES.md), relevant canonical successor | Preserve unique evidence, add banner/archive decision; deletion only with separate approval |

## Canonical ownership boundaries

| Topic | Canonical location | Duplication rule |
| --- | --- | --- |
| Whole-system purpose and boundary | [System context](../system-context.md) | Repository READMEs summarize and link; they do not redefine the system |
| Shared terms, identifiers, and units | [Glossary](../glossary.md) | Code identifiers may retain legacy spelling; prose uses preferred terms |
| Current ESP↔Glennergy interface | [Interface contract](../interface-contract.md) | Component docs may show a short example but must link for exact schemas |
| Cross-project limitations and planned direction | [Current limitations](../current-limitations.md) | Plans must not be presented as implemented behavior |
| Firmware implementation | Glennergy-ESP `docs/` guides | Server docs link only when cross-project context is needed |
| Server implementation and operations | Glennergy `Docs/` guides | ESP owns no duplicate server operations manual |
| Generated API reference | Doxygen output in each repository | Derived reference is not canonical narrative architecture |
| Campaign evidence and audits | `docs/documentation_campaign/` | Governance records support, but do not replace, reader-facing guides |

## Review workflow

For a documentation-affecting source change:

1. Identify affected rows above during change planning.
2. Reinspect authoritative `dev`; do not copy behavior from an old plan.
3. Update the evidence register and material claim ledger when a claim changes.
4. Update every canonical owner before adjusting summaries or examples.
5. Run applicable checks from the validation matrix.
6. Record any runtime, hardware, production, external-service, or rendering
   check that could not be performed.
7. Independently review cross-project, safety-sensitive, destructive, secret,
   and current-versus-planned claims.
8. Update the document disposition register when a competing document changes
   status.

Build success is not runtime proof. Static source review is not hardware or
production proof. A placeholder address must remain in public documentation
unless the publication decision is explicitly changed.

## Periodic drift review

Run a focused drift review after endpoint/schema changes, ESP-IDF upgrades,
server deployment changes, UI regeneration, or implementation of a currently
planned feature. Otherwise, a review at each release boundary is sufficient.
Compare fetched `origin/dev` and `origin/main` references: local branch names
may be stale and do not independently prove production state.

The review should confirm:

- every canonical document remains reachable from a documentation index;
- no archived or planning artifact silently became a competing source;
- README summaries still match deeper guides;
- example commands remain safe and correctly scoped;
- secrets, private keys, real endpoint addresses, and customer/property data
  remain absent;
- deferred validation and cleanup items remain visible until resolved.
