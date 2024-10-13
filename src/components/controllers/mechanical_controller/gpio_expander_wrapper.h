// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  GPIO_EXPANDER_1 = 0,
  GPIO_EXPANDER_2,
  GPIO_EXPANDER_MAX
} gpio_expander_instance_t;

typedef enum {
  LIMIT_SW_1 = 0,
  LIMIT_SW_2 = 1,
  LIMIT_SW_3 = 2,
  LIMIT_SW_4 = 3,
  MOTOR_LD_4 = 4,
  MOTOR_LD_3 = 5,
  MOTOR_LD_2 = 6,
  MOTOR_LD_1 = 7,
} gpio_expander_1_pins_t;

typedef enum {
  VALVE_1 = 0,
  VALVE_2 = 1,
  VALVE_3 = 2,
  UNUSED_1 = 3,
  UNUSED_2 = 4,
  VALVE_4 = 5,
  LIMIT_SW_5 = 6,
  LIMIT_SW_6 = 7,
} gpio_expander_2_pins_t;

typedef enum {
  GPIO_EXPANDER_INPUT_LEVEL_HIGH,
  GPIO_EXPANDER_INPUT_LEVEL_LOW,
  GPIO_EXPANDER_INPUT_LEVEL_UNKNOWN,
} gpio_expander_input_level_t;

bool init_gpio_expanders();

bool expander_1_gpio_set(uint8_t pin, bool value);

bool expander_2_gpio_set(uint8_t pin, bool value);

gpio_expander_input_level_t gpio_expander_get_level(
    gpio_expander_instance_t instance, uint8_t pin_mask);
