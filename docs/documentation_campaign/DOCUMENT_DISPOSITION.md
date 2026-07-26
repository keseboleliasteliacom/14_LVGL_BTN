# Existing Document Disposition Register

Status: Initial inventory; final action approved in Phase 2

Allowed dispositions: `retain-current`, `update`, `replace`, `archive`,
`redirect/banner`, and `delete-later` (deletion requires separate approval).

| Repository/path | Current role/risk | Proposed disposition | Canonical successor/action | Status |
| --- | --- | --- | --- | --- |
| ESP `README.md` | Vendor-example entry point | Replace | Glennergy-ESP README design | Proposed |
| ESP `docs/API_ENDPOINTS.md` | Detailed but stale API draft with production address | Replace or update with banner | Canonical cross-project interface contract | Proposed |
| ESP `docs/easy_api_doc.md` | Swedish duplicate, stale link/encoding | Redirect/banner or archive | Interface overview/contract | Proposed |
| ESP `SIGNALFLOW.md` | Swedish/stale queue and loop behavior | Archive or replace | Firmware architecture/connectivity diagrams | Proposed |
| ESP planning/roadmap files | Mixed historical/current intent | Classify and archive/ADR selectively | Limitations, ADRs and current architecture | Proposed |
| ESP `docs/UART_COMMANDS.md` | Current reference with stale persistence claims | Update | Canonical UART reference | Proposed |
| ESP presentation assets/docs | Useful snapshots that may drift | Retain as presentation snapshots or replace derived views | Mermaid canonical diagrams | Proposed |
| ESP Doxygen standards/workflows | Contributor tooling | Retain and cross-link | Documentation maintenance map | Proposed |
| Glennergy `README.md` | Useful production operations entry point | Update | Layered server README | Proposed |
| Glennergy `SYSTEMD_MIGRATION_PLAN.md` | Historical plan mixed with implemented state | Archive/banner as historical decision record | Server architecture and operations | Proposed |
| Glennergy `AI_CONTEXT.md` | Partially stale agent orientation | Update or replace | Maintenance/contributor guidance | Proposed |
| Glennergy `Docs/architecture-overview.md` | Useful but ignored/untracked and partly stale | Replace with tracked canonical artifact | System context/server architecture | Proposed |
| Glennergy Doxygen standards/workflows | Contributor tooling with portability issues | Update selectively and retain | Documentation maintenance map | Proposed |
| Generated `html/`/`latex/` | Derived API output | Keep generated/noncanonical | Doxygen source comments | Proposed |

No file is removed merely because this table proposes replacement or archival.
