#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "../app_types.h"
#include <stdbool.h>
#include "Memory/NVS.h"


void Config_SetDefaults(config_data_t* config);
void Config_LoadFromNVS(config_data_t* config);

#ifdef __cplusplus
extern "C" {
#endif

// Todo - enum or proper error codes?
int Config_WriteToNVS_FetchIntervalMinutes(uint32_t interval);
int Config_WriteToNVS_TestMode(bool mode);
int Config_WriteToNVS_SensorIntervalMs(uint32_t interval);

#ifdef __cplusplus
}
#endif 


#endif