/**
 * @file WifiConfig.c
 * @brief Implementation of the Wi-Fi configuration storage helpers.
 *
 * @ingroup WIFI_CONFIG
 */

#include "WifiConfig.h"

static const char* TAG = "WifiConfig";


/**
 * @brief Implementation of Config_WriteToNVS_WifiSSID.
 *
 * See header for full contract documentation.
 */
int Config_WriteToNVS_WifiSSID(char* ssid) {
    int result = NVS_WriteString("wifi", "ssid", ssid);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Implementation of Config_WriteToNVS_WifiPassword.
 *
 * See header for full contract documentation.
 */
int Config_WriteToNVS_WifiPassword(char* pw) {
    int result = NVS_WriteString("wifi", "pw", pw);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Implementation of Config_LoadFromNVS_WifiSSID.
 *
 * See header for full contract documentation.
 */
int Config_LoadFromNVS_WifiSSID(char* ssid) {
    int wifi_ssid_result = NVS_ReadString("wifi", "ssid", ssid, WIFI_MAX_SSID_LEN);
    if (wifi_ssid_result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Implementation of Config_LoadFromNVS_WifiPassword.
 *
 * See header for full contract documentation.
 */
int Config_LoadFromNVS_WifiPassword(char* pw) {
    int wifi_password_result = NVS_ReadString("wifi", "pw", pw, WIFI_MAX_PASSWORD_LEN);
    if (wifi_password_result != 0) {
        return -1;
    }
    return -0;
}
