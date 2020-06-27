#ifndef JSON_H
#define JSON_H

#include <cJSON.h>

#define JSON_TAG "JSON_CLIENT"

cJSON *buildDHT11Json(int16_t temperature, int16_t humidity);

#endif
