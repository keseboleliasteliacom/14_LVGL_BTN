# Embedded Doxygen Documentation Standard (AI-Enforced Version)

This project uses a strict, AI-enforced Doxygen documentation standard for embedded firmware.

The goal is to produce consistent, useful documentation while preserving code, preserving developer intent, and making embedded runtime constraints clear without turning every file into a design document.

---

## AI Compliance Contract

When an AI agent processes any file in this project, it MUST:

1. Follow the rules in this document
2. Never modify logic, control flow, initialization, signatures, task behavior, ISR behavior, or hardware access behavior
3. Only add or update documentation and comments
4. Preserve developer intent and non-Doxygen comments
5. Prefer concise, useful documentation over repetitive boilerplate

If a rule conflicts with code preservation, preserve the code and ordinary developer comments.

---

## File Roles

Different file types require different documentation depth:

- Header files (`.h`, `.hpp`) are the authoritative public API contract
- Source files (`.c`, `.cpp`) focus on implementation details and internal helpers
- App entry, task, demo, and integration files should remain practical and system-oriented
- ISR, callback, and event-handler code should document context and safety constraints clearly, but briefly

Do not document source files as heavily as headers unless the code genuinely requires it.

---

## Mandatory Rules

### 1. Code Integrity

AI MUST NOT:

- Change logic
- Change control flow
- Change variable initialization
- Change function signatures
- Move code
- Remove code
- Change peripheral names, queue names, event IDs, ISR/task behavior, timing constants, log strings, or format strings
- Change line endings or formatting-only details unless required to place comments

AI MAY:

- Add or update Doxygen comments
- Add or update normal comments
- Add `// Suggestion:` comments

### 2. Suggestions Policy

All improvements must be written as comments:

```c
// Suggestion: <clear and specific improvement>
```

Suggestions must:

- Not affect execution
- Not replace existing code
- Remain optional and reviewable

### 3. Preservation Rules

AI MUST preserve:

- All existing non-Doxygen comments in any language
- All debug prints and log statements (`printf`, `fprintf`, `ESP_LOG*`, similar logging)
- All TODOs and notes
- All commented-out includes
- All commented-out code
- Existing formatting that does not need to move for documentation placement

AI MUST NOT:

- Remove ordinary developer comments
- Rewrite ordinary developer comments unnecessarily
- Translate original non-Doxygen comments
- Change formatting only for style reasons
- Relocate comments unless needed to place documentation directly above the documented item

AI MAY rewrite, simplify, merge, or trim existing Doxygen comments when needed to match this repository standard.
When an existing Doxygen block conflicts with the target style examples, prefer the target style examples.

### 4. Language Rules

- All new Doxygen documentation must be in English
- Original non-Doxygen comments must remain unchanged

---

## Required File-Level Tags

Every documented file must include:

- `@file`
- `@brief`

Additionally:

- Header files should include `@defgroup`
- Source files that belong to a public module should include `@ingroup`

---

## Embedded-Specific Documentation Priorities

Document these when they are relevant and knowable from the code:

- whether a function is safe from task context, ISR context, callback context, or only one of them
- whether a function blocks, polls, waits on queues/events, or performs network/storage I/O
- initialization order and dependencies
- hardware/peripheral ownership or use (GPIO, I2C, SPI, UART, Wi-Fi, storage, touch, display, etc.)
- shared state, queue/event usage, or lock/critical-section assumptions
- units, ranges, scaling, time bases, or buffer-size limits when not obvious
- failure behavior, retries, and safe-state implications when meaningful

Do not invent concurrency, ISR, or ownership guarantees that are not supported by the code.

---

## Header File Standard

Headers are the main source of truth for public API documentation.

Public types and functions in headers should usually contain:

- `@brief`
- `@param` for all parameters when applicable
- `@return` when the function returns a value

Add these when they communicate meaningful embedded contract information:

- `@note`
- `@warning`
- `@pre`
- `@post`

Typical reasons to include these tags:

- initialization order or state requirements
- task/ISR context restrictions
- blocking behavior or timeout expectations
- ownership or lifetime rules when they truly matter
- hardware side effects or shared-state assumptions
- units, ranges, scaling, or buffer limits
- real misuse risks

Simple debug, print, dump, or trace helper declarations in headers should usually stay lightweight.

For these functions, prefer:

- `@brief`
- `@param` when applicable

Do not add `@pre`, `@post`, `@warning`, or `@note` to simple debug helpers unless they communicate something non-obvious or important.
Do not expand simple debug helper declarations into full contract-style blocks by default.

---

## Source File Standard

Source files are implementation-oriented and should stay concise.

Public function implementations in source files should usually use one of these styles:

1. A short implementation summary
2. A short implementation summary plus a note such as:
   `See header for full contract documentation.`

Do not repeat full public API contracts in `.c` or `.cpp` files when the header already documents them.
For public non-static functions with a documented paired header, prefer the default source-file pattern: `@brief Implementation of <function>.` followed by `See header for full contract documentation.`

Internal or `static` helper functions in source files must be documented, but briefly.

Include `@param`, `@return`, `@note`, or `@warning` only when they communicate something non-obvious or important, such as:

- internal ownership assumptions
- task vs ISR restrictions
- queue/event/locking assumptions
- blocking behavior or timeouts
- hardware side effects
- hidden failure behavior

Prefer ordinary `//` comments for local implementation steps inside function bodies.
Do not add Doxygen mini-blocks for local steps in a function body unless a local declaration genuinely needs API-style documentation.

If two versions are both correct, prefer the more concise one.
If an existing Doxygen block is already sufficiently aligned with this standard, prefer leaving it unchanged over rewriting it for minor wording differences alone.

---

## App Entry, Task, Demo, and Integration Files

These files should remain practical.

They must include:

- `@file`
- `@brief`
- `@ingroup` when part of a public module

Document the file purpose and important runtime constraints when relevant, for example:

- creates tasks
- initializes peripherals
- coordinates modules
- waits on queues/events
- forwards data to a consumer
- blocks on I/O or synchronization primitives

Do not turn these files into full API references.

---

## ISR, Callback, and Event-Handler Guidance

For interrupt handlers, callbacks, and event handlers:

- clearly state the context when relevant
- mention whether blocking is forbidden or avoided
- document shared-state or queue handoff assumptions when meaningful
- keep the documentation short and operational

Do not speculate about thread safety or ISR safety if the code does not make it clear.

---

## Struct Documentation

Public structs should include:

- a short description of purpose
- ownership or lifetime notes when relevant
- queue, buffer, range, or valid-element notes when relevant
- units or scaling notes when relevant

Do not invent ownership notes when the code does not imply a meaningful ownership rule.
Keep individual field comments short by default. Prefer putting the main explanation at the struct level and using concise field comments unless a field is subtle, safety-critical, or easy to misuse.
For ordinary fields, prefer short inline comments such as `/**< Number of valid entries. */` over full multi-line Doxygen blocks.
Do not turn each struct field into a mini documentation section unless the field truly needs that level of explanation.

---

## Function Documentation Rules

All functions should be documented, but the level of detail depends on the file role.

### Public Functions in Headers

Required:

- `@brief`
- `@param` for each parameter when applicable
- `@return` if non-void

Optional, when meaningful:

- `@note`
- `@warning`
- `@pre`
- `@post`

For `esp_err_t`-returning functions, describe success/failure at a useful level without copying full vendor documentation into every function.

### Public Functions in Source Files

Required:

- `@brief`

Optional, only when useful:

- `@param`
- `@return`
- `@note`
- `@warning`

Prefer concise implementation-oriented wording. If the header already contains the contract, do not duplicate it.

### Internal Functions

Required:

- `@brief`

Optional, only when useful:

- `@param`
- `@return`
- `@note`
- `@warning`

### RTOS Task Functions

Required:

- `@brief`

Optional, when meaningful:

- task purpose
- expected argument type
- blocking or loop behavior
- queue/event dependencies

Avoid turning task-loop docs into a full per-step narrative.

---

## Target Style Examples

Use the examples below as the preferred style reference when multiple valid documentation styles are possible.

### Public Header Function

```c
/**
 * @brief Initializes the UART transport module.
 *
 * @param[in] baud_rate UART baud rate in bits per second.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 *
 * @pre UART GPIO configuration must already be valid for the target board.
 * @note Call from task context during system initialization.
 */
esp_err_t UART_Initialize(uint32_t baud_rate);
```

### Public Source Function

```c
/**
 * @brief Implementation of UART_Initialize.
 *
 * See header for full contract documentation.
 */
esp_err_t UART_Initialize(uint32_t baud_rate)
{
    ...
}
```

### Debug Helper Function

```c
/**
 * @brief Prints the current queue state for debugging.
 *
 * @param state Pointer to state to print.
 */
void QueueState_Print(const QueueState *state);
```

### Struct Documentation

```c
/**
 * @brief Snapshot of sensor data produced by the worker task.
 *
 * Contains the latest processed values and bookkeeping needed by the
 * consumer path.
 *
 * @note Units are degrees Celsius and percent relative humidity.
 */
typedef struct
{
    float temperature_c;   /**< Latest temperature in degrees Celsius. */
    float humidity_pct;    /**< Latest relative humidity percentage. */
    uint32_t sample_count; /**< Number of valid samples processed. */
} SensorSnapshot;
```

### App Entry / Main File

```c
/**
 * @file main.c
 * @brief Application entry point for system bring-up and task startup.
 *
 * Initializes platform services, configures hardware, and starts the main
 * FreeRTOS tasks used by the application.
 *
 * @ingroup APP
 *
 * @note Intended as system orchestration, not a reusable module API.
 * @warning Startup order matters because several modules depend on earlier initialization.
 */
```

### RTOS Task Function

```c
/**
 * @brief Background worker task for sensor acquisition.
 *
 * Waits for work or timing events, samples the sensor hardware, and publishes
 * updated results to the shared output path.
 *
 * @param arg Pointer to the task context.
 *
 * @note Runs in task context and may block on queues or delays.
 */
void Sensor_Work(void *arg)
{
    ...
}
```

### Event Callback / Handler

```c
/**
 * @brief Handles Wi-Fi events from the ESP-IDF event loop.
 *
 * Updates connection state and forwards relevant events to the internal queue.
 *
 * @warning Keep callback work short and avoid unnecessary blocking.
 */
static void wifi_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ...
}
```

