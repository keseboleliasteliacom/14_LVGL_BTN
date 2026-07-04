#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "../app_types.h"
#include "Memory/NVS.h"

#define WIFI_MAX_SSID_LEN 33
#define WIFI_MAX_PASSWORD_LEN 65

#ifdef __cplusplus
extern "C" {
#endif

int Config_WriteToNVS_WifiSSID(char* ssid);
int Config_WriteToNVS_WifiPassword(char* pw);
int Config_LoadFromNVS_WifiSSID(char* out);
int Config_LoadFromNVS_WifiPassword(char* out);




#ifdef __cplusplus
}
#endif


#endif