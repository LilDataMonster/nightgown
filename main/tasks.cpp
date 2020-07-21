#include <esp_event.h>
#include <esp_sleep.h>
#include <esp_log.h>
#include <esp_system.h>
// JSON formatting
#include <cJSON.h>

// DHT sensor
#include <dht.h>

//BME680 sensor
#include <bme680.h>

// project headers
#include <dht11.hpp>
#include <bme680.hpp>
#include <http.hpp>
#include <json.hpp>
#include <tasks.hpp>
#include <wifi.hpp>
#include <ble.hpp>

#define SLEEP_DURATION CONFIG_SLEEP_DURATION
#define BLE_ADVERTISE_DURATION CONFIG_BLE_ADVERTISE_DURATION
int dht_en, bme680_en = 0;
#if CONFIG_DHT11_SENSOR_ENABLED
    dht11_en = 1;
#elif CONFIG_DHT11_SENSOR_DISABLED
    dht11_en = 0;
#endif

#if CONFIG_BME680_SENSOR_ENABLED
    bme680_en = 1;
#elif CONFIG_BME680_SENSOR_DISABLED
    bme680_en = 0;
#endif
// sleep task will go to sleep when messageFinished is true
static bool messageFinished = false;

#define DHT_GPIO CONFIG_ESP_DHT11_GPIO
static const gpio_num_t dht_gpio = static_cast<gpio_num_t>(DHT_GPIO);
static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;

void dht_task(void *pvParameters) {
    // DHT sensors that come mounted on a PCB generally have
    // pull-up resistors on the data pin.  It is recommended
    // to provide an external pull-up resistor otherwise...
    gpio_set_pull_mode(dht_gpio, GPIO_PULLUP_ONLY);

    int16_t temperature, humidity;
    LDM::DHT* dht_sensor = (LDM::DHT*)pvParameters;

    while(true){
        if(dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
            dht_sensor->setHumidity(humidity / 10.f);
            dht_sensor->setTemperature(temperature / 10.f);
            DHT_INFO("Humidity: %.2f, Temp: %.2fC", dht_sensor->getHumidity(), dht_sensor->getTemperature());
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

#define I2C_SCL CONFIG_I2C_SCL
static const gpio_num_t scl_gpio = static_cast<gpio_num_t>(I2C_SCL);
#define I2C_SDA CONFIG_I2C_SDA
static const gpio_num_t sda_gpio = static_cast<gpio_num_t>(I2C_SDA);
#define I2C_PORT CONFIG_I2C_PORT
static const gpio_num_t i2c_port_num = static_cast<gpio_num_t>(I2C_PORT);
#define ADDR BME680_I2C_ADDR_1

/*
#define SDA_GPIO 18
#define SCL_GPIO 19
#define PORT 0
#define ADDR BME680_I2C_ADDR_1
*/
void bme680_task(void *pvParameters) {
    bme680_t sensor;
    memset(&sensor, 0, sizeof(bme680_t));

    ESP_ERROR_CHECK(bme680_init_desc(&sensor, ADDR, i2c_port_num, sda_gpio, scl_gpio));
    //ESP_ERROR_CHECK(bme680_init_desc(&sensor, (gpio_num_t)ADDR, (gpio_num_t)PORT, (gpio_num_t)SDA_GPIO, (gpio_num_t)SCL_GPIO));

    // init the sensor
    ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

    // Changes the oversampling rates to 4x oversampling for temperature
    // and 2x oversampling for humidity. Pressure measurement is skipped.
    bme680_set_oversampling_rates(&sensor, BME680_OSR_4X, BME680_OSR_4X, BME680_OSR_4X);

    // Change the IIR filter size for temperature and pressure to 7.
    bme680_set_filter_size(&sensor, BME680_IIR_SIZE_3);

    // Change the heater profile 0 to 200 degree Celcius for 100 ms.
    bme680_set_heater_profile(&sensor, 0, 200, 100);
    bme680_use_heater_profile(&sensor, 0);

    // Set ambient temperature to 10 degree Celsius
    bme680_set_ambient_temperature(&sensor, 10);

    // as long as sensor configuration isn't changed, duration is constant
    uint32_t duration;
    bme680_get_measurement_duration(&sensor, &duration);

    TickType_t last_wakeup = xTaskGetTickCount();

    bme680_values_float_t values;
    
	LDM::BME680* bme680_sensor = (LDM::BME680*)pvParameters;
    while (1)
    {
        // trigger the sensor to start one TPHG measurement cycle
        if (bme680_force_measurement(&sensor) == ESP_OK)
        {
            // passive waiting until measurement results are available
            vTaskDelay(duration);

            // get the results and do something with them
            if (bme680_get_results_float(&sensor, &values) == ESP_OK) {
              BME680_INFO("BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f Ohm\n",
                        values.temperature, values.humidity, values.pressure, values.gas_resistance);
              bme680_sensor->setHumidity(values.humidity);
			        bme680_sensor->setTemperature(values.temperature);
              bme680_sensor->setPressure(values.pressure);
              bme680_sensor->setGas(values.gas_resistance);
              BME680_INFO("Humidity: %.2f, Temp: %.2fC, %.2f hPa, %.2f Ohm\n", 
                  bme680_sensor->getHumidity(), bme680_sensor->getTemperature(), bme680_sensor->getPressure(), bme680_sensor->getGas());
            } else {
              BME680_INFO("Could not read data from BME680 sensor");
            }
        }
	              // passive waiting until 1 second is over
                //vTaskDelay(pdMS_TO_TICKS(10000));
        vTaskDelayUntil(&last_wakeup, 1000 / portTICK_PERIOD_MS);
    }


}

#define HTTP_POST_ENDPOINT CONFIG_ESP_POST_ENDPOINT
#define HTTP_TASK_LOG "HTTP_TASK"
//#define GPIO_OUTPUT_PIN 13
//#define GPIO_OUTPUT_PIN_SEL  (1ULL << GPIO_OUTPUT_PIN)

void http_task(void *pvParameters) {
    // gpio_config_t io_conf;
    // //disable interrupt
    // io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // //set as output mode
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // //bit mask of the pins that you want to set,e.g.GPIO18/19
    // io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // //disable pull-down mode
    // io_conf.pull_down_en = 0;
    // //disable pull-up mode
    // io_conf.pull_up_en = 0;
    // //configure GPIO with the given settings
    // gpio_config(&io_conf);
    //
    // gpio_set_level(GPIO_OUTPUT_PIN, 1);

    // setup wifi and http client
    LDM::WiFi wifi;
    LDM::HTTP http(const_cast<char*>(HTTP_POST_ENDPOINT));

    // create JSON message
    if(dht_en)
        LDM::DHT* dht_sensor = (LDM::DHT*)pvParameters;
    if(bme680_en)
        LDM::BME680* bme680_sensor = (LDM::BME680*)pvParameters;
    wifi.init_sta();
    
    if(dht_en)
        cJSON *message = buildDHT11Json(dht_sensor->getTemperature(), dht_sensor->getHumidity());
    if(bme680_en)
        cJSON *message = buildBME680Json(bme680_sensor->getTemperature(), bme680_sensor->getHumidity(), bme680_sensor->getPressure(), bme680_sensor->getGas());

    // POST
    http.postJSON(message);

    // cleanup JSON message
    cJSON_Delete(message);
    message = NULL;

    // vEventGroupDelete(s_wifi_event_group);
    wifi.deinit_sta();
    http.deinit();

    messageFinished = true;
    vTaskDelete(NULL);
}

#define SLEEP_TASK_LOG "SLEEP_TASK"
void sleep_task(void *pvParameters) {
    while(true) {
        if(messageFinished) {
            ESP_LOGI(SLEEP_TASK_LOG, "Entering Deep Sleep");
            esp_deep_sleep(SLEEP_DURATION * 1E6);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#ifndef CONFIG_IDF_TARGET_ESP32S2
#define BLE_TASK_LOG "BLE_TASK"
void ble_task(void *pvParameters) {
    ESP_LOGI(BLE_TASK_LOG, "Starting BLE");

    LDM::BLE ble("Nightgown");
    ble.init();
    ble.setupCallback();

    // get sensor data
    if(dht_en)
        LDM::DHT* dht_sensor = (LDM::DHT*)pvParameters;
        uint8_t humidity = dht_sensor->getHumidity();
        uint8_t temperature = dht_sensor->getTemperature();
        ESP_LOGI(BLE_TASK_LOG, "Updating humidity: %d, temperature: %d", humidity, temperature);
        ble.updateValueDHT(humidity, temperature);

    if(bme680_en)
        LDM::BME680* bme680_sensor = (LDM::BME680*)pvParameters;
        uint8_t humidity = bme680_sensor->getHumidity();
        uint8_t temperature = bme680_sensor->getTemperature();
        uint8_t pressure = bme680_sensor->getPressure();
        uint8_t gas = bme680_sensor->getGas();
        ESP_LOGI(BLE_TASK_LOG, "Humidity: %.2f, Temp: %.2fC, %.2f hPa, %.2f Ohm\n", 
                  humidity, temperature, pressure, gas);
        ble.updateValueBME(humidity, temperature, pressure, gas);


    // advertise BLE data for a while
    vTaskDelay(pdMS_TO_TICKS(BLE_ADVERTISE_DURATION * 1E3));
    ble.deinit();

    messageFinished = true;
    vTaskDelete(NULL);
}
#endif
