#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define BUTTON GPIO_NUM_4

void app_main(void)
{
    gpio_config_t pin = {
        .pin_bit_mask = 1ULL << BUTTON,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&pin));

    int stable = gpio_get_level(BUTTON);
    int candidate = stable;
    int repeats = 0;
    unsigned count = 0;

    TickType_t period = pdMS_TO_TICKS(10);
    if (period == 0) period = 1;
    TickType_t wake = xTaskGetTickCount();

    while (1) {
        xTaskDelayUntil(&wake, period);
        int level = gpio_get_level(BUTTON);

        if (level != candidate) {
            candidate = level;
            repeats = 0;
        } else if (repeats < 5) {
            repeats++;
        }

        if (repeats == 5 && candidate != stable) {
            stable = candidate;

            if (stable == 0) {
                printf("Count: %u\n", ++count);
            }
        }
    }
}