/**
 * @file Cache.c
 * @brief Implementation of the CACHE module.
 *
 * @ingroup CACHE
 */

#include "Cache.h"
#include <string.h>
#include "esp_log.h"

/**
 * @brief Cache module log tag.
 */
static const char *TAG = "Cache";

/**
 * @brief Implementation of Cache_Initialize.
 *
 * See header for full contract documentation.
 */
int Cache_Initialize(Cache_t *cache)
{
    if (cache == NULL)
    {
        ESP_LOGW(TAG, "Failed to initialize cache");
        return 1;
    }

    cache->data = NULL;

    return 0;
}

/**
 * @brief Implementation of Cache_WriteFileJSON.
 *
 * Writes the provided JSON text through the SPIFFS layer.
 *
 * See header for full contract documentation.
 */
int Cache_WriteFileJSON(Cache_t *cache, const char *data, const char *file_name)
{
    int result = Spiffs_WriteToFileJSON(file_name, data);

    if (result != 0)
    {
        ESP_LOGW(TAG, "Failed to write json file");
        return 2;
    }

    return 0;
}

/**
 * @brief Implementation of Cache_LoadFileJSON.
 *
 * Loads JSON from SPIFFS and stores a heap copy in the cache object.
 *
 * See header for full contract documentation.
 */
int Cache_LoadFileJSON(Cache_t *cache, const char *file_name)
{
    char *json = Spiffs_ReadFromFileJSON(file_name);

    if (json == NULL)
    {
        return 2;
    }

    cache->data = strdup(json);

    free(json);

    if (cache->data == NULL)
    {
        return 3;
    }

    return 0;
}

/**
 * @brief Implementation of Cache_Dispose.
 *
 * Releases the cached heap buffer when present.
 *
 * See header for full contract documentation.
 */
void Cache_Dispose(Cache_t *cache)
{
    if (cache != NULL && cache->data != NULL)
    {
        free(cache->data);
        cache = NULL;
    }
}
