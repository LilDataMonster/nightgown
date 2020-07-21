#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <cJSON.h>

namespace LDM {
class Sensor {
public:
    virtual bool init() = 0;
    virtual bool deinit() = 0;
    virtual bool readSensor() = 0;
    virtual cJSON *buildJson() = 0;

// private:
//     float temperature;
//     float humidity;
};
}

#endif
