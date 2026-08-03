/**
 * @file NVS.c
 * @brief Implementation of the NVS helper module.
 *
 * @ingroup NVS
 *
 * Provides simple wrappers around ESP-IDF NVS operations for the default
 * storage namespace and named namespaces.
 */

#include "NVS.h"

static esp_err_t err;
nvs_handle_t my_handle;

const static char* TAG = "NVS";

/**
 * @brief Initializes the default NVS flash partition.
 *
 * Returns `0` on success. If the NVS partition is full or incompatible, the
 * partition is erased and reinitialized before returning `-1`.
 */
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

/**
 * @brief Implementation of NVS_WriteToFile.
 *
 * See header for full contract documentation.
 */
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

/**
 * @brief Implementation of NVS_LoadFromFile.
 *
 * See header for full contract documentation.
 */
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

/**
 * @brief Implementation of NVS_ReadString.
 *
 * See header for full contract documentation.
 */
int NVS_ReadString(const char* nvs_namespace, const char* key, char* buffer, size_t size)
{
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READONLY, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    size_t required_size = size;
    result = nvs_get_str(handle, key, buffer, &required_size);
    nvs_close(handle);

    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when reading string key \"%s\".", esp_err_to_name(result), key);
        return -2;
    }

    return 0;
}

/**
 * @brief Implementation of NVS_WriteString.
 *
 * See header for full contract documentation.
 */
int NVS_WriteString(const char* nvs_namespace, const char* key, const char* value)
{
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    result = nvs_set_str(handle, key, value);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when writing string key \"%s\".", esp_err_to_name(result), key);
        nvs_close(handle);
        return -2;
    }

    result = nvs_commit(handle);
    nvs_close(handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when committing string key \"%s\".", esp_err_to_name(result), key);
        return -3;
    }

    return 0;
}

/**
 * @brief Implementation of NVS_ReadU32.
 *
 * See header for full contract documentation.
 */
int NVS_ReadU32(const char* nvs_namespace, const char* key, uint32_t* out_value)
{
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READONLY, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    result = nvs_get_u32(handle, key, out_value);
    nvs_close(handle);

    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when reading u32 key \"%s\".", esp_err_to_name(result), key);
        return -2;
    }

    return 0;
}

/**
 * @brief Implementation of NVS_WriteU32.
 *
 * See header for full contract documentation.
 */
int NVS_WriteU32(const char* nvs_namespace, const char* key, uint32_t value)
{
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    result = nvs_set_u32(handle, key, value);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when writing u32 key \"%s\".", esp_err_to_name(result), key);
        nvs_close(handle);
        return -2;
    }

    result = nvs_commit(handle);
    nvs_close(handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when committing u32 key \"%s\".", esp_err_to_name(result), key);
        return -3;
    }

    return 0;
}

/**
 * @brief Implementation of NVS_ReadBool.
 *
 * See header for full contract documentation.
 */
int NVS_ReadBool(const char* nvs_namespace, const char* key, bool* out_value)
{
    uint8_t stored_value = 0;
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READONLY, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    result = nvs_get_u8(handle, key, &stored_value);
    nvs_close(handle);

    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when reading bool key \"%s\".", esp_err_to_name(result), key);
        return -2;
    }

    *out_value = stored_value != 0;
    return 0;
}

/**
 * @brief Implementation of NVS_WriteBool.
 *
 * See header for full contract documentation.
 */
int NVS_WriteBool(const char* nvs_namespace, const char* key, bool value)
{
    nvs_handle_t handle;
    esp_err_t result = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when opening NVS namespace \"%s\".", esp_err_to_name(result), nvs_namespace);
        return -1;
    }

    result = nvs_set_u8(handle, key, value ? 1 : 0);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when writing bool key \"%s\".", esp_err_to_name(result), key);
        nvs_close(handle);
        return -2;
    }

    result = nvs_commit(handle);
    nvs_close(handle);
    if (result != ESP_OK) {
        ESP_LOGW(TAG, "Error %s when committing bool key \"%s\".", esp_err_to_name(result), key);
        return -3;
    }

    return 0;
}
