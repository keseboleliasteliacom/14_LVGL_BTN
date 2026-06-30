#ifndef HUMIDITY_SENSOR_HPP
#define HUMIDITY_SENSOR_HPP

#include <stdint.h>
#include "hal_classes.h"
#include <time.h>

/**
 * @file humidity_sensor.hpp
 * @brief Humidity sensor HAL interface and reading types.
 *
 * Defines the abstract humidity sensor contract used by the HAL layer and the
 * reading structure returned by implementations.
 *
 * @ingroup HAL
 */

namespace hal {
    /**
     * @brief Humidity sensor reading snapshot.
     *
     * Contains the measured humidity value and timestamps captured by the
     * implementation.
     */
    // struct HumidityReading {
    //     float humidity; /**< Humidity value in project-defined units. */
    //     uint32_t monotonic_timestamp; /**< Monotonic timestamp associated with the reading. */
    //     time_t unix_timestamp; /**< Unix timestamp associated with the reading. */
    // };

    /**
     * @brief Abstract interface for humidity sensor implementations.
     *
     * Implementations provide presence detection and reading acquisition through
     * the HAL layer.
     */
    class IHumiditySensor{
        public:
            virtual ~IHumiditySensor() = default;
            /**
             * @brief Reads the current humidity value.
             *
             * @param[out] reading Destination for the sensor reading.
             *
             * @return Sensor-specific error code.
             */
            virtual SensorError read(HumidityReading& reading) = 0;
            /**
             * @brief Checks whether the sensor is present.
             *
             * @return `true` if the sensor is present, otherwise `false`.
             */
            virtual bool is_present() = 0;        
    };
}

#endif
