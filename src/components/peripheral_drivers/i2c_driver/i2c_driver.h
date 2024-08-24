// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdint.h>
#include "driver/i2c.h"

typedef enum { I2C_DRIVER_OK = 0, I2C_DRIVER_ERROR } i2c_driver_status_t;

typedef struct {
} i2c_driver_t;

i2c_driver_status_t i2c_driver_init(i2c_driver_t *i2c_driver);

i2c_driver_status_t i2c_driver_send_data(i2c_driver_t *i2c_driver,
                                         uint8_t *data, uint16_t size);

i2c_driver_status_t i2c_driver_receive_data(i2c_driver_t *i2c_driver,
                                            uint8_t *data, uint16_t size);

i2c_driver_status_t i2c_driver_send_receive_data(i2c_driver_t *i2c_driver,
                                                 uint8_t *data, uint16_t size,
                                                 uint8_t *data_out,
                                                 uint16_t *size_out);

i2c_driver_status_t i2c_driver_deinit(i2c_driver_t *i2c_driver);
