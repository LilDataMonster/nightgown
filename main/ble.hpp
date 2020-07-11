#ifndef __BLE_HPP__
#define __BLE_HPP__

#include <esp_err.h>
#include <string>

#include <esp_gap_ble_api.h>

namespace LDM {
class BLE {
public:
	BLE() {};
  esp_err_t init();
  esp_err_t setupCallback();
  esp_err_t updateValue(uint8_t data);

private:
  std::string deviceName;
  void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

  uint8_t raw_adv_data[] = {
    0x02, 0x01, 0x06,
    0x02, 0x0a, 0xeb, 0x03, 0x03, 0xab, 0xcd
  };
  uint8_t raw_scan_rsp_data[] = {
    0x0f, 0x09, 0x45, 0x53, 0x50, 0x5f, 0x47, 0x41, 0x54, 0x54, 0x53, 0x5f, 0x44,
    0x45, 0x4d, 0x4f
  };
  uint8_t adv_config_done = 0;
  esp_ble_adv_params_t adv_params;

};
}
#endif
