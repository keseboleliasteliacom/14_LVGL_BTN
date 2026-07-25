/**
 * @file time_sync.c
 * @brief Implementation of the SNTP time synchronization module.
 *
 * @ingroup SNTP
 */

#include "time_sync.h"
#include <stdatomic.h>

const static char* TAG = "SNTP";

// Todo - Check out if other areas that are currently using queues could use the middlegorund, atomic bool with store and load, if worth
static atomic_bool s_time_synced = ATOMIC_VAR_INIT(false);
static bool s_sntp_initialized = false;

/**
 * @brief Implementation of TimeSync_Start.
 *
 * See header for full contract documentation.
 */
esp_err_t TimeSync_Start() {
    if (s_sntp_initialized == false)
    {
        // Init the default sntp config
        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
        // init and star tthe sntp service
        esp_err_t init_result = esp_netif_sntp_init(&config);

        if (init_result != ESP_OK) {
            ESP_LOGE(TAG, "Could not init SNTP: %s", esp_err_to_name(init_result));
            return init_result;
        }

        s_sntp_initialized = true;
    }


    esp_err_t sync_result = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000));
    if (sync_result == ESP_OK) {
        atomic_store(&s_time_synced, true);
        ESP_LOGI(TAG, "Time successfully syncronised.");
    }
    else {
        ESP_LOGW(TAG, "Could not successfully sync time.");
    }

    return sync_result;
}

/**
 * @brief Implementation of TimeSync_IsSynced.
 *
 * See header for full contract documentation.
 */
bool TimeSync_IsSynced() {
    return atomic_load(&s_time_synced);
}

// /**
//  * @brief Implementation of TimeSync_Start.
//  *
//  * See header for full contract documentation.
//  */
// esp_err_t TimeSync_Start() {
//     // Init the default SNTP config 
//     esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

//     // Init and start the SNTP service
//     esp_err_t sntp_result = esp_netif_sntp_init(&config);

//     if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) == ESP_OK) {
//         ESP_LOGI("SNTP", "Time successfully syncronised.");
//     }
//     else {
//         ESP_LOGI("SNTP", "Could not successfully sync time.");
//     }

//     return sntp_result;
// }
