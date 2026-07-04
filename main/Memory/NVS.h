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

//int NVS_Initialize();

void FullNVS();

int NVS_Init();


int NVS_WriteToFile(const char* key, const char* value);

int NVS_LoadFromFile(const char* key, char* value, size_t length);

int NVS_ReadString(const char* nvs_namespace, const char* key, char* buffer, size_t size);
int NVS_WriteString(const char* nvs_namespace, const char* key, const char* value);
int NVS_ReadU32(const char* nvs_namespace, const char* key, uint32_t* out_value);
int NVS_WriteU32(const char* nvs_namespace, const char* key, uint32_t value);
int NVS_ReadBool(const char* nvs_namespace, const char* key, bool* out_value);
int NVS_WriteBool(const char* nvs_namespace, const char* key, bool value);

#ifdef __cplusplus
}
#endif

#endif
