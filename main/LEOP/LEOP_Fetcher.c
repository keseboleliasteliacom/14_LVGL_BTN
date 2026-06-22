/**
 * @file LEOP_Fetcher.c
 * @brief Implementation of the LEOP fetcher module.
 *
 * @ingroup LEOP_FETCHER
 */

#include "LEOP_Fetcher.h"
#include "../WiFi.h"
#include "esp_log.h"

static const char *TAG = "LEOP";

#define LEOP_SERVER_URL "http://31.59.105.197"
#define LEOP_RECOMMENDATION_ENDPOINT LEOP_SERVER_URL "/id=2?recommendation"
#define LEOP_WEATHER_ENDPOINT LEOP_SERVER_URL "/id=2?weather"
#define LEOP_PRICE_ENDPOINT LEOP_SERVER_URL "/id=2?price"

QueueHandle_t recommendation_queue = NULL;
QueueHandle_t weather_queue = NULL;
QueueHandle_t price_queue = NULL;

/**
 * @brief Implementation of LEOPFetcher_Initialize.
 *
 * See header for full contract documentation.
 */
int LEOPFetcher_Initialize(LEOPData *leop_data, uint32_t interval)
{

    if (leop_data == NULL)
    {
        ESP_LOGE(TAG, "leop_data is NULL");
        return -1;
    }

    Recommendation_Initialize(&leop_data->recommendations);
    Weather_Initialize(&leop_data->weather);
    Price_Initialize(&leop_data->price_list);

    //leop_data->leop_conf.time_interval = interval;


    recommendation_queue = xQueueCreate(1, sizeof(RecommendationList));

    if (recommendation_queue == NULL)
    {
        ESP_LOGW(TAG, "Failed to create recommendation queue");
        return -1;
    }

    weather_queue = xQueueCreate(1, sizeof(WeatherList ));

    if (weather_queue == NULL)
    {
        ESP_LOGW(TAG, "Failed to create weather queue");
        return -2;
    }

    price_queue = xQueueCreate(1, sizeof(PriceList));

    if (price_queue == NULL)
    {
        ESP_LOGW(TAG, "Failed to create price queue");
        return -3;
    }

    return 0;
}

/**
 * @brief Implementation of LEOPFetcher_Work.
 *
 * See header for full contract documentation.
 */
void LEOPFetcher_Work(void *arg)
{
    LEOPData *leop_data = (LEOPData *)arg;

    if (leop_data == NULL)
        return;

    while (1)
    {
        if (WiFi_IsConnected())
        {
            ESP_LOGI(TAG, "Fetching %s", LEOP_RECOMMENDATION_ENDPOINT);
            leop_data->recommendations.status.recommendation_fetched =
                (Recommendation_Fetch(LEOP_RECOMMENDATION_ENDPOINT, &leop_data->recommendations) == 0);

            ESP_LOGI(TAG, "Fetching %s", LEOP_WEATHER_ENDPOINT);
            leop_data->weather.status.weather_fetched =
                (Weather_Fetch(LEOP_WEATHER_ENDPOINT, &leop_data->weather) == 0);
            
                ESP_LOGI(TAG, "Fetching %s", LEOP_PRICE_ENDPOINT);
            leop_data->price_list.status.electricity_fetched =
                (Price_Fetch(LEOP_PRICE_ENDPOINT, &leop_data->price_list) == 0);

            if (xQueueSend(recommendation_queue, &leop_data->recommendations, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to recommendation queue");

            if (xQueueSend(weather_queue, &leop_data->weather, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to weather queue");

            if (xQueueSend(price_queue, &leop_data->price_list, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to price queue");

        }
        else
        {
            ESP_LOGI(TAG, "WiFi is not connected, waiting 60 seconds until retrying, loading cached data:");

            leop_data->recommendations.status.recommendation_fetched =
                (Recommendation_FetchCache(&leop_data->recommendations) == 0);

            leop_data->weather.status.weather_fetched =
                (Weather_FetchCache(&leop_data->weather) == 0);

            leop_data->price_list.status.electricity_fetched =
                (Price_FetchCache(&leop_data->price_list) == 0);



            if (xQueueSend(recommendation_queue, &leop_data->recommendations, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to recommendation queue");

            if (xQueueSend(weather_queue, &leop_data->weather, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to weather queue");

            if (xQueueSend(price_queue, &leop_data->price_list, 0) != pdPASS)
                ESP_LOGW(TAG, "Failed to send data to price queue");

            vTaskDelay(pdMS_TO_TICKS(60000));
            continue;
        }

        // Delay by our fetch time interval in minutes.
        // From MS, * 1000 to get S, * 60 to get M
        vTaskDelay(pdMS_TO_TICKS((*leop_data->leop_conf.time_interval * 1000 * 60)));
    }
}
