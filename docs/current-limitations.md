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
| UI | Three of five Settings fields work; Wi-Fi status can remain visually connected after loss |
| HTTP | Current routes are backwards, unauthenticated and plain HTTP |
| Compatibility | Timestamp, UV type, cache validation and bounds behavior have known gaps |
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

Three of five Settings fields are implemented:

- uptime;
- restart reason;
- time synchronization.

Two fields remain placeholders:

- system status displays `Starting...`;
- last sensor update displays `No data yet`.

### Wi-Fi status display

The Wi-Fi screen changes from red/disconnected to green/connected after a
successful connection. It does not currently consume a corresponding
disconnect/loss update, so it can remain green after connectivity is lost. The
LEOP status display has a more complete changing-state model.

### Shared application state

Several firmware tasks read or write sections of the shared `app_state_t`
without a common mutex. Depth-one queues provide snapshots for selected UI
flows, but they do not protect every shared-state access. This is an implemented
concurrency limitation, not evidence that a race has been reproduced.

### Task-name arguments

UART and Sensor task creation currently pass the address of their task-name
pointer rather than the string pointer expected by `xTaskCreate`. Their intended
metadata names are `UART` and `Sensor`, but the runtime task names can be invalid
or garbage until those calls are corrected.

### LVGL locking

Most tab updates run under the UI task's LVGL lock. The Wi-Fi connected-result
branch currently changes labels and colors without acquiring that lock. This is
a concrete unlocked widget-update path, not merely an unverified concurrency
risk.

## Current interface limitations

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
- Complete the two placeholder Settings fields.
- Make the Wi-Fi status display track disconnection.
- Resolve recommendation-result semantics.
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
| Applies to | Glennergy-ESP `dev` at `b5a502a` and Glennergy `dev` at `42798be` |
| Verification boundary | Static repository inspection; unresolved behavior remains explicitly unresolved |

</details>

Update this page whenever one of these limitations is fixed, reclassified,
reproduced at runtime, or replaced by an approved design. Move resolved items
into release/history notes rather than leaving contradictory current statements.
