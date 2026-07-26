# Documentation Validation Matrix

Status: Proposed checks; execution begins with drafting

| Check | Applies to | Evidence produced | Gate |
| --- | --- | --- | --- |
| Current-snapshot check | All artifacts | Recorded `dev` SHA and drift review | Before drafting and final review |
| Claim-to-code review | Architecture, interface, config, troubleshooting | Reviewer findings linked to evidence IDs and symbols | Before artifact acceptance |
| Cross-project producer/consumer comparison | Interface contract and sequences | Schema/route/type comparison | Before interface acceptance |
| Markdown link validation | READMEs and indexes | Link-check output or manual record | Before final integration |
| Mermaid parsing/rendering | All diagrams | Successful rendered output | Before diagram acceptance |
| Command safety classification | Setup/operations/troubleshooting | Each command marked local, environment-dependent, hardware or production-sensitive | Before command execution |
| Safe local command verification | Build/config docs where available | Command, environment and result | Before claiming “verified” |
| Secret/address scan | All public docs | Search result for keys, private material and literal deployment addresses | Before commit and final review |
| Terminology consistency | All artifacts | Glossary comparison | Every artifact review |
| Current/planned separation | Interface, limitations and future work | Independent reviewer confirmation | Every artifact review |
| Newcomer review | READMEs and navigation | Fresh-context usability report | Before final acceptance |
| Independent final audit | Full campaign | Severity-ranked factual findings and resolution record | Phase 4 completion gate |

Validation must match the claim. A successful Markdown render does not prove an
architecture statement, and a successful build does not prove deployment or
hardware behavior.
