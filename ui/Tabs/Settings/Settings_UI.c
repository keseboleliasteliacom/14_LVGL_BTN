/**
 * @file Settings_UI.c
 * @brief Implementation of the Settings tab UI update helpers.
 *
 * @ingroup SETTINGS_UI
 */

#include "Settings_UI.h"

#include "../../screens/Main_UI.h"
#include "../../../main/SNTP/time_sync.h"
#include "../../../main/Config/AppConfig.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "TimeFormat.h" // from "Utils" folder

static const char *TAG = "Settings_UI";

typedef struct
{
    lv_obj_t *sensor_dropdown;
    lv_obj_t *fetch_dropdown;
    lv_obj_t *apply_button;
    lv_obj_t *status_label;
    app_state_t *app;
    uint32_t sensor_custom_value;
    uint32_t fetch_custom_value;
    uint32_t displayed_sensor_value;
    uint32_t displayed_fetch_value;
    bool sensor_has_custom_option;
    bool fetch_has_custom_option;
    bool sensor_dirty;
    bool fetch_dirty;
    bool values_loaded;
} settings_controls_t;

static settings_controls_t controls;

static const uint32_t sensor_presets[] = {1000, 2000, 3000, 4000, 5000, 10000, 30000, 60000};
static const char *sensor_preset_labels[] = {
    "1 second", "2 seconds", "3 seconds", "4 seconds", "5 seconds",
    "10 seconds", "30 seconds", "60 seconds"};

static const uint32_t fetch_presets[] = {1, 2, 3, 4, 5, 15, 30, 60, 120, 360, 720, 1440};
static const char *fetch_preset_labels[] = {
    "1 minute", "2 minutes", "3 minutes", "4 minutes", "5 minutes",
    "15 minutes", "30 minutes", "1 hour", "2 hours", "6 hours", "12 hours", "24 hours"};

static void Settings_UI_ApplyEvent(lv_event_t *event);
static void Settings_UI_DropdownChanged(lv_event_t *event);
static void Settings_UI_SyncConfig(app_state_t *app);

/**
 * @brief Sets the standard white label text style used by Settings labels.
 *
 * @param label Label object to style.
 */
static void Settings_UI_SetLabelStyle(lv_obj_t *label)
{
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
}

/**
 * @brief Finds a preset value in an array of preset values.
 *
 * @param[in] values Preset values to search.
 * @param[in] count Number of entries in @p values.
 * @param[in] value Value to search for.
 *
 * @return Matching preset index, or `-1` if the value is not present.
 */
static int Settings_UI_FindPreset(const uint32_t *values, size_t count, uint32_t value)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (values[i] == value)
        {
            return (int)i;
        }
    }
    return -1;
}

/**
 * @brief Builds a newline-separated dropdown option string.
 *
 * @param[out] buffer Output buffer for the dropdown text.
 * @param[in] buffer_size Size of @p buffer in bytes.
 * @param[in] labels Preset labels to append.
 * @param[in] count Number of labels in @p labels.
 * @param[in] custom_label Optional first option shown for a custom current value.
 */
static void Settings_UI_BuildOptions(
    char *buffer,
    size_t buffer_size,
    const char *const *labels,
    size_t count,
    const char *custom_label)
{
    size_t used = 0;
    buffer[0] = '\0';

    if (custom_label != NULL)
    {
        int written = snprintf(buffer, buffer_size, "%s\n", custom_label);
        if (written < 0 || (size_t)written >= buffer_size)
        {
            return;
        }
        used = (size_t)written;
    }

    for (size_t i = 0; i < count && used < buffer_size; ++i)
    {
        int written = snprintf(
            buffer + used,
            buffer_size - used,
            "%s%s",
            labels[i],
            i + 1 < count ? "\n" : "");
        if (written < 0 || (size_t)written >= buffer_size - used)
        {
            buffer[buffer_size - 1] = '\0';
            return;
        }
        used += (size_t)written;
    }
}

/**
 * @brief Updates the sensor interval dropdown to show the current value.
 *
 * @param[in] value Sensor interval in milliseconds.
 */
static void Settings_UI_SetSensorDropdown(uint32_t value)
{
    char options[160];
    char custom[32];
    int preset = Settings_UI_FindPreset(
        sensor_presets,
        sizeof(sensor_presets) / sizeof(sensor_presets[0]),
        value);

    controls.sensor_has_custom_option = preset < 0;
    controls.sensor_custom_value = value;
    if (controls.sensor_has_custom_option)
    {
        snprintf(custom, sizeof(custom), "Current: %lu ms", (unsigned long)value);
    }
    Settings_UI_BuildOptions(
        options,
        sizeof(options),
        sensor_preset_labels,
        sizeof(sensor_preset_labels) / sizeof(sensor_preset_labels[0]),
        controls.sensor_has_custom_option ? custom : NULL);
    lv_dropdown_set_options(controls.sensor_dropdown, options);
    lv_dropdown_set_selected(
        controls.sensor_dropdown,
        controls.sensor_has_custom_option ? 0U : (uint16_t)preset);
    controls.displayed_sensor_value = value;
}

/**
 * @brief Updates the fetch interval dropdown to show the current value.
 *
 * @param[in] value Fetch interval in minutes.
 */
static void Settings_UI_SetFetchDropdown(uint32_t value)
{
    char options[192];
    char custom[40];
    int preset = Settings_UI_FindPreset(
        fetch_presets,
        sizeof(fetch_presets) / sizeof(fetch_presets[0]),
        value);

    controls.fetch_has_custom_option = preset < 0;
    controls.fetch_custom_value = value;
    if (controls.fetch_has_custom_option)
    {
        snprintf(custom, sizeof(custom), "Current: %lu minutes", (unsigned long)value);
    }
    Settings_UI_BuildOptions(
        options,
        sizeof(options),
        fetch_preset_labels,
        sizeof(fetch_preset_labels) / sizeof(fetch_preset_labels[0]),
        controls.fetch_has_custom_option ? custom : NULL);
    lv_dropdown_set_options(controls.fetch_dropdown, options);
    lv_dropdown_set_selected(
        controls.fetch_dropdown,
        controls.fetch_has_custom_option ? 0U : (uint16_t)preset);
    controls.displayed_fetch_value = value;
}

/**
 * @brief Returns the currently selected sensor interval.
 *
 * @return Sensor interval in milliseconds.
 */
static uint32_t Settings_UI_GetSensorSelection(void)
{
    uint16_t selected = lv_dropdown_get_selected(controls.sensor_dropdown);
    if (controls.sensor_has_custom_option)
    {
        if (selected == 0)
        {
            return controls.sensor_custom_value;
        }
        --selected;
    }
    return sensor_presets[selected];
}

/**
 * @brief Returns the currently selected fetch interval.
 *
 * @return Fetch interval in minutes.
 */
static uint32_t Settings_UI_GetFetchSelection(void)
{
    uint16_t selected = lv_dropdown_get_selected(controls.fetch_dropdown);
    if (controls.fetch_has_custom_option)
    {
        if (selected == 0)
        {
            return controls.fetch_custom_value;
        }
        --selected;
    }
    return fetch_presets[selected];
}

/**
 * @brief Handles dropdown value changes and updates the unsaved-state label.
 *
 * @param[in] event LVGL value-changed event.
 */
static void Settings_UI_DropdownChanged(lv_event_t *event)
{
    lv_obj_t *target = lv_event_get_target(event);
    if (target == controls.sensor_dropdown)
    {
        controls.sensor_dirty = Settings_UI_GetSensorSelection() != controls.displayed_sensor_value;
    }
    else if (target == controls.fetch_dropdown)
    {
        controls.fetch_dirty = Settings_UI_GetFetchSelection() != controls.displayed_fetch_value;
    }

    lv_label_set_text(
        controls.status_label,
        controls.sensor_dirty || controls.fetch_dirty ? "Unsaved changes" : "No changes");
}

/**
 * @brief Applies the selected Settings values to NVS and the shared app state.
 *
 * @param[in] event LVGL click event.
 */
static void Settings_UI_ApplyEvent(lv_event_t *event)
{
    (void)event;
    if (controls.app == NULL)
    {
        lv_label_set_text(controls.status_label, "Configuration unavailable");
        return;
    }

    uint32_t sensor_value = Settings_UI_GetSensorSelection();
    uint32_t fetch_value = Settings_UI_GetFetchSelection();
    bool changed = false;
    bool failed = false;

    if (sensor_value != controls.app->config_data.sensor_interval_ms)
    {
        if (Config_WriteToNVS_SensorIntervalMs(sensor_value) == 0)
        {
            controls.app->config_data.sensor_interval_ms = sensor_value;
            changed = true;
        }
        else
        {
            failed = true;
            ESP_LOGW(TAG, "Failed to save sensor interval");
        }
    }

    if (fetch_value != controls.app->config_data.fetch_interval_minutes)
    {
        if (Config_WriteToNVS_FetchIntervalMinutes(fetch_value) == 0)
        {
            controls.app->config_data.fetch_interval_minutes = fetch_value;
            changed = true;
        }
        else
        {
            failed = true;
            ESP_LOGW(TAG, "Failed to save LEOP fetch interval");
        }
    }

    controls.sensor_dirty = false;
    controls.fetch_dirty = false;
    Settings_UI_SetSensorDropdown(controls.app->config_data.sensor_interval_ms);
    Settings_UI_SetFetchDropdown(controls.app->config_data.fetch_interval_minutes);
    lv_label_set_text(
        controls.status_label,
        failed ? (changed ? "Partially saved" : "Save failed") : (changed ? "Saved" : "No changes"));
}

/**
 * @brief Syncs the UI state to the shared application configuration.
 *
 * @param[in] app Shared application state snapshot.
 */
static void Settings_UI_SyncConfig(app_state_t *app)
{
    if (app == NULL)
    {
        return;
    }

    controls.app = app;
    if (!controls.sensor_dirty &&
        (!controls.values_loaded || controls.displayed_sensor_value != app->config_data.sensor_interval_ms))
    {
        Settings_UI_SetSensorDropdown(app->config_data.sensor_interval_ms);
    }
    if (!controls.fetch_dirty &&
        (!controls.values_loaded || controls.displayed_fetch_value != app->config_data.fetch_interval_minutes))
    {
        Settings_UI_SetFetchDropdown(app->config_data.fetch_interval_minutes);
    }

    if (!controls.values_loaded)
    {
        controls.values_loaded = true;
        lv_obj_clear_state(controls.apply_button, LV_STATE_DISABLED);
        lv_label_set_text(controls.status_label, "Ready");
    }
}


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

/**
 * @brief Formats the time elapsed since the last successful recommendation update.
 *
 * @param[out] buffer Output buffer for the formatted text.
 * @param[in] buffer_size Size of @p buffer in bytes.
 * @param[in] last_update_seconds Monotonic seconds-since-boot timestamp of the last successfull recommendation udatep.
 * @param[in] uptime_seconds Current monotonic uptime in seconds.
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

    lv_obj_t *sensor_label = lv_label_create(ui_SettingsConfigContainer);
    lv_label_set_text(sensor_label, "Sensor update interval");
    lv_obj_set_width(sensor_label, 360);
    lv_obj_set_pos(sensor_label, 0, -145);
    lv_obj_set_align(sensor_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(sensor_label, LV_TEXT_ALIGN_LEFT, 0);
    Settings_UI_SetLabelStyle(sensor_label);

    controls.sensor_dropdown = lv_dropdown_create(ui_SettingsConfigContainer);
    lv_obj_set_size(controls.sensor_dropdown, 360, LV_SIZE_CONTENT);
    lv_obj_set_pos(controls.sensor_dropdown, 0, -105);
    lv_obj_set_align(controls.sensor_dropdown, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(controls.sensor_dropdown, lv_color_hex(0x301E4D), 0);
    lv_obj_set_style_text_color(controls.sensor_dropdown, lv_color_white(), 0);
    lv_obj_add_event_cb(
        controls.sensor_dropdown,
        Settings_UI_DropdownChanged,
        LV_EVENT_VALUE_CHANGED,
        NULL);

    lv_obj_t *fetch_label = lv_label_create(ui_SettingsConfigContainer);
    lv_label_set_text(fetch_label, "LEOP fetch interval");
    lv_obj_set_width(fetch_label, 360);
    lv_obj_set_pos(fetch_label, 0, -45);
    lv_obj_set_align(fetch_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(fetch_label, LV_TEXT_ALIGN_LEFT, 0);
    Settings_UI_SetLabelStyle(fetch_label);

    controls.fetch_dropdown = lv_dropdown_create(ui_SettingsConfigContainer);
    lv_obj_set_size(controls.fetch_dropdown, 360, LV_SIZE_CONTENT);
    lv_obj_set_pos(controls.fetch_dropdown, 0, -5);
    lv_obj_set_align(controls.fetch_dropdown, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(controls.fetch_dropdown, lv_color_hex(0x301E4D), 0);
    lv_obj_set_style_text_color(controls.fetch_dropdown, lv_color_white(), 0);
    lv_obj_add_event_cb(
        controls.fetch_dropdown,
        Settings_UI_DropdownChanged,
        LV_EVENT_VALUE_CHANGED,
        NULL);

    controls.apply_button = lv_btn_create(ui_SettingsConfigContainer);
    lv_obj_set_size(controls.apply_button, 140, 48);
    lv_obj_set_pos(controls.apply_button, 110, 70);
    lv_obj_set_align(controls.apply_button, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(controls.apply_button, lv_color_hex(0x6E10CE), 0);
    lv_obj_add_state(controls.apply_button, LV_STATE_DISABLED);
    lv_obj_add_event_cb(
        controls.apply_button,
        Settings_UI_ApplyEvent,
        LV_EVENT_CLICKED,
        NULL);

    lv_obj_t *apply_label = lv_label_create(controls.apply_button);
    lv_label_set_text(apply_label, "Apply");
    lv_obj_center(apply_label);

    controls.status_label = lv_label_create(ui_SettingsConfigContainer);
    lv_label_set_text(controls.status_label, "Loading...");
    lv_obj_set_width(controls.status_label, 360);
    lv_obj_set_pos(controls.status_label, 0, 125);
    lv_obj_set_align(controls.status_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(controls.status_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(controls.status_label, lv_color_hex(0xD8CCE5), 0);
}

/**
 * @brief Implementation of Settings_UI_Update.
 *
 * See header for full contract documentation.
 */
void Settings_UI_Update(app_state_t *app)
{
    if (app == NULL)
    {
        return;
    }

    Settings_UI_SyncConfig(app);

    uint64_t uptime_seconds =
        (uint64_t)esp_timer_get_time() / 1000000ULL;

    static uint64_t previous_update_seconds = UINT64_MAX;

    // Settings UI does not require frequent updates, so we can limit the information labels update to once a second instead of the usual 50ms update timer
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
