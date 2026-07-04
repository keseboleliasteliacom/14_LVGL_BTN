#include "WifiConfig.h"

static const char* TAG = "WifiConfig";

//#define NVS_MAX_CHARACTER_LIMIT 15


int Config_WriteToNVS_WifiSSID(char* ssid) {
    int result = NVS_WriteString("wifi", "ssid", ssid);
    if (result != 0) {
        return -1;
    }
    return 0;
}

int Config_WriteToNVS_WifiPassword(char* pw) {
    int result = NVS_WriteString("wifi", "pw", pw);
    if (result != 0) {
        return -1;
    }
    return 0;
}

int Config_LoadFromNVS_WifiSSID(char* ssid) {
    //char* wifi_id[15];
    int wifi_ssid_result = NVS_ReadString("wifi", "ssid", ssid, WIFI_MAX_SSID_LEN);
    if (wifi_ssid_result != 0) {
        return -1;
    }
    return 0;
}

int Config_LoadFromNVS_WifiPassword(char* pw) {
    int wifi_password_result = NVS_ReadString("wifi", "pw", pw, WIFI_MAX_PASSWORD_LEN);
    if (wifi_password_result != 0) {
        return -1;
    }
    return -0;
}