// components/main/output_handler.c
#include <stdio.h>
#include <math.h>
#include "esp_log.h"

static const char *TAG = "output_handler";

/**
 * Minimal output handler for the assignment (serial-only).
 * Prints x_value and y_value and a simple text marker that you can
 * screenshot/grep for the report.
 */
void HandleOutput(float x_value, float y_value)
{
    // Print a clear marker so you can grep it in the serial log
    // Use MicroPrintf style if you use it elsewhere; ESP_LOGI is fine.
    ESP_LOGI(TAG, "HANDLE_OUTPUT: x=%0.6f, y=%0.6f", (double)x_value, (double)y_value);

    // Optional: print status for thresholds to mimic visualization
    if (y_value > 0.5f) {
        ESP_LOGI(TAG, "HANDLE_OUTPUT: STATE=GREEN (y>0.5)");
    } else if (y_value < -0.5f) {
        ESP_LOGI(TAG, "HANDLE_OUTPUT: STATE=RED (y<-0.5)");
    } else {
        ESP_LOGI(TAG, "HANDLE_OUTPUT: STATE=WHITE (-0.5<=y<=0.5)");
    }
}
