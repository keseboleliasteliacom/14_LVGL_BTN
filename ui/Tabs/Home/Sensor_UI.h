#ifndef SENSOR_UI_H
#define SENSOR_UI_H

#include "lvgl.h"

/**
 * @file Sensor_UI.h
 * @brief Public API for the sensor UI tab.
 *
 * Declares the LVGL objects and update hooks used by the Home tab sensor
 * display.
 *
 * @ingroup SENSOR_UI
 */

/**
 * @defgroup SENSOR_UI Sensor UI
 * @brief Home tab sensor display for temperature, humidity, and pressure.
 *
 * This module creates and updates LVGL objects on the Home tab. It depends on
 * LVGL being initialized and on the UI screen objects being available before
 * the initialize function is called.
 * @{
 */

/**
 * @brief LVGL object handles used by the sensor UI.
 *
 * The pointers reference UI elements created during initialization and updated
 * later by the tab refresh logic.
 */
typedef struct
{
    lv_obj_t *arc_temperature_dyn;
    lv_obj_t *arc_humidity_dyn;
    lv_obj_t *arc_pressure_dyn;
    lv_obj_t *temperature_label_dyn;
    lv_obj_t *pressure_label_dyn;
    lv_obj_t *humidity_label_dyn;
    lv_obj_t *latest_data_label;
} Sensor_UI;

/**
 * @brief Creates the sensor tab widgets.
 *
 * Initializes the dynamic LVGL objects used by the Home tab sensor display.
 */
void Sensor_UI_Initialize();

/**
 * @brief Refreshes the sensor tab values.
 *
 * Pulls the latest queued sensor data, if available, and updates the LVGL
 * labels.
 */
void Sensor_UI_Update(void);

/** @} */

#endif
