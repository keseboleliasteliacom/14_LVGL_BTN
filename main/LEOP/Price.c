/**
 * @file Price.c
 * @brief Implementation of the price cache and fetch module.
 *
 * @ingroup PRICE
 */

#include "Price.h"
#include "../HTTP.h"
#include "../JSONParser/DataParser.h"
#include "esp_log.h"

static const char *TAG = "Price";

/**
 * @brief Implementation of Price_Initialize.
 *
 * See header for full contract documentation.
 */
int Price_Initialize(PriceList *p_list)
{
    p_list->count = 0;
    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        p_list->price[i] = (Price){0};
    }

    p_list->status.electricity_fetched = false;

    Cache_Initialize(&p_list->cache);

    return 0;
}

/**
 * @brief Fetches price data from a remote URL and updates the cache.
 *
 * Performs HTTP retrieval, writes the raw JSON to the local cache, and parses
 * the response into the provided list.
 */
int Price_Fetch(const char *url, PriceList *p_list)
{
    HTTPResponse http_response = {0};

    HTTPClient_GET(url, &http_response);

    if (http_response.data == NULL)
    {
        ESP_LOGW(TAG, "HTTP response is invalid");
        return 1;
    }

    //Dumps raw response data to SPIFFS for caching
    int res = Cache_WriteFileJSON(&p_list->cache, http_response.data, "Price.json");

    if (res != 0)
    {
        ESP_LOGW(TAG, "Failed to cache data");
    }

    int ret = DataParser_ParsePrice(http_response.data, p_list);

    if (ret != 0)
    {
        return 2;
    }

    /*
    for (int i = 0; i < p_list->count; i++)
    {
        //ESP_LOGI(TAG, "%lf", p_list->price[i].current_prices);
    }*/

    HTTPClient_Dispose(&http_response);

    return 0;
}

/**
 * @brief Loads cached price data from local storage and parses it.
 *
 * Uses the cached JSON file, then clears the cache buffer after parsing.
 */
int Price_FetchCache(PriceList *p_list)
{
    int res = Cache_LoadFileJSON(&p_list->cache, "Price.json");

    if(res != 0)
    {
        ESP_LOGE(TAG, "Failed to load cache");
        return 1;
    }

    res = DataParser_ParsePrice(p_list->cache.data, p_list);

    if (res != 0)
    {
        ESP_LOGE(TAG, "Failed to parse cached data");
        return 2;
    }

    Cache_Dispose(&p_list->cache);

    return 0;
}

/**
 * @brief Clears the price list contents.
 *
 * Resets the entry count and zeroes the stored prices.
 */
void Price_Dispose(PriceList *p_list)
{
    p_list->count = 0;

    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        p_list->price[i] = (Price){0};
    }
}
