#ifndef NVS_H
#define NVS_H

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file NVS.h
 * @brief Public API for the NVS helper module.
 *
 * Provides helpers for initializing NVS and reading or writing simple values
 * in the default storage namespace or a named namespace.
 *
 * @defgroup NVS NVS
 * @brief Helpers for ESP-IDF NVS storage access.
 *
 * The module wraps common NVS operations used by the firmware. Functions
 * perform NVS open, read/write, and commit operations as needed.
 * @{
 */

/**
 * @brief Initializes the default NVS flash partition.
 *
 * Reinitializes the partition after erasing it when ESP-IDF reports that the
 * NVS pages are exhausted or the stored version is incompatible.
 *
 * @return 0 on success, or -1 if the partition had to be erased and
 * reinitialized.
 */
int NVS_Init();

/**
 * @brief Writes a string value to the default storage namespace.
 *
 * @param key NVS key to write.
 * @param value Null-terminated string value to store.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_WriteToFile(const char* key, const char* value);

/**
 * @brief Loads a string value from the default storage namespace.
 *
 * @param key NVS key to read.
 * @param value Output buffer for the stored string.
 * @param length Size of the output buffer in bytes.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_LoadFromFile(const char* key, char* value, size_t length);

/**
 * @brief Reads a string value from an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to read.
 * @param buffer Output buffer for the stored string.
 * @param size Size of the output buffer in bytes.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_ReadString(const char* nvs_namespace, const char* key, char* buffer, size_t size);

/**
 * @brief Writes a string value to an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to write.
 * @param value Null-terminated string value to store.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_WriteString(const char* nvs_namespace, const char* key, const char* value);

/**
 * @brief Reads a 32-bit unsigned integer from an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to read.
 * @param out_value Output value.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_ReadU32(const char* nvs_namespace, const char* key, uint32_t* out_value);

/**
 * @brief Writes a 32-bit unsigned integer to an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to write.
 * @param value Value to store.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_WriteU32(const char* nvs_namespace, const char* key, uint32_t value);

/**
 * @brief Reads a boolean value from an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to read.
 * @param out_value Output value.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_ReadBool(const char* nvs_namespace, const char* key, bool* out_value);

/**
 * @brief Writes a boolean value to an NVS namespace.
 *
 * @param nvs_namespace NVS namespace to open.
 * @param key NVS key to write.
 * @param value Value to store.
 *
 * @return 0 on success, or a negative value on failure.
 */
int NVS_WriteBool(const char* nvs_namespace, const char* key, bool value);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
