// Copyright 2024 MACKI, Krzysztof Gliwiński
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "gpio_expander_wrapper.h"

typedef enum {
  LIMIT_SWITCH_PRESSED = 0,
  LIMIT_SWITCH_NOT_PRESSED,
} limit_switch_state_t;

typedef struct {
  uint8_t limit_switch_pin_num;
  gpio_expander_instance_t gpio_expander_instance;
  limit_switch_state_t state;
} limit_switch_t;

typedef struct {
  limit_switch_t top_limit_switch;
  limit_switch_t bottom_limit_switch;
} limit_switch_pair_t;

/*!
 * @brief Checks the state of the limit switch.
 * @param limit_switch Pointer to the limit switch.
 * @return LIMIT_SWITCH_PRESSED if the limit switch is pressed,
 * LIMIT_SWITCH_NOT_PRESSED otherwise.
 */
limit_switch_state_t check_limit_switch_state(limit_switch_t* limit_switch);
