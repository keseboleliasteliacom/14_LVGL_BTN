# ESP-IDF Build and Test Workflow Plan

This document is a gradual roadmap for adding automated build and test coverage
for the Glennergy ESP32-S3 firmware. Each level builds on the previous one, so
the project can gain useful CI coverage without attempting full hardware
simulation immediately.

## Project baseline

- Target: ESP32-S3
- Board: Waveshare ESP32-S3-Touch-LCD-4.3
- Display controller: ST7262
- Touch controller: GT911
- ESP-IDF version currently recorded in `dependencies.lock`: 5.3.5
- Flash configuration: 8 MB, QIO, 80 MHz
- PSRAM configuration: octal, 80 MHz
- Existing tests: ESP-IDF Unity test sources under `main/test`
- Existing automation: Doxygen workflows under `.github/workflows`

## Recommended progression

Work through Levels 1–3 first. They provide the best return for the effort and
do not depend on accurately simulating the board-specific peripherals. Add
QEMU or Wokwi after hardware access has been separated from application logic.
Use real hardware-in-the-loop when exact board behavior must be verified.

---

## Level 1 — Build-only CI

**Effort:** Small  
**Value:** High  
**Runner:** GitHub-hosted Linux runner

### Goal

Prove that a clean environment can compile and link the firmware for ESP32-S3.

### Work items

- [x] Add `.github/workflows/esp-idf-build.yml`.
- [x] Run on pull requests, pushes to all branches, and manual dispatch.
- [x] Check out the repository.
- [x] Install or use an official ESP-IDF 5.3.5 environment.
- [x] Run `idf.py build` from the repository root.
- [x] Upload the application, bootloader, partition table, ELF, map file, and
      flashing metadata as workflow artifacts.
- [x] Add a job timeout and GitHub Actions concurrency cancellation.
- [x] Document how to download and flash the resulting artifacts.

### Completion criteria

- A new checkout builds successfully without relying on files from a
  developer's machine.
- A pull request cannot be merged accidentally while the required build check
  is failing, once branch protection is enabled.
- The workflow produces enough artifacts to inspect or flash the exact build.

### What this level catches

- Compilation and linking errors
- Missing source files or dependencies
- ESP32-S3 target configuration errors
- Partition and firmware-size failures detected by the normal build

### What this level does not test

- Firmware boot or runtime behavior
- Display, touch, sensors, SD card, networking, or physical timing

### Using a Level 1 build artifact

1. Open the repository's **Actions** page on GitHub.
2. Select a successful **ESP-IDF Build** run.
3. Download the `glennergy-esp32s3-<commit SHA>` artifact and extract it.
4. Connect the board and open an ESP-IDF 5.3.5 terminal.
5. From the extracted artifact directory, flash the complete image set with:

   ```text
   python -m esptool --chip esp32s3 -p PORT write_flash "@flash_args"
   ```

   Replace `PORT` with the board's serial port, such as `COM5` on Windows or
   `/dev/ttyACM0` on Linux. The artifact's `flash_args` file supplies the flash
   settings, offsets, bootloader, partition table, and application image.

Only flash artifacts produced from a trusted commit. Level 1 compiles the
committed `sdkconfig`; it does not yet resolve or enforce configuration drift
between `sdkconfig` and `sdkconfig.defaults`.

---

## Level 2 — Strong build validation

**Effort:** Small to medium  
**Value:** Very high  
**Runner:** GitHub-hosted Linux runner

### Goal

Make builds reproducible and detect unhealthy growth or configuration drift.

### Work items

- [ ] Build from a clean directory and a documented configuration source.
- [ ] Decide whether CI should use the committed `sdkconfig`, regenerate it
      from `sdkconfig.defaults`, or validate both.
- [ ] Run `idf.py size`, `idf.py size-components`, and `idf.py size-files`.
- [ ] Save size reports as workflow artifacts or job summaries.
- [ ] Establish maximum application partition usage.
- [ ] Establish warning thresholds for flash, static RAM, and IRAM growth.
- [ ] Fail on selected compiler warnings after the current warning baseline is
      understood.
- [ ] Cache safe, reproducible downloads such as managed components.
- [ ] Confirm that `dependencies.lock` remains consistent with the build.
- [ ] Optionally compare firmware size against the target branch on pull
      requests.

### Completion criteria

- CI reports where flash and memory are being used.
- Unexpected firmware growth is visible before merge.
- The intended relationship between `sdkconfig` and `sdkconfig.defaults` is
  documented and enforced.

---

## Level 3 — Unit and component tests

**Effort:** Medium  
**Value:** High  
**Runner:** Host, emulator, simulator, or hardware depending on the tests

### Goal

Automatically exercise hardware-independent application behavior.

### Initial candidates

- JSON parsing
- Price, weather, and recommendation parsing
- State machines and message handling
- Queue behavior
- UART command parsing
- Sensor-value validation
- Error and timeout handling
- NVS behavior behind a mock interface

### Work items

- [ ] Inventory the Unity tests under `main/test` and record their current
      build/run status.
- [ ] Make the test application build independently and reproducibly.
- [ ] Separate application logic from direct hardware-driver calls where
      necessary.
- [ ] Add mocks or fakes for NVS, sensors, time, networking, and peripherals.
- [ ] Run fast hardware-independent tests on each pull request.
- [ ] Publish test results and serial logs.
- [ ] Add regression tests whenever a suitable bug is fixed.
- [ ] Consider host-based tests for pure C/C++ logic and target tests for code
      that depends on FreeRTOS or ESP-IDF.

### Completion criteria

- Tests run without manual interaction.
- Failures produce useful logs and a non-zero workflow result.
- Business logic can be tested without an ST7262 display or GT911 touch panel.

---

## Level 4 — ESP32-S3 QEMU smoke tests

**Effort:** Medium to large  
**Value:** Moderate for this hardware-heavy project  
**Runner:** GitHub-hosted Linux runner

### Goal

Boot the firmware, or a simulation-specific variant, on Espressif's ESP32-S3
QEMU implementation and verify basic runtime behavior.

### Suitable coverage

- CPU, memory, flash, and basic UART execution
- FreeRTOS task startup
- Expected boot messages
- Panic and reboot detection
- UART diagnostic-shell smoke tests
- Selected NVS, flash, or security scenarios

### Work items

- [ ] Check that the chosen ESP-IDF version and QEMU version work together for
      ESP32-S3.
- [ ] Identify every early-boot dependency on unsupported hardware.
- [ ] Add a simulation-specific configuration rather than changing production
      behavior silently.
- [ ] Replace or bypass unsupported display, touch, sensor, SD, and IO-expander
      initialization in that configuration.
- [ ] Run the firmware with a strict timeout.
- [ ] Assert expected serial output and fail on panic or error markers.
- [ ] Save complete emulator logs.
- [ ] Optionally evaluate the QEMU virtual framebuffer through an alternate
      `esp_lcd` driver.

### Limitations

QEMU does not reproduce the complete Waveshare board. Passing QEMU tests must
not be treated as proof that the real display, touch controller, sensors, or
electrical timing work correctly.

---

## Level 5 — Wokwi virtual-board integration tests

**Effort:** Medium to large  
**Value:** Potentially high  
**Runner:** GitHub Actions using Wokwi services

### Goal

Exercise ESP32-S3 firmware against a richer virtual circuit and scripted input
scenarios.

### Suitable coverage

- Virtual GPIO, I2C, SPI, UART, and supported sensors
- Button and sensor-event injection
- Serial output assertions
- Selected Wi-Fi, HTTP, MQTT, or backend flows
- Repeatable scripted scenarios
- Screenshots or visual regression where supported

### Work items

- [ ] Verify support for every peripheral needed by the selected scenario.
- [ ] Create `wokwi.toml` and `diagram.json`.
- [ ] Store the Wokwi token as a GitHub Actions secret, never in the repository.
- [ ] Add one short boot scenario before modeling additional peripherals.
- [ ] Add deterministic input scenarios and expected serial results.
- [ ] Integrate with `pytest-embedded` or the Wokwi CI action.
- [ ] Save serial logs and useful screenshots as artifacts.
- [ ] Clearly document substituted, simplified, or unsupported devices.

### Limitations

A virtual ESP32-S3 is not necessarily an exact model of the Waveshare board.
The ST7262, GT911, IO expander, and timing-sensitive behavior may require
substitutes or custom models.

---

## Level 6 — Hardware-in-the-loop

**Effort:** Large  
**Value:** Highest practical fidelity  
**Runner:** Self-hosted runner connected to a real board

### Goal

Build, flash, reset, and test the actual Waveshare ESP32-S3-Touch-LCD-4.3 board
without routine manual intervention.

### Infrastructure

- A dedicated PC, mini-PC, or Raspberry Pi runner
- A permanently connected test board
- Stable board selection by USB serial number
- Reliable reset or remotely controlled USB power
- Optional GPIO loopback, sensor fixtures, camera, or measurement equipment

### Work items

- [ ] Install and secure a dedicated self-hosted GitHub Actions runner.
- [ ] Label the runner so unrelated jobs cannot reserve the device.
- [ ] Add locking/concurrency so only one test can use a board at a time.
- [ ] Build or download a known firmware artifact.
- [ ] Reset or power-cycle the board into a known state.
- [ ] Flash the firmware and collect serial output.
- [ ] Use `pytest-embedded` for flash, serial, and target assertions.
- [ ] Test PSRAM, flash, display, touch, BME280, SD, networking, NVS, and UART
      incrementally.
- [ ] Preserve logs and diagnostic output after failures.
- [ ] Guarantee cleanup and board recovery even when a test times out.
- [ ] Run slower hardware tests on demand, nightly, or after merge rather than
      necessarily on every commit.

### Completion criteria

- CI can flash and verify the real target without a person operating it.
- A failed or interrupted test does not leave the board permanently reserved.
- Board-specific failures can be distinguished from build and runner failures.

---

## Level 7 — Full device test laboratory

**Effort:** Very large  
**Value:** Appropriate for production-level validation

### Possible extensions

- [ ] Maintain multiple boards or hardware revisions.
- [ ] Test supported ESP-IDF upgrade candidates in a version matrix.
- [ ] Add programmable power cycling and brownout scenarios.
- [ ] Add network loss, latency, DNS failure, and backend fault injection.
- [ ] Simulate sensors electrically or through controlled bus devices.
- [ ] Add GPIO loopback and protocol-analysis fixtures.
- [ ] Automate logic-analyzer or oscilloscope measurements.
- [ ] Add camera-based LVGL screenshot comparison.
- [ ] Automate touch input where product risk justifies the fixture.
- [ ] Test OTA update, interruption, rollback, and corrupted images.
- [ ] Track heap, stack high-water marks, task health, and watchdog behavior.
- [ ] Add performance and power-consumption thresholds.
- [ ] Run long-duration soak and recovery tests.

---

## Suggested milestones

### Milestone A — Reliable compilation

- Complete Level 1.
- Add the workflow as a required pull-request check after it is stable.

### Milestone B — Reproducible and measurable builds

- Complete Level 2.
- Record initial flash and memory baselines.

### Milestone C — Fast regression testing

- Complete the hardware-independent portion of Level 3.
- Run those tests on every pull request.

### Milestone D — Runtime smoke testing

- Select QEMU or Wokwi based on a small proof of concept.
- Do not invest in detailed virtual hardware models until the boot smoke test
  demonstrates value.

### Milestone E — Exact board behavior

- Complete a minimal Level 6 setup with one real board.
- Add peripheral tests one at a time, beginning with serial boot health and
  PSRAM before display, touch, sensors, storage, and networking.

## General workflow rules

- Pin ESP-IDF and important action versions; upgrade deliberately.
- Never commit API tokens, Wi-Fi credentials, certificates, or other secrets.
- Use timeouts for builds, emulators, serial waits, and hardware tests.
- Upload enough logs and artifacts to diagnose a failure without rerunning it.
- Keep simulation-specific behavior explicitly separated from production
  configuration.
- Treat emulation as additional coverage, not proof of exact hardware behavior.
- Keep pull-request checks fast; schedule slower integration and hardware tests
  separately when appropriate.

## References

- [ESP-IDF v5.3.5 ESP32-S3 Getting Started](https://docs.espressif.com/projects/esp-idf/en/v5.3.5/esp32s3/get-started/index.html)
- [ESP32-S3 QEMU](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-guides/tools/qemu.html)
- [ESP-IDF applications on the host](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/host-apps.html)
- [pytest-embedded](https://docs.espressif.com/projects/pytest-embedded/en/latest/)
- [ESP-IDF Wokwi integration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/third-party-tools/wokwi.html)
- [Wokwi CI action](https://github.com/wokwi/wokwi-ci-action)
