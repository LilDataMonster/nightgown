#include <esp_event.h>
#include <esp_sleep.h>
#include <esp_log.h>

// JSON formatting
#include <cJSON.h>

// DHT sensor
#include <dht.h>

// project headers
#include <dht11.hpp>
#include <http.hpp>
#include <json.hpp>
#include <tasks.hpp>
#include <wifi.hpp>

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

	while(1){
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
	LDM::HTTP http((char*)HTTP_POST_ENDPOINT);

    // create JSON message
	LDM::DHT* dht_sensor = (LDM::DHT*)pvParameters;

	wifi.init_sta();
    cJSON *message = buildDHT11Json(dht_sensor->getTemperature(), dht_sensor->getHumidity());

    // POST
	http.postJSON(message);

    // cleanup JSON message
    cJSON_Delete(message);
    message = NULL;

    // vEventGroupDelete(s_wifi_event_group);
    wifi.deinit_sta();
    http.deinit();

    ESP_LOGI(HTTP_TASK_LOG, "Stopping WIFI");
    ESP_LOGI(HTTP_TASK_LOG, "Entering Deep Sleep");
    esp_deep_sleep(30 * 1E6);
}
