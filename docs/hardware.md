# Hardware guide

> **In short:** The firmware targets a Waveshare ESP32-S3-Touch-LCD-4.3, but
> this guide describes source configuration rather than observed hardware.

This page describes the hardware configuration encoded in authoritative `dev`.
It does not report a physical inspection, schematic review, or live-board test.
Use it with the [firmware architecture](architecture.md) and
[development guide](development.md).

## Confidence labels

- **Configured** means the value is used by compiled source or committed build
  configuration.
- **Repository-declared** means project metadata names the part, but this
  campaign did not independently identify it on hardware.
- **Unverified** means static source cannot establish the electrical or runtime
  result.

## Board and memory configuration

| Item | Recorded value | Evidence level |
| --- | --- | --- |
| MCU target | ESP32-S3 | Configured in `sdkconfig`, `sdkconfig.defaults`, and `dependencies.lock` |
| Board | Waveshare ESP32-S3-Touch-LCD-4.3 | Repository-declared in the root README and board-support symbol names |
| Flash | 8 MB, QIO, 80 MHz | Configured in `sdkconfig.defaults` |
| External RAM | Octal PSRAM at 80 MHz | Configured in `sdkconfig.defaults` |
| CPU frequency | 240 MHz | Configured in `sdkconfig.defaults` |

The LCD frame buffers are configured for PSRAM. These settings describe the
firmware build; they do not prove the connected board's fitted memory parts or
signal integrity.

## Display path

The compiled board-support component configures a 16-bit RGB panel with two
frame buffers.

| Setting | Configured value |
| --- | --- |
| Resolution | 1024 × 600 pixels |
| Pixel clock | 30.85 MHz |
| RGB data width / pixel size | 16 bits / 16 bits |
| Frame buffers | 2, allocated in PSRAM |
| HSYNC / VSYNC / DE / PCLK | GPIO 46 / GPIO 3 / GPIO 5 / GPIO 7 |
| RGB data 0–15 | GPIO 14, 38, 18, 17, 10, 39, 0, 45, 48, 47, 21, 1, 2, 42, 41, 40 |

The root README declares an ST7262 LCD controller. The active driver configures
the ESP-IDF RGB panel interface and does not probe or identify a controller, so
ST7262 should be treated as repository-declared rather than runtime-verified.
One stale implementation comment says a full-frame image is 800 × 480, while
the compiled constants and draw call use 1024 × 600; the configured constants
are authoritative for current behavior.

Direct LCD reset, display-enable, and backlight GPIOs are set to `-1` in the RGB
driver. Backlight control instead writes IO-expander output 2. The expander is
configured at I²C address `0x24`; its header also labels output 1 for touch
reset and output 3 for LCD reset. Static source does not prove the external
wiring or active voltage levels beyond those software assignments.

## Touch path

The repository-declared and compiled touch controller is GT911. It shares I²C
controller 0 on GPIO 8 (SDA) and GPIO 9 (SCL), and the touch interrupt is
configured on GPIO 4. The direct reset GPIO is disabled (`-1`); reset is handled
through the board support path and IO expander.

The touch configuration uses the display's 1024 × 600 bounds. Driver support
allows up to five touch points, but this campaign did not test coordinate
orientation, calibration, multi-touch behavior, interrupt polarity, or reset
timing on a physical panel.

## Shared I²C bus

The board stack creates I²C controller 0 with internal pull-ups enabled,
GPIO 8 as SDA, GPIO 9 as SCL, and a configured 100 kHz bus speed. The active
BME280 V2 wrapper requests the same controller and pins using a 100 kHz fallback
configuration; its comments and code expect the managed `i2c_bus` wrapper to
adopt the native bus already created by the touch/display startup.

| Device or role | Address | Source-derived status |
| --- | --- | --- |
| IO expander | `0x24` | Configured |
| GT911 | `0x5D` | Configured default; `0x14` exists as a driver alternative but is not selected by current initialization |
| BME280 | Tries `0x77`, then `0x76` | Configured probe order |

The shared 100 kHz configuration and bus adoption deserve care when changing
initialization order or bus ownership. Static inspection does not prove the
effective clock after adoption, whether external pull-ups are fitted, or
whether every attached device meets the resulting timing.

## BME280 environmental sensor

`Sensor_Work` instantiates `hal::BME280SensorV2`, the only tracked hardware
sensor wrapper. It reads temperature, relative humidity, and pressure through
the managed Espressif BME280 component.

At initialization it tries address `0x77` first, then `0x76`. After five
consecutive failed read cycles it marks the sensor disconnected, deletes its
device handle, waits at least five seconds before a reconnect attempt, and then
reuses its bus wrapper. These are code paths, not measured recovery guarantees.
The sensor task itself starts sampling after a three-second delay and uses the
configured runtime sensor interval thereafter.

No physical test in this campaign establishes sensor presence, measurement
accuracy, wiring, supply voltage, pressure units at the UI boundary, heat
effects from the enclosure/board, or reconnect timing.

## Hardware operation safety

Before flashing, probing, rewiring, or power-cycling hardware:

1. Confirm it is the intended development unit, not a production or shared
   device.
2. Obtain permission for the operation and record the current firmware/config
   when recovery matters.
3. Verify the serial port and power arrangement.
4. Power down before changing wiring.
5. Check the board and peripheral documentation for voltage, grounding,
   pull-up, and pin-conflict requirements.

Do not attach an I²C scanner or another bus master merely from this page. GPIO
8/9 are already shared by board peripherals, and an intrusive probe can alter
timing or device state. Do not publish serial logs, Wi-Fi credentials, private
addresses, tokens, API keys, or SSH material gathered during hardware work.

## What remains unverified

- Exact board revision and fitted controller markings.
- Physical pin routing against an authoritative schematic.
- Display timing, color order, backlight behavior, and visual output.
- Touch orientation, accuracy, reset, interrupts, and multi-touch.
- Effective shared-bus clock, pull-ups, and electrical margins.
- BME280 presence, wiring, accuracy, and recovery on the target board.
- Flash/PSRAM part identity and behavior under load.

Record those facts only after an authorized, reproducible hardware observation.
See [current limitations](current-limitations.md#testing-and-verification-limits)
for the boundary between source-derived claims and runtime evidence.

## Source map

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Status | Source-derived board configuration; physical behavior unverified |
| Firmware target | ESP32-S3 |
| Repository-declared board | Waveshare ESP32-S3-Touch-LCD-4.3 |
| Last verified | Glennergy-ESP `baf9b58d04e827f024c8975b140f7a417e462370` |

</details>

- `sdkconfig.defaults` — target, flash, PSRAM, CPU, and LVGL defaults.
- `dependencies.lock` — ESP-IDF 5.3.5 and managed-component versions.
- `components/rgb_lcd_port/` — active RGB timing, pins, frame buffers, and
  backlight control.
- `components/i2c/` and `components/io_extension/` — shared board bus and
  expander configuration.
- `components/touch/gt911.*` — touch controller setup and interrupt pin.
- `main/hal/bme280_sensor_v2.*` — active sensor bus, address probing, reads,
  failure handling, and reconnect behavior.
- `main/sensor/sensor.cpp` — active V2 selection and sampling task.
- `main/main.c` — peripheral initialization order.
