# Owner Notes Register

Owner notes provide intent and observed context. They must be verified against
code where possible and must not be silently presented as code-confirmed facts.

| ID | Area | Owner note | Initial classification | Verification needed |
| --- | --- | --- | --- | --- |
| ON-001 | Branch authority | `dev` is authoritative; `main` is stable production | Owner decision | Git snapshots recorded |
| ON-002 | System relationship | Glennergy acts as the LEOP server for Glennergy-ESP | Owner-confirmed context | Trace both repositories |
| ON-003 | External scope | No implemented components outside the repositories are currently known | Owner-reported | Inventory integrations/configuration |
| ON-004 | Settings UI | Three of five fields work; two remain on the TODO list | Partially implemented | Identify all fields and behavior |
| ON-005 | Property data | The current flow uses a premade/example property and data | Temporary/example behavior | Trace producer, storage, and consumers |
| ON-006 | Two-way communication | ESP property registration with Glennergy is planned | Planned | Design separately after current contract discovery |
| ON-007 | Property persistence | Glennergy should save registered property information in `Glennergy-Fastigheter.json` | Planned | Verify filename, schema, concurrency and write path |
| ON-008 | Recommendation endpoint | Current form is `id=3?recommendation`; intended form is `recommendation?id=3` | Implemented/transitional and planned | Verify literal routing and both consumers |
| ON-009 | Wi-Fi UI | `WifiValueLabel` does not change colour by status like `LeopValueLabel` | Known unfinished behavior | Verify generated/manual UI code and runtime |
| ON-010 | Cleanup | Both repositories contain obsolete comments and suspected unused material | Cleanup candidates | Static/reference analysis; no deletion implied |
| ON-011 | Completion target | Aim for roughly 90% final documentation while code is roughly 95% complete | Campaign success target | Track through coverage matrix |
