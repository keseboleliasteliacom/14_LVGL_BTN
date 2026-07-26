# Cleanup Candidate Register

This register records suspected obsolete or unused material discovered while
documenting. Inclusion does not authorize deletion or source behavior changes.

| ID | Repository/path | Candidate issue | Evidence | Risk/next verification | Status |
| --- | --- | --- | --- | --- | --- |
| CC-001 | ESP `main/LEOP/leop.cpp`, `fake_leop.*` | Legacy/fake LEOP path compiled but not active | Active task uses `LEOPFetcher_Work`; references are internal/legacy | Confirm teaching/test intent and link map | Strong candidate |
| CC-002 | ESP `main/fake/*`, `main/sensor/fake_sensor.*` | Fake config/status/sensor modules apparently inactive | Calls absent/commented; real modules active | Confirm desired simulation support | Strong candidate |
| CC-003 | ESP `main/hal/bme280_sensor.*` | BME280 V1 retained while V2 is active | V1 instantiation commented; V2 used by sensor task | Confirm fallback/history value | Strong candidate |
| CC-004 | ESP `main/main.c` sample JSON | Unused hard-coded sample data | No active consumer found | Verify generated/debug use | Strong candidate |
| CC-005 | ESP `main/CMakeLists.txt` | Duplicate source entries and legacy/current paths compiled | Source list inspection | Build/link-map validation | Review candidate |
| CC-006 | ESP generated UI/comment blocks | Large commented legacy screen implementation | `ui/screens/ui_Screen1.c` | SquareLine regeneration ownership | Review candidate |
| CC-007 | ESP board-support components | Several components have no first-party app reference | Repository reference search | Component discovery/vendor dependencies make deletion risky | Weak candidate |
| CC-008 | Glennergy `API/Meteo/` | Older C implementation excluded from production build | Root Makefile selects `API/Meteocpp` | Confirm historical/teaching value | Confirmed non-production |
| CC-009 | Glennergy `Client-CPP/` | Standalone client excluded from production graph | Root Makefile and reference search | Identify manual/demo purpose | Review candidate |
| CC-010 | Glennergy legacy HTTP/cache helpers | Alternate/wrapper paths appear unused | Active connection path uses `Server/HTTP/HTTPRequest.*` | Full build/reference audit | Review candidate |
| CC-011 | Glennergy commented implementations/stubs | Old TCP server, Homesystem writes, Crontab stubs | Source inspection | Preserve useful history elsewhere first | Strong candidate |
| CC-012 | Glennergy tracked executables | Reproducible binary artifacts tracked in source tree | Git file inventory | Confirm release/course requirements | Repository hygiene candidate |
| CC-013 | Both repositories’ stale comments/plans | Implemented and proposed behavior are mixed | Documentation audits | Classify/archive before removing | Documentation cleanup |
| CC-014 | Glennergy property examples | Duplicate legacy JSON and invalid/mismatched seed entries | Schema/config inspection | Depends on property ID/capacity decisions | Data cleanup candidate |
