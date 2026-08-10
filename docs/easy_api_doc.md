# Quick HTTP integration overview

Glennergy-ESP requests three read-only datasets from the Glennergy LEOP server:

| Dataset | Used by |
| --- | --- |
| Recommendation | Electricity screen and last-update status |
| Weather | Weather screen |
| Electricity price | Electricity screen |

The firmware initiates the requests, parses up to 128 objects per dataset, and
publishes latest-value snapshots to the UI. It can attempt to reuse SPIFFS
caches while Wi-Fi is unavailable. A connected status does not by itself prove
that every dataset is fresh.

For exact routes, JSON fields, status behavior, parsing limitations, and cache
consequences, read the [ESP interface contract](interface-contract.md). For the
complete producer-side API, read the
[Glennergy server HTTP API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md).

The route order, identity model, authentication, and future two-way property
registration remain planned work. Public examples intentionally omit the real
server address.
