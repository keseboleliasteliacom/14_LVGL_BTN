/**
 * @file WiFi.c
 * @brief Implementation of the Wi-Fi module.
 *
 * @ingroup WIFI
 */

#include "WiFi.h"

#include <string.h>

#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "SNTP/time_sync.h"

#include "Config/WifiConfig.h"

#define TAG "WiFi"

#define WIFI_AUTH_MODE WIFI_AUTH_WPA2_PSK

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define WIFI_RETRY_ATTEMPT 100

#define WIFI_RECONNECT_BASE_DELAY_MS 1000
#define WIFI_RECONNECT_MAX_DELAY_MS 30000



static int wifi_retry_count = 0;

static esp_netif_t *netif = NULL;

static esp_event_handler_instance_t ip_event;
static esp_event_handler_instance_t wifi_event;

static EventGroupHandle_t wifi_event_group = NULL;
static bool wifi_reconnect_pending = false;
static TickType_t wifi_reconnect_time = 0;
static volatile bool wifi_started = false; // Track if station has started, volatile rereads value because event callback can change it
static volatile bool connect_on_sta_start = false;

static wifi_state w_state = {0};
static wifi_info active_wifi_info = {0}; // Credentials that is currently applied to wifi driver
static wifi_info pending_wifi_info = {0};
static bool save_credentials_on_connect = false;

QueueHandle_t wifi_cmd_queue = NULL;

QueueHandle_t wifi_result_queue = NULL;

QueueHandle_t event_queue = NULL;

esp_err_t WiFi_Connect(wifi_data *w_info);
esp_err_t WiFi_Dispose(void);
esp_err_t WiFi_Disconnect(void);
// New helper function
static esp_err_t WiFi_ApplyConfig(const wifi_data *w_data);

static bool first_boot = true;




static wifi_connection_cb_t wifi_connection_cb = NULL;
static void *wifi_connection_ctx = NULL;

/**
 * @brief Registers the Wi-Fi connection state callback.
 *
 * Stores the callback and its context for later use by the connection state
 * update path.
 *
 * @param[in] cb Callback invoked when the connection state changes.
 * @param[in] ctx Opaque context passed to the callback.
 */
void WiFi_SetConnectionCallback(wifi_connection_cb_t cb, void *ctx)
{
    wifi_connection_cb = cb;
    wifi_connection_ctx = ctx;
}

/**
 * @brief Updates the cached connection state and notifies the registered callback.
 *
 * @param[in] connected New connection state.
 */
static void WiFi_SetConnectedState(bool connected)
{
    w_state.is_connected = connected;
    if (wifi_connection_cb != NULL)
    {
        wifi_connection_cb(connected, wifi_connection_ctx);
    }
}

/**
 * @brief Handles IP events from the ESP-IDF event loop.
 *
 * Updates Wi-Fi connection state and starts time synchronization after the
 * station obtains an address.
 *
 * @warning Keep callback work short and avoid blocking.
 */
static void ip_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Handling IP event, event code 0x%" PRIx32, event_id);
    switch (event_id)
    {
    case (IP_EVENT_STA_GOT_IP):
    {
        ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));

        WiFi_SetConnectedState(true);
        wifi_retry_count = 0;
        wifi_reconnect_pending = false;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);

        // When we have confirmed status that we are connected and have ip, notify UI via wifi queue
        wifi_status status = WIFI_STATUS_CONNECTED;
        xQueueOverwrite(event_queue, &status);
        

        
        if (TimeSync_IsSynced() == false)
        {
            // TODO: Do not block the IP event callback while waiting up to 10 seconds for SNTP.
            // Move synchronization waiting to the Wi-Fi worker task or use an asynchronous
            // SNTP completion callback so ESP-IDF event-loop processing remains responsive.
            esp_err_t sntp_result = TimeSync_Start();
            if (sntp_result == ESP_OK) {
                ESP_LOGI(TAG, "System time syncronized.");
            }
            else {
                ESP_LOGW(TAG, "System time sync failed.");
            }
        }
        
        break;
    }
    case (IP_EVENT_STA_LOST_IP):
        ESP_LOGI(TAG, "Lost IP");
        WiFi_SetConnectedState(false);
        break;
    case (IP_EVENT_GOT_IP6):
        ip_event_got_ip6_t *event_ip6 = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6: " IPV6STR, IPV62STR(event_ip6->ip6_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        ESP_LOGI(TAG, "IP event not handled");
        break;
    }
}

/**
 * @brief Handles Wi-Fi events from the ESP-IDF event loop.
 *
 * Forwards connection and scan status updates to the internal event queue.
 *
 * @warning Keep callback work short and avoid blocking.
 */
static void wifi_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Handling Wi-Fi event, event code 0x%" PRIx32, event_id);

    wifi_status status;

    switch (event_id)
    {
    case (WIFI_EVENT_WIFI_READY):
        status = WIFI_STATUS_INITIALIZED;
        ESP_LOGI(TAG, "Wi-Fi ready");
        xQueueSend(event_queue, &status, 0);
        break;
    case (WIFI_EVENT_SCAN_DONE):
        ESP_LOGI(TAG, "Wi-Fi scan done");
        break;
    case (WIFI_EVENT_STA_START):
    {
        wifi_started = true;
        ESP_LOGI(TAG, "Wi-Fi station started");

        
        if (connect_on_sta_start)
        {
            connect_on_sta_start = false;
            ESP_LOGI(TAG, "Connecting to configured AP...");

            esp_err_t err = esp_wifi_connect();
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Initial Wi-Fi connection request failed: %s", esp_err_to_name(err));
                wifi_reconnect_pending = true;
                wifi_reconnect_time = xTaskGetTickCount() + pdMS_TO_TICKS(WIFI_RECONNECT_BASE_DELAY_MS);
            }
        }
        break;
    }
    case (WIFI_EVENT_STA_STOP):
        // set to false so our next action will be esp_wifi_start and now esp_wifi_conncet
        wifi_started = false;
        ESP_LOGI(TAG, "Wi-Fi stopped");
        break;
    // When station stop recieveing router beacons. 
    // We are handling the reconnect already, but now we get warning prints instead of harder-to-read errors in console.
    case (WIFI_EVENT_STA_BEACON_TIMEOUT):
        ESP_LOGW(TAG, "Wi-Fi beascon timeout.");
        break;
    case (WIFI_EVENT_STA_CONNECTED):
        ESP_LOGI(TAG, "Wi-Fi AP connected, waiting for IP...");
        break;
    case (WIFI_EVENT_STA_DISCONNECTED):
        wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;

        ESP_LOGW(TAG, "Wifi disocnnected with reason: %d", disc->reason);
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        WiFi_SetConnectedState(false);
        if (wifi_retry_count < WIFI_RETRY_ATTEMPT)
        {
            wifi_retry_count++;
            uint32_t delay_ms = WIFI_RECONNECT_BASE_DELAY_MS * wifi_retry_count;
            if (delay_ms > WIFI_RECONNECT_MAX_DELAY_MS)
            {
                delay_ms = WIFI_RECONNECT_MAX_DELAY_MS;
            }
            ESP_LOGI(TAG, "Scheduling wifi reconnect attempt %d/%d in %"PRIu32" ms", wifi_retry_count, WIFI_RETRY_ATTEMPT, delay_ms);
            wifi_reconnect_pending = true;
            wifi_reconnect_time = xTaskGetTickCount() + pdMS_TO_TICKS(delay_ms);

            // Publish status to UI
            status = WIFI_STATUS_RECONNECTING;
            xQueueOverwrite(event_queue, &status);
        }
        else {
            status = WIFI_STATUS_DISCONNECTED;
            xQueueOverwrite(event_queue, &status);
        }
        break;
    case (WIFI_EVENT_STA_AUTHMODE_CHANGE):
        ESP_LOGI(TAG, "Wi-Fi authmode changed");
        break;
    case (WIFI_EVENT_HOME_CHANNEL_CHANGE):
        ESP_LOGD(TAG, "Wi-Fi home channel changed");
        break;
    default:
        ESP_LOGI(TAG, "Wi-Fi event not handled");
        break;
    }
}

/**
 * @brief Creates the internal Wi-Fi queues.
 *
 * The queues are used for command dispatch, result delivery, and event
 * handoff between callbacks and the worker task.
 */
void WiFi_CreateQueues()
{
    event_queue = xQueueCreate(1, sizeof(wifi_status));

    if (event_queue == NULL)
    {
        ESP_LOGI(TAG, "Failed to create event queue!");
    }

    wifi_cmd_queue = xQueueCreate(1, sizeof(wifi_data));

    if (wifi_cmd_queue == NULL)
    {
        ESP_LOGI(TAG, "Failed to create wifi cmd queue!");
    }

    wifi_result_queue = xQueueCreate(1, sizeof(wifi_data));

    if (wifi_result_queue == NULL)
    {
        ESP_LOGI(TAG, "Failed to create wifi cmd queue!");
    }
}

/**
 * @brief Implementation of WiFi_Initialize.
 *
 * See header for full contract documentation.
 */
esp_err_t WiFi_Initialize()
{
    esp_err_t ret_value = nvs_flash_init();

    if (ret_value == ESP_ERR_NVS_NO_FREE_PAGES || ret_value == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret_value = nvs_flash_init();
    }

    wifi_event_group = xEventGroupCreate();

    ret_value = esp_netif_init();
    if (ret_value != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize TCP/IP network stack");
        return ret_value;
    }

    ret_value = esp_event_loop_create_default();
    if (ret_value != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create default event loop");
        return ret_value;
    }

    netif = esp_netif_create_default_wifi_sta();
    if (netif == NULL)
    {
        ESP_LOGE(TAG, "Failed to create default WiFi STA interface");
        return ESP_FAIL;
    }

    WiFi_CreateQueues();

    // Wi-Fi stack configuration parameters
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
     esp_wifi_set_ps(WIFI_PS_NONE); // Disable powersaving mode as it can cause problems with some routerns/APs under reconnect

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_cb, NULL, &wifi_event));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_cb, NULL, &ip_event));
    return ESP_OK;
}

/**
 * @brief Scans for nearby Wi-Fi access points.
 *
 * Populates the provided result buffer with up to 10 scan records.
 */
esp_err_t WiFi_Scan(wifi_data *w_data)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_start());

    vTaskDelay(100);

    wifi_scan_config_t scan_config = {0};

    uint16_t max_number = 10;
    uint16_t number = 0;

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, 1));

    vTaskDelay(pdMS_TO_TICKS(100));

    esp_wifi_scan_get_ap_num(&number);
    if (number > max_number)
        number = max_number;

    esp_err_t res = esp_wifi_scan_get_ap_records(&number, w_data->ap_info);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to scan wifi");
        return res;
    }

    w_data->number = number;

    for (int i = 0; i < w_data->number; i++)
    {
        ESP_LOGI(TAG, "Hittade: %s (RSSI: %d)", w_data->ap_info[i].ssid, w_data->ap_info[i].rssi);
    }

    return ESP_OK;
}

/**
 * @brief Wi-Fi worker task.
 *
 * Waits for commands on the Wi-Fi command queue and forwards results after
 * scan, connect, or disconnect operations complete.
 *
 * @param[in] arg Task context pointer supplied by the creator.
 *
 * @note Runs in task context and blocks on queues and delays.
 */
void WiFi_Work(void *arg)
{
    //app_state_t* app = (app_state_t*)arg;

    wifi_status status = {0};
    wifi_data w_data = {0};

    // If we we just botted and started the work function, attempt to try to load wifi details from NVS and try connect.

    if (first_boot == true) {
        int read_ssid_result = Config_LoadFromNVS_WifiSSID(w_data.wifi_info.ssid);
        int read_pw_result = Config_LoadFromNVS_WifiPassword(w_data.wifi_info.password);
        if (read_ssid_result != 0 || read_pw_result != 0) {
            ESP_LOGW(TAG, "Could not load wifi details from NVS.");
        }
        // Attempt to read form NVS with proper error handling if empty or error
        else {
            ESP_LOGI(TAG, "Loaded Wi-Fi details from NVS.");

            if (w_data.wifi_info.ssid[0] == '\0')
            {
                ESP_LOGW(TAG, "Stored Wi-Fi SSID is empty.");
            }
            else
            {
                esp_err_t err = esp_wifi_set_mode(WIFI_MODE_STA);
                if (err != ESP_OK)
                {
                    ESP_LOGE(TAG, "Failed to set station mode: %s", esp_err_to_name(err));
                }
                else if ((err = WiFi_ApplyConfig(&w_data)) != ESP_OK)
                {
                    ESP_LOGE(TAG, "Stored Wi-Fi configuration could not be applied");
                }
                else
                {
                    // trigger new connection and attempt to start wifi
                    connect_on_sta_start = true;
                    err = esp_wifi_start();
                    if (err != ESP_OK)
                    {
                        connect_on_sta_start = false;
                        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
                    }
                }
            }
        }
        first_boot = false;
    }

    while (1)
    {
        // Check is there are any current events without blocking
        if (xQueueReceive(event_queue, &status, 0) == pdPASS)
        {
            wifi_data result = {0};
            result.status = status;
            result.wifi_info = active_wifi_info;

            // switch requires handling all cases, but currently we only want to have specially handling connecting if we need to save credentials when connecting as a one time thing.
            switch (status)
            {
                case WIFI_STATUS_INITIALIZED:
                    break;
                case WIFI_STATUS_SCAN_DONE:
                    break;
                case WIFI_STATUS_CONNECTING:
                    break;
                case WIFI_STATUS_CONNECTED:
                    if (save_credentials_on_connect)
                    {
                        int ssid_result = Config_WriteToNVS_WifiSSID(pending_wifi_info.ssid);
                        int password_result = Config_WriteToNVS_WifiPassword(pending_wifi_info.password);

                        if (ssid_result != 0 || password_result != 0)
                        {
                            ESP_LOGW(TAG, "Connected, but failed to save Wi-Fi credentials");
                        }
                        else
                        {
                            ESP_LOGI(TAG, "Wi-Fi credentials saved after successful connection");
                        }

                        save_credentials_on_connect = false;
                    }
                    xQueueOverwrite(wifi_result_queue, &result);
                    break;
                case WIFI_STATUS_RECONNECTING:
                    xQueueOverwrite(wifi_result_queue, &result);
                    break;
                case WIFI_STATUS_DISCONNECTED:
                    xQueueOverwrite(wifi_result_queue, &result);
                    break;
            }
        }

        if (wifi_reconnect_pending && xTaskGetTickCount() >= wifi_reconnect_time)
        {
            wifi_reconnect_pending = false;
            ESP_LOGI(TAG, "Retrying wifi-connection...");
            esp_err_t err = esp_wifi_connect();

            if (err != ESP_OK)
            {
                ESP_LOGW(TAG, "Wi-Fi reconnect request failed: %s", esp_err_to_name(err));
            }
        }

        if (xQueueReceive(wifi_cmd_queue, &w_data, pdMS_TO_TICKS(100)) == pdPASS)
        {
            switch (w_data.cmd)
            {
            case WIFI_CMD_SCAN:
                // Since esp_wifi_scan_start(&scan_config, 1) is blocking(1 is the blocking flag/value),
                // so we dont need to wait for the WIFI_STATUS_SCAN_DONE from event queue.
                if (WiFi_Scan(&w_data) == ESP_OK)                
                    {
                        w_data.status = WIFI_STATUS_SCAN_DONE;
                        xQueueSend(wifi_result_queue, &w_data, 0);
                        ESP_LOGI(TAG, "WiFi_Work got status after scan: %d", status);
                    }
                    break;
            case WIFI_CMD_CONNECT:
                pending_wifi_info = w_data.wifi_info;
                save_credentials_on_connect = true;
                if (WiFi_Connect(&w_data) != ESP_OK)
                {
                    save_credentials_on_connect = false;
                }
                break;
            case WIFI_CMD_DISCONNECT:
                if (xQueueReceive(event_queue, &status, pdMS_TO_TICKS(5000)))
                {
                    if (status == WIFI_STATUS_DISCONNECTED)
                    {
                        w_data.status = status;
                        xQueueSend(wifi_result_queue, &w_data, 0);
                    }
                }
                break;

            default:
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief Connects to a Wi-Fi access point.
 *
 * Copies the SSID and password into the station configuration and starts the
 * ESP-IDF connection attempt.
 *
 * @param[in] w_data Connection data containing the credentials.
 *
 * @return
 * - `ESP_OK` on success
 * - `ESP_FAIL` if the connection request cannot be started
 */
static esp_err_t WiFi_ApplyConfig(const wifi_data *w_data)
{
    if (w_data == NULL || w_data->wifi_info.ssid[0] == '\0')
    {
        ESP_LOGE(TAG, "Cannot configure Wi-Fi: SSID is empty");
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t wifi_config = {0};

    strlcpy((char *)wifi_config.sta.ssid, w_data->wifi_info.ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password, w_data->wifi_info.password, sizeof(wifi_config.sta.password));

    esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to apply Wi-Fi configuration: %s", esp_err_to_name(err));
        return err;
    }

    active_wifi_info = w_data->wifi_info;
    return ESP_OK;
}

esp_err_t WiFi_Connect(wifi_data *w_data)
{
    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set station mode: %s", esp_err_to_name(err));
        return err;
    }

    err = WiFi_ApplyConfig(w_data);
    if (err != ESP_OK)
    {
        return err;
    }

    if (!wifi_started)
    {
        connect_on_sta_start = true;
        err = esp_wifi_start();
        if (err != ESP_OK)
        {
            connect_on_sta_start = false;
            ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
            return err;
        }

        return ESP_OK;
    }

    err = esp_wifi_connect();

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Wi-Fi connection request failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

/**
 * @brief Returns the current Wi-Fi connection state.
 *
 * @return `true` when connected, otherwise `false`.
 */
bool WiFi_IsConnected()
{
    return w_state.is_connected;
}

/**
 * @brief Disconnects the station from the access point.
 *
 * Deletes the Wi-Fi event group before requesting disconnect from ESP-IDF.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 */
esp_err_t WiFi_Disconnect(void)
{
    /*
    if (wifi_event_group)
    {
        vEventGroupDelete(wifi_event_group);
    }

    */
    return esp_wifi_disconnect();
}

/**
 * @brief Releases Wi-Fi resources.
 *
 * Stops the Wi-Fi stack, unregisters event handlers, clears the default
 * driver, and destroys the network interface.
 *
 * @return
 * - `ESP_OK` on success
 * - an ESP-IDF error code on failure
 */
esp_err_t WiFi_Dispose(void)
{
    esp_err_t ret_value = esp_wifi_stop();
    if (ret_value == ESP_ERR_WIFI_NOT_INIT)
    {
        ESP_LOGE(TAG, "WiFi not initialize!");
        return ret_value;
    }

    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(netif));
    esp_netif_destroy(netif);

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, ip_event));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event));

    return ESP_OK;
}
