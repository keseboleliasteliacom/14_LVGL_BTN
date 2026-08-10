#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "../app_types.h"
#include "Memory/NVS.h"

/**
 * @file WifiConfig.h
 * @brief Public API for Wi-Fi configuration storage helpers.
 *
 * Provides helpers for storing and loading Wi-Fi credentials in NVS.
 *
 * @ingroup WIFI_CONFIG
 */

/**
 * @defgroup WIFI_CONFIG WIFI_CONFIG
 * @brief Wi-Fi configuration storage helpers.
 *
 * Wraps NVS access for Wi-Fi SSID and password persistence. The helpers are
 * intended for task context during configuration save and load operations.
 * @{
 */

#define WIFI_MAX_SSID_LEN 33
#define WIFI_MAX_PASSWORD_LEN 65

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes the Wi-Fi SSID to NVS.
 *
 * @param[in] ssid Null-terminated SSID string to store.
 *
 * @return 0 on success, -1 on failure.
 */
int Config_WriteToNVS_WifiSSID(char* ssid);

/**
 * @brief Writes the Wi-Fi password to NVS.
 *
 * @param[in] pw Null-terminated password string to store.
 *
 * @return 0 on success, -1 on failure.
 */
int Config_WriteToNVS_WifiPassword(char* pw);

/**
 * @brief Loads the Wi-Fi SSID from NVS.
 *
 * @param[out] out Buffer that receives the stored SSID.
 *
 * @return 0 on success, -1 on failure.
 */
int Config_LoadFromNVS_WifiSSID(char* out);

/**
 * @brief Loads the Wi-Fi password from NVS.
 *
 * @param[out] out Buffer that receives the stored password.
 *
 * @return 0 on success, -1 on failure.
 */
int Config_LoadFromNVS_WifiPassword(char* out);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
