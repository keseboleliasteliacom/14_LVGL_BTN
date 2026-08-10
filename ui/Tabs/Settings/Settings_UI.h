#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include "../../../main/app_types.h"

/**
 * @file Settings_UI.h
 * @brief Public API for the Settings tab UI helpers.
 *
 * Provides the initialization and refresh functions used by the Settings tab
 * after the LVGL objects have been created.
 *
 * @ingroup SETTINGS_UI
 */

/**
 * @defgroup SETTINGS_UI SETTINGS_UI
 * @brief Settings tab UI update helpers
 *
 * Initializes and updates the Settings tab labels. Call initialization after
 * the relevant UI objects exist, and call update while holding the LVGL lock.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the Settings tab UI state.
 *
 * Call after Main_UI_Initialize() has created the Settings labels.
 */
void Settings_UI_Initialize(void);

/**
 * @brief Refreshes the dynamic Settings information labels.
 *
 * @param app Current shared application state.
 *
 * @note Must be called while the LVGL lock is held.
 */
void Settings_UI_Update(app_state_t *app);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
