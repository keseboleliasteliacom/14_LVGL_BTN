# Troubleshooting Glennergy-ESP

> **In short:** Diagnose read-only first, separate cached data from live state,
> and confirm the exact device before any flash, restart or erase action.

Start with repository evidence and observations that do not change device or
external state. A successful build proves compilation only; it does not prove
boot, display, touch, Wi-Fi, sensor, or Glennergy behavior.

## Safety boundary

The following require confirmation of the exact development device, port,
authorization, recovery needs, and expected side effects before use:

- opening a serial port or capturing device logs;
- sending UART `config` or `reboot` commands;
- flashing firmware or using a CI flashing artifact;
- erasing flash, NVS, SPIFFS, partitions, or saved Wi-Fi credentials;
- power-cycling, rewiring, probing, or disconnecting a sensor;
- fault injection, repeated restart testing, or long soak tests;
- contacting a live Glennergy endpoint or using a real property identifier.

Do not use production endpoints, expose real addresses, retrieve secrets, or
print Wi-Fi credentials, API keys, tokens, SSH keys, or private data while
following this guide. This page intentionally provides no erase or destructive
fault-injection commands.

## Read-only-first workflow

1. Record the exact source revision and whether the symptom is source-derived
   or observed on authorized hardware.
2. Inspect the relevant current code/configuration and [known limitations](current-limitations.md).
3. Build locally with `idf.py build` in an ESP-IDF 5.3.5 environment when
   available; do not flash yet.
4. Compare the symptom with the status semantics in the [UI guide](ui-guide.md)
   and [firmware architecture](architecture.md).
5. Only if needed and authorized, open a serial session and begin with the
   read-only commands in [UART diagnostics](UART_COMMANDS.md#suggested-read-only-triage).
6. Escalate to state-changing or physical tests only with a specific hypothesis
   and a recovery plan.

## Build fails

Check these static facts first:

- `idf.py --version` should be closest to the locked and CI baseline, 5.3.5;
- the target is ESP32-S3;
- managed components match `dependencies.lock`;
- the ESP-IDF environment/export script was loaded in the current shell;
- generated UI changes did not remove object names used by `ui/Tabs/`;
- unrelated changes to `sdkconfig`, `sdkconfig.defaults`, or the lock file are
  understood before regeneration.

Run `idf.py build` from the repository root. `idf.py set-target esp32s3` can
rewrite local configuration, so use it only when the workspace target is wrong
and review the resulting diff. There is no comprehensive routinely executed
test suite; a green build is the minimum automated evidence, not runtime proof.

## Device does not boot or the display remains blank

Static checks:

- confirm the intended target and board configuration in [hardware](hardware.md);
- inspect `app_main()` ordering: touch, two-second delay, LCD/backlight, LVGL,
  Wi-Fi, SPIFFS, UI, then tasks;
- identify initialization wrapped in `ESP_ERROR_CHECK`, because those failures
  can abort/restart the firmware;
- remember that several task-creation and LEOP-initialization return values are
  not coordinated by `app_main()`.

Do not infer a wiring or panel failure from source alone. Flashing, monitoring,
power cycling, checking supply rails, or probing signals requires the hardware
gate above. Record board revision, firmware SHA, power setup, and the first
failure/restart reason if an authorized observation is performed.

## Touch does not respond or coordinates are wrong

Current source configures GT911 on the shared I²C controller, interrupt GPIO 4,
and 1024 × 600 bounds. Static inspection cannot establish physical orientation,
calibration, interrupt polarity, or reset behavior. Check for accidental UI
regeneration and initialization-order changes first. Do not attach an I²C
scanner or rewire/reset the controller without authorization; the same bus is
used by board peripherals and the BME280 wrapper.

## Wi-Fi scan or connection does not update

Current behavior to account for:

- Wi-Fi command and result queues have depth one;
- several sends are non-blocking and can be dropped when a queue is full;
- connection becomes true only after the station obtains an IP address;
- reconnect delay grows from one second to a maximum of 30 seconds, for up to
  100 scheduled attempts;
- reconnecting and disconnected results should update both the tab and header;
- the disconnect button intentionally suppresses automatic reconnect, but its
  zero-wait queue send can be dropped if the command queue is full.

Therefore, a green Wi-Fi label is not sufficient evidence of current
connectivity. Compare the LEOP header state and, in an authorized serial
session, the read-only `status` output. Do not print saved credentials or NVS
contents. Changing or erasing credentials is a state-changing action outside
the read-only phase.

## LEOP shows No WiFi, Checking, Degraded, or Not connected

| UI state | Source-backed interpretation | First checks |
| --- | --- | --- |
| `No WiFi` | `WiFi_IsConnected()` is false | Diagnose Wi-Fi before server behavior |
| `Checking...` | Wi-Fi became available; full fetch or health check is pending | Allow for current fetch/timeout timing |
| `Degraded` | One or two of recommendation/weather/price succeeded | Identify which visible dataset updated; stale others may remain |
| `Not connected` | Three consecutive total fetch/probe failures were observed | Check transport and server availability without exposing endpoint details |
| `Connected` | Full fetch succeeded, or a health probe succeeded | Do not infer that every displayed dataset is fresh |

The firmware currently performs three synchronous plain-HTTP GETs on the full
fetch interval. It does not implement authentication, registration, or a
dedicated health route. Between full fetches it probes the recommendation
route; a successful probe can report connected without refreshing all data.
Exact routes and timeout/schema behavior belong in the
[interface contract](interface-contract.md).

Do not test against the real server or disclose its address without explicit
authorization. Static checks can still compare both repositories' route and
schema implementations.

## Values are stale, missing, or return after going offline

The LEOP task publishes latest-value depth-one snapshots. When Wi-Fi is absent,
it loads recommendation, weather, and price cache once per offline period.
Important limitations:

- the UI does not label a snapshot as network versus cache;
- intermediate queue values can be overwritten before the UI consumes them;
- malformed online responses do not automatically fall back to cache;
- response bodies can be written to SPIFFS before semantic parsing succeeds;
- a failed category can leave its previous visible values in place;
- status reflects connectivity classification, not per-dataset age.

Inspect source and non-sensitive status first. Reading or erasing SPIFFS on a
device is gated because it can expose or destroy persisted payloads.

## Sensor values are absent or stale

The active task waits three seconds, initializes BME280 V2, then reads at the
configured interval. It probes address `0x77` then `0x76`. Five consecutive
failed reads mark the device disconnected; reconnect attempts occur no sooner
than five seconds later.

Current UI behavior matters:

- an invalid snapshot leaves the last numeric readings visible;
- the Home latest-data label reports unavailability/age;
- Settings `Last data update` measures the latest successful recommendation
  fetch, not sensor age or completion of all three LEOP fetches;
- UART `status` summarizes current Wi-Fi, LEOP, sensor validity and time sync;
- authorized UART `sensor` output is the more direct validity check.

Do not disconnect the BME280, scan the shared I²C bus, alter wiring, or inject
read failures merely to test recovery without the physical-operation gate.
Static source cannot prove electrical presence, accuracy, or recovery timing.

## UI freezes, corrupts, or shows inconsistent status

Check the known synchronization boundaries before assuming a hardware fault:

- the common `app_state_t` has no common mutex;
- queue consumers receive copied snapshots, while UI/UART also read shared
  state directly;
- the periodic tab updates, including `WiFi_UI_Update()`, run under the outer
  LVGL port lock;
- generated and custom UI code are coupled through object names.

Compare recent UI or generated-source changes and build first. Runtime stress,
fault injection, repeated interaction, or soak testing requires authorization
and a recorded test setup.

## Settings appears stuck

This may be expected current behavior. Of five Settings rows:

- Uptime, restart reason, recommendation-update age, and time synchronization
  are implemented;
- System status always displays `Starting...`;
- Last data update displays `No data yet.` until the first successful
  recommendation fetch, then a monotonic age.

Use the Home tab for sensor freshness and the LEOP/Wi-Fi header for connectivity;
the Settings update age is narrower than either signal.

## UART does not respond or output looks wrong

Before opening a real port, verify the expected UART0 settings in
[UART diagnostics](UART_COMMANDS.md). In an authorized session, use 115200 8N1
with the existing UART0 pins and a carriage-return or line-feed ending.

Input is lowercased and tokenized on literal spaces. Use one space between
tokens. Lines beyond 127 characters are silently truncated. Remember that
`status` is a summary rather than a per-dataset freshness check, `leop` prints recommendation only, and
task stack figures are estimates. Do not use hidden `reboot` or persistent
`config` commands during read-only triage.

## Escalation record

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Status | Current source-backed diagnostic guide |
| Audience | Developers, maintainers, and authorized device testers |
| Last verified | Glennergy-ESP `693dc8819ac5b6d8fb29ce057d287814a3b9a14d` |
| Evidence boundary | Static inspection; no hardware, serial port, or live endpoint used |

</details>

If static and authorized read-only checks do not resolve the issue, record:

- source SHA and dirty-state summary;
- hardware model/revision if physically observed;
- exact symptom and first occurrence;
- which evidence is static, build-derived, serial-observed, or hardware-observed;
- non-sensitive error codes and timestamps;
- whether data might be cached;
- the next proposed state-changing action, its target, risk, and recovery plan.

This keeps a failure investigation reproducible without publishing secrets,
the production endpoint, or unsupported claims.
