#include <stdint.h>
#include <cJSON.h>

// project headers
#include <json.hpp>

cJSON *buildDHT11Json(float temperature, float humidity){

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperature", temperature);
    cJSON_AddNumberToObject(root, "humidity", humidity);

    return root;
}
