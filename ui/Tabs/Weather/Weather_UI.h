#ifndef WEATHER_UI_H
#define WEATHER_UI_H

#include "lvgl.h"

/**
 * @file Weather_UI.h
 * @brief Public API for the Weather UI module.
 *
 * Provides the LVGL widget definitions and setup/update functions used by the
 * weather tab and dashboard views.
 *
 * @defgroup WEATHER_UI Weather UI
 * @brief Weather tab and dashboard user interface.
 *
 * Creates and updates LVGL objects that present forecast data from the weather
 * fetcher. Call the setup functions before any update function so the widget
 * pointers are valid.
 * @{
 */

/**
 * @brief Collection of LVGL objects used by the hourly weather grid.
 *
 * Each array entry corresponds to one forecast cell in the 24-hour view.
 */
typedef struct
{
    lv_obj_t *hourLabel[24]; /**< Hour label objects. */
    lv_obj_t *tempLabel[24]; /**< Temperature label objects. */
    lv_obj_t *iconLabel[24]; /**< Weather indicator label objects. */
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
 * Stores the current-weather card, the forecast list container, and the row
 * widget handles created by the implementation.
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
 * Refreshes the current-conditions card and 24-row forecast list with the
 * latest fetched weather snapshot when available.
 */
void Weather_UI_Update_test();

/**
 * @brief Creates the weather dashboard view.
 *
 * Initializes the LVGL styles and builds the current conditions card plus the
 * 24-hour forecast list on the weather tab page.
 */
void weather_dashboard_create(void);

/** @} */

#endif
