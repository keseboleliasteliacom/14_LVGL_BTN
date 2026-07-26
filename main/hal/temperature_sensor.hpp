#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP

#include <stdint.h>
#include "hal_classes.h"
#include <time.h>


/**
 * @file temperature_sensor.hpp
 * @brief Public HAL interface for temperature sensor readings.
 *
 * Defines the temperature sensor reading snapshot and the abstract sensor
 * interface used by the HAL layer.
 *
 * @defgroup HAL_TEMPERATURE_SENSOR HAL_TEMPERATURE_SENSOR
 * @brief Temperature sensor abstraction for the HAL layer.
 *
 * Provides a small interface for obtaining temperature readings and checking
 * whether the sensor is present.
 * @{
 */

namespace hal {
    /**
     * @brief Snapshot of a temperature sensor reading.
     *
     * Contains the measured temperature and both timestamp forms used by the
     * HAL layer.
     */
    // struct TemperatureReading {
    //     float celcius; /**< Temperature value in degrees Celsius. */
    //     uint32_t monotonic_timestamp; /**< Monotonic timestamp associated with the reading. */
    //     time_t unix_timestamp; /**< UNIX timestamp associated with the reading. */
    // };

    /**
     * @brief Abstract temperature sensor interface.
     *
     * Implementations provide reading acquisition and presence detection.
     */
    class ITemperatureSensor{
        public:
            virtual ~ITemperatureSensor() = default;
            virtual SensorError read(TemperatureReading& reading) = 0;
            virtual bool is_present() = 0;
    };
}

/** @} */

#endif
