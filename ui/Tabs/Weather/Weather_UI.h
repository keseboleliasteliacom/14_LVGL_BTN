#ifndef WEATHER_UI_H
#define WEATHER_UI_H

#include "lvgl.h"

/**
 * @file Weather_UI.h
 * @brief Public API for the Weather UI module.
 *
 * Provides the LVGL setup and update functions for the weather tab views.
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
 * Stores references to the current-weather card and the 24-row forecast list.
 * The arrays are used by the implementation to keep per-row widget handles.
 */
typedef struct
{
    lv_obj_t *icon; /**< Current weather icon. */
    lv_obj_t *current_temp; /**< Current temperature label. */
    lv_obj_t *current_weather; /**< Current weather description label. */
    lv_obj_t *forecast_list; /**< Container for forecast rows. */

    lv_obj_t* left_card; /**< Left-side card container. */
    lv_obj_t* right_card; /**< Right-side card container. */

    lv_obj_t *forecast_rows1[24]; /**< Reserved forecast row handles. */
    lv_obj_t *forecast_temp[24]; /**< Reserved forecast temperature handles. */
    lv_obj_t *forecast_time[24]; /**< Reserved forecast time handles. */
    lv_obj_t *forecast_icon[24]; /**< Reserved forecast icon handles. */

    forecast_row_t forecast_rows[24]; /**< Created forecast row widgets. */

} Weather_UI_test;

/**
 * @brief Creates the hourly weather grid UI.
 *
 * Builds the 24-cell forecast layout on the weather tab page.
 */
void Weather_UI_Initialize();

/**
 * @brief Updates the hourly weather grid from the weather queue.
 *
 * Reads the latest fetched weather snapshot when available and refreshes the
 * grid labels with time, temperature, UV index, and weather code values.
 */
void Weather_UI_Update();

/**
 * @brief Updates the dashboard view from the weather queue.
 *
 * Refreshes the current-weather card and the 24-row forecast list when a new
 * fetched weather snapshot is available.
 */
void Weather_UI_Update_test();

/**
 * @brief Creates the weather dashboard view.
 *
 * Initializes the shared LVGL styles and builds the current conditions card
 * plus the 24-hour forecast list on the weather tab page.
 */
void weather_dashboard_create(void);

/** @} */

#endif
