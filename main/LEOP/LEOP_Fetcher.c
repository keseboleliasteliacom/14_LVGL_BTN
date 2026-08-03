/**
 * @file LEOP_Fetcher.c
 * @brief Implementation of the LEOP fetcher module.
 *
 * @ingroup LEOP_FETCHER
 */

#include "LEOP_Fetcher.h"
#include "../WiFi.h"
#include "../HTTP.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <limits.h>
#include "../app_types.h"
#include "esp_timer.h"

static const char *TAG = "LEOP";

#define LEOP_SERVER_URL "http://31.59.105.197"
#define LEOP_RECOMMENDATION_ENDPOINT LEOP_SERVER_URL "/id=2?recommendation"
#define LEOP_WEATHER_ENDPOINT LEOP_SERVER_URL "/id=2?weather"
#define LEOP_PRICE_ENDPOINT LEOP_SERVER_URL "/id=2?price"
#define LEOP_HEALTH_ENDPOINT LEOP_RECOMMENDATION_ENDPOINT

#define LEOP_HEALTHY_CHECK_INTERVAL_MS 60000
#define LEOP_FAILURE_RETRY_INTERVAL_MS 10000
#define LEOP_HEALTH_TIMEOUT_MS 5000
#define LEOP_FAILURE_THRESHOLD 3

QueueHandle_t recommendation_queue = NULL;
QueueHandle_t weather_queue = NULL;
QueueHandle_t price_queue = NULL;
QueueHandle_t leop_status_queue = NULL;

typedef struct
{
    bool recommendation_ok;
    bool weather_ok;
    bool price_ok;
} leop_fetch_result_t;

static leop_connection_state_t last_published_state = LEOP_CONNECTION_NO_WIFI;
static bool status_has_been_published = false;
static leop_connection_cb_t connection_callback = NULL;
static void *connection_callback_ctx = NULL;

/**
 * @brief Registers the LEOP connection-state callback.
 *
 * The callback is invoked from LEOP worker task context when the published
 * connection state changes.
 */
void LEOPFetcher_SetConnectionCallback(leop_connection_cb_t cb, void *ctx)
{
    connection_callback = cb;
    connection_callback_ctx = ctx;
}

/**
 * @brief Returns whether the current tick count has reached the deadline.
 *
 * @param[in] now Current tick count.
 * @param[in] deadline Deadline tick count.
 *
 * @return `true` when the deadline has been reached or passed.
 */
static bool LEOPFetcher_DeadlineReached(TickType_t now, TickType_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}

/**
 * @brief Publishes the current LEOP connection status.
 *
 * The status is sent through the shared queue and mirrored to the optional
 * callback from worker task context.
 *
 * @param[in] state Connection state to publish.
 * @param[in] consecutive_failures Number of consecutive failures.
 * @param[in] http_status_code Latest HTTP status code, if available.
 */
static void LEOPFetcher_PublishStatus(leop_connection_state_t state,
                                      uint8_t consecutive_failures,
                                      int http_status_code)
{
    if (leop_status_queue == NULL)
    {
        return;
    }

    if (status_has_been_published && state == last_published_state)
    {
        return;
    }

    leop_status_message_t message = {
        .state = state,
        .consecutive_failures = consecutive_failures,
        .http_status_code = http_status_code,
    };

    if (xQueueOverwrite(leop_status_queue, &message) == pdPASS)
    {
        last_published_state = state;
        status_has_been_published = true;

        if (connection_callback != NULL)
        {
            connection_callback(state, connection_callback_ctx);
        }
    }
}

/**
 * @brief Publishes the latest LEOP data snapshots to the shared queues.
 *
 * @param[in] leop_data Source data to publish.
 */
static void LEOPFetcher_PublishData(const LEOPData *leop_data)
{
    if (recommendation_queue != NULL)
    {
        xQueueOverwrite(recommendation_queue, &leop_data->recommendations);
    }
    if (weather_queue != NULL)
    {
        xQueueOverwrite(weather_queue, &leop_data->weather);
    }
    if (price_queue != NULL)
    {
        xQueueOverwrite(price_queue, &leop_data->price_list);
    }
}

/**
 * @brief Loads cached LEOP data and publishes the resulting snapshots.
 *
 * Updates the fetch flags from the cache helpers before publishing the local
 * snapshots to the shared queues.
 *
 * @param[in,out] leop_data LEOP state to update from cache.
 */
static void LEOPFetcher_LoadCachedData(LEOPData *leop_data)
{
    leop_data->recommendations.status.recommendation_fetched =
        (Recommendation_FetchCache(&leop_data->recommendations) == 0);
    leop_data->weather.status.weather_fetched =
        (Weather_FetchCache(&leop_data->weather) == 0);
    leop_data->price_list.status.electricity_fetched =
        (Price_FetchCache(&leop_data->price_list) == 0);

    LEOPFetcher_PublishData(leop_data);
}

/**
 * @brief Fetches all LEOP remote payloads.
 *
 * Updates the in-memory snapshots, publishes them to the shared queues, and
 * records the last successful recommendation update time in seconds.
 *
 * @param[in,out] leop_data LEOP state to update with fetched data.
 * @param[in,out] last_update_recommendation_success Seconds since boot for the
 *        last successful recommendation fetch.
 *
 * @return Per-source fetch success flags.
 */
static leop_fetch_result_t LEOPFetcher_FetchAll(LEOPData *leop_data, uint32_t *last_update_recommendation_success)
{
    leop_fetch_result_t result = {0};

    ESP_LOGI(TAG, "Fetching %s", LEOP_RECOMMENDATION_ENDPOINT);
    result.recommendation_ok =
        (Recommendation_Fetch(LEOP_RECOMMENDATION_ENDPOINT, &leop_data->recommendations) == 0);
    leop_data->recommendations.status.recommendation_fetched = result.recommendation_ok;
    
    // Add a monotonic timestamp of the last time we fetched a successful recommendation.
    // Currently used to display as info on the settings tab
    if (result.recommendation_ok == true)
    {
        *last_update_recommendation_success = (uint32_t)(esp_timer_get_time() / 1000000ULL);
    }

    ESP_LOGI(TAG, "Fetching %s", LEOP_WEATHER_ENDPOINT);
    result.weather_ok =
        (Weather_Fetch(LEOP_WEATHER_ENDPOINT, &leop_data->weather) == 0);
    leop_data->weather.status.weather_fetched = result.weather_ok;

    ESP_LOGI(TAG, "Fetching %s", LEOP_PRICE_ENDPOINT);
    result.price_ok =
        (Price_Fetch(LEOP_PRICE_ENDPOINT, &leop_data->price_list) == 0);
    leop_data->price_list.status.electricity_fetched = result.price_ok;

    LEOPFetcher_PublishData(leop_data);
    return result;
}

/**
 * @brief Converts the configured fetch interval to RTOS ticks.
 *
 * Uses the application-provided interval in minutes and clamps the result to
 * the RTOS tick range.
 *
 * @param[in] leop_data LEOP state containing the interval pointer.
 *
 * @return Fetch interval in ticks, clamped to the RTOS tick range.
 */
static TickType_t LEOPFetcher_FetchIntervalTicks(const LEOPData *leop_data)
{
    uint32_t minutes = 1;
    if (leop_data->leop_conf.time_interval != NULL &&
        *leop_data->leop_conf.time_interval > 0)
    {
        minutes = *leop_data->leop_conf.time_interval;
    }

    uint64_t interval_ms = (uint64_t)minutes * 60ULL * 1000ULL;
    uint64_t max_ms = (uint64_t)UINT32_MAX * portTICK_PERIOD_MS;
    if (interval_ms > max_ms)
    {
        interval_ms = max_ms;
    }

    return pdMS_TO_TICKS(interval_ms);
}

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

    leop_status_queue = xQueueCreate(1, sizeof(leop_status_message_t));

    if (leop_status_queue == NULL)
    {
        ESP_LOGW(TAG, "Failed to create LEOP status queue");
        return -4;
    }

    status_has_been_published = false;

    return 0;
}

/**
 * @brief Implementation of LEOPFetcher_Work.
 *
 * See header for full contract documentation.
 */
void LEOPFetcher_Work(void *arg)
{
    app_state_t *app_data = (app_state_t *)arg;
    LEOPData *leop_data = &app_data->leop_data;

    uint32_t *last_recommendation_success_seconds = &app_data->last_recommendation_update_seconds;

    if (leop_data == NULL)
        return;

    TickType_t now = xTaskGetTickCount();
    TickType_t next_fetch = now;
    TickType_t next_health_check = now;
    uint8_t consecutive_failures = 0;
    bool previous_wifi_connected = false;
    bool offline_cache_loaded = false;

    LEOPFetcher_PublishStatus(LEOP_CONNECTION_NO_WIFI, 0, 0);

    while (1)
    {
        now = xTaskGetTickCount();
        bool wifi_connected = WiFi_IsConnected();

        if (!wifi_connected)
        {
            if (previous_wifi_connected || !status_has_been_published)
            {
                LEOPFetcher_PublishStatus(LEOP_CONNECTION_NO_WIFI, 0, 0);
            }

            if (!offline_cache_loaded)
            {
                ESP_LOGI(TAG, "WiFi unavailable; loading cached LEOP data");
                LEOPFetcher_LoadCachedData(leop_data);
                offline_cache_loaded = true;
            }

            previous_wifi_connected = false;
            consecutive_failures = 0;
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
            continue;
        }

        offline_cache_loaded = false;

        if (!previous_wifi_connected)
        {
            previous_wifi_connected = true;
            consecutive_failures = 0;
            next_health_check = now;
            LEOPFetcher_PublishStatus(LEOP_CONNECTION_CHECKING, 0, 0);
        }

        if (LEOPFetcher_DeadlineReached(now, next_fetch))
        {
            leop_fetch_result_t result = LEOPFetcher_FetchAll(leop_data, last_recommendation_success_seconds);
            bool any_succeeded = result.recommendation_ok || result.weather_ok || result.price_ok;
            bool all_succeeded = result.recommendation_ok && result.weather_ok && result.price_ok;

            if (all_succeeded)
            {
                consecutive_failures = 0;
                LEOPFetcher_PublishStatus(LEOP_CONNECTION_CONNECTED, 0, 200);
                next_health_check = xTaskGetTickCount() + pdMS_TO_TICKS(LEOP_HEALTHY_CHECK_INTERVAL_MS);
            }
            else if (any_succeeded)
            {
                consecutive_failures = 0;
                LEOPFetcher_PublishStatus(LEOP_CONNECTION_DEGRADED, 0, 200);
                next_health_check = xTaskGetTickCount() + pdMS_TO_TICKS(LEOP_HEALTHY_CHECK_INTERVAL_MS);
            }
            else
            {
                consecutive_failures++;
                if (consecutive_failures >= LEOP_FAILURE_THRESHOLD)
                {
                    LEOPFetcher_PublishStatus(LEOP_CONNECTION_UNAVAILABLE, consecutive_failures, 0);
                }
                next_health_check = xTaskGetTickCount() + pdMS_TO_TICKS(LEOP_FAILURE_RETRY_INTERVAL_MS);
            }

            next_fetch = xTaskGetTickCount() + LEOPFetcher_FetchIntervalTicks(leop_data);
        }
        else if (LEOPFetcher_DeadlineReached(now, next_health_check))
        {
            int status_code = 0;
            esp_err_t probe_result = HTTPClient_Probe(LEOP_HEALTH_ENDPOINT,
                                                     LEOP_HEALTH_TIMEOUT_MS,
                                                     &status_code);

            if (probe_result == ESP_OK)
            {
                consecutive_failures = 0;
                LEOPFetcher_PublishStatus(LEOP_CONNECTION_CONNECTED, 0, status_code);
                next_health_check = xTaskGetTickCount() + pdMS_TO_TICKS(LEOP_HEALTHY_CHECK_INTERVAL_MS);
            }
            else
            {
                if (consecutive_failures < UINT8_MAX)
                {
                    consecutive_failures++;
                }

                if (consecutive_failures >= LEOP_FAILURE_THRESHOLD)
                {
                    LEOPFetcher_PublishStatus(LEOP_CONNECTION_UNAVAILABLE,
                                              consecutive_failures,
                                              status_code);
                }

                next_health_check = xTaskGetTickCount() + pdMS_TO_TICKS(LEOP_FAILURE_RETRY_INTERVAL_MS);
            }
        }

        now = xTaskGetTickCount();
        TickType_t next_deadline = next_fetch;
        if ((int32_t)(next_health_check - next_deadline) < 0)
        {
            next_deadline = next_health_check;
        }

        TickType_t wait_ticks = 0;
        if (!LEOPFetcher_DeadlineReached(now, next_deadline))
        {
            wait_ticks = next_deadline - now;
        }

        ulTaskNotifyTake(pdTRUE, wait_ticks);
    }
}
