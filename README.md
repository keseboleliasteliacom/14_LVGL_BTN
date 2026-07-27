# Glennergy-ESP

Glennergy-ESP is the touchscreen firmware for the Glennergy energy project. It
runs on an ESP32-S3, displays local environmental readings, and retrieves
weather, electricity-price, and calculated recommendation data from the
Glennergy LEOP server.

The firmware is close to feature-complete, but several interfaces and UI fields
remain temporary or unfinished. This README provides a safe starting point;
the [documentation index](docs/README.md) links to the complete technical
guides.

`dev` is the source of truth for current development. `main` represents the
stable production line, but may not contain every behavior described in the
current `dev` documentation. Confirm a device's installed revision before
assuming the two match.

## How Glennergy fits together

The system has two cooperating projects:

- **Glennergy-ESP** runs on the ESP32-S3. It owns the display, touch interface,
  Wi-Fi client, local BME280 readings, offline cache, and device diagnostics.
- **Glennergy** runs the LEOP server. It fetches external weather and Swedish
  electricity-price data, calculates per-property results, and serves the
  latest data over HTTP.

The ESP currently initiates read-only requests to the server. Planned two-way
registration—where a device submits property information and Glennergy stores
it—is not implemented. See the [system context](docs/system-context.md) for the
complete product boundary and the [ESP interface contract](docs/interface-contract.md)
for every endpoint and behavior relevant to the firmware. Glennergy also owns
the complete [server HTTP API reference](https://github.com/Glennergy-Optimizer/glennergy/blob/dev/Docs/http-api.md).

## Current features

- A five-tab LVGL interface: Home, Electricity, Weather, WiFi, and Settings.
- Local temperature, relative-humidity, and pressure readings from a BME280.
- Wi-Fi network scanning, connection, reconnect scheduling, and saved
  credentials in NVS.
- Recommendation, weather, and price retrieval from Glennergy.
- SPIFFS caching of received JSON response bodies for attempted offline reuse.
  Category-invalid JSON can replace a previously useful cache, so cached data
  is not an integrity guarantee.
- Connection-state indicators for Wi-Fi and the LEOP server.
- A UART0 diagnostic shell for read-only status and controlled configuration
  changes.
- ESP-IDF build automation and Doxygen-oriented source documentation tooling.

Some status indicators describe connectivity rather than data freshness, and
cached values are not visually distinguished from network-fetched values. Read
the [UI guide](docs/ui-guide.md) and [connectivity guide](docs/connectivity.md)
before interpreting them as live health guarantees.

## Source-declared hardware

The repository is configured for:

| Item | Current declaration/configuration |
|---|---|
| MCU target | ESP32-S3 |
| Board | Waveshare ESP32-S3-Touch-LCD-4.3 |
| Display | 1024 × 600, 16-bit RGB panel; ST7262 is repository-declared |
| Touch controller | GT911 |
| Environmental sensor | BME280, probing I²C address `0x77` then `0x76` |
| Flash / external RAM | 8 MB flash and octal PSRAM in committed defaults |

These values come from source and build configuration. They do not prove the
connected board revision, wiring, display timing, touch orientation, sensor
presence, or measurement accuracy. Use the [hardware guide](docs/hardware.md)
for exact pins, buses, evidence levels, and physical-operation safety.

## Prerequisites

For a local build you need:

- Git;
- an ESP-IDF development environment;
- ESP-IDF 5.3.5 for the closest match to CI and `dependencies.lock`;
- Python and the tools installed by ESP-IDF;
- network access if managed components are not already cached.

`main/idf_component.yml` permits ESP-IDF 5.1 or later, but the dependency lock
and current CI baseline use 5.3.5. A different permitted version is not
automatically validated as equivalent.

## Build without touching hardware

Open an ESP-IDF-enabled shell, clone the repository, and build from its root:

```text
git clone https://github.com/Glennergy-Optimizer/Glennergy-ESP.git
cd Glennergy-ESP
idf.py --version
idf.py set-target esp32s3
idf.py build
```

The target is already present in committed configuration. `set-target` is
mainly useful for a new or previously reused workspace and can rewrite local
configuration, so review any resulting changes.

A successful build proves compilation only. It does not prove boot, display,
touch, Wi-Fi, sensor, cache, or server behavior. The Unity sources under
`main/test/` are not a comprehensive routinely executed test suite. See the
[development guide](docs/development.md) for setup, source boundaries, CI
artifacts, and validation expectations.

## Flash and monitor only an authorized board

Flashing changes a physical device. Before running a flash or monitor command:

1. confirm the exact development board and serial port;
2. confirm the device is not a production, shared, or unknown unit;
3. obtain authorization to operate it;
4. record its current firmware and configuration when recovery matters;
5. understand that serial output may contain network names, request details, or
   other sensitive operational information.

Only after those checks, use the procedure in
[development: Flash and monitor a development board](docs/development.md#flash-and-monitor-a-development-board).
Do not erase flash, NVS, SPIFFS, partitions, or saved credentials as part of an
ordinary build or troubleshooting step.

## Important current limitations

The current firmware is useful now, but the following must not be presented as
finished behavior:

- **Settings is three of five:** uptime, restart reason, and time
  synchronization work; System status and Last data update remain placeholders.
- **Wi-Fi display defect:** the Wi-Fi text/color and prior SSID can remain stale
  after a disconnect.
- **Temporary server request:** the current route shape is backwards and the
  ESP uses a hard-coded integer property ID. The desired command-first route is
  planned.
- **Temporary capacity:** the current server-side result pipeline supports only
  five properties end to end.
- **Read-only integration:** the ESP fetches data, but device/property
  registration and server-side property writes are not implemented.
- **Identity and security are unfinished:** UUID-like device identity,
  authentication, authorization, registration retries, and conflict handling
  still require design and implementation.
- **Recommendation meaning is unresolved:** the numeric recommendation field
  and its chart colors must not be described as settled buy/hold/sell guidance.
- **Testing remains partial:** this documentation campaign has not verified the
  display, touch, sensors, Wi-Fi, or recovery flows on physical hardware.

The [current limitations](docs/current-limitations.md) document separates
implemented, partial, temporary, planned, and unknown behavior in detail.

## Configuration and secrets

Application settings and Wi-Fi credentials are stored through NVS. Public
examples use placeholders and intentionally omit the live LEOP address.

Never commit or paste into documentation, screenshots, logs, or issues:

- Wi-Fi passwords;
- API keys or GitHub Actions secrets;
- SSH or TLS private keys;
- tokens or credentials;
- private endpoint or administrative host details;
- real customer or property data.

Storage in NVS is persistence, not a complete product security claim. See the
[configuration guide](docs/configuration.md), [UART reference](docs/UART_COMMANDS.md),
and [connectivity guide](docs/connectivity.md) before changing live settings.

## Documentation map

| If you want to… | Read |
|---|---|
| Understand the complete two-project system | [System context](docs/system-context.md) |
| Check unfinished or temporary behavior | [Current limitations](docs/current-limitations.md) |
| Understand tasks, queues, state, and startup | [Firmware architecture](docs/architecture.md) |
| Build or work with an authorized board | [Development guide](docs/development.md) |
| Check board, display, touch, I²C, or BME280 details | [Hardware guide](docs/hardware.md) |
| Understand NVS settings and persistence | [Configuration guide](docs/configuration.md) |
| Trace Wi-Fi, server fetches, retries, and cache | [Connectivity guide](docs/connectivity.md) |
| Understand tabs and status indicators | [UI guide](docs/ui-guide.md) |
| Use the serial diagnostics safely | [UART command reference](docs/UART_COMMANDS.md) |
| Diagnose a problem read-only-first | [Troubleshooting guide](docs/troubleshooting.md) |
| Implement or review the server boundary | [Interface contract](docs/interface-contract.md) |
| Find all current and historical documents | [Documentation index](docs/README.md) |

Start with the README and system context, then follow the guide matching your
task. Detailed documents are intentionally kept separate so this entry point
remains approachable for users, evaluators, and developers.

<details>
<summary>Documentation version and scope</summary>

| Project status | Reference |
| --- | --- |
| Authoritative implementation | `dev` at `b5a502a` |
| Stable production line | `main` |
| Documentation scope | Current `dev` behavior, with planned work labelled |

</details>
