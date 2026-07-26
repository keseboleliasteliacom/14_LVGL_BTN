/**
 * @file AppConfig.c
 * @brief Implementation of the AppConfig module.
 *
 * @ingroup APP_CONFIG
 */

#include "AppConfig.h"

static const char* TAG = "AppConfig";

/**
 * @brief Internal helper that loads the fetch interval from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_FetchIntervalMinutes(config_data_t* config);

/**
 * @brief Internal helper that loads the sensor interval from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_SensorInterval(config_data_t* config);

/**
 * @brief Internal helper that loads the test mode flag from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_TestMode(config_data_t* config);


// NOTE - NVS namespaces and keys are limited to 15 characters maximum.
// So when working with keys IN NVS the variable names are shortened(e.g. LEOP's "fetch_interval_minutes" -> "leop_min")





/**
 * @brief Sets the configuration defaults.
 *
 * @param[in,out] config Configuration data to initialize.
 */
void Config_SetDefaults(config_data_t* config) {
    config->fetch_interval_minutes = 1;
    config->test_mode = false;
    config->sensor_interval_ms = 1000;
}

/**
 * @brief Implementation of Config_LoadFromNVS.
 *
 * See header for full contract documentation.
 */
void Config_LoadFromNVS(config_data_t* config) {
    Config_LoadFromNVS_FetchIntervalMinutes(config);
    Config_LoadFromNVS_TestMode(config);
    Config_LoadFromNVS_SensorInterval(config);
}

/**
 * @brief Loads the fetch interval from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_FetchIntervalMinutes(config_data_t* config) {
    int leop_fetch_interval_minutes = 0;
    int leop_fetch_result = NVS_ReadU32("config", "leop_min", &leop_fetch_interval_minutes);
    if (leop_fetch_result != 0) {
        ESP_LOGW(TAG, "Loading \"leop_min\" from NVS failed.");
    }
    else {
        config->fetch_interval_minutes = leop_fetch_interval_minutes;
    }
}

/**
 * @brief Loads the sensor interval from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_SensorInterval(config_data_t* config) {
    int sensor_interval = 0;
    int sensor_interval_result = NVS_ReadU32("config", "sensor_ms", &sensor_interval);
    if (sensor_interval_result != 0) {
        ESP_LOGW(TAG, "Loading \"sensor_ms\" from NVS failed.");
    }
    else {
        config->sensor_interval_ms = sensor_interval;
    }
}

/**
 * @brief Loads the test mode flag from NVS.
 *
 * @param[in,out] config Configuration data to update.
 */
void Config_LoadFromNVS_TestMode(config_data_t* config) {
    bool test_mode = false;
    int test_mode_result = NVS_ReadBool("config", "test_mode", &test_mode);
    if (test_mode_result != 0) {
        ESP_LOGW(TAG, "Loading \"test_mode\" from NVS failed.");
    }
    else {
        config->test_mode = test_mode;
    }
}




// TODO - Makro for config settings?
// TODO - error handling? Duplicated? Where?
/**
 * @brief Implementation of Config_WriteToNVS_FetchIntervalMinutes.
 *
 * See header for full contract documentation.
 */
int Config_WriteToNVS_FetchIntervalMinutes(uint32_t interval) {
    //int result = NVS_Write32("config", "fetch_interval_minutes", interval);
    int result = NVS_WriteU32("config", "leop_min", interval);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Implementation of Config_WriteToNVS_TestMode.
 *
 * See header for full contract documentation.
 */
int Config_WriteToNVS_TestMode(bool mode) {
    int result = NVS_WriteBool("config", "test_mode", mode);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Implementation of Config_WriteToNVS_SensorIntervalMs.
 *
 * See header for full contract documentation.
 */
int Config_WriteToNVS_SensorIntervalMs(uint32_t interval) {
    int result = NVS_WriteU32("config", "sensor_ms", interval);
    if (result != 0) {
        return -1;
    }
    return 0;
}
