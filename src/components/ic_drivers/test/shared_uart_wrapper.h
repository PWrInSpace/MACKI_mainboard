// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "sdkconfig.h"
#include "uart_wrapper.h"
#include "unity.h"

void init_uart_comm_driver();

void deinit_uart_comm_driver();

bool uart_ic_receive_data(uint8_t* data_out, size_t size_out);

bool uart_ic_send_data(uint8_t* data_in, size_t size_in);
