#ifndef NVS_H
#define NVS_H

#include <stdio.h>
#include <inttypes.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

//int NVS_Initialize();

void FullNVS();

int NVS_Init();


int NVS_WriteToFile(const char* key, const char* value);

int NVS_LoadFromFile(const char* key, char* value, size_t length);


#endif