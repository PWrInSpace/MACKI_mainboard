// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "driver/gpio.h"

typedef void (*gpio_isr_t)(void*);

typedef enum { GPIO_LEVEL_LOW = 0, GPIO_LEVEL_HIGH } gpio_level_t;

typedef enum { GPIO_OUTPUT, GPIO_INPUT } pin_mode_t;

typedef enum {
  GPIO_PIN_SOLENOID_1 = 0,
} pin_numbers_t;

typedef struct {
  pin_numbers_t pin_number;
  pin_mode_t mode;  // will be set in config functions
} pin_config_t;

bool gpio_pin_config_output(pin_config_t* gpio, bool pull_up_en,
                            bool pull_down_en);

bool gpio_pin_set_level(pin_config_t* gpio, gpio_level_t level);

gpio_level_t gpio_pin_get_level(pin_config_t* gpio);

bool gpio_pin_config_input(pin_config_t* gpio, bool pull_up_en,
                           bool pull_down_en, gpio_isr_t isr_handler);
