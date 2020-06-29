#ifndef __JSON_HPP__
#define __JSON_HPP__

#include <cJSON.h>

#define JSON_TAG "JSON_CLIENT"

cJSON *buildDHT11Json(float temperature, float humidity);

#endif
