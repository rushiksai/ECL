#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "assignment";

void app_main(void)
{
    int blink_pin = CONFIG_BLINK_GPIO;
    ESP_LOGI(TAG, "Blink pin from menuconfig: %d", blink_pin);

    gpio_reset_pin(blink_pin);
    gpio_set_direction(blink_pin, GPIO_MODE_OUTPUT);

    int cnt = 0;
    while (1) {
        ESP_LOGI(TAG, "Assignment running. Counter: %d", cnt++);
        gpio_set_level(blink_pin, cnt % 2);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
