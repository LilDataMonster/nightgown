#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_pm.h>
#include <esp_sleep.h>
#include <nvs_flash.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <dht11.hpp>
//#include <http.hpp>

#include <tasks.hpp>

extern "C" {

static RTC_DATA_ATTR struct timeval sleep_enter_time;
static LDM::DHT dht_sensor;

void app_main(void);

}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER: {
            printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
            break;
        }
      	case ESP_SLEEP_WAKEUP_ULP: {
        		printf("ESP_SLEEP_WAKEUP_ULP\n");
        		break;
      	}
        case ESP_SLEEP_WAKEUP_UNDEFINED: {
        		printf("Wakeup was not caused by deep sleep\n");
            break;
        }
        default:
            printf("Not a deep sleep reset\n");
    }

// #if CONFIG_PM_ENABLE
//     // Configure dynamic frequency scaling:
//     // maximum and minimum frequencies are set in sdkconfig,
//     // automatic light sleep is enabled if tickless idle support is enabled.
// #if CONFIG_IDF_TARGET_ESP32
//     esp_pm_config_esp32_t pm_config = {
// #elif CONFIG_IDF_TARGET_ESP32S2
//     esp_pm_config_esp32s2_t pm_config = {
// #endif
//             .max_freq_mhz = CONFIG_EXAMPLE_MAX_CPU_FREQ_MHZ,
//             .min_freq_mhz = CONFIG_EXAMPLE_MIN_CPU_FREQ_MHZ,
// #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
//             .light_sleep_enable = true
// #endif
//     };
//     ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
// #endif // CONFIG_PM_ENABLE


    xTaskCreate(dht_task, "dht_task", configMINIMAL_STACK_SIZE * 3, (void*)&dht_sensor, 5, NULL);
    xTaskCreate(http_task, "http_task", 8192, (void*)&dht_sensor, 5, NULL);
    xTaskCreate(ble_task, "ble_task", 8192, (void*)&dht_sensor, 5, NULL);
}
