#ifndef __TASKS_HPP__
#define __TASKS_HPP__

extern "C" {

void sensor_task(void *pvParameters);
void http_task(void *pvParameters);
void sleep_task(void *pvParameters);
void xbee_task(void *pvParameters);

#ifndef CONFIG_IDF_TARGET_ESP32S2
void ble_task(void *pvParameters);
#endif

}
#endif
