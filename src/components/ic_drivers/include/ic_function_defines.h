// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool (*_i2c_ic_send_receive_data)(uint8_t *data_out, size_t size_out,
                                          uint8_t device_address,
                                          uint8_t reg_address);

typedef bool (*_i2c_ic_send_data)(uint8_t *data_out, size_t size_out,
                                  uint8_t device_address);

typedef bool (*_gpio_pin_set)(uint8_t pin, bool value);
