#ifndef PRICE_H
#define PRICE_H

#include <stddef.h>
#include "../Cache/Cache.h"
#include "LEOP_Limits.h"

/**
 * @file Price.h
 * @brief Public API for the price cache and fetch module.
 *
 * Provides the data structures and functions used to initialize, fetch,
 * restore, and clear price data.
 *
 * @defgroup PRICE PRICE
 * @brief Price data management.
 *
 * Handles remote price retrieval, cached JSON loading, and in-memory
 * bookkeeping for up to 128 price entries.
 *
 * @note Fetching depends on HTTP and JSON parsing support provided by other
 * modules.
 * @{
 */

typedef struct{
    bool electricity_fetched;
}PriceStatus;

/**
 * @brief Price sample with value and timestamp.
 *
 * The timestamp buffer is fixed-size and stores the source timestamp string.
 */
typedef struct
{
    double current_prices;
    char timestamp[20];
} Price;

/**
 * @brief Collection of fetched or cached price entries.
 *
 * Stores up to 128 entries along with load count, cache state, and fetch
 * status.
 */
typedef struct
{
    Price price[LEOP_FORECAST_MAX_ENTRIES];
    size_t count;
    Cache_t cache;
    PriceStatus status;
} PriceList;

/**
 * @brief Initializes a price list.
 *
 * @param[in,out] r_list Pointer to the price list to reset and prepare.
 *
 * @return 0 on success.
 */
int Price_Initialize(PriceList *r_list);

/**
 * @brief Fetches price data from a remote URL.
 *
 * @param[in] url Remote JSON source URL.
 * @param[in,out] r_list Pointer to the price list to populate.
 *
 * @return 0 on success.
 */
int Price_Fetch(const char *url, PriceList *r_list);

/**
 * @brief Loads price data from the local cache.
 *
 * @param[in,out] p_list Pointer to the price list to populate from cache.
 *
 * @return 0 on success.
 */
int Price_FetchCache(PriceList *p_list);

/**
 * @brief Clears the price list contents.
 *
 * @param[in,out] r_list Pointer to the price list to reset.
 */
void Price_Dispose(PriceList *r_list);

/** @} */

#endif
