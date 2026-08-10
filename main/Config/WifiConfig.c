/**
 * @file WifiConfig.c
 * @brief Implementation of the Wi-Fi configuration storage helpers.
 *
 * @ingroup WIFI_CONFIG
 */

#include "WifiConfig.h"

static const char* TAG = "WifiConfig";


/**
 * @brief Stores the Wi-Fi SSID in NVS.
 *
 * Uses the `wifi/ssid` NVS entry and maps storage failures to `-1`.
 */
int Config_WriteToNVS_WifiSSID(char* ssid) {
    int result = NVS_WriteString("wifi", "ssid", ssid);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Stores the Wi-Fi password in NVS.
 *
 * Uses the `wifi/pw` NVS entry and maps storage failures to `-1`.
 */
int Config_WriteToNVS_WifiPassword(char* pw) {
    int result = NVS_WriteString("wifi", "pw", pw);
    if (result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Loads the Wi-Fi SSID from NVS.
 *
 * Reads the `wifi/ssid` NVS entry into the provided buffer.
 */
int Config_LoadFromNVS_WifiSSID(char* ssid) {
    int wifi_ssid_result = NVS_ReadString("wifi", "ssid", ssid, WIFI_MAX_SSID_LEN);
    if (wifi_ssid_result != 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief Loads the Wi-Fi password from NVS.
 *
 * Reads the `wifi/pw` NVS entry into the provided buffer.
 */
int Config_LoadFromNVS_WifiPassword(char* pw) {
    int wifi_password_result = NVS_ReadString("wifi", "pw", pw, WIFI_MAX_PASSWORD_LEN);
    if (wifi_password_result != 0) {
        return -1;
    }
    return -0;
}
