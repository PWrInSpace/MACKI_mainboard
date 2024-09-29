// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "ic_function_defines.h"

typedef enum {
  TMC2209_DRIVER_OK,
  TMC2209_UART_TRANSACTION_ERROR,
  TMC2209_DRIVER_UNINITIALIZED,
  TMC2209_DRIVER_ERROR
} tmc2209_driver_status_t;

typedef struct {
  uint8_t address;
  uint8_t en_pin;
  bool initialized;
  _uart_send_data _send_data;
  _uart_receive_data _receive_data;
  _gpio_pin_set _enable_pin_set;
  _delay_ms _delay;
  // TODO(Glibus): Add setting delay on driver side
  size_t uart_read_delay_ms;
} tmc2209_driver_t;

/*!
 * @brief Initializes the TMC2209 driver
 *
 * @param driver Pointer to the driver structure
 * @return tmc2209_driver_status_t Status of the initialization
 */
tmc2209_driver_status_t tmc2209_driver_init(tmc2209_driver_t *driver);

/*!
 * @brief Enables the driver through the EN pin
 *
 * @param driver Pointer to the driver structure
 * @return tmc2209_driver_status_t Status of the enable operation
 */
tmc2209_driver_status_t tmc2209_driver_enable(tmc2209_driver_t *driver);

/*!
 * @brief Disables the driver through the EN pin
 *
 * @param driver Pointer to the driver structure
 * @return tmc2209_driver_status_t Status of the disable operation
 */
tmc2209_driver_status_t tmc2209_driver_disable(tmc2209_driver_t *driver);

/*!
 * @brief Writes a value to the register
 *
 * @param driver Pointer to the driver structure
 * @param reg_address Address of the register
 * @param reg_value Value to be written to the register
 * @return tmc2209_driver_status_t Status of the write operation
 */
tmc2209_driver_status_t tmc2209_write_register(tmc2209_driver_t *driver,
                                               uint8_t reg_address,
                                               uint32_t reg_value);

/*!
 * @brief Reads a value from the register
 *
 * @param driver Pointer to the driver structure
 * @param reg_address Address of the register
 * @param reg_value Pointer to the variable where the value will be stored
 * @return tmc2209_driver_status_t Status of the read operation
 */
tmc2209_driver_status_t tmc2209_read_register(tmc2209_driver_t *driver,
                                              uint8_t reg_address,
                                              uint32_t *reg_value);
