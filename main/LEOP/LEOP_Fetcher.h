#ifndef LEOP_FETCHER_H
#define LEOP_FETCHER_H
#include "Recommendation.h"
#include "Weather.h"
#include "Price.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @file LEOP_Fetcher.h
 * @brief Public API for the LEOP fetcher module.
 *
 * The module fetches recommendation, weather, and price data, falls back to
 * cached snapshots when Wi-Fi is unavailable, and publishes state through
 * FreeRTOS queues.
 *
 * @defgroup LEOP_FETCHER LEOP_FETCHER
 * @brief Fetcher module for LEOP data.
 *
 * The module initializes local storage for fetched data, updates it from the
 * network or cached sources, and publishes snapshots through FreeRTOS queues.
 * @{
 */

extern QueueHandle_t recommendation_queue;
extern QueueHandle_t weather_queue;
extern QueueHandle_t price_queue;
extern QueueHandle_t leop_status_queue;

/**
 * @brief Application-level LEOP connectivity state published by the worker.
 */
typedef enum
{
    LEOP_CONNECTION_NO_WIFI,
    LEOP_CONNECTION_CHECKING,
    LEOP_CONNECTION_CONNECTED,
    LEOP_CONNECTION_DEGRADED,
    LEOP_CONNECTION_UNAVAILABLE,
} leop_connection_state_t;

/**
 * @brief Latest LEOP connectivity result published to consumers.
 *
 * Contains the current worker-published state together with the failure count
 * and associated HTTP status code.
 */
typedef struct
{
    leop_connection_state_t state; /**< Current published connection state. */
    uint8_t consecutive_failures; /**< Number of consecutive failed attempts. */
    int http_status_code; /**< HTTP status code associated with the state. */
} leop_status_message_t;

typedef void (*leop_connection_cb_t)(leop_connection_state_t state, void *ctx);

/**
 * @brief Registers a callback for LEOP connection-state changes.
 *
 * The callback runs in LEOP worker task context when the published state
 * changes.
 *
 * @param[in] cb Callback to invoke when the published state changes.
 * @param[in] ctx Opaque callback context.
 */
void LEOPFetcher_SetConnectionCallback(leop_connection_cb_t cb, void *ctx);

/**
 * @brief Configuration for LEOP fetch timing.
 *
 * The interval points to the fetch interval in minutes owned by the
 * application state.
 */
typedef struct{
    uint32_t* time_interval; /**< Pointer to the fetch interval in minutes. */
}LEOPConfig;

/**
 * @brief Aggregated LEOP data and fetch status.
 *
 * Stores the current recommendation, weather, and price payloads together with
 * the fetch configuration used by the worker.
 */
typedef struct{
    RecommendationList recommendations; /**< Latest recommendation payload. */
    WeatherList weather; /**< Latest weather payload. */
    PriceList price_list; /**< Latest price payload. */
    LEOPConfig leop_conf; /**< Fetch configuration shared with the worker. */
}LEOPData;

/**
 * @brief Initializes the LEOP fetcher state and queues.
 *
 * @param[in,out] leop_data Pointer to the LEOP state to initialize.
 * @param[in] interval Initial interval value supplied by the caller.
 *
 * @return `0` on success or a negative value on failure.
 *
 * @note Creates single-element FreeRTOS queues used to publish fetched data.
 */
int LEOPFetcher_Initialize(LEOPData *leop_data, uint32_t interval);

/**
 * @brief Worker task entry point for LEOP data fetching.
 *
 * @param[in] arg Pointer to `app_state_t` passed to the task.
 *
 * @note Runs in task context and blocks on delays, notifications, and network
 * operations while fetching data.
 */
void LEOPFetcher_Work(void *arg);

/** @} */

#endif
