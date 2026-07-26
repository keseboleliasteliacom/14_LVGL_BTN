#ifndef SENSOR_UI_H
#define SENSOR_UI_H

#include "lvgl.h"

/**
 * @file Sensor_UI.h
 * @brief Public API for the sensor UI tab.
 *
 * Declares the LVGL handles and update hooks used by the Home tab sensor
 * display.
 *
 * @ingroup SENSOR_UI
 */

/**
 * @defgroup SENSOR_UI Sensor UI
 * @brief Home tab sensor display for temperature, humidity, and pressure.
 *
 * Creates and updates LVGL objects on the Home tab. LVGL must be initialized
 * and the screen objects must be available before initialization.
 * @{
 */

/**
 * @brief LVGL object handles used by the sensor UI.
 *
 * The pointers reference objects created during initialization and refreshed by
 * the tab update logic.
 */
typedef struct
{
    lv_obj_t *arc_temperature_dyn;   /**< Dynamic temperature arc. */
    lv_obj_t *arc_humidity_dyn;      /**< Dynamic humidity arc. */
    lv_obj_t *arc_pressure_dyn;      /**< Dynamic pressure arc. */
    lv_obj_t *temperature_label_dyn; /**< Dynamic temperature label. */
    lv_obj_t *pressure_label_dyn;    /**< Dynamic pressure label. */
    lv_obj_t *humidity_label_dyn;    /**< Dynamic humidity label. */
    lv_obj_t *latest_data_label;     /**< Latest-data status label. */
} Sensor_UI;

/**
 * @brief Creates the sensor tab widgets.
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
