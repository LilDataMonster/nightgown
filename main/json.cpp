#include "json.hpp"

#include <stdint.h>
#include <cJSON.h>

// project header

cJSON *buildDHT11Json(float temperature, float humidity){
  cJSON *root;

  root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "temperature", temperature);
  cJSON_AddNumberToObject(root, "humidity", humidity);

  return root;
}
