// Copyrigth 2024 Glibuss

#include "i2c_wrapper.h"
#include "lis2dw12_driver.h"
#include "tmp1075_driver.h"
#include "ads1115_driver.h"

void init_i2c_driver();

void deinit_i2c_driver();

bool i2c_ic_send_receive_data(uint8_t* data_out, size_t size_out,
                              uint8_t device_address, uint8_t reg_address);

bool i2c_ic_send_data(uint8_t* data, size_t data_size, uint8_t device_address);

lis2dw12_driver_t lis2dw12_driver = {
    .address = 0b0011000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

tmp1075_driver_t tmp1075_driver = {
    .address = 0b1001000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

ads1115_driver_t ads1115_driver = {
    .address = 0b1001001,
    .initiated = false,
    ._i2c_send_receive = &i2c_ic_send_receive_data,
    ._i2c_send = &i2c_ic_send_data,
};

