/**
 * @file HTTP.c
 * @brief Implementation of the HTTP client module.
 *
 * @ingroup HTTP
 */

#include "HTTP.h"
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "WiFi.h"

static const char *TAG = "HTTPClient";

/**
 * @brief Handles HTTP client events and accumulates response data.
 *
 * Runs in the ESP-IDF HTTP client event callback context. Response bytes are
 * appended to the caller-owned buffer during the data callback.
 *
 * @param[in] event HTTP client event handle provided by ESP-IDF.
 *
 * @return `ESP_OK` on success or `ESP_FAIL` if response buffer allocation fails.
 *
 * @warning Keep this callback non-blocking.
 */
esp_err_t HTTPClient_EventHandler(esp_http_client_event_handle_t event)
{
    HTTPResponse *resp = (HTTPResponse *)event->user_data;

    switch (event->event_id)
    {
    case HTTP_EVENT_ON_CONNECTED:

        resp->data = NULL;
        resp->length = 0;

        break;
    case HTTP_EVENT_ON_DATA:
        // ESP_LOGI(TAG, "%s", (char *)event->data);
        if (event->data_len > 0)
        {
            char *new_ptr = realloc(resp->data, resp->length + event->data_len + 1);

            if (new_ptr == NULL)
            {
                ESP_LOGE(TAG, "Failed to allocate memory");
                return ESP_FAIL;
            }

            resp->data = new_ptr;

            memcpy(resp->data + resp->length, event->data, event->data_len);

            resp->length += event->data_len;

            resp->data[resp->length] = '\0';
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        break;
    default:
        break;
    }

    return ESP_OK;
}

/**
 * @brief Implementation of HTTPClient_GET.
 *
 * See header for full contract documentation.
 */
esp_err_t HTTPClient_GET(const char *url, HTTPResponse *http_response)
{
    if (url == NULL || http_response == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    http_response->status_code = 0;

    if (!WiFi_IsConnected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    esp_http_client_config_t http_config = {0};
    http_config.url = url;
    http_config.method = HTTP_METHOD_GET;
    http_config.cert_pem = NULL;
    http_config.event_handler = HTTPClient_EventHandler;
    http_config.user_data = http_response;
    http_config.timeout_ms = 5000;

    ESP_LOGI(TAG, "HTTP GET: %s", url);

    esp_http_client_handle_t http_client = esp_http_client_init(&http_config);
    if (http_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_http_client_perform(http_client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP GET failed: %s", esp_err_to_name(err));
    }
    else
    {
        http_response->status_code = esp_http_client_get_status_code(http_client);
        int content_length = esp_http_client_get_content_length(http_client);

        ESP_LOGI(TAG, "HTTP status=%d, content_length=%d", http_response->status_code, content_length);
    }

    esp_http_client_cleanup(http_client);
    return err;
}

/**
 * @brief Performs an HTTP probe without retaining the response body.
 */
esp_err_t HTTPClient_Probe(const char *url, int timeout_ms, int *status_code)
{
    if (url == NULL || status_code == NULL || timeout_ms <= 0)
    {
        return ESP_ERR_INVALID_ARG;
    }

    *status_code = 0;

    if (!WiFi_IsConnected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    esp_http_client_config_t config = {0};
    config.url = url;
    config.method = HTTP_METHOD_GET;
    config.timeout_ms = timeout_ms;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        *status_code = esp_http_client_get_status_code(client);
        if (*status_code < 200 || *status_code >= 300)
        {
            ESP_LOGW(TAG, "HTTP probe returned status %d", *status_code);
            err = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGW(TAG, "HTTP probe failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

/**
 * @brief Releases the response buffer owned by an HTTPResponse.
 *
 * @param[in,out] http_response Response object whose data buffer should be freed.
 */
void HTTPClient_Dispose(HTTPResponse *http_response)
{
    if (http_response != NULL && http_response->data != NULL)
    {
        free(http_response->data);
        http_response->data = NULL;
        http_response->length = 0;
    }
}
