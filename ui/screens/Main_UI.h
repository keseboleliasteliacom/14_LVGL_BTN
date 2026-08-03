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
 * Declares the LVGL objects created by the main screen setup code and the
 * initialization entry point used during display bring-up.
 *
 * @ingroup UI
 */

/**
 * @defgroup UI UI
 * @brief User interface screens and display integration.
 *
 * This module provides the main LVGL screen and its exported widget handles.
 * UI initialization depends on LVGL and the display layer being ready.
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
//extern lv_obj_t * ui_Group_Settings; 

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
 * @brief Initializes the main UI screen and its widgets.
 *
 * Creates the screen, tab view, and status labels used by the application UI.
 */
void Main_UI_Initialize();

/** @} */

#endif
