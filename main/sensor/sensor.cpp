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
#include "../hal/bme280_sensor.hpp"
#include "../hal/bme280_sensor_v2.hpp"

/**
 * @brief Module tag used for ESP-IDF logging.
 */
static constexpr char* TAG = "Sensor.cpp";
static constexpr time_t MIN_VALID_UNIX_TIME = 1704067200; // 2024-01-01 00:00:00 UTC
static bool fake_mode = false;

QueueHandle_t Sensor_Queue = NULL;

/**
 * @brief Initializes sensor state and the sensor update queue.
 *
 * @param[in,out] app Application state to reset before sensor updates begin.
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

bool Sensor_Read_v3(sensor_data_t* sensor, hal::BME280SensorV2& environment_sensor)
{
    hal::EnvironmentReading reading = hal::EnvironmentReading();

    hal::SensorError sensor_reading = environment_sensor.read(reading);

    if (sensor_reading != hal::SensorError::Ok) {
        ESP_LOGW(TAG, "Something went wrong when reading data from sensor."); // TODO - add proper info to output
        sensor->valid = false;
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
 * @brief Reads the BME280 sensor and publishes a snapshot to the queue.
 *
 * Updates the application sensor state only when all sensor reads succeed.
 *
 * @param[in,out] sensor Sensor state to update with the latest measurement.
 * @param[in,out] environment_sensor BME280 sensor wrapper used for hardware access.
 *
 * @return `true` when all readings succeed, otherwise `false`.
 */
bool Sensor_Read_v2(sensor_data_t* sensor, hal::BME280Sensor& environment_sensor)
{
    hal::TemperatureReading temperatur = hal::TemperatureReading();
    hal::HumidityReading humidityReading = hal::HumidityReading();
    hal::PressureReading pressureReading = hal::PressureReading();

    hal::SensorError result = environment_sensor.read(temperatur);
    hal::SensorError humidityResult = environment_sensor.read(humidityReading);
    hal::SensorError pressureResult = environment_sensor.read(pressureReading);

    if (result != hal::SensorError::Ok || humidityResult != hal::SensorError::Ok || pressureResult != hal::SensorError::Ok) {
        ESP_LOGW(TAG, "Something went wrong with reading data from sensor.\nTemperature code: %d, humidity code: %d", static_cast<int>(result), static_cast<int>(humidityResult));
        // Om något inte är okej med error codes för SensorError så hanteras det här.
        // Om något inte är okej, oavsett anledning, så blir sensor->valid false.
        // Då uppdateras inte värderna i strukten.
        // UIn visar då de senaste värderna, med (TODO-fix this) en timestamp och något typ av errornotering, så användaren lätt vet att tex 15:37:02 var senaste OK sensorreaden
        sensor->valid = false;
        // Flyttat ut adderingen av last_reconnect_attempt_ms här, så den bara körs 1 gång om något failar, inte 3 fails per attempt(då vi gör en read på temp, en på humidity och en på pressure)
        environment_sensor.increment_read_failure();
        return false;
    }

    sensor->temperature = temperatur.celcius;
    sensor->humidity = humidityReading.humidity;
    sensor->pressure = pressureReading.pressure;
    // Todo - använda SensorError enum och koppla till valid?
    sensor->valid = true;
    // Todo - Låta read skicka temperatur?
    sensor->last_update_seconds = esp_timer_get_time() / 1000000ULL;
    sensor->last_unix_time = temperatur.unix_timestamp;
    sensor->wall_time_valid = sensor->last_unix_time >= MIN_VALID_UNIX_TIME;

    sensor_data_t sensor_snapshot = *sensor;
    xQueueOverwrite(Sensor_Queue, &sensor_snapshot);

    return true;
}

/**
 * @brief Sensor worker task.
 *
 * Initializes the BME280 wrapper, then periodically reads the sensor and
 * stores the latest snapshot in the shared application state.
 *
 * @param[in] parameter Pointer to the application state passed to the task.
 *
 * @note Runs in task context and blocks with `vTaskDelay()`.
 */
void Sensor_Work(void* parameter) {
    app_state_t* app = (app_state_t*)parameter;
    // Todo - ska default-config värden, om inget existerar i caches, 
    // skapas/initeras här(där respektive modul initieras de värden som är relevanta) eller i main?
    //app->config_data.sensor_interval_ms = 1000; 
    uint32_t sensor_read_interval;

    Sensor_Init_v2(app);
    vTaskDelay(pdMS_TO_TICKS(3000));
    //hal::BME280Sensor environment_sensor = hal::BME280Sensor();
    //environment_sensor.bme280_sensor_init();

    hal::BME280SensorV2 environment_sensor_v2 = hal::BME280SensorV2();
    if (!environment_sensor_v2.bme280_sensor_init()) {
        ESP_LOGW(TAG, "BME280 was unavailable during initial startup; periodic reconnect attempts will continue.");
    }
    
    while (1) {
        sensor_read_interval = app->config_data.sensor_interval_ms;
        //Sensor_Read_v2(&app->sensor_data, environment_sensor);
        Sensor_Read_v3(&app->sensor_data, environment_sensor_v2);
        vTaskDelay(pdMS_TO_TICKS(sensor_read_interval));
    }
}
