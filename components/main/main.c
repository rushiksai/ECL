// components/main/main.c
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lvgl.h"

// Prototype from output_handler.c
void HandleOutput(float x_value, float y_value);

// If your lv_port provides an init function, call it from here.
// Example names vary: lv_port_esp32_init(), lvgl_driver_init(), etc.
// If you already have a port init in your project, remove or adapt the placeholder below.
extern void lv_port_esp32_init(void); // weak link: only if present

static const char *TAG = "esp_test_main";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting assignment app_main");

    // Initialize LVGL library
    lv_init();

    // If your lv_port has an init function, call it.
    // Many lv_port_esp32 examples provide lv_port_esp32_init(). If not present, it's okay.
    #ifdef CONFIG_LV_PORT_ESP32_PRESENT
    lv_port_esp32_init();
    #else
    // If there is no port init symbol, you must ensure your display driver is initialized elsewhere.
    ESP_LOGI(TAG, "lv_port_esp32_init symbol not defined; make sure display driver is initialized.");
    #endif

    // Sine test loop: call HandleOutput repeatedly
    float t = 0.0f;
    const float dt = 0.1f; // step
    while (1) {
        float x = t;
        float y = sinf(t); // sine between -1 and 1
        HandleOutput(x, y);
        t += dt;
        // log for serial evidence
        ESP_LOGI(TAG, "test: x=%.3f y=%.3f", (double)x, (double)y);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
