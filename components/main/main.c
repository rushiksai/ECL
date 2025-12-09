// components/main/main.c  (overwrite or adapt app_main)
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// declare HandleOutput implemented in output_handler.c
void HandleOutput(float x_value, float y_value);

static const char *TAG = "assignment_main";

void app_main(void)
{
    ESP_LOGI(TAG, "Assignment app started (serial-only output).");

    float t = 0.0f;
    const float dt = 0.1f; // step
    while (1) {
        // generate test sine value (simulate model output)
        float x = t;
        float y = sinf(2.0f * 3.14159265f * 1.0f * t); // frequency=1Hz
        // call the output handler
        HandleOutput(x, y);

        t += dt;
        if (t > 10.0f) t = 0.0f;
        vTaskDelay(pdMS_TO_TICKS(200)); // 200 ms between prints
    }
}
