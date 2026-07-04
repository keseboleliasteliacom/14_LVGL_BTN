#ifndef ENVIRONMENT_SENSOR_HPP
#define ENVIRONMENT_SENSOR_HPP

#include <stdint.h>
#include "Hal_classes.h"
#include <time.h>


namespace hal {
    struct EnvironmentReading {
        TemperatureReading temperatureR;
        HumidityReading humidityR;
        PressureReading pressureR;
        uint32_t monotonic_timestamp; 
        time_t unix_timestamp; 
    };

    class IEnvironmentSensor{
        public:
            virtual ~IEnvironmentSensor() = default;
            virtual SensorError read(EnvironmentReading& reading) = 0;
            virtual bool is_present() = 0;
    };
}

#endif