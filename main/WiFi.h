#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include "esp_log.h"
#include <inttypes.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdbool.h>


#define WIFI_SSID_MAX_LEN 33
#define WIFI_PASSWORD_MAX_LEN 65

/**
 * @file WiFi.h
 * @brief Public API for the Wi-Fi module.
 *
 * Provides the public types and functions for configuring Wi-Fi station
 * connectivity, managing worker-task commands, and reporting connection status.
 *
 * @defgroup WIFI WiFi
 * @brief Wi-Fi control and worker task interface.
 *
 * The module initializes the ESP-IDF Wi-Fi stack, owns the worker task command
 * and result queues, and coordinates scan, connect, and disconnect operations.
 *
 * @note Functions in this module are intended for task context. The worker task
 * blocks on queues, and Wi-Fi operations may perform network I/O.
 * @{
 */

extern QueueHandle_t wifi_cmd_queue; /**< Command queue consumed by WiFi_Work. */

extern QueueHandle_t wifi_result_queue; /**< Result queue published by WiFi_Work. */

typedef enum
{
    WIFI_CMD_START,
    WIFI_CMD_SCAN,
    WIFI_CMD_CONNECT,
    WIFI_CMD_DISCONNECT,
    WIFI_CMD_STOP,
} wifi_cmd_t;

typedef enum
{
    WIFI_STATUS_INITIALIZED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_RECONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_SCAN_DONE,
    WIFI_STATUS_DISCONNECTED,
} wifi_status;

/**
 * @brief Shared Wi-Fi connectivity state.
 *
 * Exposes whether the station currently has usable IP connectivity.
 */
typedef struct 
{
    bool is_connected; /**< `true` after the station has obtained an IP address. */
}wifi_state;



/**
 * @brief Station credentials used by the Wi-Fi module.
 */
typedef struct
{
    char ssid[WIFI_SSID_MAX_LEN]; /**< SSID string. */
    char password[WIFI_PASSWORD_MAX_LEN]; /**< Password string. */
} wifi_info;

/**
 * @brief Command and result payload used by the Wi-Fi worker task.
 *
 * Contains the requested command, the resulting status, scan output storage,
 * and the station credentials used for connection.
 *
 * @note The scan result buffer stores up to 10 access point records.
 */
typedef struct
{
    wifi_cmd_t cmd; /**< Requested command. */
    wifi_status status; /**< Resulting status. */
    uint16_t number; /**< Number of valid scan results. */
    wifi_ap_record_t ap_info[10]; /**< Scan result buffer. */
    wifi_info wifi_info; /**< Station credentials. */
} wifi_data;

typedef void (*wifi_connection_cb_t)(bool connected, void *ctx);

/**
 * @brief Registers a callback for Wi-Fi connection state changes.
 *
 * @param[in] cb Callback invoked when the connection state changes.
 * @param[in] ctx Opaque context passed to the callback.
 */
void WiFi_SetConnectionCallback(wifi_connection_cb_t cb, void *ctx);

/**
 * @brief Initializes the Wi-Fi module.
 *
 * Sets up NVS, the ESP-IDF network stack, the default event loop, the Wi-Fi
 * station interface, queues, and event handlers.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 *
 * @note Call from task context during system initialization.
 * @warning Initialization order matters because the module depends on NVS,
 * network stack, and event loop setup.
 */
esp_err_t WiFi_Initialize();

/**
 * @brief Wi-Fi worker task.
 *
 * Waits for commands on the Wi-Fi command queue and performs scan, connect,
 * or disconnect operations before forwarding results.
 *
 * @param[in] arg Task context pointer supplied by the creator.
 *
 * @pre WiFi_Initialize() must have created the queues and registered the
 * event handlers before this task starts.
 * @note Runs in task context and blocks on queues and delays.
 */
void WiFi_Work(void *arg);

/**
 * @brief Connects the station to an access point.
 *
 * Applies the supplied credentials and requests station start or connect.
 *
 * @param[in] w_data Connection data containing the SSID and password.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 *
 * @note Call from task context.
 * @note If the station is not started yet, the request is deferred until the
 * station start event handler runs.
 */
esp_err_t WiFi_Connect(wifi_data *w_data);

/**
 * @brief Returns whether the station currently has usable IP connectivity.
 *
 * @return `true` when connected, otherwise `false`.
 */
bool WiFi_IsConnected();

/**
 * @brief Disconnects the station from the access point.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 */
esp_err_t WiFi_Disconnect(void);

/**
 * @brief Releases Wi-Fi resources.
 *
 * Stops and deinitializes the Wi-Fi stack, removes handlers, and destroys the
 * network interface.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 *
 * @note Call after Wi-Fi activity has stopped.
 */
esp_err_t WiFi_Dispose(void);

/** @} */

#endif
