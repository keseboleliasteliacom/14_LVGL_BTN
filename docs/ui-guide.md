# User interface guide

> **In short:** The UI has five tabs. Home, Electricity, Weather and WiFi show
> current snapshots; Settings currently has three working fields out of five.

The firmware presents one main LVGL screen with five tabs: **Home**,
**Electricity**, **Weather**, **WiFi**, and **Settings**. A persistent header
shows the Glennergy name, the selected/current Wi-Fi network text, and LEOP
connection state.

This guide describes what current code intends to display. It does not claim
that layout, touch coordinates, colors, timing, or peripheral behavior have
been observed on a physical panel.

## Tab overview

| Tab | Main purpose | Important limitation |
| --- | --- | --- |
| Home | Local BME280 temperature, humidity and pressure | Freshness depends on sensor validity |
| Electricity | Recommendation snapshot | Recommendation meaning remains unresolved |
| Weather | Forecast snapshot | Timestamp and UV compatibility gaps apply |
| WiFi | Scan and connect | Connected styling may remain after disconnect |
| Settings | Uptime, restart and time-sync status | System Status and Last Update are placeholders |

## Header status

### Wi-Fi name

The header starts as `Not Connected`. After the Wi-Fi worker publishes a
successful connection result, it displays the loaded or selected SSID. The
current UI has no corresponding disconnect-result update, so the old SSID can
remain visible after connectivity is lost.

### LEOP state

The LEOP label consumes the latest changed state from a depth-one queue:

| Text | Color | Meaning in current firmware |
| --- | --- | --- |
| `No WiFi` | Red | Wi-Fi is unavailable |
| `Checking...` | Yellow | Wi-Fi became available and LEOP is being checked |
| `Connected` | Green | All three fetches succeeded, or a later health probe succeeded |
| `Degraded` | Orange | At least one, but not all, full-fetch categories succeeded |
| `Not connected` | Red | Repeated total fetch/probe failures reached the threshold |

The label reports the LEOP task's connection classification, not freshness of
each displayed value. A health probe currently reuses the recommendation route
and can change a prior degraded state to connected without refetching all three
datasets. Cached data can also remain visible while offline.

## Home tab

The Home tab displays the latest BME280 temperature, relative humidity, and
pressure in three gauges, plus a latest-data message.

- A valid Sensor queue snapshot replaces all three numeric labels.
- An invalid snapshot leaves the previous numeric values visible and updates
  the latest-data message instead.
- Before any successful reading, the message is `Latest data: unavailable`.
- When wall time is valid, the message can include local date/time and elapsed
  monotonic age; otherwise it reports elapsed age only.

The displayed pressure label includes `hPa`; temperature and humidity formatting
comes directly from the current module. Static inspection does not establish
sensor accuracy or the physical units supplied by every lower-level boundary.

## Electricity tab

The left panel displays 96 continuous recommendation scores normalized to
0–100 for bar height. Bar colors use LEOP's separate quartile category: green
for buy, yellow for hold, red for sell, and gray for unknown. Twelve two-hour
labels keep the 24-hour axis readable.

The right panel shows a simultaneously visible, scrollable list of 24 hourly
prices in SEK/kWh, sampled every fourth item from the 96-entry price list.

## Weather tab

The active path is the newer weather dashboard (`weather_dashboard_create` and
`Weather_UI_Update_test`), despite the testing-style function name. It creates:

- a current-weather card using the first weather entry;
- a text description and icon selected from the weather code;
- a scrollable 24-hour forecast list sampled every fourth entry, with time,
  temperature, icon, and code-derived presentation.

The older `Weather_UI_Initialize`/`Weather_UI_Update` path remains in source but
is not called by current screen startup. Weather values update only when a
queue snapshot is marked fetched. Cached and network-fetched snapshots are not
visually distinguished.

## WiFi tab

The WiFi tab provides:

- a scan button;
- a network dropdown populated from the latest scan result;
- a password-mode text area and on-screen keyboard;
- a status label initialized as red `Disconnected`.

Selecting a network updates the selected SSID. Pressing OK on the password
keyboard submits a non-blocking, depth-one connect command. Scan and connect
commands can be dropped if that queue is full; the UI does not currently show a
delivery error.

On a successful result the tab changes the status text to green `Connected`
and updates the header SSID. It does **not** handle a later disconnected result,
so `WifiValueLabel`/the Wi-Fi status can remain green and the prior SSID can
remain visible after loss. This is a known defect, not evidence that the device
is still connected. Compare the LEOP header state or read-only diagnostics when
the display is inconsistent.

Passwords are stored through the Wi-Fi module's NVS path after a successful
connection. Do not expose the password field, NVS contents, or logs/screenshots
containing network details.

## Settings tab: three of five fields work

The Settings tab currently contains five read-only rows. Exactly three have
implemented live/current behavior:

| Field | Current behavior | Status |
| --- | --- | --- |
| Uptime | Re-formatted from monotonic device uptime once per second | Implemented |
| Last restart reason | Set once from `esp_reset_reason()` during UI initialization | Implemented |
| System status | Always returns `Starting...` | **Placeholder / TODO** |
| Last data update | Always returns `No data yet` | **Placeholder / TODO** |
| Time synchronized | Shows `Synchronized` or `Waiting` from the SNTP module | Implemented |

Therefore, `Starting...` and `No data yet` in Settings do not prove that the
system is still starting or has no sensor data. The Home tab and UART sensor
diagnostic use different paths and can show valid data while those placeholders
remain unchanged.

## Generated and custom code boundary

SquareLine Studio generated the base UI project, but the `ui/` tree now mixes
generated files with application-specific orchestration and tab modules.
`ui/screens/ui_Screen1.c` carries a generated header while also starting the
custom tab modules and running the update task. Custom code depends on generated
LVGL object names.

Regeneration can overwrite integration code or rename objects used by custom
modules. Treat it as a reviewed source change: preserve custom callbacks,
compare the full diff, rebuild, and validate on an authorized board. See
[development](development.md#source-boundaries).

## LVGL locking limitation

The UI update task normally acquires the LVGL port lock around Sensor,
Electricity, Weather, Price, Settings, and LEOP updates. Several of those
modules also acquire the same lock internally.

`WiFi_UI_Update()` runs before that outer lock. Its scan-result path takes its
own lock, but its successful-connect path directly changes labels and colors
without acquiring the lock, then delays the UI task for one second. This is a
known concurrency defect. The current architecture must not be described as
having every LVGL mutation serialized.

## Current limitations at a glance

- Settings implements three of five displayed fields.
- The Wi-Fi label does not reliably return to disconnected text/color.
- The Wi-Fi successful-result LVGL update is currently unlocked.
- Header/LEOP state does not establish data freshness.
- Cached versus live server data is not identified visually.
- Recommendation color semantics are presentation behavior, not a resolved
  recommendation contract.
- Display, touch, layout, and timing remain unverified by this campaign.

See [current limitations](current-limitations.md) for the cross-project backlog,
[firmware architecture](architecture.md#ui-and-generated-code-boundary) for task
ownership, and [troubleshooting](troubleshooting.md) for safe diagnosis.

## Implementation evidence

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Audience | Users, evaluators, firmware developers, and maintainers |
| Applies to | Glennergy-ESP `dev` at `b5a502a` |
| Evidence | Static source and generated UI inspection |
| Not verified | Display, touch, layout, timing, or other physical-hardware behavior |

</details>

- `ui/screens/Main_UI.c`: screen, header, five tabs, and Settings rows;
- `ui/screens/ui_Screen1.c`: initialization and update orchestration;
- `ui/Tabs/Home/Sensor_UI.c`: Home values and age text;
- `ui/Tabs/Electricity/Electricity_UI.c` and `Price_UI.c`: chart and price list;
- `ui/Tabs/Weather/Weather_UI.c`: active and legacy weather layouts;
- `ui/Tabs/Wifi/WiFi_UI.c`: scan/connect UI and known status/locking defects;
- `ui/Tabs/Settings/Settings_UI.c`: three implemented and two placeholder rows;
- `main/LEOP/LEOP_Fetcher.c`: LEOP state publication.

Re-check this page whenever generated objects, tabs, queue consumers, status
text, Settings fields, or LVGL locking changes.
