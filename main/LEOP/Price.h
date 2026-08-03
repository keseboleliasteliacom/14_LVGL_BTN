#ifndef PRICE_H
#define PRICE_H

#include <stddef.h>
#include "../Cache/Cache.h"
#include "LEOP_Limits.h"

/**
 * @file Price.h
 * @brief Public API for the price cache and fetch module.
 *
 * Provides the data structures and functions used to initialize, fetch from
 * HTTP, restore cached JSON, and clear price data.
 *
 * @defgroup PRICE PRICE
 * @brief Price data management.
 *
 * Handles remote price retrieval, cached JSON loading, and in-memory
 * bookkeeping for up to LEOP_FORECAST_MAX_ENTRIES entries.
 *
 * @note Fetching depends on HTTP, cache, and JSON parsing support provided by
 * other modules.
 * @{
 */

/**
 * @brief Tracks whether electricity price data has been fetched.
 */
typedef struct{
    bool electricity_fetched; /**< True when electricity price data has been fetched. */
}PriceStatus;

/**
 * @brief Price sample with value and timestamp.
 *
 * The timestamp buffer is fixed-size and stores the source timestamp string.
 */
typedef struct
{
    double current_prices;   /**< Parsed price value. */
    char timestamp[20];      /**< Source timestamp string. */
} Price;

/**
 * @brief Collection of fetched or cached price entries.
 *
 * Stores up to LEOP_FORECAST_MAX_ENTRIES entries along with the current
 * count, cache storage, and fetch status.
 */
typedef struct
{
    Price price[LEOP_FORECAST_MAX_ENTRIES]; /**< Parsed price entries. */
    size_t count;                           /**< Number of valid entries. */
    Cache_t cache;                          /**< Raw JSON cache storage. */
    PriceStatus status;                     /**< Fetch status flags. */
} PriceList;

/**
 * @brief Initializes a price list.
 *
 * @param[in,out] r_list Pointer to the price list to reset and prepare.
 *
 * @return 0 on success.
 *
 * @note Resets the stored entries and prepares the cache for use.
 */
int Price_Initialize(PriceList *r_list);

/**
 * @brief Fetches price data from a remote URL.
 *
 * @param[in] url Remote JSON source URL.
 * @param[in,out] r_list Pointer to the price list to populate.
 *
 * @return
 * - 0 on success
 * - 1 if the HTTP response did not contain data
 * - 2 if the response could not be parsed
 *
 * @note Performs network I/O and attempts to write the raw response to cache.
 */
int Price_Fetch(const char *url, PriceList *r_list);

/**
 * @brief Loads price data from the local cache.
 *
 * @param[in,out] p_list Pointer to the price list to populate from cache.
 *
 * @return
 * - 0 on success
 * - 1 if the cache file could not be loaded
 * - 2 if the cached JSON could not be parsed
 *
 * @note Uses local storage and disposes the cache buffer after a successful
 * parse.
 */
int Price_FetchCache(PriceList *p_list);

/**
 * @brief Clears the price list contents.
 *
 * @param[in,out] r_list Pointer to the price list to reset.
 *
 * @note Resets the entry count and zeroes stored prices.
 */
void Price_Dispose(PriceList *r_list);

/** @} */

#endif
