#ifndef __DHT_HPP__
#define __DHT_HPP__

#if CONFIG_DHT11_SENSOR_ENABLED

#include <cJSON.h>
#include <sensor.hpp>

// DHT sensor
#include <dht.h>

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

    static const gpio_num_t dht_gpio = static_cast<gpio_num_t>(DHT_GPIO);
    static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
    // const gpio_num_t dht_gpio = const_cast<gpio_num_t>(DHT_GPIO);
};
}
#endif

#endif
