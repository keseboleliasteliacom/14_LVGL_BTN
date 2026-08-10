#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "stddef.h"
#include "../Cache/Cache.h"
#include "LEOP_Limits.h"

/**
 * @file Recommendation.h
 * @brief Public API for the recommendation module.
 *
 * Provides the data types and functions used to initialize, fetch, cache, and
 * clear recommendation data.
 */

/**
 * @defgroup RECOMMENDATION Recommendation
 * @brief Recommendation data handling and cache access.
 *
 * The module stores recommendation entries in a fixed-size list, tracks fetch
 * state, and uses the cache helper for persisted JSON data.
 *
 * @note Functions operate on a caller-provided RecommendationList instance.
 * @note Fetch functions depend on the HTTP and JSON parsing pipeline used by
 * the paired implementation.
 * @{
 */

/**
 * @brief Tracks whether recommendation data has been fetched.
 */
typedef struct{
    bool recommendation_fetched;
}RecommendationStatus;

typedef enum
{
    LEOP_RECOMMENDATION_UNKNOWN = 0,
    LEOP_RECOMMENDATION_BUY,
    LEOP_RECOMMENDATION_HOLD,
    LEOP_RECOMMENDATION_SELL
} RecommendationAction;

/**
 * @brief Single recommendation entry.
 *
 * The timestamp buffer stores a fixed-length, null-terminated string.
 */
typedef struct{
    int id;
    double recommendation; /**< Recommendation score parsed from the source data. */
    RecommendationAction action; /**< Recommendation category. */
    char timestamp[20];
}Recommendation;

/**
 * @brief Container for recommendation entries, status, and cache state.
 *
 * Holds up to LEOP_FORECAST_MAX_ENTRIES entries in the fixed-size array and
 * keeps the cache and fetch status used by the recommendation workflow.
 */
typedef struct{
    Recommendation rec[LEOP_FORECAST_MAX_ENTRIES];
    size_t count;
    Cache_t cache;
    RecommendationStatus status;
}RecommendationList;

/**
 * @brief Initializes a recommendation list.
 *
 * @param[in,out] r_list Pointer to the recommendation list to initialize.
 *
 * @return `0` on success.
 */
int Recommendation_Initialize(RecommendationList *r_list);

/**
 * @brief Fetches recommendation data from a URL.
 *
 * @param[in] url Source URL for the recommendation JSON data.
 * @param[in,out] r_list Pointer to the destination recommendation list.
 *
 * @return `0` on success.
 */
int Recommendation_Fetch(const char *url, RecommendationList *r_list);

/**
 * @brief Loads recommendation data from the cache file.
 *
 * @param[in,out] r_list Pointer to the destination recommendation list.
 *
 * @return `0` on success.
 */
int Recommendation_FetchCache(RecommendationList* r_list);

/**
 * @brief Clears a recommendation list.
 *
 * @param[in,out] r_list Pointer to the recommendation list to reset.
 */
void Recommendation_Dispose(RecommendationList* r_list);

/** @} */

#endif
