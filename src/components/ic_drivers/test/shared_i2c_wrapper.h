// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "i2c_wrapper.h"
#include "sdkconfig.h"
#include "unity.h"

void init_i2c_driver();

void deinit_i2c_driver();

bool i2c_ic_send_receive_data(uint8_t* data_out, size_t size_out,
                              uint8_t device_address, uint8_t reg_address);

bool i2c_ic_send_data(uint8_t* data, size_t data_size, uint8_t device_address);
