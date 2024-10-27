// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "gpio_wrapper.h"

typedef struct {
  uint8_t gpio_pin_num;
  bool initialized;
} emergency_button_t;

bool emergency_button_init(emergency_button_t* button);

bool emergency_button_is_pressed(emergency_button_t* button);

bool emergency_button_deinit(emergency_button_t* button);
