#ifndef TASKS_H
#define TASKS_H

#include <esp_log.h>

#define DHT_TAG "DHT_SENSOR"
#define DHT_INFO(fmt, ...)   ESP_LOGI(DHT_TAG, fmt, ##__VA_ARGS__)
#define DHT_ERROR(fmt, ...)  ESP_LOGE(DHT_TAG, fmt, ##__VA_ARGS__)

#define BME680_TAG "BME680_SENSOR"
#define BME680_INFO(fmt, ...)   ESP_LOGI(BME680_TAG, fmt, ##__VA_ARGS__)
#define BME680_ERROR(fmt, ...)  ESP_LOGE(BME680_TAG, fmt, ##__VA_ARGS__)

extern "C" {

void dht_task(void *pvParameters);
void bme680_task(void *pvParameters);
void http_task(void *pvParameters);

}

#endif
