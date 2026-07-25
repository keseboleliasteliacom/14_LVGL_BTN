#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include "../../../main/app_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes values that remain constant during this boot.
 *
 * Call after Main_UI_Initialize() has created the Settings labels.
 */
void Settings_UI_Initialize(void);

/**
 * @brief Refreshes the dynamic Settings information labels.
 *
 * Must be called while the LVGL lock is held.
 *
 * @param app Current shared application state.
 */
void Settings_UI_Update(const app_state_t *app);

#ifdef __cplusplus
}
#endif

#endif