/**
 * @file Recommendation.c
 * @brief Implementation of the recommendation module.
 *
 * @ingroup RECOMMENDATION
 */

#include "Recommendation.h"
#include "../HTTP.h"
#include "../JSONParser/DataParser.h"
#include "esp_log.h"

#ifdef RECOMMENDATION_TEST
static const char* http_mock = "[
    {
        "id": 2,
        "type": 0.0,
        "timestamp": "2026-06-10T14:45:00+02:00",
        "temp": 16.200000762939453
    },
    {
        "id": 2,
        "type": 0.0010183617495829668,
        "timestamp": "2026-06-10T15:00:00+02:00",
        "temp": 16.299999237060547
}]";
#endif

static const char *TAG = "Recommendation";

/**
 * @brief Implementation of Recommendation_Initialize.
 *
 * See header for full contract documentation.
 */
int Recommendation_Initialize(RecommendationList *r_list)
{
    r_list->count = 0;
    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        r_list->rec[i] = (Recommendation){0};
    }

    r_list->status.recommendation_fetched = false;

    Cache_Initialize(&r_list->cache);

    return 0;
}

/**
 * @brief Implementation of Recommendation_Fetch.
 *
 * Fetches recommendation JSON through the HTTP helper, caches the raw payload,
 * and parses the result into the provided list.
 *
 * See header for full contract documentation.
 */
int Recommendation_Fetch(const char *url, RecommendationList *r_list)
{
    HTTPResponse http_response = {0};

    #ifdef RECOMMENDATION_TEST
    http_response.data = http_mock;
    #else
    HTTPClient_GET(url, &http_response);
    #endif
    
    if (http_response.data == NULL)
    {
        ESP_LOGW(TAG, "HTTP response is invalid");
        return 1;
    }

    int res = Cache_WriteFileJSON(&r_list->cache, http_response.data, "Recommendations.json");

    if (res != 0)
    {
        ESP_LOGW(TAG, "Failed to cache data");
    }


    int ret = DataParser_ParseRecommendation(http_response.data, r_list);

    if (ret != 0)
    {
        HTTPClient_Dispose(&http_response);
        return 2;
    }

    /* debug
    for (int i = 0; i < r_list->count; i++)
    {
        ESP_LOGI(TAG, "%lf", r_list->rec[i].recommendation);
    }
    */

    HTTPClient_Dispose(&http_response);

    return 0;
}

/**
 * @brief Implementation of Recommendation_FetchCache.
 *
 * Loads cached recommendation JSON and parses it into the provided list.
 *
 * See header for full contract documentation.
 */
int Recommendation_FetchCache(RecommendationList *r_list)
{
    int res = Cache_LoadFileJSON(&r_list->cache, "Recommendations.json");

    if (res != 0)
    {
        ESP_LOGE(TAG, "Failed to load cache");
        return 1;
    }

    res = DataParser_ParseRecommendation(r_list->cache.data, r_list);

    if (res != 0)
    {
        ESP_LOGE(TAG, "Failed to parse cached data");
        return 2;
    }

    for (int i = 0; i < r_list->count; i++)
    {
        //ESP_LOGI(TAG, "%lf", r_list->rec[i].recommendation);
    }

    Cache_Dispose(&r_list->cache);

    return 0;
}

/**
 * @brief Implementation of Recommendation_Dispose.
 *
 * Clears the list contents and resets the entry count.
 *
 * See header for full contract documentation.
 */
void Recommendation_Dispose(RecommendationList *r_list)
{
    r_list->count = 0;

    for (int i = 0; i < LEOP_FORECAST_MAX_ENTRIES; i++)
    {
        r_list->rec[i] = (Recommendation){0};
    }
}
