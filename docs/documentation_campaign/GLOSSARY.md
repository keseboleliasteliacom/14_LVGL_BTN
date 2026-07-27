# Shared Glossary

Status: Phase 1 candidates; unresolved terms require owner decisions.

| Term | Candidate meaning | Status/evidence |
| --- | --- | --- |
| Glennergy | Linux server stack acting as the LEOP server | Owner- and code-confirmed |
| Glennergy-ESP | ESP32-S3 firmware that reads data from Glennergy | Owner- and code-confirmed; registration is planned |
| LEOP | Project term for the Glennergy-facing data/service integration | Exact expansion remains unknown |
| Property | A configured installation/building represented by `Homesystem_t` and property JSON | Canonical English term and ID ownership unresolved |
| Fastighet | Swedish term appearing in property configuration filenames | Prefer an agreed English term in prose |
| Homesystem | Current server code type for one configured property/system | Reconcile with “property” terminology |
| InputCache | In-memory aggregation process and local IPC server | Confirmed |
| Meteo | Weather-data producer; C++ implementation is production | Confirmed |
| Spotpris | Swedish electricity spot-price producer | Confirmed |
| Algorithm / Algoritm | Calculation process; identifiers retain Swedish spelling in places | Prefer “Algorithm” in prose; preserve identifiers exactly |
| Recommendation | API dataset/field whose intended score/type semantics remain deliberately unresolved | Current implementation evidence must be stated without inventing intent |
| Device identity | Planned UUID-like identifier unique to each ESP32-S3 unit | Exact source, provisioning, property mapping and authorization are not designed |
| Property ID | Current API uses temporary unauthenticated incrementing integers | Distinguish from planned device identity |
| Electricity area | Swedish price zone `SE1` through `SE4` | Confirmed |
| Quarter-hour sample | One 15-minute interval; current result arrays contain 96 samples | Confirmed |
| Live data | Data fetched from the server during current connectivity | Confirmed concept |
| Cached data | JSON snapshots stored in ESP SPIFFS and used while offline | Confirmed concept |
| Connected / Degraded / Unavailable | LEOP states derived from category fetch/probe results | Confirmed |
| Authoritative implementation | Current `dev` snapshot | Owner decision |
| Stable production | Current remote `main` snapshot | Owner decision |
