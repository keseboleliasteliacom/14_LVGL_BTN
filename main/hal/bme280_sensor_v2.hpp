#ifndef BME280_SENSOR_V2_HPP
#define BME280_SENSOR_V2_HPP

#include "environment_sensor.hpp"
#include "i2c_bus.h"
#include "bme280.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../app_queues.h"

namespace hal {
    class BME280SensorV2 : public IEnvironmentSensor {
        public:
            BME280SensorV2();
            SensorError read(EnvironmentReading& reading) override;
            bool is_present() override;
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

            void BME280Sensor_init_i2c_config();
            bool bme280_init_at_address(uint8_t address);
    };
}

#endif