#ifndef PRICE_UI_H
#define PRICE_UI_H

#include "lvgl.h"

/**
 * @file Price_UI.h
 * @brief Public API for the electricity price UI tab.
 *
 * Declares the LVGL widgets used to show hourly electricity prices and the
 * functions that create and refresh the tab content.
 *
 * @ingroup Electricity
 */

/**
 * @defgroup Electricity Electricity
 * @brief Electricity-related UI and data presentation.
 *
 * Contains the tab content used to display electricity pricing data. The UI
 * depends on the LVGL runtime and on price data being available through the
 * associated queue-based update path.
 * @{
 */

/**
 * @brief LVGL widget handles for the electricity price view.
 *
 * The arrays contain 32 entries, one per possible hour shown in the tab.
 */
typedef struct
{
    lv_obj_t* price_panel; /**< Root container for the price list. */
    lv_obj_t *hourLabel[32]; /**< Hour labels, indexed by display row. */
    lv_obj_t *priceLabel[32]; /**< Price labels, indexed by display row. */
} Price_UI;

/**
 * @brief Creates the electricity price UI.
 *
 * Initializes the LVGL widgets for the tab and prepares the 24 hourly rows.
 */
void Price_UI_Initialize();

/**
 * @brief Refreshes the electricity price UI from the latest queued data.
 *
 * Reads the latest price snapshot if one is available and updates the LVGL
 * labels in the tab.
 */
void Price_UI_Update();

/** @} */

#endif
