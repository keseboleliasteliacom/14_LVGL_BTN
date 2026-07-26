#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include "esp_check.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

/**
 * @file time_sync.h
 * @brief Public API for the SNTP time synchronization module.
 *
 * Starts SNTP and exposes a simple status query for the rest of the
 * application.
 *
 * @defgroup SNTP SNTP
 * @brief Time synchronization support based on ESP-IDF SNTP services.
 *
 * Initializes the SNTP client and provides a sync status query for other
 * modules.
 * @{
 */

/**
 * @brief Starts SNTP time synchronization.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 */
esp_err_t TimeSync_Start();

/**
 * @brief Returns whether time synchronization has completed.
 *
 * @return `true` if synchronized, otherwise `false`.
 */
bool TimeSync_IsSynced();

/** @} */

#endif
