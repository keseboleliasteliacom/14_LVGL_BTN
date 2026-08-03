/**
 * @file DataParser.c
 * @brief Implementation of the DataParser module.
 *
 * @ingroup DATAPARSER
 */

#include "DataParser.h"
#include <string.h>
#include "jansson.h"
#include "esp_log.h"

static const char *TAG = "DataParser";

/**
 * @brief Parses a recommendation JSON array into a recommendation list.
 *
 * See header for full contract documentation.
 */
int DataParser_ParseRecommendation(const char *raw_data, RecommendationList *r_list)
{
    json_error_t error;
    json_t *root = json_loads(raw_data, 0, &error);

    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to load data from http response");
        return 1;
    }

    if (!json_is_array(root))
    {
        ESP_LOGW(TAG, "data is not an array");
        json_decref(root);
        return 2;
    }

    size_t array_size = json_array_size(root);

    if(array_size <= 0)
    {
        ESP_LOGW(TAG, "array is empty");
        json_decref(root);
        return 3;
    }

    if (array_size > LEOP_FORECAST_MAX_ENTRIES)
    {
        ESP_LOGW(TAG, "Recommendation array truncated from %u to %u entries",
                 (unsigned)array_size, LEOP_FORECAST_MAX_ENTRIES);
        array_size = LEOP_FORECAST_MAX_ENTRIES;
    }
    r_list->count = array_size;

    for (int i = 0; i < array_size; i++)
    {
        json_t *obj = json_array_get(root, i);

        json_t *id = json_object_get(obj, "id");
        json_t *rec = json_object_get(obj, "score");
        json_t *recommendation = json_object_get(obj, "recommendation");
        json_t *timestamp = json_object_get(obj, "timestamp");

        /* Accept cached responses written before the API field rename. */
        if (rec == NULL)
        {
            rec = json_object_get(obj, "type");
        }

        r_list->rec[i].id = json_integer_value(id);
        r_list->rec[i].recommendation = json_real_value(rec);
        r_list->rec[i].action = LEOP_RECOMMENDATION_UNKNOWN;

        const char *action = json_string_value(recommendation);
        if (action != NULL)
        {
            if (strcmp(action, "buy") == 0)
                r_list->rec[i].action = LEOP_RECOMMENDATION_BUY;
            else if (strcmp(action, "hold") == 0)
                r_list->rec[i].action = LEOP_RECOMMENDATION_HOLD;
            else if (strcmp(action, "sell") == 0)
                r_list->rec[i].action = LEOP_RECOMMENDATION_SELL;
        }
        else if (r_list->rec[i].recommendation < 0.25)
        {
            r_list->rec[i].action = LEOP_RECOMMENDATION_BUY;
        }
        else if (r_list->rec[i].recommendation < 0.75)
        {
            r_list->rec[i].action = LEOP_RECOMMENDATION_HOLD;
        }
        else
        {
            r_list->rec[i].action = LEOP_RECOMMENDATION_SELL;
        }
        strncpy(r_list->rec[i].timestamp,
                json_string_value(timestamp),
                sizeof(r_list->rec[i].timestamp) - 1);

        r_list->rec[i].timestamp[sizeof(r_list->rec[i].timestamp) - 1] = '\0';
    }

    json_decref(root);
    return 0;
}

/**
 * @brief Parses a weather JSON array into a weather list.
 *
 * See header for full contract documentation.
 */
int DataParser_ParseWeather(const char *raw_data, WeatherList *w_list)
{
    json_error_t error;
    json_t *root = json_loads(raw_data, 0, &error);

    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to load data from http response");
        return 1;
    }

    if (!json_is_array(root))
    {
        ESP_LOGW(TAG, "data is not an array");
        json_decref(root);
        return 2;
    }

    size_t array_size = json_array_size(root);

    if(array_size <= 0)
    {
        ESP_LOGW(TAG, "array is empty");
        json_decref(root);
        return 3;
    }

    if (array_size > LEOP_FORECAST_MAX_ENTRIES)
    {
        ESP_LOGW(TAG, "Weather array truncated from %u to %u entries",
                 (unsigned)array_size, LEOP_FORECAST_MAX_ENTRIES);
        array_size = LEOP_FORECAST_MAX_ENTRIES;
    }
    w_list->count = array_size;

    for (int i = 0; i < array_size; i++)
    {
        json_t *obj = json_array_get(root, i);

        json_t *temp = json_object_get(obj, "temp");
        json_t *uv_index = json_object_get(obj, "uv_index");
        json_t *weather_code = json_object_get(obj, "weather_code");
        json_t *timestamp = json_object_get(obj, "timestamp");

        w_list->weather[i].temp = json_real_value(temp);
        w_list->weather[i].weather_code = json_integer_value(weather_code);
        w_list->weather[i].uv_index = json_integer_value(uv_index);
        strncpy(w_list->weather[i].timestamp,
                json_string_value(timestamp),
                sizeof(w_list->weather[i].timestamp) - 1);

        w_list->weather[i].timestamp[sizeof(w_list->weather[i].timestamp) - 1] = '\0';
    }

    json_decref(root);
    return 0;
}

/**
 * @brief Parses a price JSON array into a price list.
 *
 * See header for full contract documentation.
 */
int DataParser_ParsePrice(const char *raw_data, PriceList *p_list)
{
    json_error_t error;
    json_t *root = json_loads(raw_data, 0, &error);

    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to load data from http response");
        return 1;
    }

    if (!json_is_array(root))
    {
        ESP_LOGW(TAG, "data is not an array");
        json_decref(root);
        return 2;
    }

    size_t array_size = json_array_size(root);

    if(array_size <= 0)
    {
        ESP_LOGW(TAG, "array is empty");
        json_decref(root);
        return 3;
    }

    if (array_size > LEOP_FORECAST_MAX_ENTRIES)
    {
        ESP_LOGW(TAG, "Price array truncated from %u to %u entries",
                 (unsigned)array_size, LEOP_FORECAST_MAX_ENTRIES);
        array_size = LEOP_FORECAST_MAX_ENTRIES;
    }
    p_list->count = array_size;

    for (int i = 0; i < array_size; i++)
    {
        json_t *obj = json_array_get(root, i);

        json_t *price = json_object_get(obj, "price_sek_per_kwh");
        json_t *timestamp = json_object_get(obj, "timestamp");

        /* Accept cached responses written before the API field rename. */
        if (price == NULL)
        {
            price = json_object_get(obj, "price SEK");
        }

        p_list->price[i].current_prices = json_real_value(price);
        strncpy(p_list->price[i].timestamp,
                json_string_value(timestamp),
                sizeof(p_list->price[i].timestamp) - 1);

        p_list->price[i].timestamp[sizeof(p_list->price[i].timestamp) - 1] = '\0';
    }

    json_decref(root);
    return 0;
}
