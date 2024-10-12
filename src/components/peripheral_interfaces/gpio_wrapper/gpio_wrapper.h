// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "driver/gpio.h"

typedef void (*gpio_isr_t)(void* arg);

typedef enum { GPIO_LEVEL_LOW = 0, GPIO_LEVEL_HIGH } gpio_level_t;

typedef enum { GPIO_OUTPUT = 0, GPIO_INPUT, GPIO_UNUSED } pin_mode_t;

typedef enum {
  GPIO_PIN_RESET_EXP_1 = 18,
  GPIO_PIN_RESET_EXP_2 = 8,
  GPIO_OUTPUT_PIN_NUM_MAX
} pin_output_numbers_t;

bool gpio_wrapper_init();

bool gpio_pin_config_output(uint8_t gpio_num, bool pull_up_en,
                            bool pull_down_en);

bool gpio_pin_set_level(uint8_t gpio, gpio_level_t level);

gpio_level_t gpio_pin_get_level(uint8_t gpio);

bool gpio_pin_config_input(uint8_t gpio, bool pull_up_en,
                           bool pull_down_en, gpio_isr_t isr_handler);
