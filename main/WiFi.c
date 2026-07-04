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



// static const int WIFI_RETRY_ATTEMPT = 3;

static int wifi_retry_count = 0;

static esp_netif_t *netif = NULL;

static esp_event_handler_instance_t ip_event;
static esp_event_handler_instance_t wifi_event;

static EventGroupHandle_t wifi_event_group = NULL;

static wifi_state w_state = {0};

QueueHandle_t wifi_cmd_queue = NULL;

QueueHandle_t wifi_result_queue = NULL;

QueueHandle_t event_queue = NULL;

esp_err_t WiFi_Connect(wifi_data *w_info);
esp_err_t WiFi_Dispose(void);
esp_err_t WiFi_Disconnect(void);

static bool first_boot = true;


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
        ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));

        w_state.is_connected = true;
        wifi_retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);

        esp_err_t time_result = TimeSync_Start();
        break;
        // esp_err_t time_result = TimeSync_Start();
        // ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
        // ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));
        // wifi_retry_count = 0;
        // xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        // break;
    case (IP_EVENT_STA_LOST_IP):
        ESP_LOGI(TAG, "Lost IP");
        w_state.is_connected = false;
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
        ESP_LOGI(TAG, "Wi-Fi ready");
        xQueueSend(event_queue, &status, portMAX_DELAY);
        break;
    case (WIFI_EVENT_SCAN_DONE):
        ESP_LOGI(TAG, "Wi-Fi scan done");
        status = WIFI_STATUS_SCAN_DONE;
        xQueueSend(event_queue, &status, portMAX_DELAY);
        break;
    case (WIFI_EVENT_STA_START):
        ESP_LOGI(TAG, "Wi-Fi started, connecting to AP...");
        // esp_wifi_connect();
        break;
    case (WIFI_EVENT_STA_STOP):
        ESP_LOGI(TAG, "Wi-Fi stopped");
        break;
    case (WIFI_EVENT_STA_CONNECTED):
        ESP_LOGI(TAG, "Wi-Fi connected");
        status = WIFI_STATUS_CONNECTED;
        xQueueSend(event_queue, &status, portMAX_DELAY);
        break;
    case (WIFI_EVENT_STA_DISCONNECTED):
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        w_state.is_connected = false;
        status = WIFI_STATUS_DISCONNECTED;
        xQueueSend(event_queue, &status, portMAX_DELAY);
        break;
    case (WIFI_EVENT_STA_AUTHMODE_CHANGE):
        ESP_LOGI(TAG, "Wi-Fi authmode changed");
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
    // ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // ESP_LOGI(TAG, "Connection to WiFi network [%s]...", wifi_config.sta.ssid);
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
    wifi_status status = {0};
    wifi_data w_data = {0};

    // If we we just botted and started the work function, attempt to try to load wifi details from NVS and try connect.

    if (first_boot == true) {
        int read_ssid_result = Config_LoadFromNVS_WifiSSID(w_data.wifi_info.ssid);
        int read_pw_result = Config_LoadFromNVS_WifiPassword(w_data.wifi_info.password);
        if (read_ssid_result != 0 || read_pw_result != 0) {
            ESP_LOGW(TAG, "Could not load wifi details from NVS.");
        }
        else {
            ESP_LOGI(TAG, "Loaded wifi details form nvs.");
            // Sending to its own queue once feels bad?
            // status = WIFI_CMD_CONNECT;
            // xQueueSend(event_queue, &status, portMAX_DELAY);
            // But calling function directly also seems bad
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            WiFi_Connect(&w_data);
        }
        first_boot = false;

    }

    while (1)
    {

        if (xQueueReceive(wifi_cmd_queue, &w_data, pdMS_TO_TICKS(5000)) == pdPASS)
        {
            switch (w_data.cmd)
            {
            case WIFI_CMD_SCAN:
                //if (xQueueReceive(event_queue, &status, pdMS_TO_TICKS(5000)))
                // Since esp_wifi_scan_start(&scan_config, 1) is blocking(1 is the blocking flag/value),
                // so we dont need to wait for the WIFI_STATUS_SCAN_DONE from event queue.

                if (WiFi_Scan(&w_data) == ESP_OK)                
                    {
                        w_data.status = WIFI_STATUS_SCAN_DONE;
                        xQueueSend(wifi_result_queue, &w_data, 0);
                        ESP_LOGI(TAG, "WiFi_Work got status after scan: %d", status);
                        // if (status == WIFI_STATUS_SCAN_DONE)
                        // {
                        //     w_data.status = status;
                        //     xQueueSend(wifi_result_queue, &w_data, 0);
                        // }
                    }
                    break;
            case WIFI_CMD_CONNECT:
                WiFi_Connect(&w_data);
                if (xQueueReceive(event_queue, &status, pdMS_TO_TICKS(5000)))
                {
                    if (status == WIFI_STATUS_CONNECTED)
                    {
                        //w_state.is_connected = true;
                        w_data.status = status;
                        //esp_err_t time_sync = TimeSync_Start();
                        xQueueSend(wifi_result_queue, &w_data, 0);
                    }
                }
                break;
            case WIFI_CMD_DISCONNECT:
                WiFi_Disconnect();
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
                // xQueueSend(wifi_queue, &w_data, portMAX_DELAY);
                break;
            }
            // ESP_LOGI(TAG, "WiFi Work: %d", wifi->number);
            // xQueueSend(wifi_queue, wifi, portMAX_DELAY);
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
esp_err_t WiFi_Connect(wifi_data *w_data)
{
    wifi_config_t wifi_config = {0};

    strlcpy((char *)wifi_config.sta.ssid, w_data->wifi_info.ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password, w_data->wifi_info.password, sizeof(wifi_config.sta.password));
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_err_t err = esp_wifi_connect();

    if (err != ESP_OK)
    {
        return ESP_FAIL;
    }
    if (first_boot == false)
    {
        
    }
    // If we succesfully connected, we save the details so we automatically reconnect upon next boot/reboot
    //int write_ssid_result = Config_WriteToNVS_WifiSSID((char*)wifi_config.sta.ssid);
    //int write_pw_result = Config_WriteToNVS_WifiPassword((char*)wifi_config.sta.password);
    // ESP_LOGI(TAG, "ssid_result %d", write_ssid_result);
    // ESP_LOGI(TAG, "pw_result %d", write_pw_result);
    
    // // vTaskDelay(pdMS_TO_TICKS(200));
    // char ssid_test[WIFI_SSID_MAX_LEN] = {0};
    // int ssid_test_result = Config_LoadFromNVS_WifiSSID(ssid_test);
    
    // char pw_test[WIFI_PASSWORD_MAX_LEN] = {0};
    // int pw_test_result = Config_LoadFromNVS_WifiPassword(pw_test);
    // if (pw_test_result != 0 || ssid_test_result != 0) {
    //     ESP_LOGW(TAG, "WARNINGSSS");
    // }
    // else {
    //     ESP_LOGI(TAG, "ssid: %s,  password: %s", ssid_test, pw_test);
    // }
    ESP_LOGI(TAG, "w_data ssid: %s,  w_data pw: %s", w_data->wifi_info.ssid, w_data->wifi_info.password);


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
    if (wifi_event_group)
    {
        vEventGroupDelete(wifi_event_group);
    }

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
