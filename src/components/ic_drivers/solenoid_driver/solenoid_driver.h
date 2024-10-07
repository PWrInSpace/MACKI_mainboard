// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "ic_function_defines.h"

typedef enum {
  SOLENOID_DRIVER_OK,
  SOLENOID_DRIVER_ERROR
} solenoid_driver_status_t;

typedef enum {
  SOLENOID_OPEN,
  SOLENOID_CLOSED,
  SOLENOID_UNKNOWN
} solenoid_state_t;

typedef struct {
  bool initialized;
  _gpio_pin_set _set_pin;
  solenoid_state_t current_state;
  uint8_t gpio_pin;
} solenoid_driver_t;

solenoid_driver_status_t solenoid_driver_init(solenoid_driver_t *driver);

solenoid_driver_status_t solenoid_driver_open(solenoid_driver_t *driver);

solenoid_driver_status_t solenoid_driver_close(solenoid_driver_t *driver);

solenoid_state_t solenoid_driver_get_state(solenoid_driver_t *driver);
