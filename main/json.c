#include <stdint.h>
#include <cJSON.h>

// project header
#include <json.h>

cJSON *buildDHT11Json(int16_t temperature, int16_t humidity){
  cJSON *root;

  root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "temperature", temperature);
  cJSON_AddNumberToObject(root, "humidity", humidity);

  return root;
}
