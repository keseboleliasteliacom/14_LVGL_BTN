#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Price.h"
#include "Recommendation.h"
#include "LEOP_Fetcher.h"
#include "Weather.h" 

/**
 * @file app_types.h
 * @brief Shared application data types for state, configuration, and task handles.
 *
 * Defines the common data structures used to exchange application state
 * between modules.
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
 * @brief Latest BME280 sensor readings and validity state.
 *
 * Contains the most recent sensor values together with timestamps and
 * synchronization flags used by the application.
 */
typedef struct {
    bool valid; /**< True when the sensor reading is valid. */
    uint32_t last_update_seconds; /**< monotonic seconds since boot at the last successfull sensor udpate */
    time_t last_unix_time; /**< Last update time in Unix seconds. */
    bool wall_time_valid; /**< True when wall time has been synchronized. */
    double temperature; /**< Temperature in project-defined units. */
    double pressure; /**< Pressure in project-defined units. */
    double humidity; /**< Relative humidity in project-defined units. */
} sensor_data_t;



/**
 * @brief Runtime application configuration.
 *
 * Stores values that may be updated while the system is running, including
 * fetch timing and sensor update intervals.
 */
typedef struct {
    uint32_t fetch_interval_minutes; /**< Fetch interval in minutes. */
    bool test_mode; /**< True when running in test mode. */
    uint32_t sensor_interval_ms; /**< Sensor interval in milliseconds. */

} config_data_t;

/**
 * @brief System status flags.
 *
 * Tracks connectivity and health indicators used by the application.
 */
typedef struct {
    bool wifi_connected; /**< True when station has usable IPv4 connection */
    bool leop_connected; /**< True when LEOP data is available(CONNECTED or DEGRADED). */
    bool sensor_ok; /**< True when the sensor path is considered healthy. */
    uint32_t update_counter; /**< Number of completed application updates. */
} system_status_t;

/**
 * @brief Task handle information for one application task.
 *
 * Stores the task name, FreeRTOS handle, and configured stack size.
 */
typedef struct {
    const char * name; /**< Task name. */
    TaskHandle_t handle; /**< FreeRTOS task handle. */
    uint32_t stack_size; /**< Configured task stack size. */
} task_info_t;

/**
 * @brief Collection of application task handles.
 *
 * Groups the task records used by the application to track its worker tasks.
 */
typedef struct {
    task_info_t wifi_task; /**< Wi-Fi task information. */
    task_info_t ui_task; /**< UI task information. */
    task_info_t uart_task; /**< UART task information. */
    task_info_t sensor_task; /**< Sensor task information. */
    task_info_t leop_task; /**< LEOP task information. */
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
