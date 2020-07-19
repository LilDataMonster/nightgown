#ifndef TASKS_H
#define TASKS_H

#include <esp_log.h>

#define DHT_TAG "DHT_SENSOR"
#define DHT_INFO(fmt, ...)   ESP_LOGI(DHT_TAG, fmt, ##__VA_ARGS__)
#define DHT_ERROR(fmt, ...)  ESP_LOGE(DHT_TAG, fmt, ##__VA_ARGS__)

extern "C" {

void dht_task(void *pvParameters);
void http_task(void *pvParameters);

#ifndef CONFIG_IDF_TARGET_ESP32S2
void ble_task(void *pvParameters);
#endif

}

#endif
