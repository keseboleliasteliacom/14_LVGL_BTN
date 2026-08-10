#ifndef WEATHER_H
#define WEATHER_H

#include <stddef.h>
#include "../Cache/Cache.h"
#include "LEOP_Limits.h"

/**
 * @file Weather.h
 * @brief Public API for the weather data module.
 *
 * Provides fixed-size weather storage plus helpers for live fetch and cached
 * data loading.
 *
 * @ingroup WEATHER
 */

/**
 * @defgroup WEATHER WEATHER
 * @brief Weather data storage and retrieval helpers.
 *
 * The module owns a fixed-size list of weather entries together with cache and
 * status bookkeeping used by the fetch paths.
 * @{
 */

/**
 * @brief Fetch status flags for the weather module.
 *
 * Updated by the live and cached fetch paths to reflect whether the most
 * recent load succeeded.
 */
typedef struct{
    bool weather_fetched; /**< True when the latest fetch completed successfully. */
}WeatherStatus;

/**
 * @brief Single weather entry.
 *
 * The timestamp buffer stores a fixed-size, null-terminated time string.
 */
typedef struct
{
    float temp;           /**< Temperature value in project-defined units. */
    int uv_index;         /**< UV index value. */
    int weather_code;     /**< Weather condition code. */
    char timestamp[20];   /**< Null-terminated timestamp string. */
} Weather;

/**
 * @brief Fixed-size weather list and associated state.
 *
 * Contains up to `LEOP_FORECAST_MAX_ENTRIES` weather entries, a current entry
 * count, cache storage, and fetch status used by the module.
 */
typedef struct
{
    Weather weather[LEOP_FORECAST_MAX_ENTRIES]; /**< Weather entry storage. */
    size_t count;        /**< Number of valid entries in weather[]. */
    Cache_t cache;       /**< Backing cache used by the fetch helpers. */
    WeatherStatus status; /**< Fetch status flags. */
} WeatherList;

/**
 * @brief Initializes a weather list.
 *
 * Resets the entry storage, count, status, and cache bookkeeping.
 *
 * @param[in,out] r_list Weather list to reset and initialize.
 *
 * @return 0 on success.
 */
int Weather_Initialize(WeatherList *r_list);

/**
 * @brief Fetches weather data from a URL.
 *
 * Performs an HTTP GET, attempts to cache the payload, and parses the
 * response into the provided weather list.
 *
 * @param[in] url Source URL for the weather payload.
 * @param[in,out] r_list Weather list to populate.
 *
 * @return
 * - `0` on success
 * - `1` if the HTTP response is invalid
 * - `2` if parsing the response fails
 */
int Weather_Fetch(const char *url, WeatherList *r_list);

/**
 * @brief Loads weather data from the local cache file.
 *
 * Reads the cached JSON payload and parses it into the provided weather list.
 *
 * @param[in,out] w_list Weather list to populate from cache.
 *
 * @return
 * - `0` on success
 * - `1` if the cache file cannot be loaded
 * - `2` if parsing the cached payload fails
 *
 * @pre Call Weather_Initialize() before using this function.
 */
int Weather_FetchCache(WeatherList *w_list);

/**
 * @brief Clears weather list contents.
 *
 * Resets the entry count and clears the stored weather entries.
 *
 * @param[in,out] r_list Weather list to reset.
 */
void Weather_Dispose(WeatherList *r_list);

/** @} */

#endif
