#ifndef __DHT_HPP__
#define __DHT_HPP__

#include <cJSON.h>
#include <sensor.hpp>

#if CONFIG_DHT11_SENSOR_ENABLED

#define DHT_GPIO CONFIG_ESP_DHT11_GPIO

namespace LDM {
class DHT : public Sensor {
public:
    DHT();
    float getHumidity(void);
    float getTemperature(void);
    void setHumidity(float humidity);
    void setTemperature(float temperature);

    bool init(void);
    bool deinit(void);
    bool readSensor(void);
    cJSON *buildJson(void);

private:
    float temperature;
    float humidity;

    bme680_t sensor;
    uint32_t duration;
    const gpio_num_t dht_gpio = DHT_GPIO;
    const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
};
}
#endif

#endif
