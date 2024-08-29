// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdint.h>

typedef enum { UART_OK, UART_ERROR } uart_status_t;

typedef struct {
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t flow_control;
    uint8_t mode;
    uint8_t buffer_size;
    void (*tx_callback)(void);
    void (*rx_callback)(void);
} uart_driver_t;

uart_status_t uart_init(uart_driver_t* driver);

