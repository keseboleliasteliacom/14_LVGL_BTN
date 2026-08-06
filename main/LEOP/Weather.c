/**
 * @file Weather.c
 * @brief Implementation of the WEATHER module.
 *
 * @ingroup WEATHER
 */

#include "Weather.h"
#include "../HTTP.h"
#include "../JSONParser/DataParser.h"
#include "esp_log.h"

static const char *TAG = "Weather";

/**
 * @brief Implementation of Weather_Initialize.
 *
 * See header for full contract documentation.
 */
int Weather_Initialize(WeatherList *w_list)
{
    w_list->count = 0;
    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        w_list->weather[i] = (Weather){0};
    }

    w_list->status.weather_fetched = false;

    Cache_Initialize(&w_list->cache);

    return 0;
}

/**
 * @brief Fetches weather data from the network and updates the list.
 *
 * Performs an HTTP GET, attempts to cache the payload, and parses the
 * response into the provided weather list.
 */
int Weather_Fetch(const char *url, WeatherList *w_list)
{
    HTTPResponse http_response = {0};

    HTTPClient_GET(url, &http_response);

    if (http_response.data == NULL)
    {
        ESP_LOGW(TAG, "HTTP response is invalid");
        return 1;
    }

    int res = Cache_WriteFileJSON(&w_list->cache, http_response.data, "Weather.json");

    if (res != 0)
    {
        ESP_LOGW(TAG, "Failed to cache data");
    }

    int ret = DataParser_ParseWeather(http_response.data, w_list);

    if (ret != 0)
    {
        return 2;
    }

    /* debug
    for (int i = 0; i < w_list->count; i++)
    {
        ESP_LOGI(TAG, "%.f", w_list->weather[i].temp);
    }
    */

    HTTPClient_Dispose(&http_response);

    return 0;
}

/**
 * @brief Loads weather data from the local cache file.
 *
 * Reads the cached JSON payload and parses it into the provided weather list.
 */
int Weather_FetchCache(WeatherList *w_list)
{
    int res = Cache_LoadFileJSON(&w_list->cache, "Weather.json");

    if(res != 0)
    {
        ESP_LOGE(TAG, "Failed to load cache");
        return 1;
    }

    res = DataParser_ParseWeather(w_list->cache.data, w_list);

    if (res != 0)
    {
        ESP_LOGE(TAG, "Failed to parse cached data");
        return 2;
    }

    Cache_Dispose(&w_list->cache);

    return 0;
}

/**
 * @brief Clears weather list contents.
 *
 * Resets the entry count and clears the stored weather entries.
 */
void Weather_Dispose(WeatherList *w_list)
{
    w_list->count = 0;

    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        w_list->weather[i] = (Weather){0};
    }
}
