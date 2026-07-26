# Documentation Coverage Matrix

Status: Canonical artifact set drafted and independently reviewed; final owner
acceptance and Mermaid render/readability decision pending.

| Artifact/area | Audience | Current source | Target state | Evidence status | Validation status |
| --- | --- | --- | --- | --- | --- |
| Shared system context | All | Existing READMEs/plans and code | Canonical overview | Drafted and independently reviewed | Whitespace/link/secret scan pass; Mermaid render pending |
| Cross-project interface | General and technical developers | Both HTTP implementations and old API docs | Overview plus exact current/planned contract | Drafted and independently corrected | Whitespace/link/secret scan and JSON-example parse pass; runtime/schema fixtures pending |
| Glennergy README | New developers/operators | `glennergy/README.md` | Approachable current entry point | Rewritten and independently reviewed | Whitespace/link/secret and safety review pass |
| Glennergy-ESP README | New/embedded developers | `Glennergy-ESP/README.md` | Approachable current entry point | Rewritten and independently reviewed | Whitespace/link/secret and safety review pass |
| Server architecture | Server developers/operators | Code, systemd, scripts and plans | Verified detailed document | Drafted and independently corrected | Whitespace/link/secret scan and semantic review pass; Mermaid render pending |
| Firmware architecture | Embedded developers | Code, tasks, queues and UI | Verified detailed document | Drafted and independently corrected | Whitespace/link/secret scan and semantic review pass; Mermaid render pending |
| Setup/build/deployment | Developers/operators | Scripts and workflows | Verified commands and prerequisites | Server and firmware development/operations guides complete | Static/source and safety review pass; toolchain/host command execution pending |
| Configuration | Developers/operators | Code and examples | Safe overview plus reference | ESP NVS and server property guides complete; identity unknowns explicit | Whitespace/link/example/semantic review pass; runtime persistence tests pending |
| Runtime diagrams | Mixed | Code paths and existing assets | Major current flows represented as Mermaid source | Core source diagrams drafted and semantically reviewed | Renderer unavailable; final render/readability inspection pending |
| Limitations/planned work | All | Owner notes, code and TODOs | Clearly separated from current behavior | Canonical guide complete | Independent current/planned review pass; update as product work changes |
| UART/configuration reference | Embedded developers | UART code and former stale command doc | Exact commands, validation and persistence | Rewritten from parser/NVS evidence | Whitespace/link/secret and semantic review pass; device execution pending |
| Security and secret handling | Developers/operators | Current transport/deployment boundaries | Current limitations and safe configuration | Canonical server security guide and cross-project limitations complete | Independent safety/secret/address review pass; external host controls unverified |
| Cleanup register | Maintainers | Reference/build/doc audits | Evidence-ranked separate backlog | Candidates and source-disclosure findings recorded | Separate authorization required; no deletion/source cleanup performed |
