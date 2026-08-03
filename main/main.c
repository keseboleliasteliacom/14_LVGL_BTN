/**
 * @file main.c
 * @brief Application entry point for system bring-up and task startup.
 *
 * Initializes board peripherals, LVGL, storage, Wi-Fi, and background tasks.
 * Startup order matters because several modules depend on earlier
 * initialization and on the shared application state.
 *
 * @ingroup main
 *
 * @note Intended as system orchestration rather than reusable API logic.
 * @warning Some initialization steps block briefly during hardware bring-up and
 * task startup.
 */
#define LV_CONF_INCLUDE_SIMPLE 1

#include "rgb_lcd_port.h" // Header for Waveshare RGB LCD driver
#include "gt911.h"        // Header for touch screen operations (GT911)
#include "lvgl_port.h"
#include "lvgl_demo.h"
#include "../ui/ui.h"
#include "freertos/task.h"
#include "WiFi.h"
#include "HTTP.h"
#include "UART/UART.hpp"
#include "sensor/sensor.h"
#include "LEOP/LEOP_Fetcher.h"
#include "Memory/Spiffs.h"
#include "Cache/Cache.h"
#include <stdlib.h>
#include <time.h>
#include "fake/fake_config.hpp"
#include "Memory/NVS.h"
#include "Config/AppConfig.h"


//#define WIFI_PASS "rockyunit953"
//#define WIFI_SSID "NETGEAR49"


static app_state_t app;

// Handlers so we can get stack statistics per task
static TaskHandle_t wifi_task_handle = NULL;
static TaskHandle_t ui_task_handle = NULL;
static TaskHandle_t uart_task_handle = NULL;
static TaskHandle_t sensor_task_handle = NULL;
static TaskHandle_t leop_task_handle = NULL;

#define WIFI_STACK_SIZE     8192
#define UI_STACK_SIZE       16384
#define UART_STACK_SIZE     4096
#define SENSOR_STACK_SIZE   4096
#define LEOP_STACK_SIZE     4096

/**
 * @brief Updates the Wi-Fi connection state in the shared application data.
 *
 * @param connected New connection state.
 * @param ctx Pointer to the application state.
 */
static void on_wifi_connection_changed(bool connected, void *ctx)
{
    app_state_t *app = (app_state_t *)ctx;
    app->system_status.wifi_connected = connected;

    if (leop_task_handle != NULL)
    {
        xTaskNotifyGive(leop_task_handle);
    }
}

/**
 * @brief Mirrors the authoritative LEOP state into shared diagnostics state.
 *
 * @param state Reported LEOP connection state.
 * @param ctx Pointer to the application state.
 */
static void on_leop_connection_changed(leop_connection_state_t state, void *ctx)
{
    app_state_t *app = (app_state_t *)ctx;
    app->system_status.leop_connected =
        (state == LEOP_CONNECTION_CONNECTED ||
         state == LEOP_CONNECTION_DEGRADED);
}



/**
 * @brief Initializes the application task metadata.
 *
 * Populates task names and stack sizes in the shared application state.
 *
 * @param app Pointer to the application state to update.
 */
void init_app_system_task_handlers(app_state_t* app) {
    app->system_task_handlers.wifi_task.name = "WIFI_Work";
    app->system_task_handlers.ui_task.name = "UI_Update";
    app->system_task_handlers.uart_task.name = "UART";
    app->system_task_handlers.sensor_task.name = "Sensor";
    app->system_task_handlers.leop_task.name = "LEOP";

    app->system_task_handlers.wifi_task.stack_size = WIFI_STACK_SIZE;
    app->system_task_handlers.ui_task.stack_size = UI_STACK_SIZE;
    app->system_task_handlers.uart_task.stack_size = UART_STACK_SIZE;
    app->system_task_handlers.sensor_task.stack_size = SENSOR_STACK_SIZE;
    app->system_task_handlers.leop_task.stack_size = LEOP_STACK_SIZE;
}


static const char *TAG = "main";

const char *data =
"{"
"\"stad\": \"Göteborg\","
"\"temperatur\": 12,"
"\"enhet\": \"C\","
"\"väder\": \"molnigt\","
"\"vindstyrka\": 6,"
"\"vind_enhet\": \"m/s\","
"\"luftfuktighet\": 78,"
"\"datum\": \"2026-06-02\""
"}";

/**
 * @brief Application entry point.
 *
 * Initializes time zone settings, loads fallback configuration, brings up
 * peripherals, and starts the worker tasks used by the application.
 *
 * @note Runs in task context and performs several blocking initialization
 * steps before launching background work.
 */
void app_main()
{
    // Time stuff
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();

    NVS_Init();


    // Inits with "fake" config data, should be used as a default fallback if settings from NVS can't be loaded
    //fake_config_data(&app.config_data);
    Config_SetDefaults(&app.config_data);
    Config_LoadFromNVS(&app.config_data);

    ESP_LOGI(TAG, "TEST: Config values.");
    ESP_LOGI(TAG, "fetch_interval_minutes: %lu", app.config_data.fetch_interval_minutes);
    ESP_LOGI(TAG, "test_mode: %d", app.config_data.test_mode);
    ESP_LOGI(TAG, "sensor_interval_ms: %lu", app.config_data.sensor_interval_ms);
    

    // Init the name and stack sizes for our tasks
    init_app_system_task_handlers(&app);    

    static esp_lcd_panel_handle_t panel_handle = NULL; // Declare a handle for the LCD panel
    static esp_lcd_touch_handle_t tp_handle = NULL;
    // Initialize the GT911 touch screen controller
    tp_handle = touch_gt911_init();

    vTaskDelay(pdMS_TO_TICKS(2000));

    // Initialize the Waveshare ESP32-S3 RGB LCD hardware
    panel_handle = waveshare_esp32_s3_rgb_lcd_init();

    // Turn on the LCD backlight
    wavesahre_rgb_lcd_bl_on();

    ESP_ERROR_CHECK(lvgl_port_init(panel_handle, tp_handle)); // Initialize LVGL with the panel and touch handles

    WiFi_Initialize();
    // Setup out callback
    WiFi_SetConnectionCallback(on_wifi_connection_changed, &app);

    Spiffs_Initialize();

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(-1))
    {
        ui_init();
        // Release the mutex
        lvgl_port_unlock();
    }

    xTaskCreate(WiFi_Work, app.system_task_handlers.wifi_task.name, app.system_task_handlers.wifi_task.stack_size, NULL, 5, &wifi_task_handle);

    xTaskCreate(ui_update_task, app.system_task_handlers.ui_task.name, app.system_task_handlers.ui_task.stack_size, &app, 5, &ui_task_handle);

    xTaskCreate(UART_Work, app.system_task_handlers.uart_task.name, app.system_task_handlers.uart_task.stack_size, &app, 4, &uart_task_handle);

    xTaskCreate(Sensor_Work, app.system_task_handlers.sensor_task.name, app.system_task_handlers.sensor_task.stack_size, &app, 4, &sensor_task_handle);


    // Använd appens leop_data istället för en statisk lokal här.
    // TODO - Behöver dock lägga till mutex så småningom efter både UART och LEOP har access till samma resurs
    LEOPFetcher_Initialize(&app.leop_data, 3000);
    LEOPFetcher_SetConnectionCallback(on_leop_connection_changed, &app);
    app.leop_data.leop_conf.time_interval = &app.config_data.fetch_interval_minutes;
    ESP_LOGI(TAG, "Leop data config time interval: %ld", *app.leop_data.leop_conf.time_interval);

    //xTaskCreate(LEOPFetcher_Work, "LEOP", LEOP_STACK_SIZE, &leop_data, 4, NULL);
    xTaskCreate(LEOPFetcher_Work, app.system_task_handlers.leop_task.name, app.system_task_handlers.leop_task.stack_size, &app, 4, &leop_task_handle);
    //  ESP_ERROR_CHECK(WiFi_Dispose());

    // Set the task handles after the tasks has been started, so we actually store info/data instead of NULL
    app.system_task_handlers.wifi_task.handle = wifi_task_handle;
    app.system_task_handlers.ui_task.handle = ui_task_handle;
    app.system_task_handlers.uart_task.handle = uart_task_handle;
    app.system_task_handlers.sensor_task.handle = sensor_task_handle;
    app.system_task_handlers.leop_task.handle = leop_task_handle;
}
