// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "ic_function_defines.h"

typedef struct {
  uint8_t address;
  bool initialized;
  _i2c_ic_send_data _send_data;
  _i2c_ic_send_receive_data _send_receive_data;
  _gpio_pin_set _enable_pin_set;
  _delay_ms _delay;
  // TODO(Glibus): Add setting delay on driver side
  size_t uart_read_delay_ms;
} tmc2209_driver_t;

