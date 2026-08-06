/**
 * @file Sensor_UI.c
 * @brief Implementation of the sensor UI tab.
 *
 * @ingroup SENSOR_UI
 */

#include "Sensor_UI.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "esp_timer.h"
#include "lvgl_port.h"
#include "../../screens/ui_Screen1.h"
#include "../../../main/app_queues.h"
//#include "../../../main/sensor/sensor.h"
//#include "../../../main/hal/temperature_sensor.hpp";
//#include "../../../main/hal/temperature_sensor_c_api.h";
#include "../../../main/app_types.h"

/**
 * @brief Cached LVGL handles for the sensor tab.
 *
 * The pointers are initialized during UI setup and refreshed by the update
 * path.
 */
static Sensor_UI sensor_ui = {
    .arc_humidity_dyn = NULL,
    .arc_pressure_dyn = NULL,
    .arc_temperature_dyn = NULL,
    .humidity_label_dyn = NULL,
    .pressure_label_dyn = NULL,
    .temperature_label_dyn = NULL,
    .latest_data_label = NULL,
};

const static char* TAG = "Sensor_UI";


typedef int64_t elapsed_seconds_t;

/**
 * @brief Computes a non-negative elapsed-time difference.
 *
 * @param[in] now Current monotonic time in seconds.
 * @param[in] then Previous monotonic time in seconds.
 *
 * @return Elapsed seconds, or 0 if the timestamps are out of order.
 */
static elapsed_seconds_t monotonic_diff(uint64_t now, uint64_t then)
{
    if (now < then)
    {
        return 0;
    }

    return (elapsed_seconds_t)(now - then);
}

/**
 * @brief Formats an elapsed duration for display.
 *
 * @param[in] diff Elapsed time in seconds.
 * @param[out] buffer Destination buffer for the formatted string.
 * @param[in] buffer_size Size of @p buffer in bytes.
 */
static void format_elapsed(elapsed_seconds_t diff,
                           char *buffer,
                           size_t buffer_size)
{
    if (diff < 0)
    {
        diff = 0;
    }

    uint64_t seconds = (uint64_t)diff;

    uint64_t days = seconds / 86400;
    seconds %= 86400;

    uint64_t hours = seconds / 3600;
    seconds %= 3600;

    uint64_t minutes = seconds / 60;
    seconds %= 60;

    if (days > 0) {
        snprintf(buffer, buffer_size,
                 "%llud %lluh %llum %llus ago",
                 (unsigned long long)days,
                 (unsigned long long)hours,
                 (unsigned long long)minutes,
                 (unsigned long long)seconds);
    }
    else if (hours > 0) {
        snprintf(buffer, buffer_size,
                 "%lluh %llum %llus ago",
                 (unsigned long long)hours,
                 (unsigned long long)minutes,
                 (unsigned long long)seconds);
    }
    else if (minutes > 0) {
        snprintf(buffer, buffer_size,
                 "%llum %llus ago",
                 (unsigned long long)minutes,
                 (unsigned long long)seconds);
    }
    else {
        snprintf(buffer, buffer_size,
                 "%llus ago",
                 (unsigned long long)seconds);
    }
}

/**
 * @brief Builds the latest-data status text.
 *
 * Uses the monotonic update timestamp when available, and includes the wall
 * clock timestamp only when the source data reports it as valid.
 *
 * @param[in] sensor_data Source sensor data snapshot.
 * @param[out] buffer Destination buffer for the formatted string.
 * @param[in] buffer_size Size of @p buffer in bytes.
 */
static void format_latest_data(const sensor_data_t *sensor_data,
                               char *buffer,
                               size_t buffer_size)
{
    if (sensor_data->last_update_seconds == 0)
    {
        snprintf(buffer, buffer_size, "Latest data: unavailable");
        return;
    }

    char elapsed[64];
    uint64_t now_seconds = (uint64_t)(esp_timer_get_time() / 1000000ULL);
    format_elapsed(monotonic_diff(now_seconds, sensor_data->last_update_seconds),
                   elapsed,
                   sizeof(elapsed));

    if (sensor_data->wall_time_valid)
    {
        char timestamp[32];
        struct tm local_time;

        localtime_r(&sensor_data->last_unix_time, &local_time);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local_time);
        snprintf(buffer, buffer_size, "Latest data: %s (%s)", timestamp, elapsed);
    }
    else
    {
        snprintf(buffer, buffer_size, "Latest data: %s", elapsed);
    }
}

/**
 * @brief Creates the Home tab sensor widgets.
 *
 * Initializes the LVGL objects used by the sensor display.
 */
void Sensor_UI_Initialize()
{
    sensor_ui.temperature_label_dyn = lv_label_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.temperature_label_dyn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(sensor_ui.temperature_label_dyn, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(sensor_ui.temperature_label_dyn, -321);
    lv_obj_set_y(sensor_ui.temperature_label_dyn, -79);
    lv_obj_set_align(sensor_ui.temperature_label_dyn, LV_ALIGN_CENTER);
    lv_label_set_text(sensor_ui.temperature_label_dyn, "98.76°");
    lv_obj_set_style_text_font(sensor_ui.temperature_label_dyn, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    sensor_ui.pressure_label_dyn = lv_label_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.pressure_label_dyn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(sensor_ui.pressure_label_dyn, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(sensor_ui.pressure_label_dyn, 315);
    lv_obj_set_y(sensor_ui.pressure_label_dyn, -77);
    lv_obj_set_align(sensor_ui.pressure_label_dyn, LV_ALIGN_CENTER);
    lv_label_set_text(sensor_ui.pressure_label_dyn, "9999 hPa");
    lv_obj_set_style_text_font(sensor_ui.pressure_label_dyn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    sensor_ui.humidity_label_dyn = lv_label_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.humidity_label_dyn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(sensor_ui.humidity_label_dyn, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(sensor_ui.humidity_label_dyn, 5);
    lv_obj_set_y(sensor_ui.humidity_label_dyn, -83);
    lv_obj_set_align(sensor_ui.humidity_label_dyn, LV_ALIGN_CENTER);
    lv_label_set_text(sensor_ui.humidity_label_dyn, "99.999%");
    lv_obj_set_style_text_font(sensor_ui.humidity_label_dyn, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    sensor_ui.latest_data_label = lv_label_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.latest_data_label, LV_SIZE_CONTENT);
    lv_obj_set_height(sensor_ui.latest_data_label, LV_SIZE_CONTENT);
    lv_obj_set_x(sensor_ui.latest_data_label, -11);
    lv_obj_set_y(sensor_ui.latest_data_label, 133);
    lv_obj_set_align(sensor_ui.latest_data_label, LV_ALIGN_CENTER);
    lv_label_set_text(sensor_ui.latest_data_label, "Latest data: unavailable");



    sensor_ui.arc_temperature_dyn = lv_arc_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.arc_temperature_dyn, 226);
    lv_obj_set_height(sensor_ui.arc_temperature_dyn, 214);
    lv_obj_set_x(sensor_ui.arc_temperature_dyn, 34);
    lv_obj_set_y(sensor_ui.arc_temperature_dyn, -66);
    lv_obj_set_align(sensor_ui.arc_temperature_dyn, LV_ALIGN_LEFT_MID);
    lv_arc_set_value(sensor_ui.arc_temperature_dyn, 50);
    lv_obj_set_style_border_color(sensor_ui.arc_temperature_dyn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(sensor_ui.arc_temperature_dyn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(sensor_ui.arc_temperature_dyn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(sensor_ui.arc_temperature_dyn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(sensor_ui.arc_temperature_dyn, lv_color_hex(0xC86868), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(sensor_ui.arc_temperature_dyn, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(sensor_ui.arc_temperature_dyn, lv_color_hex(0xC36A6A), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sensor_ui.arc_temperature_dyn, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    sensor_ui.arc_humidity_dyn = lv_arc_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.arc_humidity_dyn, 226);
    lv_obj_set_height(sensor_ui.arc_humidity_dyn, 214);
    lv_obj_set_x(sensor_ui.arc_humidity_dyn, 323);
    lv_obj_set_y(sensor_ui.arc_humidity_dyn, -63);
    lv_obj_set_align(sensor_ui.arc_humidity_dyn, LV_ALIGN_CENTER);
    lv_arc_set_value(sensor_ui.arc_humidity_dyn, 50);

    lv_obj_set_style_arc_color(sensor_ui.arc_humidity_dyn, lv_color_hex(0x95C08B), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(sensor_ui.arc_humidity_dyn, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(sensor_ui.arc_humidity_dyn, lv_color_hex(0x95C08B), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sensor_ui.arc_humidity_dyn, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    sensor_ui.arc_pressure_dyn = lv_arc_create(ui_TabPage_Home);
    lv_obj_set_width(sensor_ui.arc_pressure_dyn, 226);
    lv_obj_set_height(sensor_ui.arc_pressure_dyn, 214);
    lv_obj_set_x(sensor_ui.arc_pressure_dyn, -339);
    lv_obj_set_y(sensor_ui.arc_pressure_dyn, -64);
    lv_obj_set_align(sensor_ui.arc_pressure_dyn, LV_ALIGN_RIGHT_MID);
    lv_arc_set_value(sensor_ui.arc_pressure_dyn, 50);
}

/**
 * @brief Refreshes the Home tab sensor values.
 *
 * Reads one queued sensor update without blocking and updates the LVGL labels
 * when new data is available.
 */
void Sensor_UI_Update(void)
{
    sensor_data_t sensor_data;

    if (Sensor_Queue == NULL || xQueueReceive(Sensor_Queue, &sensor_data, 0) != pdPASS)
    {
        return;
    }

    if (sensor_data.valid)
    {
        char temperature[50];
        char humidity[50];
        char pressure[50];

        snprintf(temperature, sizeof(temperature), "%2.1f°C", sensor_data.temperature);
        snprintf(humidity, sizeof(humidity), "%2.1f%%", sensor_data.humidity);
        snprintf(pressure, sizeof(pressure), "%.1f hPa", sensor_data.pressure);

        lv_label_set_text(sensor_ui.temperature_label_dyn, temperature);
        lv_label_set_text(sensor_ui.humidity_label_dyn, humidity);
        lv_label_set_text(sensor_ui.pressure_label_dyn, pressure);
        lv_label_set_text(sensor_ui.latest_data_label, "");
    }
    else
    {
        char latest_data[128];
        format_latest_data(&sensor_data, latest_data, sizeof(latest_data));
        lv_label_set_text(sensor_ui.latest_data_label, latest_data);
    }
}
