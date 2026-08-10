#ifndef SPIFFS_H
#define SPIFFS_H

#include "esp_spiffs.h"
#include <stdbool.h>

/**
 * @file Spiffs.h
 * @brief Public API for the SPIFFS storage module.
 *
 * Provides initialization and simple file helpers for the mounted SPIFFS
 * partition.
 *
 * @defgroup SPIFFS SPIFFS
 * @brief SPIFFS file helpers for persistent storage.
 *
 * The module mounts the configured SPIFFS partition and exposes basic text and
 * JSON read/write helpers. Call initialization before using the file helpers.
 * @{
 */

/**
 * @brief Initializes the SPIFFS partition mount.
 *
 * @return `0` on success, non-zero on failure.
 *
 * @note Call before any read or write helper in this module.
 */
int Spiffs_Initialize();

/**
 * @brief Writes a text file to SPIFFS.
 *
 * @param[in] file_name File name relative to the SPIFFS base path.
 * @param[in] content Null-terminated text content to write.
 *
 * @return `0` on success, non-zero on failure.
 */
int Spiffs_WriteToFile(const char* file_name, const char *content);

/**
 * @brief Reads a text file from SPIFFS.
 *
 * @param[in] file_name File name relative to the SPIFFS base path.
 * @param[out] buffer Destination buffer for the file contents.
 * @param[in] buffer_size Size of the destination buffer in bytes.
 *
 * @return `0` on success, non-zero on failure.
 */
int Spiffs_ReadFromFile(const char* file_name, char *buffer, size_t buffer_size);

/**
 * @brief Writes JSON data to SPIFFS.
 *
 * @param[in] file_name File name relative to the SPIFFS base path.
 * @param[in] raw_data Null-terminated JSON text to write.
 *
 * @return `0` on success, non-zero on failure.
 */
int Spiffs_WriteToFileJSON(const char *file_name, const char *raw_data);

/**
 * @brief Reads JSON data from SPIFFS.
 *
 * @param[in] file_name File name relative to the SPIFFS base path.
 *
 * @return Newly allocated JSON text on success, or `NULL` on failure.
 */
char *Spiffs_ReadFromFileJSON(const char *file_name);

/** @} */

#endif
