#include "esp_log.h"
#include "sensor.h"

static const char* TAG = "sensor";

void sensor_init(void)
{
    ESP_LOGI(TAG, "sensor_init placeholder");
}

int sensor_read(void)
{
    ESP_LOGI(TAG, "sensor_read placeholder -> returning 42");
    return 42;
}
