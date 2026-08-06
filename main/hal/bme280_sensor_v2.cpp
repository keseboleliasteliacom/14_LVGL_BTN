/**
 * @file bme280_sensor_v2.cpp
 * @brief Implementation of the BME280 sensor HAL.
 *
 * @ingroup HAL
 */

#include "bme280_sensor_v2.hpp"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define BME280_SDA_GPIO GPIO_NUM_8
#define BME280_SCL_GPIO GPIO_NUM_9
#define BME280_I2C_PORT I2C_NUM_0
#define BME280_I2C_FREQ_HZ 100000
#define BME280_WAVESHARE_DEFAULT_ADDRESS 0x77

#define BME280_MAX_READ_FAILURES 5
#define BME280_RECONNECT_INTERVAL_MS 5000

static constexpr char* TAG = "bme280_sensor_v2.cpp";

/**
 * @brief Constructs the BME280 sensor wrapper and configures I2C settings.
 */
hal::BME280SensorV2::BME280SensorV2()
{
    BME280Sensor_config_i2c_fallback();
}

/**
 * @brief Reports whether the BME280 sensor is present.
 *
 * @return True when the sensor is initialized and has an active device handle.
 */
bool hal::BME280SensorV2::is_present() {
    return this->bme280_ready && this->bme280 != NULL;
}

/**
 * @brief Returns the current Unix time.
 *
 * @return Current system time as Unix time.
 */
static time_t clock_unix_time() {
    time_t now;
    time(&now);

    return now;
}

/**
 * @brief Logs native shared-bus reachability for the active BME280 address.
 */
void hal::BME280SensorV2::log_i2c_diagnostics() const
{
    if (this->active_i2c_address == 0) {
        ESP_LOGW(TAG, "I2C diagnostic skipped: no active BME280 address");
        return;
    }

    i2c_master_bus_handle_t native_bus = NULL;
    esp_err_t bus_result = i2c_master_get_bus_handle(BME280_I2C_PORT, &native_bus);
    if (bus_result != ESP_OK) {
        ESP_LOGW(TAG, "I2C diagnostic could not obtain bus %d: %s (0x%x)",
                 BME280_I2C_PORT, esp_err_to_name(bus_result), (unsigned int)bus_result);
        return;
    }

    esp_err_t probe_result = i2c_master_probe(native_bus, this->active_i2c_address, 100);
    if (probe_result == ESP_OK) {
        ESP_LOGW(TAG,
                 "I2C diagnostic: address 0x%02X ACKed; the preceding ESP_FAIL occurred during a BME280 read or data validation",
                 this->active_i2c_address);
    } else {
        ESP_LOGW(TAG, "I2C diagnostic: address 0x%02X probe failed: %s (0x%x)",
                 this->active_i2c_address, esp_err_to_name(probe_result),
                 (unsigned int)probe_result);
    }
}

/**
 * @brief Implementation of read.
 *
 * See header for full contract documentation.
 */
hal::SensorError hal::BME280SensorV2::read(hal::EnvironmentReading& reading) {
    if (!this->bme280_ready || this->bme280 == NULL) {
        int64_t now_ms = esp_timer_get_time() / 1000;
        if (now_ms - this->last_reconnect_attempt_ms >= BME280_RECONNECT_INTERVAL_MS) {
            this->last_reconnect_attempt_ms = now_ms;
            ESP_LOGI(TAG, "Trying to reconnect bme280");
            this->bme280_ready = bme280_sensor_init();

            if (this->bme280_ready) {
                ESP_LOGI(TAG, "BME280 reconnected =)");
            }
        }
        // If reconnect is unsuccessful, we return error
        // But if successful, we continue and read 
        if (!this->bme280_ready || this->bme280 == NULL) {
            return hal::SensorError::CommunicationFailure;
        }
    }

    esp_err_t temp_result = bme280_read_temperature(this->bme280, &reading.temperatureR.celcius);
    esp_err_t humidity_result = bme280_read_humidity(this->bme280, &reading.humidityR.humidity);
    esp_err_t pressure_result = bme280_read_pressure(this->bme280, &reading.pressureR.pressure);
    
    if (temp_result != ESP_OK || humidity_result != ESP_OK || pressure_result != ESP_OK) 
    {
        this->bme280_read_failures++;
        ESP_LOGW(TAG, "BME280 read failed %u/%u: temp=%s, humidity=%s, pressure=%s",
                 this->bme280_read_failures, BME280_MAX_READ_FAILURES,
                 esp_err_to_name(temp_result), esp_err_to_name(humidity_result),
                 esp_err_to_name(pressure_result));

        // Probe only on the first consecutive failure. This adds useful native
        // bus diagnostics without adding traffic after every failed reading.
        if (this->bme280_read_failures == 1) {
            this->log_i2c_diagnostics();
        }

        if (this->bme280_read_failures >= BME280_MAX_READ_FAILURES) {
            ESP_LOGW(TAG, "BME280 marked disconnected after repeated read failures.");
            this->bme280_ready = false;
            // Start the reconnect cooldown when the sensor is declared
            // disconnected, rather than from an older reconnect attempt.
            this->last_reconnect_attempt_ms = esp_timer_get_time() / 1000;
            this->bme280_read_failures = 0;
            if (this->bme280 != NULL) {
                bme280_delete(&this->bme280);
            }
        }
        return hal::SensorError::CommunicationFailure;
    }

    this->bme280_read_failures = 0;
    reading.monotonic_timestamp = esp_timer_get_time() / 1000000ULL;
    reading.unix_timestamp = clock_unix_time();
    return hal::SensorError::Ok;
}

/**
 * @brief Initializes the BME280 instance at a specific I2C address.
 *
 * @param[in] address I2C address to probe.
 *
 * @return True on successful initialization, false otherwise.
 */
bool hal::BME280SensorV2::bme280_init_at_address(uint8_t address)
{
    this->bme280 = bme280_create(this->i2c_bus_wrapper, address);
    if (this->bme280 == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create bme280 handle at adress 0x%02X", address);
        return false;
    }

    esp_err_t err = bme280_default_init(this->bme280);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "BME280 init failed at adress 0x%02X", address);
        bme280_delete(&this->bme280);
        return false;
    }

    // If reconnected, wait 150ms so sensor is given enough time to produce first valid smaple
    // Todo - check if this has an effect on reconnect message logging order or sensor status itself 
    // Verified, keep in 
    // When initalized or reconnecting, give the sensor a grace time to load up.
    // Otherwise it tries to read instantly before things are fully ready, and one warning message is delivered even if sensor reads succesfully shortly after.
    vTaskDelay(pdMS_TO_TICKS(150));

    this->active_i2c_address = address;
    ESP_LOGI(TAG, "BME280 init'ed at address 0x%02X", address);
    return true;
}

/**
 * @brief Configures the default I2C parameters used by the BME280 sensor.
 */
void hal::BME280SensorV2::BME280Sensor_config_i2c_fallback()
{
    this->i2c_config.mode = I2C_MODE_MASTER; // ESP I2C buss is master
    this->i2c_config.sda_io_num = BME280_SDA_GPIO; // sets the SDA to the previously defined GPIO pin
    this->i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE; 
    this->i2c_config.scl_io_num = BME280_SCL_GPIO; // sets the SCL to the previously defined GPIO pin
    this->i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    this->i2c_config.master.clk_speed = BME280_I2C_FREQ_HZ; // sets clock speed to previously defined clock speed
}

/**
 * @brief Initializes the BME280 sensor and I2C bus.
 *
 * The first successful address probe keeps the created bus and sensor handle.
 * If the bus already exists, it is reused for reconnect attempts.
 *
 * @return True on success, false if the bus or sensor cannot be initialized.
 */

bool hal::BME280SensorV2::bme280_sensor_init() {
// Obtain the i2c_bus wrapper once.
// In normal application startup, the underlying native ESP-IDF bus already exists because the touch/display stack initialized it.
// espressif/i2c_bus V2 adopts that existing bus.
// Keep the wrapper for later BME280 reconnect attempts.
if (this->i2c_bus_wrapper == NULL) {
        this->i2c_bus_wrapper = i2c_bus_create(BME280_I2C_PORT, &this->i2c_config);
        if (this->i2c_bus_wrapper == NULL) {
            ESP_LOGE(TAG, "Failed to create I2C bus for BME280");
            return false;
        }
    }

    // Waveshare BME280 defaults to 0x77 when ADDR is left unconnected.
    this->bme280_ready = bme280_init_at_address(BME280_WAVESHARE_DEFAULT_ADDRESS);
    if (this->bme280_ready) {
        return true;
    }
    
    // If ADDR is connected to GND, the address is 0x76.
    this->bme280_ready = bme280_init_at_address(BME280_I2C_ADDRESS_DEFAULT);
    
    return this->bme280_ready;

}
