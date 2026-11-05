#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_err.h"

#define BUTTON GPIO_NUM_4

static volatile uint32_t edges = 0;

static void button_isr(void *arg)
{
    edges++;
}

void app_main(void)
{
    gpio_config_t pin = {
        .pin_bit_mask = 1ULL << BUTTON,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&pin));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON, button_isr, NULL));

    uint32_t previous = 0;
    unsigned count = 0;
    bool pressed = false;
    int64_t release_time = 0;

    while (1) {
        uint32_t current = edges;
        int level = gpio_get_level(BUTTON);
        int64_t now = esp_timer_get_time();

        if (current != previous && level == 0 && !pressed) {
            pressed = true;
            printf("Count: %u\n", ++count);
        }
        previous = current;

        if (pressed) {
            if (level == 0) {
                release_time = 0;
            } else if (release_time == 0) {
                release_time = now;
            } else if (now - release_time >= 50000) {
                pressed = false;
                release_time = 0;
            }
        }

        vTaskDelay(1);
    }
}