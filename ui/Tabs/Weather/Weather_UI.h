#ifndef WEATHER_UI_H
#define WEATHER_UI_H

#include "lvgl.h"

/**
 * @file Weather_UI.h
 * @brief Public API for the Weather UI module.
 *
 * Provides the UI setup and update functions for the weather tabs and
 * dashboard views.
 *
 * @defgroup WEATHER_UI Weather UI
 * @brief Weather tab and dashboard user interface.
 *
 * Creates and updates LVGL objects that present forecast data from the weather
 * fetcher. Call the setup function before any update function so the widget
 * pointers are valid.
 * @{
 */

/**
 * @brief Collection of LVGL objects used by the hourly weather grid.
 *
 * Each entry corresponds to one forecast cell in the 24-hour view.
 */
typedef struct
{
    lv_obj_t *hourLabel[24]; /**< Hour label objects. */
    lv_obj_t *tempLabel[24]; /**< Temperature label objects. */
    lv_obj_t *iconLabel[24]; /**< Weather icon label objects. */
    lv_obj_t *weather_code[24]; /**< Weather code label objects. */
} Weather_UI;

/**
 * @brief LVGL widgets that make up one forecast row.
 */
typedef struct
{
    lv_obj_t *row; /**< Row container. */
    lv_obj_t *icon; /**< Weather icon widget. */
    lv_obj_t *temp; /**< Temperature label widget. */
    lv_obj_t *time; /**< Time label widget. */
} forecast_row_t;

/**
 * @brief Widget collection for the weather dashboard view.
 *
 * Stores references to the current-weather card, the forecast list, and the
 * per-row widgets created by the implementation.
 */
typedef struct
{
    lv_obj_t *icon; /**< Current weather icon. */
    lv_obj_t *current_temp; /**< Current temperature label. */
    lv_obj_t *current_weather; /**< Current weather description label. */
    lv_obj_t *forecast_list; /**< Container for forecast rows. */

    lv_obj_t* left_card; /**< Left-side card container. */
    lv_obj_t* right_card; /**< Right-side card container. */

    lv_obj_t *forecast_rows1[24]; /**< Unused forecast row handles. */
    lv_obj_t *forecast_temp[24]; /**< Unused forecast temperature handles. */
    lv_obj_t *forecast_time[24]; /**< Unused forecast time handles. */
    lv_obj_t *forecast_icon[24]; /**< Unused forecast icon handles. */

    forecast_row_t forecast_rows[24]; /**< Created forecast row widgets. */

} Weather_UI_test;

/**
 * @brief Creates the hourly weather grid UI.
 */
void Weather_UI_Initialize();

/**
 * @brief Updates the hourly weather grid from the weather queue.
 */
void Weather_UI_Update();

/**
 * @brief Updates the dashboard view from the weather queue.
 */
void Weather_UI_Update_test();

/**
 * @brief Creates the weather dashboard view.
 */
void weather_dashboard_create(void);

/** @} */

#endif
