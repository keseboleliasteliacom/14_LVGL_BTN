#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include "esp_err.h"

/**
 * @file HTTP.h
 * @brief Public API for the HTTP client module.
 *
 * Provides helpers for issuing HTTP GET requests and storing the response
 * payload in a dynamically allocated buffer.
 */

/**
 * @defgroup HTTP HTTP
 * @brief HTTP client helpers for fetching response data.
 *
 * The module performs network I/O through ESP-IDF's HTTP client and relies on
 * Wi-Fi connectivity before requests are issued.
 * @{
 */

/**
 * @brief HTTP response buffer owned by the caller.
 *
 * The response data is accumulated in a heap buffer and tracked with its byte
 * length.
 */
typedef struct
{
    char* data;   /**< Pointer to the response buffer. */
    size_t length; /**< Number of valid bytes in data. */
    int status_code; /**< HTTP response status code, or zero if no response was received. */
} HTTPResponse;

/**
 * @brief Performs an HTTP GET request and stores the response body.
 *
 * @param[in] url Request URL.
 * @param[out] http_response Response storage used by the HTTP client.
 *
 * @note The request is only attempted when Wi-Fi is connected.
 * @note Response data is written through the ESP-IDF HTTP event callback.
 */
esp_err_t HTTPClient_GET(const char *url, HTTPResponse* http_response);

/**
 * @brief Performs a lightweight HTTP reachability probe.
 *
 * The response body is discarded. A request is considered successful only
 * when the HTTP transaction completes and the server returns a 2xx status.
 *
 * @param[in] url URL to probe.
 * @param[in] timeout_ms Request timeout in milliseconds.
 * @param[out] status_code HTTP response status code, or zero when unavailable.
 *
 * @return `ESP_OK` for a 2xx response, otherwise an ESP-IDF error code.
 */
esp_err_t HTTPClient_Probe(const char *url, int timeout_ms, int *status_code);

/**
 * @brief Releases the heap buffer owned by an HTTPResponse.
 *
 * @param[in,out] http_response Response object whose data buffer should be freed.
 */
void HTTPClient_Dispose(HTTPResponse *http_response);

/** @} */

#endif
