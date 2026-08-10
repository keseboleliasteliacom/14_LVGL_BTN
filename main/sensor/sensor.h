#ifndef SENSOR_HPP
#define SENSOR_HPP

#include "../app_types.h"

/**
 * @file sensor.h
 * @brief Public interface for the sensor worker task.
 *
 * Declares the task entry point used by the sensor module to initialize
 * hardware access and publish updates into the shared application state.
 *
 * @ingroup SENSOR
 */

/**
 * @defgroup SENSOR SENSOR
 * @brief Sensor worker module.
 *
 * Initializes the BME280 wrapper, reads environmental data in task context,
 * and publishes the latest snapshot to shared application state.
 *
 * @{
 */

// sensor.h needs to have proper C naming during linking.
// So sensor.cpp includes this, but Extern C tells compiler to use C linkage instead of C++ linkage
#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief Sensor worker task entry point.
 *
 * Initializes sensor state, waits for the startup delay, and then periodically
 * reads the sensor before updating the shared application state.
 *
 * @param parameter Pointer to the application state passed to the task.
 *
 * @note Runs in task context and blocks with `vTaskDelay()`.
 */
void Sensor_Work(void* parameter);

#ifdef __cplusplus
    }
#endif

/** @} */


#endif
