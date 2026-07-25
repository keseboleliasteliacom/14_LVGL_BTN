#ifndef BME280_SENSOR_V2_HPP
#define BME280_SENSOR_V2_HPP

#include "environment_sensor.hpp"
#include "i2c_bus.h"
#include "bme280.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/queue.h"
//#include "../app_queues.h"

/**
 * @file bme280_sensor_v2.hpp
 * @brief Public API for the BME280 sensor HAL.
 *
 * Provides the sensor wrapper used to initialize the I2C bus, probe the
 * device address, and read temperature, humidity, and pressure data.
 *
 * @ingroup HAL
 */

/**
 * @defgroup HAL HAL
 * @brief Hardware abstraction layer modules.
 *
 * Sensor HAL modules own peripheral setup and hardware-specific retry logic.
 * For BME280, initialization depends on the I2C bus and the expected device
 * address on the target board.
 * @{
 */

namespace hal {
    /**
     * @brief BME280-based environment sensor wrapper.
     *
     * Manages BME280 bus setup, address probing, and read retry handling for
     * the environment sensor interface.
     */
    // Uses the shared board I2C bus to manage BME280 device initalization, address probing, measurements and reconnect handling.
    // During normal app startup, the board I2C bus is initialized by the display/touch hardware stack.
    // The expressiv V2 component retrieves and adopts that existing ESP_IDF bus.
    class BME280SensorV2 : public IEnvironmentSensor {
        public:
            /**
             * @brief Constructs the BME280 sensor wrapper.
             *
             * Initializes the I2C configuration used by the sensor instance.
             */
            BME280SensorV2();

            /**
             * @brief Reads the current environment values from the BME280.
             *
             * @param[out] reading Sensor reading output populated on success.
             *
             * @return Sensor error status for the read operation.
             */
            SensorError read(EnvironmentReading& reading) override;

            /**
             * @brief Reports whether the BME280 sensor is present.
             *
             * @return `true` if the sensor is present, `false` otherwise.
             */
            bool is_present() override;

            /**
             * @brief Initializes the BME280 sensor and I2C bus.
             *
             * @return `true` on success, `false` if the bus or sensor cannot be initialized.
             */
            // Retrieves the existing I2C_NUM_0 bus initalized by the board display/touch stack and creates BME280 device handle.
            // If no compatible bus has been initialized, the i2c_bus component may init it using the supplied fallbakc config.

            bool bme280_sensor_init();

        private:
            uint8_t bme280_read_failures = 0;
            int64_t last_reconnect_attempt_ms = 0;

            i2c_bus_handle_t i2c_bus_wrapper = NULL;
            bme280_handle_t bme280 = NULL;
            bool bme280_ready = false;
            uint8_t active_i2c_address = 0;

            i2c_config_t i2c_config;

            /**
             * @brief Configures the default I2C parameters used by the sensor.
             */
            void BME280Sensor_config_i2c_fallback();

            /**
             * @brief Initializes the BME280 instance at a specific I2C address.
             *
             * @param[in] address I2C address to probe.
             *
             * @return `true` on success, `false` otherwise.
             */
            bool bme280_init_at_address(uint8_t address);

            /**
             * @brief Probes the active address on the native shared I2C bus.
             *
             * The BME280 component maps several lower-level failures to
             * `ESP_FAIL`. This diagnostic probe preserves the native ESP-IDF
             * result so logs can distinguish an ACK, NACK/not-found result,
             * bus timeout, or failure to obtain the shared bus handle.
             */
            void log_i2c_diagnostics() const;
    };
}

/** @} */

#endif
