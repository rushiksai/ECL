// components/main/main.c
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "assignment";

/* External output handler implemented in components/main/output_handler.c */
void HandleOutput(float x_value, float y_value);

void app_main(void)
{
    // Give system some time to finish other initializations (LVGL, display, etc.)
    vTaskDelay(pdMS_TO_TICKS(2000));

    int blink_pin = CONFIG_BLINK_GPIO;
    ESP_LOGI(TAG, "Blink pin from menuconfig: %d", blink_pin);

    gpio_reset_pin(blink_pin);
    gpio_set_direction(blink_pin, GPIO_MODE_OUTPUT);

    int cnt = 0;
    while (1) {
        // Blink LED as before
        gpio_set_level(blink_pin, cnt % 2);

        // Generate test sine wave input
        float angle_deg = (float)(cnt % 360);
        float x = angle_deg * (3.14159265f / 180.0f); // convert to radians
        float y = sinf(x); // y in [-1, 1]

        // Call the LVGL output handler to visualize the y value
        HandleOutput(x, y);

        // Log for serial monitor (useful for report)
        ESP_LOGI(TAG, "Counter: %d, x: %.3f, y: %.4f", cnt, x, y);

        cnt++;
        // wait 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
