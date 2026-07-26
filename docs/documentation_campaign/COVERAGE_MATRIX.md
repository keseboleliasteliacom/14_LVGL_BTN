# Documentation Coverage Matrix

Status: Initial target set; Phase 1 inventory pending.

| Artifact/area | Audience | Current source | Target state | Evidence status | Validation status |
| --- | --- | --- | --- | --- | --- |
| Shared system context | All | Existing READMEs/plans and code | Canonical overview | Drafted with claim ledger | Whitespace/link/secret scan pass; Mermaid render pending |
| Cross-project interface | General and technical developers | Both HTTP implementations and old API docs | Overview plus exact current/planned contract | Drafted and independently corrected | Whitespace/link/secret scan pass; runtime/schema fixtures pending |
| Glennergy README | New developers/operators | `glennergy/README.md` | Approachable current entry point | Audited | Design pending |
| Glennergy-ESP README | New/embedded developers | `Glennergy-ESP/README.md` | Approachable current entry point | Audited; replacement high priority | Design pending |
| Server architecture | Server developers/operators | Code, systemd, scripts and plans | Verified detailed document | Discovery complete | Design pending |
| Firmware architecture | Embedded developers | Code, tasks, queues and UI | Verified detailed document | Discovery complete | Design pending |
| Setup/build/deployment | Developers/operators | Scripts and workflows | Verified commands and prerequisites | Discovery complete | Command execution pending |
| Configuration | Developers/operators | Code and examples | Safe overview plus reference | Discovery complete; ID/capacity decisions pending | Design pending |
| Runtime diagrams | Mixed | Code paths and existing assets | Rendered diagrams as code | Flows traced | Design/render pending |
| Limitations/planned work | All | Owner notes, code and TODOs | Clearly separated from current behavior | Canonical draft complete | Independent current/planned review pass; ongoing |
| UART/configuration reference | Embedded developers | UART code and stale command doc | Exact commands, validation and persistence | Discovery complete | Design pending |
| Security and secret handling | Developers/operators | Current transport/deployment boundaries | Current limitations and safe configuration | Discovery complete | Design pending |
| Cleanup register | Maintainers | Reference/build/doc audits | Evidence-ranked separate backlog | Initial candidates recorded | Separate authorization required |
