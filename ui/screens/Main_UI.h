#ifndef MAIN_UI_H
#define MAIN_UI_H
#include "../../main/WiFi.h"
#include "../../main/app_queues.h"
#include "../app_types.h"

#include "lvgl.h"

/**
 * @file Main_UI.h
 * @brief Public declarations for the main LVGL screen.
 *
 * Declares the LVGL objects created by the main UI modules and the
 * initialization entry point used during display bring-up.
 *
 * @ingroup UI
 */

/**
 * @defgroup UI UI
 * @brief User interface screens and display integration.
 *
 * Main LVGL screen objects exported for the display layer and application
 * status updates. Initialization depends on LVGL and the screen setup order
 * used by the UI bring-up code.
 * @{
 */

extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Glennergy_Label;
extern lv_obj_t * ui_Tab_Main;
extern lv_obj_t * ui_TabPage_Home;
extern lv_obj_t * ui_TabPage_Electricity;
extern lv_obj_t * ui_TabPage_Weather;
extern lv_obj_t * ui_Chart_Weather;
extern lv_obj_t * ui_TabPage_WiFi;
extern lv_obj_t * ui_Group_WiFi;
extern lv_obj_t * ui_LEOP_Label;
extern lv_obj_t * ui_LEOP_Connected_Label;
extern lv_obj_t * ui_TabPage_Settings;
extern lv_obj_t * ui_SettingsContainer;
extern lv_obj_t * ui_SettingsConfigContainer;
extern lv_obj_t * ui_SettingsConfigTitle;

extern lv_obj_t *ui_UptimeInfoLabel;
extern lv_obj_t *ui_UptimeValueLabel;
extern lv_obj_t *ui_RestartInfoLabel;
extern lv_obj_t *ui_RestartValueLabel;
extern lv_obj_t *ui_SystemInfoLabel;
extern lv_obj_t *ui_SystemValueLabel;
extern lv_obj_t *ui_LastInfoLabel;
extern lv_obj_t *ui_LastValueLabel;
extern lv_obj_t *ui_TimeInfoLabel;
extern lv_obj_t *ui_TimeValueLabel;

/**
 * @brief Creats the LVGL objects objects owned by main screen module.
 *
 * Creates the main screen, tab view and pages, Settings containers, and status labels.
 * Tab-specific modules create their own widgets after this function returns
 * 
 * @note Exported pointers owned by other UI modules may remain NULL until those modules are initialized
 */
void Main_UI_Initialize();

/** @} */

#endif
