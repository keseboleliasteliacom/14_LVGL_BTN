# UART diagnostic shell

> **In short:** UART0 provides read-only diagnostics, three persistent setting
> changes and restart; it is a development shell, not a stable external API.

The firmware exposes a small development shell on UART0. It is useful for
inspecting the latest in-memory state and changing three configuration values.
It is not a stable external API, and its output should not be parsed as a
machine-readable protocol.

Opening a real serial port and sending commands interacts with hardware. Verify
the device and port and obtain authorization first. Serial output may contain
network names, request paths, timestamps, or other operational details; inspect
captures before sharing them. Never paste passwords, tokens, private keys, or
other secrets into this shell.

## Serial settings and input handling

| Setting | Current value |
| --- | --- |
| Port | `UART_NUM_0` |
| Baud | 115200 |
| Format | 8 data bits, no parity, 1 stop bit |
| Hardware flow control | Disabled |
| Pins | Existing UART0 pins (`UART_PIN_NO_CHANGE`) |
| Line ending | Carriage return or line feed |
| Editable line capacity | 127 characters plus terminator |

The worker echoes accepted bytes. On a carriage return or line feed it trims
leading and trailing whitespace, converts the whole line to lowercase, and
splits tokens on the literal space character. Commands and keys are therefore
case-insensitive, but values are lowercased too. Use exactly one space between
tokens: repeated internal spaces create empty tokens and can make `config`
fail its three-token check. Bytes beyond the 127-character capacity are
silently ignored until Enter is pressed.

## Command summary

| Command | Mutates state? | Purpose |
| --- | --- | --- |
| `help` | No | Print the normal command list |
| `help immersive` | No, but blocks | Run a long novelty help sequence that waits for Enter repeatedly |
| `status` | No | Print shared system-status fields |
| `sensor` | No | Print the latest shared BME280 snapshot |
| `leop` | No | Print the latest shared recommendation list |
| `pconfig` | No | Print the three runtime configuration values |
| `config <key> <value>` | **Yes** | Change RAM and attempt to persist the selected value to NVS |
| `diag` | No | Print uptime, heap, task count, and task stack high-water estimates |
| `reboot` | **Yes** | Immediately restart the device; hidden testing command, absent from `help` |

An unknown command prints `Unknown command: <normalized input>`. An empty
trimmed line prints the shell's no/incorrect-input message.

## Read-only diagnostics

### `status`

Prints an overall `All systems OK.` or `Degraded.` classification followed by:

- uptime in seconds;
- `Wifi`: mirrored by the Wi-Fi connection callback;
- `LEOP`: `Connected` for the internal connected or degraded LEOP states and
  `Disconnected` for the other states;
- `Sensor`: derived from the active sensor snapshot validity/update fields;
- `Time sync`: `Synchronized` or `Not synchronized`.

The overall classification requires Wi-Fi, LEOP, a valid sensor snapshot, and
synchronized time. It is a current-state summary, not a freshness guarantee for
all three LEOP datasets.

### `sensor`

If `valid` is false, the command prints `Sensor: No valid data yet.` and stops.
For a valid snapshot it prints:

- monotonic last-update seconds;
- local last-update time, or `not synced yet` when wall time is not valid;
- temperature in degrees Celsius, pressure in hPa, and humidity as a percent.

The command reads shared state directly without a common application mutex. A
coherent multi-field snapshot is not guaranteed while the Sensor task writes.

### `leop`

Prints `leop_data.recommendations.count`, followed by each recommendation value
and timestamp. It does not print weather, prices, transport status, or whether
the displayed list came from the network or SPIFFS cache. Recommendation
semantics remain unresolved; the numeric values must not be interpreted as a
settled buy/sell rule. The command also reads the shared container without a
common mutex.

### `pconfig`

Prints:

- `fetch_interval_minutes`;
- `sensor_interval_ms`;
- `test_mode` as `Enabled` or `Disabled`.

These are the current RAM values after defaults and any successfully loaded NVS
values. `test_mode` is stored and displayed but has no other active consumer in
the authoritative source snapshot.

### `diag`

Prints uptime in seconds, current default-capability free heap, minimum free
heap since boot, total FreeRTOS task count, and one line for each of the five
application task handles.

The task lines derive `used = configured_stack_size -
uxTaskGetStackHighWaterMark(handle)`. Treat them as rough diagnostics. Their
units and interpretation depend on the ESP-IDF FreeRTOS port, and the displayed
configured size is not an independently measured allocation. A missing handle
prints `<name>: no handle:`. The metadata names are also passed to task
creation; see [firmware architecture](architecture.md#application-tasks).

## Persistent configuration command

Syntax:

```text
config <key> <value>
```

| Key | Accepted value | Runtime consumer | NVS namespace/key |
| --- | --- | --- | --- |
| `fetch_interval_minutes` | Decimal integer `1` through `1440` | Next LEOP fetch interval calculation | `config` / `leop_min` as `u32` |
| `sensor_interval_ms` | Decimal integer `1000` through `60000` | Sensor task reads it before each delay | `config` / `sensor_ms` as `u32` |
| `test_mode` | Exactly `true` or `false` after lowercasing | No active behavior beyond storage/display | `config` / `test_mode` as `u8` boolean |

Examples for an authorized development device:

```text
config fetch_interval_minutes 15
config sensor_interval_ms 5000
config test_mode true
```

Each accepted value is written to RAM first, then committed to NVS. On the next
boot, NVS replaces the compiled default when the key can be read. If the NVS
write fails, the shell logs a warning but leaves the new RAM value active; the
value can therefore appear changed until restart without having persisted.

Important current validation details:

- decimal parsing must consume the entire token, but it does not explicitly
  detect `strtol` overflow before casting to `int`;
- an invalid `test_mode` value produces a user-facing error;
- the generic syntax text includes all three supported keys;
- `config` changes live task behavior and performs flash writes, so it is not a
  read-only diagnostic command.

## Restart command

`reboot` calls `esp_restart()` immediately after printing and flushing a short
message. It is a hidden testing command and is not listed by `help`. Use it only
after confirming the exact development device, that a restart is authorized,
and that interrupting its current work is acceptable. It does not prompt for
confirmation.

## Suggested read-only triage

After an authorized serial session is already open, begin with:

```text
help
pconfig
status
sensor
leop
diag
```

Do not start with `config`, `reboot`, flash erase, or other state-changing
operations. See [troubleshooting](troubleshooting.md) for symptom-oriented
checks and [development](development.md#flash-and-monitor-a-development-board)
for the hardware authorization boundary.

## Implementation evidence and maintenance

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Status | Current implementation, including known defects |
| Audience | Firmware developers and authorized device testers |
| Last verified | Glennergy-ESP `baf9b58d04e827f024c8975b140f7a417e462370` |
| Evidence | Static source inspection; no serial session was opened |

</details>

- `main/UART/UART.cpp`: UART setup, line editing, normalization, and worker;
- `main/UART/uart_diag_shell.cpp`: command dispatch, validation, output, and
  restart behavior;
- `main/Config/AppConfig.c`: defaults and NVS keys;
- `main/Memory/NVS.c`: storage and commit behavior;
- `main/main.c`: task metadata and shared application state.

Re-check this page whenever the parser, help text, configuration keys/ranges,
shared state, task diagnostics, or restart command changes. This edition was
validated statically; no serial port, hardware, secret, or production endpoint
was accessed.
