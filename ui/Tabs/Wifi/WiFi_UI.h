#ifndef WIFI_UI_H
#define WIFI_UI_H

#include "lvgl.h"

/**
 * @file WiFi_UI.h
 * @brief Public API for the Wi-Fi settings UI.
 *
 * Provides the LVGL widgets and update hooks used by the settings tab to
 * display scan results and connection status.
 */

/**
 * @defgroup WIFI_UI WiFi UI
 * @brief Wi-Fi settings screen integration.
 *
 * Creates and updates the LVGL controls used for Wi-Fi scanning and
 * connection feedback. The UI depends on the shared Wi-Fi command/result
 * queues and must be initialized after the LVGL screen objects are available.
 * @{
 */

/**
 * @brief Runtime state for the Wi-Fi UI widgets.
 *
 * Stores the LVGL objects used by the settings tab and the last selected SSID
 * copied from the dropdown.
 */
typedef struct{
    lv_obj_t* network_dropdown_dyn;
    lv_obj_t* password_textarea_dyn;
    lv_obj_t* status_label_sta;
    lv_obj_t* status_label_dyn;
    lv_obj_t* scan_button_dyn;
    lv_obj_t* wifi_label;
    lv_obj_t* ssid_label;
    lv_obj_t* group_label;
    char selected_ssid[64];
}WiFi_UI;

/**
 * @brief Creates the Wi-Fi settings UI widgets and registers callbacks.
 *
 * Call after the parent LVGL objects for the settings screen have been created.
 */
void WiFi_UI_Initialize();

/**
 * @brief Polls Wi-Fi results and updates the UI state.
 *
 * Call from the UI task context to consume queued Wi-Fi status updates and
 * refresh the visible labels and dropdown options.
 */
void WiFi_UI_Update();

/** @} */

#endif
