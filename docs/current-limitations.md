# Current limitations and planned completion work

> **In short:** The system is close to complete, but identity/registration,
> endpoint shape, recommendation meaning and parts of the UI are not final.

Glennergy is close to its intended implementation, but several features and
interfaces remain partial, temporary or unresolved. This page prevents those
gaps from being mistaken for completed behavior. It is not an authorization to
change or delete source code.

## Limitations at a glance

| Area | What to know first |
| --- | --- |
| Property selection | The ESP uses temporary integer property ID `2`; UUID-like device identity and registration are planned |
| Capacity | Five properties is a temporary test limit |
| Recommendation | The API separates continuous `score` from the quartile-based buy/hold/sell category |
| UI | Settings System Status remains a placeholder; command-queue delivery errors are not shown |
| HTTP | Current routes are backwards, unauthenticated and plain HTTP |
| Compatibility | Timestamp, UV type, cache validation and bounds behavior have known gaps |
| Recovery | Reconnection can report LEOP connected before the independently scheduled full dataset refresh |
| Responsiveness | Time synchronization after obtaining an IP can hold the default event-loop callback for about ten seconds |
| Verification | Full hardware, production and stable end-to-end behavior was not tested in this campaign |

The sections below explain each item precisely and distinguish confirmed
behavior from planned work.

## Temporary behavior

### Example property and integer ID

Glennergy-ESP currently requests data for hard-coded property ID `2`. Glennergy
selects results through an unauthenticated integer in the current request path.
This is temporary test behavior.

The intended direction is a UUID-like identity unique to each ESP32-S3 unit.
The UUID source, provisioning, mapping between device and property,
authentication, authorization, registration payload, migration and recovery
process have not been designed. Documentation must not imply otherwise.

### Five-property processing limit

Important Glennergy runtime structures currently process at most five
properties. The owner has classified this as a temporary test limit, not final
product capacity. The repository seed contains more entries, and several later
entries do not satisfy the schema consumed by the active code.

## Partially implemented firmware behavior

### Settings screen

Four of five Settings fields are implemented:

- uptime;
- restart reason;
- age since the last successful recommendation fetch;
- time synchronization.

One field remains a placeholder:

- system status displays `Starting...`;

The `Last data update` row is not sensor data: it reports recommendation-fetch
age and shows `No data yet.` only until the first successful recommendation
response. The Settings configuration panel can persist sensor and LEOP update
interval presets; it reports partial failure when only one NVS write succeeds.

### Wi-Fi status display

The Wi-Fi screen now consumes connected, reconnecting, and disconnected states
and provides an intentional disconnect button. UI command sends remain
zero-wait operations: if the depth-one queue is full, scan, connect, or
disconnect input can be dropped without user-facing error.

### Shared application state

Several firmware tasks read or write sections of the shared `app_state_t`
without a common mutex. Depth-one queues provide snapshots for selected UI
flows, but they do not protect every shared-state access. This is an implemented
concurrency limitation, not evidence that a race has been reproduced.

In particular, the LEOP task can modify response counts and arrays while the
UART `leop` command reads and iterates the same containers directly.

### LVGL locking

The periodic UI task now places Wi-Fi and the other tab updates under its LVGL
lock. This protects that update path's widget mutations, but does not provide a
common mutex for the shared `app_state_t` values read by UI and other tasks.

## Current interface limitations

### Event-loop time synchronization

After Wi-Fi obtains an IP address, the ESP-IDF event callback starts time
synchronization when required and can wait for up to about ten seconds. This
can delay other work on the default event loop. Static inspection confirms the
blocking path; it does not establish a reproduced device failure.

### Reconnection and data freshness

Wi-Fi recovery makes the LEOP health deadline immediately due but preserves the
separate full-fetch deadline. A successful recommendation-route probe can
therefore publish `CONNECTED` while recommendation, weather, and price remain
unchanged until the next scheduled full fetch. Connectivity state must not be
interpreted as proof that all displayed datasets were just refreshed.

### Server property-configuration error propagation

The server property readers do not consistently turn unusable or empty
configuration into process failure. InputCache can convert a parser `-1` to an
unsigned count and report successful initialization, while Meteo treats zero
accepted properties as a successful no-op and can publish a zero-property
message. Service success alone therefore does not prove that usable property
data was loaded.

### Transitional endpoint structure

The implemented server grammar is:

```text
/id=<integer>?recommendation
/id=<integer>?weather
/id=<integer>?price
```

The planned route direction is command-first, for example:

```text
/recommendation?id=<integer>
```

The planned route does not work yet. Both repositories need a compatibility and
migration plan before changing the implemented contract.

### Transport and access control

The firmware currently uses plain HTTP and the API does not authenticate or
authorize requests. This is a significant prerequisite for future
state-changing property registration. Public documentation uses
`<LEOP_BASE_URL>` or `https://leop.example.com` and intentionally omits the real
VPS address.

### Timestamp truncation

Glennergy timestamps can include a timezone offset. Current ESP response fields
retain only 19 characters, so the offset is lost. Documentation must not claim
that the firmware preserves the full server timestamp.

### UV-index conversion

Glennergy converts the upstream value to integer storage and then serializes
`uv_index` as a JSON real. Glennergy-ESP reads that member using
`json_integer_value`, which expects a JSON integer. The current consumer can
therefore store zero for the server-emitted real-valued member; this is more
than a fractional-precision loss.

### Online error and cache behavior

Normal ESP category fetches primarily judge success from whether the response
body can be parsed; they do not reject a response solely from its HTTP status.
Malformed online responses do not automatically cause a cache fallback. Cache
loading is tied to offline/no-Wi-Fi behavior.

## Testing and verification limits

- Glennergy has build and deployment-verification tooling but no unified root
  `make test` target.
- Glennergy-ESP contains parser/unit-test sources and CI/build planning, but
  broader QEMU, Wokwi and hardware validation described in planning files is
  not current comprehensive test coverage.
- This documentation campaign can statically verify code structure and safely
  execute applicable local checks. It cannot claim live production or hardware
  behavior without separate observed evidence and authorization.

## Planned major work

- Correct the endpoint structure with a compatibility strategy.
- Design two-way device/property registration.
- Add a UUID-like device identity and appropriate authentication and
  authorization.
- Persist validated registration information safely on Glennergy.
- Complete the remaining System Status placeholder.
- Provide user-visible delivery/error feedback for Wi-Fi commands.
- Decide whether successful recovery should trigger an immediate full dataset
  fetch rather than waiting for the previous periodic deadline.
- Decide whether the current quartile recommendation categories are the final
  product policy.
- Remove or archive confirmed obsolete code/comments through separate reviewed
  cleanup changes.

## Cleanup candidates are separate

Both repositories contain legacy/fake modules, stale comments, historical
plans, generated material and possible unused code. These are tracked in the
documentation campaign’s cleanup register. A candidate is not considered safe
to delete merely because static reference searches did not find an active call.

## Update triggers

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Audience | Users, evaluators, developers, and maintainers |
| Applies to | Glennergy-ESP `dev` at `baf9b58d04e827f024c8975b140f7a417e462370` and Glennergy `dev` at `0048c08ed01fa385d114cd3461e2cad9d7aceb73` |
| Verification boundary | Static repository inspection; unresolved behavior remains explicitly unresolved |

</details>

Update this page whenever one of these limitations is fixed, reclassified,
reproduced at runtime, or replaced by an approved design. Move resolved items
into release/history notes rather than leaving contradictory current statements.
