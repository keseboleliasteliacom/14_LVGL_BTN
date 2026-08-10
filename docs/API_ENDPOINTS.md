# Glennergy HTTP API from the ESP perspective

This former API draft is retained as a stable link, but its duplicated schemas
and implementation notes have been retired. They had fallen behind the current
server and firmware behavior.

Use these maintained references instead:

- [ESP interface contract](interface-contract.md) — every current endpoint,
  field, status behavior, parser consequence, cache rule, and compatibility
  issue relevant to Glennergy-ESP.
- [Connectivity guide](connectivity.md) — request cadence, health checks,
  retries, offline cache, reconnection, and UI publication.
- [Glennergy server HTTP API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md)
  — complete server-side route, method, status, header, and schema behavior.

The implemented firmware currently uses the transitional
`/id=<integer>?<command>` route family and accepts at most 128 objects per
dataset. The intended command-first routes and device/property registration are
planned, not implemented.

Do not add a second schema definition here. Update the interface contract and
the server-owned API reference together when the boundary changes.
