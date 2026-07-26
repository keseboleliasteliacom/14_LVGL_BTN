/**
 * @file ui_Screen1.h
 * @brief Screen 1 UI entry points and shared UI objects.
 *
 * Declares the screen initialization, teardown, and periodic update hooks used
 * by the generated SquareLine Studio UI code.
 *
 * @ingroup UI
 */

#ifndef UI_SCREEN1_H
#define UI_SCREEN1_H

#include "Main_UI.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Background UI update task for Screen 1.
 *
 * Runs periodic UI refresh work for the screen in task context.
 *
 * @param[in] arg Task argument passed from the caller.
 */
void ui_update_task(void *arg);

/**
 * @brief Initializes Screen 1 UI objects.
 */
extern void ui_Screen1_screen_init(void);

/**
 * @brief Destroys Screen 1 UI objects.
 */
extern void ui_Screen1_screen_destroy(void);

// CUSTOM VARIABLES
extern lv_obj_t * uic_WiFi;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
