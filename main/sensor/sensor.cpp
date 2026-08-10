/**
 * @file sensor.cpp
 * @brief Implementation of the sensor worker module.
 *
 * @ingroup SENSOR
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sensor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "../app_queues.h"
#include "../hal/bme280_sensor_v2.hpp"

/**
 * @brief Module tag used for ESP-IDF logging.
 */
static constexpr char* TAG = "Sensor.cpp";
static constexpr time_t MIN_VALID_UNIX_TIME = 1704067200; // 2024-01-01 00:00:00 UTC
static bool fake_mode = false;

QueueHandle_t Sensor_Queue = NULL;

/**
 * @brief Initializes sensor state and creates the sensor update queue.
 *
 * Resets the cached snapshot before periodic updates begin.
 *
 * @param[in,out] app Application state to reset before sensor updates begin.
 *
 * @note Creates a queue with depth 1 for the latest sensor snapshot.
 */
void Sensor_Init_v2(app_state_t* app) 
{
    app->sensor_data.valid = false;
    app->sensor_data.last_update_seconds = 0;
    app->sensor_data.last_unix_time = 0;
    app->sensor_data.wall_time_valid = false;
    app->sensor_data.temperature = 0;
    app->sensor_data.pressure = 0;
    app->sensor_data.humidity = 0;

    Sensor_Queue = xQueueCreate(1, sizeof(sensor_data_t));

    if (Sensor_Queue == NULL)
    {
        ESP_LOGW(TAG, "Failed to create sensor queue!");
    }

}

/**
 * @brief Reads the BME280 sensor and publishes the latest snapshot.
 *
 * On read failure, marks the snapshot invalid and still overwrites the queue
 * with the current snapshot contents.
 *
 * @param[in,out] sensor Sensor state to update with the latest measurement.
 * @param[in,out] environment_sensor BME280 sensor wrapper used for hardware access.
 *
 * @return `true` when all readings succeed, otherwise `false`.
 *
 * @note Intended for task context because it performs sensor I/O and updates the queue.
 */
bool Sensor_Read_v3(sensor_data_t* sensor, hal::BME280SensorV2& environment_sensor)
{
    hal::EnvironmentReading reading = hal::EnvironmentReading();

    hal::SensorError sensor_reading = environment_sensor.read(reading);

    if (sensor_reading != hal::SensorError::Ok) {
        // static_cast<unsigned>> is needed because SensorError is a scoped c++ enum, therefor explicit convertion is needed.

        ESP_LOGW(TAG, "Something went wrong when reading data from sensor. HAL error %u; publishing invalid snapshot.", static_cast<unsigned>(sensor_reading));
        sensor->valid = false;
        
        sensor_data_t sensor_snapshot = *sensor;
        xQueueOverwrite(Sensor_Queue, &sensor_snapshot);
        return false;
    }
    
    sensor->temperature = reading.temperatureR.celcius;
    sensor->humidity = reading.humidityR.humidity;
    sensor->pressure = reading.pressureR.pressure;

    sensor->valid = true;
    sensor->last_update_seconds = esp_timer_get_time() / 1000000ULL;
    sensor->last_unix_time = reading.unix_timestamp;
    sensor->wall_time_valid = sensor->last_unix_time >= MIN_VALID_UNIX_TIME;

    sensor_data_t sensor_snapshot = *sensor;
    xQueueOverwrite(Sensor_Queue, &sensor_snapshot);

    return true;

}


/**
 * @brief Implementation of Sensor_Work.
 *
 * See header for full contract documentation.
 */
void Sensor_Work(void* parameter) {
    app_state_t* app = (app_state_t*)parameter;
    uint32_t sensor_read_interval;

    Sensor_Init_v2(app);
    vTaskDelay(pdMS_TO_TICKS(3000));

    hal::BME280SensorV2 environment_sensor_v2 = hal::BME280SensorV2();
    if (!environment_sensor_v2.bme280_sensor_init()) {
        ESP_LOGW(TAG, "BME280 was unavailable during initial startup; periodic reconnect attempts will continue.");
    }
    
    while (1) {
        sensor_read_interval = app->config_data.sensor_interval_ms;
        // Aside from just reading from the sensor, we want to save if the read was success or failure to our current system status
        app->system_status.sensor_ok = Sensor_Read_v3(&app->sensor_data, environment_sensor_v2);
        vTaskDelay(pdMS_TO_TICKS(sensor_read_interval));
    }
}
