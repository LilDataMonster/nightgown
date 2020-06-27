#ifndef DHT11_H
#define DHT11_H

#define DHT_TAG "DHT_SENSOR"
#define DHT_INFO(fmt, ...)   ESP_LOGI(DHT_TAG, fmt, ##__VA_ARGS__)
#define DHT_ERROR(fmt, ...)  ESP_LOGE(DHT_TAG, fmt, ##__VA_ARGS__)

int16_t temperature;
int16_t humidity;

void dht_task(void *pvParameters);

#endif
