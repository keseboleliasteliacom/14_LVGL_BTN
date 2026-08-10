#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "../app_types.h"
#include <stdbool.h>
#include "Memory/NVS.h"

/**
 * @file AppConfig.h
 * @brief Public API for the application configuration module.
 *
 * Provides defaults, NVS-backed loading, and NVS write helpers for the
 * runtime configuration used by the application.
 */

/**
 * @defgroup APP_CONFIG AppConfig
 * @brief Application configuration helpers.
 *
 * Loads and stores configuration values through NVS. The module depends on the
 * NVS layer being available before configuration values are read or written.
 * @{
 */

/**
 * @brief Sets the configuration defaults.
 *
 * @param[in,out] config Configuration data to initialize.
 */
void Config_SetDefaults(config_data_t* config);

/**
 * @brief Loads configuration values from NVS.
 *
 * @param[in,out] config Configuration data to update.
 *
 * @note Reads values from the "config" NVS namespace and leaves default values
 * unchanged when a read fails.
 */
void Config_LoadFromNVS(config_data_t* config);

#ifdef __cplusplus
extern "C" {
#endif

// Todo - enum or proper error codes?
/**
 * @brief Writes the fetch interval to NVS.
 *
 * @param[in] interval Fetch interval in minutes.
 *
 * @return `0` on success, `-1` on failure.
 *
 * @note Stores the value in the "config" namespace using the "leop_min" key.
 */
int Config_WriteToNVS_FetchIntervalMinutes(uint32_t interval);

/**
 * @brief Writes the test mode flag to NVS.
 *
 * @param[in] mode Test mode state.
 *
 * @return `0` on success, `-1` on failure.
 *
 * @note Stores the value in the "config" namespace using the "test_mode" key.
 */
int Config_WriteToNVS_TestMode(bool mode);

/**
 * @brief Writes the sensor interval to NVS.
 *
 * @param[in] interval Sensor interval in milliseconds.
 *
 * @return `0` on success, `-1` on failure.
 *
 * @note Stores the value in the "config" namespace using the "sensor_ms" key.
 */
int Config_WriteToNVS_SensorIntervalMs(uint32_t interval);

#ifdef __cplusplus
}
#endif 

/** @} */

#endif
