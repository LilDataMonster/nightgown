#include <esp_log.h>
#include <esp_event.h>

// DHT sensor
#include <dht.h>

// project headers
#include <dht11.h>

#define DHT_GPIO CONFIG_ESP_DHT11_GPIO

static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const gpio_num_t dht_gpio = DHT_GPIO;

void dht_task(void *pvParameters) {
    // DHT sensors that come mounted on a PCB generally have
    // pull-up resistors on the data pin.  It is recommended
    // to provide an external pull-up resistor otherwise...

    gpio_set_pull_mode(dht_gpio, GPIO_PULLUP_ONLY);

    while (1)
    {
        //if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
        if(dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
            DHT_INFO("Humidity: %d, Temp: %dC", humidity/10, temperature/10);
        }
        else {
            DHT_INFO("Could not read data from sensor");
        }

        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        // vTaskDelay(2000 / portTICK_PERIOD_MS);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
