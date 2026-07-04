#ifndef PRESSURE_SENSOR_HPP
#define PRESSURE_SENSOR_HPP

#include <stdint.h>
#include "Hal_classes.h"
#include <time.h>

/**
 * @file pressure_sensor.hpp
 * @brief Public HAL interface for pressure sensor readings.
 *
 * Defines the pressure reading snapshot and the abstract sensor interface used
 * by the HAL layer.
 *
 * @defgroup PRESSURE_SENSOR PRESSURE_SENSOR
 * @brief Pressure sensor HAL interface.
 *
 * Provides a minimal abstraction for retrieving pressure measurements and
 * checking whether the sensor is present.
 * @{
 */

namespace hal {
    /**
     * @brief Snapshot of a pressure measurement.
     *
     * Contains the measured pressure and timestamps captured by the caller's
     * sampling path.
     */
    // struct PressureReading {
    //     float pressure; /**< Measured pressure value. */
    //     uint32_t monotonic_timestamp; /**< Monotonic timestamp for the reading. */
    //     time_t unix_timestamp; /**< UNIX timestamp for the reading. */
    // };

    /**
     * @brief Abstract interface for a pressure sensor implementation.
     */
    class IPressureSensor{
        public:
            
            virtual ~IPressureSensor() = default;
            
            /**
             * @brief Reads the current pressure value into the provided snapshot.
             *
             * @param[out] reading Output reading structure.
             *
             * @return SensorError status code.
             */
            virtual SensorError read(PressureReading& reading) = 0;

            /**
             * @brief Checks whether the sensor is present.
             *
             * @return `true` if the sensor is detected, otherwise `false`.
             */
            virtual bool is_present() = 0;
    };
}

/** @} */

#endif
