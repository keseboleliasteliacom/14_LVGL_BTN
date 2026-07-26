# Existing Document Disposition Register

Status: Initial inventory; final action approved in Phase 2

Allowed dispositions: `retain-current`, `update`, `replace`, `archive`,
`redirect/banner`, and `delete-later` (deletion requires separate approval).

| Repository/path | Current role/risk | Proposed disposition | Canonical successor/action | Status |
| --- | --- | --- | --- | --- |
| ESP `README.md` | Former vendor-example entry point | Replace | Current Glennergy-ESP product README | Completed |
| ESP `docs/API_ENDPOINTS.md` | Detailed but stale API draft with production address | Replace; retain as noncanonical stale reference pending separate banner/archive cleanup | [Canonical interface contract](../interface-contract.md) | Classified; no deletion |
| ESP `docs/easy_api_doc.md` | Swedish duplicate, stale link/encoding | Retain as noncanonical stale reference pending separate banner/archive cleanup | [Canonical interface contract](../interface-contract.md) | Classified; no deletion |
| ESP `SIGNALFLOW.md` | Swedish/stale queue and loop behavior | Retain as noncanonical historical material | Firmware architecture/connectivity diagrams | Classified; user-owned untracked file untouched |
| ESP planning/roadmap files | Mixed historical/current intent | Retain as plans/history; canonical behavior lives in current guides | Limitations, plans/ADR classification, and current architecture | Classified; no deletion |
| ESP `docs/UART_COMMANDS.md` | Formerly stale command reference | Update | Canonical UART reference | Completed |
| ESP presentation assets/docs | Useful snapshots that may drift | Retain as presentation snapshots | Mermaid canonical diagrams | Classified as noncanonical snapshots |
| ESP Doxygen standards/workflows | Contributor tooling | Retain and cross-link | Documentation maintenance map | Retained tooling; portability review separate |
| Glennergy `README.md` | Former operations-heavy entry point | Update | Layered server README | Completed |
| Glennergy `SYSTEMD_MIGRATION_PLAN.md` | Historical plan mixed with implemented state | Retain as historical/mixed plan | Server architecture and operations | Classified; no deletion |
| Glennergy `AI_CONTEXT.md` | Partially stale agent orientation | Retain as stale contributor context pending separate update | Maintenance/contributor guidance | Classified; no deletion |
| Glennergy `Docs/architecture-overview.md` | Ignored/untracked and partly stale | Replace | System context/server architecture | Replaced by tracked canonical guides |
| Glennergy Doxygen standards/workflows | Contributor tooling with portability issues | Retain | Documentation maintenance map | Retained tooling; portability review separate |
| Generated `html/`/`latex/` | Derived API output | Keep generated/noncanonical | Doxygen source comments | Classified generated output |

No file is removed merely because this table proposes replacement or archival.
