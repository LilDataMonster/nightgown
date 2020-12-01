#include <esp_event.h>
#include <esp_sleep.h>
#include <esp_log.h>
#include <esp_system.h>

// JSON formattingRX_BUF_SIZE
#include <sensor.hpp>
#include <http.hpp>
#include <tasks.hpp>
#include <wifi.hpp>
#include <ble.hpp>
#include <ota.hpp>
#include <driver/uart.h>
#include <driver/gpio.h>

#define SLEEP_DURATION CONFIG_SLEEP_DURATION
#define BLE_ADVERTISE_DURATION CONFIG_BLE_ADVERTISE_DURATION

// sleep task will go to sleep when messageFinished is true
static bool messageFinished = false;

#define SENSOR_TASK_LOG "SENSOR_TASK"
void sensor_task(void *pvParameters) {

    if(pvParameters == NULL) {
        ESP_LOGE(SENSOR_TASK_LOG, "Invalid Sensor Recieved");
        return;
    }

    LDM::Sensor *sensor = (LDM::Sensor*)pvParameters;

    // initialize sensor
    if(!sensor->init()) {
        ESP_LOGE(SENSOR_TASK_LOG, "Failed to initialize sensor");
        return;
    }

    while(true){
        sensor->readSensor();

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
#define FIRMWARE_UPGRADE_ENDPOINT CONFIG_FIRMWARE_UPGRADE_ENDPOINT
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

    if(pvParameters == NULL) {
        ESP_LOGE(HTTP_TASK_LOG, "Invalid Sensor Recieved");
        return;
    }

    // setup wifi and http client
    LDM::WiFi wifi;
    LDM::HTTP http(const_cast<char*>(HTTP_POST_ENDPOINT));

#ifdef CONFIG_OTA_ENABLED
    // setup ota updater and checkUpdates
    LDM::OTA ota(const_cast<char*>(FIRMWARE_UPGRADE_ENDPOINT));
#endif

    wifi.init_sta();

    // create JSON message
    LDM::Sensor *sensor = (LDM::Sensor*)pvParameters;
    cJSON *message = sensor->buildJson();

    // POST
    http.postJSON(message);

#ifdef CONFIG_OTA_ENABLED
    // check OTA updates
    ota.checkUpdates(true);
#endif

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
/*
    ble.setupCallback();

#if CONFIG_DHT11_SENSOR_ENABLED
    // get sensor data
    //LDM::DHT* dht_sensor = (LDM::DHT*)pvParameters;
    LDM::DHT* p_dht_sensor = &dht_sensor;

    uint8_t humidity = p_dht_sensor->getHumidity();
    uint8_t temperature = p_dht_sensor->getTemperature();
    ESP_LOGI(BLE_TASK_LOG, "Updating humidity: %d, temperature: %d", humidity, temperature);
    ble.updateValue(humidity, temperature);
#endif
*/

    // advertise BLE data for a while
    vTaskDelay(pdMS_TO_TICKS(BLE_ADVERTISE_DURATION * 1E3));
    ble.deinit();
    messageFinished = true;
    vTaskDelete(NULL);
}
#endif


#define XBEE_TASK_LOG "XBEE_TASK"
const int RX_BUF_SIZE = 1024;
#ifndef CONFIG_IDF_TARGET_ESP32S2
#define TXD_PIN (GPIO_NUM_23)
#define RXD_PIN (GPIO_NUM_22)
void xbee_task(void *pvParameters) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 4, RX_BUF_SIZE * 4, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    LDM::Sensor *sensor = (LDM::Sensor*)pvParameters;
    cJSON *message = sensor->buildJson();
    char *post_data = cJSON_Print(message);
    const char *TX_TASK_TAG = "TX_TASK";
    const int len = strlen(post_data);
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    const int txBytes = uart_write_bytes(UART_NUM_1, post_data, len);
    ESP_LOGI(TX_TASK_TAG, "Wrote %d bytes", txBytes);
    printf("%s\n", post_data);
    cJSON_Delete(message);
    message = NULL;
    vTaskDelete(NULL);
}
#endif