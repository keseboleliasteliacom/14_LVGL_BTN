#ifndef BME280_SENSOR_V2_HPP
#define BME280_SENSOR_V2_HPP

#include "environment_sensor.hpp"
#include "i2c_bus.h"
#include "bme280.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../app_queues.h"

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
            bool bme280_sensor_init();

        private:
            uint8_t bme280_read_failures = 0;
            int64_t last_reconnect_attempt_ms = 0;

            i2c_bus_handle_t bme280_bus = NULL;
            bme280_handle_t bme280 = NULL;
            bool bme280_ready = false;

            i2c_config_t i2c_config;
            i2c_port_t i2c_port;
            uint8_t adress;

            /**
             * @brief Configures the default I2C parameters used by the sensor.
             */
            void BME280Sensor_init_i2c_config();

            /**
             * @brief Initializes the BME280 instance at a specific I2C address.
             *
             * @param[in] address I2C address to probe.
             *
             * @return `true` on success, `false` otherwise.
             */
            bool bme280_init_at_address(uint8_t address);
    };
}

/** @} */

#endif
