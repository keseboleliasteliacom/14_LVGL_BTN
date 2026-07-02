#include "NVS.h"

static esp_err_t err;
nvs_handle_t my_handle;

const static char* TAG = "NVS";
// Todo - proper error code info
int NVS_Init() {
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        return -1;
    }
    ESP_ERROR_CHECK(err);

    return 0;
}

int NVS_WriteToFile(const char* key, const char* value)
{
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS handle.", esp_err_to_name(err));
        return -1;
    }
    err = nvs_set_str(my_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when writing to nvs: ", esp_err_to_name(err));
        return -2;
    }
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when commiting to NVS: ", esp_err_to_name(err));
        return -3;
    }
    nvs_close(my_handle);
    
    return 0;
}

int NVS_LoadFromFile(const char* key, char* value, size_t length) {
    //char value[20];

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle.", esp_err_to_name(err));
        return -1;
    }

    err = nvs_get_str(my_handle, key, value, &length);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error: %s, when loading string from NVS.", esp_err_to_name(err));
        nvs_close(my_handle);
        return -2;
    }
    nvs_close(my_handle);
    return 0;
}


void FullNVS() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Then retry nvm flash init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);

    // Open
    ESP_LOGI(TAG, "\n");
    ESP_LOGI(TAG, "Opening NVS handle...");
    nvs_handle_t my_handle;
    //std::unique_ptr<nvs::NVSHandle> handle nvs::open_nvs_handle("storage", NVS_READWRITE, &err);
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle.", esp_err_to_name(err));
    }
    else {
        ESP_LOGI(TAG, "Done.");

        ESP_LOGI(TAG, "Reading restart counter from NVS...");
        int32_t restart_counter = 0;
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        //err = handle->get_item("restart_counter", restart_counter);
        switch (err) {
            case ESP_OK:
                ESP_LOGI(TAG, "Done.");
                ESP_LOGI(TAG, "Restart counter = %"PRIu32"", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGW(TAG, "This value has not been initalized yet.");
                break;
            default:
                ESP_LOGE(TAG, "Error (%s) reading.", esp_err_to_name(err));
        }

        ESP_LOGI(TAG, "Updating restart counter in NVS...");
        restart_counter++;
        //err = handle->set_item("restart_counter", restart_counter);
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Done.");
        }
        else {
            ESP_LOGI(TAG, "Failed.");
        }

        //Commit the newly written values.
        // just like git, we must commit any changes using nvs_commit.
        // Implementations may write to storage at other times, but this is not guaranteed.
        ESP_LOGI(TAG, "Commiting updates in NVS...");
        //err = handle->commit();
        err = nvs_commit(my_handle);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Done.");
        }
        else {
            ESP_LOGI(TAG, "Failed.");
        }

        nvs_close(my_handle);

    }

    for (int i = 15; i >= 0; i--) {
        printf("Restarting in %d seconds...", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    esp_restart();
}