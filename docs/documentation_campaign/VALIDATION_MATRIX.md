# Documentation Validation Matrix

Status: Proposed executable checks; execution begins with drafting

Every run records status as `not-run`, `pass`, `fail`, `blocked`, or `N/A`, plus
timestamp, executor/reviewer and retained evidence path.

| Check | Exact scope | Method/tool | Pass criterion | Evidence artifact | Gate |
| --- | --- | --- | --- | --- | --- |
| Snapshot/drift check | Both repos and each artifact’s applicability | Git refs/status/diff inventory | Authoritative `dev`, stable `origin/main`, campaign SHAs, merge relationship and dirty state recorded; material drift re-reviewed | `SNAPSHOT_DRIFT_LOG.md` plus command record | Before each artifact and final audit |
| Claim ledger | Every material factual claim | Full mapping for contracts/config; enumerated mapping for architecture/operations | Claim maps to repo, SHA, path, symbol, evidence strength and validation status; negative/exhaustive claims have inventory evidence | `CLAIM_LEDGER.md` | Before artifact acceptance |
| Coverage trace | Baseline objectives, audiences, runtime areas, happy/failure/security/limits | Bidirectional coverage ledger | No unexplained evidence-without-doc or doc-claim-without-evidence orphan; deferred gaps have rationale | Expanded coverage matrix/final report | Design and final gates |
| Cross-project contract | Routes, methods, queries, statuses, schemas, exact keys/types/nullability/units/timestamps/defaults/limits | Producer/consumer comparison; parse JSON/examples or fixtures where feasible | Both pinned implementations agree or incompatibility is explicitly documented | Contract validation report | Before interface acceptance |
| Internal links and paths | Relative links, anchors, case, images and cross-repo targets | Pinned link checker selected during implementation; Linux-case file audit | All internal links pass in intended GitHub branch/PR context | Link-check log | Before integration |
| External links | Public references only | Separate network link run with retries/manual disposition | Failures investigated and marked transient, corrected or deferred | External-link report | Final review, non-flaky policy |
| Mermaid syntax | Every Mermaid block in both repos | Pinned GitHub-compatible renderer | Every block renders; output/log retained | Render log and derived preview | Before diagram acceptance |
| Diagram semantics/accessibility | Every diagram | Independent node/edge/order/status checklist; light/dark/readability review | Diagram matches enumerated code evidence; planned diagrams visibly say PLANNED; textual equivalent exists | Diagram review report | Before diagram acceptance |
| Command inventory/safety | Every command block | Extract/manual ledger; optional ShellCheck/syntax checks | cwd, shell/OS, prerequisites, effects, rollback, risk class and verification status recorded | Command ledger | Before any execution/publication |
| Safe command verification | Local non-production/non-hardware commands only | Disposable/clean environment execution where available | Version, command, exit code, timestamp and relevant output captured; claims scoped to what ran | Command execution log | Before “verified” label |
| Secret/address scan | Added/changed public docs, SVG/image metadata and final branch diff | Gitleaks or equivalent plus targeted private-key/token/Wi-Fi/IP/hostname patterns; reviewed allowlist | No newly exposed secret or unapproved deployment address; existing findings reported without value reproduction | Scan log and false-positive disposition | Before each commit/publication and final review |
| Current/planned separation | Prose, tables, examples and diagrams | Standard-label checklist plus independent semantic review | Planned routes/payloads absent from current quick-start/reference examples; every item labelled consistently | Separation review | Every artifact review |
| Terminology/duplicate facts | Both repos, README/deep docs/Doxygen | Canonical term/identifier/unit/route table and conflict search | Preferred prose and exact identifiers are consistent; duplicates link to canonical owner | Consistency report | Every artifact review |
| Markdown/style/examples | All Markdown/code/JSON | Pinned Markdown linter; heading uniqueness; spell/style project dictionary; JSON/example parsing | No unexplained lint/path/case/example error; identifiers excluded appropriately | Lint/example logs | Before integration |
| Doxygen integration | Linked/generated API docs only | Doxygen generation and warning/link review when environment available | Generation succeeds without campaign-introduced warning or is labelled unverified with reason | Doxygen log | Before claiming generated docs verified |
| Navigation reachability | Both root READMEs and indexes | Link graph/manual route inventory | Every canonical artifact reachable from the appropriate root README; generated/history docs classified | Navigation report | Before final acceptance |
| Newcomer usability | Fresh developer and operator/evaluator contexts | Clean-context task rubric | Reviewer identifies purpose/relationship/current-vs-planned, finds API/config/troubleshooting, and completes safe applicable setup tasks; blockers dispositioned | Newcomer reports | Before final acceptance |
| Independent final audit | Entire campaign at pinned SHAs | Fresh agent/context; severity rubric; finding disposition and retest | Zero unresolved critical/high factual or safety findings; lower deferrals appear in limitations/final coverage | Final audit report | Phase 4 completion gate |

Validation must match the claim. A successful Markdown render does not prove an
architecture statement, and a successful build does not prove deployment or
hardware behavior.

Static code inspection proves implemented structure, not live production or
hardware behavior. Runtime/hardware claims require observed evidence or an
explicit `not runtime verified` label.
