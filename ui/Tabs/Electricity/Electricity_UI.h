#ifndef ELECTRICITY_UI_H
#define ELECTRICITY_UI_H

#include "lvgl.h"

/**
 * @file Electricity_UI.h
 * @brief Public API for the Electricity tab UI.
 *
 * Provides the chart setup and refresh entry points used by the Electricity
 * tab to display recommendation data in LVGL.
 *
 * @defgroup ELECTRICITY_UI Electricity UI
 * @brief LVGL user interface for the Electricity tab.
 *
 * This module owns the chart widget and updates it from the recommendation
 * queue. It depends on the tab page and LVGL being initialized before use.
 * @{
 */

/**
 * @brief UI state for the Electricity tab.
 *
 * Holds the chart object used by the tab implementation.
 */
typedef struct
{
    lv_obj_t* ui_Chart_Electricity; /**< Chart widget instance. */
} Electricity_UI;

/**
 * @brief Initializes the Electricity tab UI.
 *
 * Creates the chart and supporting labels on the Electricity tab page.
 */
void Electricity_UI_Initialize();

/** @brief Returns the dashboard panel reserved for electricity prices. */
lv_obj_t *Electricity_UI_GetPricePanel(void);

/**
 * @brief Updates the Electricity tab UI.
 *
 * Pulls the latest recommendation data from the queue and refreshes the chart
 * when new data is available.
 */
void Electricity_UI_Update();

/** @} */

#endif
