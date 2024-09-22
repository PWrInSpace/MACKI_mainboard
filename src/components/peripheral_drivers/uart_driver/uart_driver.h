// Copyright 2024 MACKI

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"

typedef enum {
  UART_DRIVER_STATUS_OK,
  UART_DRIVER_STATUS_ERROR,
  UART_DRIVER_STATUS_BUSY,
  UART_DRIVER_STATUS_TIMEOUT,
  UART_DRIVER_STATUS_INVALID_PARAMETER,
  UART_DRIVER_STATUS_UNSUPPORTED_OPERATION
} uart_driver_status_t;

typedef struct {
  uart_config_t uart_config;
  uart_port_t port;
  uint8_t rx_pin;
  uint8_t tx_pin;
  int16_t rx_buf_size;
} uart_driver_config_t;

uart_driver_status_t uart_driver_init(uart_driver_config_t *config);
