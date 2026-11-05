#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define BUTTON GPIO_NUM_4

static volatile uint32_t count = 0;

static void button_isr(void *arg)
{
    count++;
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

    while (1) {
        uint32_t current = count;

        if (current != previous) {
            printf("Count: %lu\n", (unsigned long)current);
            previous = current;
        }

        vTaskDelay(1);
    }
}