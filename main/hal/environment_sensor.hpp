#ifndef ENVIRONMENT_SENSOR_HPP
#define ENVIRONMENT_SENSOR_HPP

#include <stdint.h>
#include "hal_classes.h"
#include <time.h>

/**
 * @file environment_sensor.hpp
 * @brief Public interface for the environment sensor abstraction.
 *
 * Defines the environment reading snapshot and the sensor interface used by
 * the HAL layer to obtain temperature, humidity, and pressure data.
 *
 * @defgroup ENVIRONMENT_SENSOR ENVIRONMENT_SENSOR
 * @brief Environment sensor data and interface definitions.
 *
 * This module provides the shared reading container and the abstract sensor
 * interface used by platform-specific implementations.
 * @{
 */

namespace hal {
    /**
     * @brief Snapshot of a single environment sensor sample.
     *
     * Contains the latest temperature, humidity, and pressure readings along
     * with monotonic and Unix timestamps for the sample.
     */
    struct EnvironmentReading {
        TemperatureReading temperatureR; /**< Temperature reading. */
        HumidityReading humidityR; /**< Humidity reading. */
        PressureReading pressureR; /**< Pressure reading. */
        uint32_t monotonic_timestamp; /**< Monotonic timestamp for the sample. */
        time_t unix_timestamp; /**< Unix timestamp for the sample. */
    };

    /**
     * @brief Abstract interface for environment sensor drivers.
     *
     * Implementations read the current sensor state into an EnvironmentReading
     * snapshot and report whether the sensor is present.
     */
    class IEnvironmentSensor{
        public:
            virtual ~IEnvironmentSensor() = default;

            /**
             * @brief Reads the current environment sample.
             *
             * @param[out] reading Output reading snapshot.
             *
             * @return SensorError value reported by the implementation.
             */
            virtual SensorError read(EnvironmentReading& reading) = 0;

            /**
             * @brief Checks whether the sensor is present.
             *
             * @return true when the sensor is available; otherwise false.
             */
            virtual bool is_present() = 0;
    };
}

/** @} */

#endif
