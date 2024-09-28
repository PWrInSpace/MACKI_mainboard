// Copyright 2024 MACKI

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/uart.h"

// TODO(Glibus): Finish this component when adding communication with lil macus

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
  bool initialized;
} uart_driver_config_t;

/*!
 * @brief Initializes the UART driver.
 *
 * @param config Pointer to the configuration structure.
 * @return Status of the initialization.
 */
uart_driver_status_t uart_driver_init(uart_driver_config_t *config);

/*!
 * @brief Deinitializes the UART driver.
 *
 * @param port UART port number.
 * @return Status of the deinitialization.
 */
uart_driver_status_t uart_driver_deinit(uart_driver_config_t *config);

/*!
 * @brief Reads data from the UART port.
 *
 * @param port UART port number.
 * @param data Pointer to the buffer where the data will be stored.
 * @param size Number of bytes to read.
 * @param timeout_ms Timeout in milliseconds.
 * @return Status of the read operation.
 */
uart_driver_status_t uart_driver_read(uart_driver_config_t *config, uint8_t *data,
                                      size_t size, uint32_t timeout_ms);

/*!
 * @brief Writes data to the UART port.
 *
 * @param port UART port number.
 * @param data Pointer to the buffer containing the data to write.
 * @param size Number of bytes to write.
 * @param timeout_ms Timeout in milliseconds.
 * @return Status of the write operation.
 */
uart_driver_status_t uart_driver_write(uart_driver_config_t *config, uint8_t *data,
                                       size_t size);

/*!
  * @brief Checks if the UART driver is initialized.
  *
  * @param port UART port number.
  * @return True if the driver is initialized, false otherwise.
  */
bool uart_driver_is_initialized(uart_driver_config_t *config);


