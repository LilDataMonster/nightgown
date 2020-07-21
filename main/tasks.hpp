#ifndef __TASKS_HPP__
#define __TASKS_HPP__

#include <esp_log.h>

#define DHT_TAG "DHT_SENSOR"
#define DHT_INFO(fmt, ...)   ESP_LOGI(DHT_TAG, fmt, ##__VA_ARGS__)
#define DHT_ERROR(fmt, ...)  ESP_LOGE(DHT_TAG, fmt, ##__VA_ARGS__)

#define BME680_TAG "BME680_SENSOR"
#define BME680_INFO(fmt, ...)   ESP_LOGI(BME680_TAG, fmt, ##__VA_ARGS__)
#define BME680_ERROR(fmt, ...)  ESP_LOGE(BME680_TAG, fmt, ##__VA_ARGS__)

extern "C" {

void sensor_task(void *pvParameters);
void http_task(void *pvParameters);
void sleep_task(void *pvParameters);

#ifndef CONFIG_IDF_TARGET_ESP32S2
void ble_task(void *pvParameters);
#endif

}
#endif
