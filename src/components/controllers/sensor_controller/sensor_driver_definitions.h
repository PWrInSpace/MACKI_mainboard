// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "ads1115_driver.h"
#include "lis2dw12_driver.h"
#include "shared_i2c_wrapper.h"
#include "timeout_utility.h"
#include "tmp1075_driver.h"
#include "vl53l0x_driver.h"

void delay(size_t delay) { vTaskDelay(pdMS_TO_TICKS(delay)); }

ads1115_driver_t adc_expander = {
    .address = 0b1001001,
    ._i2c_send_receive = &i2c_ic_send_receive_data,
    ._i2c_send = &i2c_ic_send_data,
    .initiated = false,
    .mode = ADS1115_DRIVER_CONTINUOUS_MODE,
    .pga = ADS1115_PGA_4_096V,
};

lis2dw12_driver_t accelerometer = {
    .address = 0b0011000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

tmp1075_driver_t temperature_sensor = {
    .address = 0b1001000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
};

vl53l0x_driver_t distance_sensor = {
    .address = 0x29,
    .initialized = false,
    ._send_data = &i2c_ic_send_data,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._delay = &delay,
    ._start_timeout = &start_timeout,
    ._check_timeout_expired = &check_timeout_expired,
};
