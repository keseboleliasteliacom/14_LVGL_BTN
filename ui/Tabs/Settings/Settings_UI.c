/**
 * @file Settings_UI.c
 * @brief Implementation of the Settings tab UI update helpers.
 *
 * @ingroup SETTINGS_UI
 */

#include "Settings_UI.h"

#include "../../screens/Main_UI.h"
#include "../../../main/SNTP/time_sync.h"

#include "esp_system.h"
#include "esp_timer.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "TimeFormat.h" // from "Utils" folder"


/*
 * Private formatting helpers
 */


/**
 * @brief Returns a short text description of the current ESP reset reason.
 */
static const char *Settings_UI_GetRestartReasonText(void);

/**
 * @brief Returns the current system status text for the Settings tab.
 *
 * @param[in] status Pointer to the system status snapshot.
 *
 * @return Status text used by the UI.
 */
static const char *Settings_UI_GetSystemStatusText(const system_status_t *status);

/**
 * @brief Formats the last sensor update text for the Settings tab.
 *
 * @param[out] buffer Output buffer for the formatted text.
 * @param[in] buffer_size Size of @p buffer in bytes.
 * @param[in] last_update_seconds Timestamp of the last update.
 * @param[in] uptime_seconds Current uptime in seconds.
 */
static void Settings_UI_FormatLastUpdate(
    char *buffer,
    size_t buffer_size,
    uint32_t last_update_seconds,
    uint64_t uptime_seconds
);

/*
 * Private helper implementations
 */



/**
 * @brief Returns a short text description of the current ESP reset reason.
 */
static const char *Settings_UI_GetRestartReasonText(void)
{
    switch (esp_reset_reason())
    {
        case ESP_RST_POWERON:
            return "Power turned on.";
        case ESP_RST_EXT:
            return "External reset.";
        case ESP_RST_SW:
            return "Software restart.";
        case ESP_RST_PANIC:
            return "Software crash.";
        case ESP_RST_INT_WDT:
            return "Interrupt watchdog.";
        case ESP_RST_TASK_WDT:
            return "Task watchdog.";
        case ESP_RST_WDT:
            return "Watchdog restart.";
        case ESP_RST_DEEPSLEEP:
            return "Woke from deep sleep.";
        case ESP_RST_BROWNOUT:
            return "Low supply voltage.";
        case ESP_RST_SDIO:
            return "SDIO reset.";
        
        case ESP_RST_UNKNOWN:
        default:
            return "Unkown.";

    }
 

    return "Unknown";
}

/**
 * @brief Returns the current system status text for the Settings tab.
 *
 * @param[in] status Pointer to the system status snapshot.
 *
 * @return Status text used by the UI.
 */
static const char *Settings_UI_GetSystemStatusText(const system_status_t *status)
{
    // Todo - Reconsider if this is needed at all?

    
    if (status == NULL)
    {
        return "Unknown";
    }

    return "Starting...";
}

// TODO - Fix this until after merging branches 
/**
 * @brief Formats the last sensor update text for the Settings tab.
 *
 * @param[out] buffer Output buffer for the formatted text.
 * @param[in] buffer_size Size of @p buffer in bytes.
 * @param[in] last_update_seconds Timestamp of the last update.
 * @param[in] uptime_seconds Current uptime in seconds.
 */
static void Settings_UI_FormatLastUpdate(char *buffer, size_t buffer_size, uint32_t last_update_seconds, uint64_t uptime_seconds)
{
    // Add the size of this suffix to any buffer_size we take in, to ensure we have enough space to write/add the " ago" part to the info string no matter the input size.
    static const char suffix[] = " ago.";
    if (buffer == NULL || buffer_size == 0) 
    {
        return;
    }

    if (last_update_seconds == 0)
    {
        snprintf(buffer, buffer_size, "No data yet.");
        return;
    }

    uint64_t age_seconds = uptime_seconds - last_update_seconds;
    size_t duration_capacity = buffer_size - sizeof(suffix) +1;

    int written = TimeFormat_FormatDuration(buffer, duration_capacity, age_seconds);

    // Validate and check if safe to write
    if (written < 0 || (size_t)written >= duration_capacity)
    {
        snprintf(buffer, buffer_size, "Unknown.");
        return;
    }

    // Appends suffix to the duration. 
    memcpy(buffer + written, suffix, sizeof(suffix));
}

/**
 * @brief Implementation of Settings_UI_Initialize.
 *
 * See header for full contract documentation.
 */
void Settings_UI_Initialize(void)
{
    //Reset reason cannot change while the device is running, so it only needs to be calculated once.
    lv_label_set_text(
        ui_RestartValueLabel,
        Settings_UI_GetRestartReasonText());
}

/**
 * @brief Implementation of Settings_UI_Update.
 *
 * See header for full contract documentation.
 */
void Settings_UI_Update(const app_state_t *app)
{
    if (app == NULL)
    {
        return;
    }

    uint64_t uptime_seconds =
        (uint64_t)esp_timer_get_time() / 1000000ULL;

    static uint64_t previous_update_seconds = UINT64_MAX;

    // Settings UI does not require frequent updates, so we can limit it to once a second instead of the usual 50ms update timer
    if (uptime_seconds == previous_update_seconds)
    {
        return;
    }
    previous_update_seconds = uptime_seconds;

    char uptime_text[32];
    char last_update_text[32];

    TimeFormat_FormatDuration(
        uptime_text,
        sizeof(uptime_text),
        uptime_seconds);

    Settings_UI_FormatLastUpdate(
        last_update_text,
        sizeof(last_update_text),
        app->last_recommendation_update_seconds,
        uptime_seconds);

    lv_label_set_text(ui_UptimeValueLabel, uptime_text);

    lv_label_set_text(
        ui_SystemValueLabel,
        Settings_UI_GetSystemStatusText(&app->system_status));

    lv_label_set_text(
        ui_LastValueLabel,
        last_update_text);

    /*
     * Add the time-synchronization label here when its public getter
     * has been finalized.
     */
    lv_label_set_text(
        ui_TimeValueLabel,
        TimeSync_IsSynced() ? "Synchronized" : "Waiting"
    );
}
