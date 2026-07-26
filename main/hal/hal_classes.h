/**
 * @file hal_classes.h
 * @brief Shared HAL types used by the hardware abstraction layer.
 *
 * Defines lightweight common types for HAL modules.
 *
 * @defgroup HAL_CLASSES HAL_CLASSES
 * @brief Shared types for HAL modules.
 *
 * Provides common enums and helper types used across HAL code.
 * @{
 */

#ifndef HAL_CLASSES_H
#define HAL_CLASSES_H

#include "time.h"

namespace hal {
    /**
     * @brief Error codes reported by sensor-oriented HAL operations.
     */
    enum class SensorError : uint8_t {
        Ok = 0,
        CommunicationFailure,
        DeviceNotFound,
        InvalidReading,
        Timeout
    };
    
    /**
     * @brief Temperature sample with monotonic and UNIX timestamps.
     */
    struct TemperatureReading {
        float celcius; /**< Temperature value in degrees Celsius. */
        uint32_t monotonic_timestamp; /**< Monotonic timestamp associated with the reading. */
        time_t unix_timestamp; /**< UNIX timestamp associated with the reading. */
    };


    /**
     * @brief Humidity sample with monotonic and UNIX timestamps.
     */
    struct HumidityReading {
        float humidity; /**< Humidity value in project-defined units. */
        uint32_t monotonic_timestamp; /**< Monotonic timestamp associated with the reading. */
        time_t unix_timestamp; /**< Unix timestamp associated with the reading. */
    };

    /**
     * @brief Pressure sample with monotonic and UNIX timestamps.
     */
    struct PressureReading {
        float pressure; /**< Measured pressure value. */
        uint32_t monotonic_timestamp; /**< Monotonic timestamp for the reading. */
        time_t unix_timestamp; /**< UNIX timestamp for the reading. */
    };
};


#endif

/** @} */
