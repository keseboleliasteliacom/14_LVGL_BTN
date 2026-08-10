# Development guide

> **In short:** Use ESP-IDF 5.3.5 to configure and build for ESP32-S3; flashing
> and monitoring require a specifically authorized development board.

This guide covers a local firmware edit-and-build workflow. Read the
[firmware architecture](architecture.md) before changing task ownership,
queues, shared state, or hardware initialization. The
[interface contract](interface-contract.md) is authoritative for the current
Glennergy connection.

## Prerequisites

- Git and an ESP-IDF development environment.
- ESP-IDF 5.3.5 for the closest match to CI and `dependencies.lock`.
- Python and the tools installed by ESP-IDF.
- Network access when managed components are not already cached.
- For device operations only: the intended Waveshare ESP32-S3 board, a
  data-capable USB connection, its serial port, and permission to operate it.

`main/idf_component.yml` permits ESP-IDF 5.1 or later, but the dependency lock
and the GitHub Actions build image currently select 5.3.5. A different allowed
version is not automatically an equivalent or validated environment.

## Prepare ESP-IDF

Install ESP-IDF using Espressif's supported setup for the host platform, then
open an ESP-IDF-enabled shell or run that installation's export script. Verify
the active toolchain before building:

```text
idf.py --version
```

Run commands below from the repository root. Managed-component versions are
recorded in `dependencies.lock`; do not casually regenerate the lock file as
part of an unrelated documentation or source change.

## Build without touching hardware

Building is the normal first validation step and does not require a connected
board:

```text
idf.py set-target esp32s3
idf.py build
```

The target is already recorded in the committed configuration, so
`set-target` is mainly useful for a new or previously reused workspace. It can
rewrite local build configuration; review resulting changes before committing
anything. The repository contains both `sdkconfig` and `sdkconfig.defaults`,
and their long-term source-of-truth relationship remains an open workflow
decision. Do not assume regenerating one from the other is lossless.

The CI workflow also builds with ESP-IDF 5.3.5 and uploads binaries, the ELF,
map, partition table, bootloader, `flash_args`, and flashing metadata. That is
build evidence only: it does not prove boot, display, touch, sensor, Wi-Fi, or
server behavior.

## Flash and monitor a development board

Flashing changes a physical device. Confirm the board identity, serial port,
firmware revision, and authorization before running it. Never point these
commands at an unknown, shared, or production unit.

```text
idf.py -p PORT flash
idf.py -p PORT monitor
```

Replace `PORT` with the verified serial port. `idf.py -p PORT flash monitor`
combines the operations. Exit the ESP-IDF serial monitor with `Ctrl-]`.

Monitoring reads device output and may expose operational details printed by
the firmware. Treat captured logs as potentially sensitive and inspect them
before sharing. Flashing a trusted CI artifact is also possible using its
included `flash_args`; see the
[build and test workflow plan](ESP_IDF_BUILD_WORKFLOW_PLAN.md) for the recorded
artifact procedure.

This documentation campaign did not flash, reset, monitor, or otherwise
interact with hardware.

## Source boundaries

The main application is registered in `main/CMakeLists.txt`; reusable or
board-support components live under `components/`. The `ui/` tree contains a
mixture of SquareLine Studio output and project-specific integration code.

Files with a SquareLine generated header, including `ui/ui.c`,
`ui/ui_helpers.*`, and `ui/screens/ui_Screen1.*`, are regeneration-sensitive.
The recorded generator is SquareLine Studio 1.6.0. Application tab modules
under `ui/Tabs/` depend on generated LVGL object names, and
`ui/screens/ui_Screen1.c` currently also contains application orchestration.
Before regenerating the UI:

1. Review the diff between generated and custom code.
2. Preserve project-specific callbacks and update-task integration.
3. Check all custom references to generated object names.
4. Build after regeneration and test the UI on authorized hardware separately.

Do not treat wholesale regeneration as a formatting-only operation. The
[architecture guide](architecture.md#ui-and-generated-code-boundary) records
the current coupling and LVGL locking limitation.

## Configuration and secrets

Runtime configuration and Wi-Fi credentials are stored through NVS. Do not
commit credentials, API keys, tokens, certificates, SSH private keys, real VPS
addresses, serial logs containing secrets, or local configuration exports.
Use `<LEOP_BASE_URL>` in public examples. GitHub Actions secrets such as
`OPENAI_API_KEY` must remain in the secret store and must not be read or copied
into documentation.

Changing the current LEOP route, property identity, registration, or access
control requires coordinated work in both repositories. See
[current limitations](current-limitations.md) and the
[interface contract](interface-contract.md) before editing that boundary.

## Validation expectations

For documentation-only changes, at minimum check whitespace and relative
links. For firmware changes, a clean `idf.py build` is the minimum automated
check available in the current repository. The Unity sources under
`main/test/` are not evidence of a comprehensive, routinely executed suite.

Use the validation level that matches the change:

| Change | Minimum evidence | Additional evidence when relevant |
| --- | --- | --- |
| Documentation | Whitespace and link checks | Render diagrams and inspect examples |
| Pure firmware logic | ESP-IDF build | Focused unit/component tests |
| Board driver or configuration | ESP-IDF build | Test on an authorized development board |
| Display, touch, sensor, timing, or recovery | ESP-IDF build | Observed hardware test with recorded setup |
| Glennergy interface | Both repositories reviewed | Contract/integration test without real secrets |

A successful build does not establish physical behavior. QEMU, Wokwi, and
hardware-in-the-loop work described in the build plan are planned validation
levels, not current general coverage.

## Useful references

<details>
<summary>Verification metadata</summary>

| Item | Value |
| --- | --- |
| Status | Current development workflow |
| Target | ESP32-S3 |
| ESP-IDF baseline | 5.3.5 |
| Last verified | Glennergy-ESP `baf9b58d04e827f024c8975b140f7a417e462370` |

</details>

- [Documentation index](README.md)
- [Hardware guide](hardware.md)
- [Firmware architecture](architecture.md)
- [System context](system-context.md)
- [Current limitations](current-limitations.md)
- [UART command reference](UART_COMMANDS.md)
- [ESP-IDF build and test workflow plan](ESP_IDF_BUILD_WORKFLOW_PLAN.md)
