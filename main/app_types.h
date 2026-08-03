#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <task.h>
#include "Price.h"
#include "Recommendation.h"
#include "LEOP_Fetcher.h"
#include "Weather.h"

/**
 * @file app_types.h
 * @brief Shared application data types for system state, configuration, and task handles.
 *
 * Defines the data structures used to share application state between modules.
 */

/**
 * @defgroup APP_TYPES APP_TYPES
 * @brief Shared application data types.
 *
 * Provides enums and structs used to exchange recommendations, sensor data,
 * configuration, system status, and task handles between modules.
 * @{
 */

typedef enum {
    RECOMMENDATION_BUY = 1,
    RECOMMENDATION_SELL = 2,
    RECOMMENDATION_HOLD = 3, 
    RECOMMENDATION_INVALID = 0,
    RECOMMENDATION_ERROR = -1
} recommendation_type_t;

/**
 * @brief One LEOP recommendation entry.
 *
 * Stores a timestamp, normalized recommendation value, and decoded
 * recommendation type.
 */
typedef struct {
    char timestamp[20]; /**< Timestamp in "YYYY-MM-DD HH:MM" format. */
    float recommendation;  /**< Normalized value in the range 0 to 1. */
    recommendation_type_t recommendation_type; /**< BUY, HOLD, SELL, INVALID, or ERROR. */
} leop_entry_t;

/**
 * @brief Latest LEOP data snapshot.
 *
 * Contains the cached LEOP entries and the number of valid entries currently
 * stored in the array.
 */
typedef struct {
    int id;
    leop_entry_t entries[128];
    uint32_t entry_count;

} leop_data_t;

/**
 * @brief Latest BME280 sensor readings.
 *
 * Contains the most recent sensor values and the associated validity flags.
 */
typedef struct {
    bool valid;
    uint32_t last_update_seconds;
    time_t last_unix_time;
    bool wall_time_valid;
    double temperature;
    double pressure;
    double humidity;
} sensor_data_t;



/**
 * @brief Runtime application configuration.
 *
 * Stores values that may be updated while the system is running, including
 * fetch timing and sensor update intervals.
 */
typedef struct {
    uint32_t fetch_interval_minutes; /**< Fetch interval in minutes. */
    bool test_mode;
    uint32_t sensor_interval_ms; /**< Sensor interval in milliseconds. */

} config_data_t;

/**
 * @brief System status flags.
 *
 * Tracks basic connectivity and health indicators used by the application.
 */
typedef struct {
    bool wifi_connected;
    bool leop_connected;
    bool sensor_ok; // TODO - Might be uncessecary as the sensor_data_t already tracks if valid or not?
    uint32_t update_counter;
} system_status_t;

/**
 * @brief Task handle information for one application task.
 *
 * Stores the task name, FreeRTOS handle, and configured stack size.
 */
typedef struct {
    const char * name;
    TaskHandle_t handle;
    uint32_t stack_size;
} task_info_t;

/**
 * @brief Collection of application task handles.
 */
typedef struct {
    task_info_t wifi_task;
    task_info_t ui_task;
    task_info_t uart_task;
    task_info_t sensor_task;
    task_info_t leop_task;
} system_task_handlers_t;


/**
 * @brief Shared application state container.
 *
 * Intended to be created by the main application and passed to modules that
 * read or update their portion of the state.
 */
typedef struct {
    LEOPData leop_data;
    sensor_data_t sensor_data;
    config_data_t config_data;
    system_status_t system_status;
    system_task_handlers_t system_task_handlers;
    uint32_t last_recommendation_update_seconds;
} app_state_t;

/** @} */

#endif
