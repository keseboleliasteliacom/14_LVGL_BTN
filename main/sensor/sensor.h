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

// sensor.h needs to have proper C naming during linking.
// So sensor.cpp includes this, but Extern C tells compiler to use C linkage instead of C++ linkage
#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief Sensor worker task entry point.
 *
 * @param parameter Pointer to the application state passed to the task.
 */
void Sensor_Work(void* parameter);

#ifdef __cplusplus
    }
#endif


#endif
