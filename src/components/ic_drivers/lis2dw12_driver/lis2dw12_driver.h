// Copyright 2024 MACKI

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "ic_function_defines.h"
#include "lis2dw12_data_structs.h"

typedef enum {
  LIS2DW12_DRIVER_OK,
  LIS2DW12_I2C_TRANSACTION_ERROR,
  LIS2DW12_DRIVER_UNINITIALIZED,
  LIS2DW12_DRIVER_TOO_MANY_SAMPLES,
  LIS2DW12_DRIVER_ERROR
} lis2dw12_driver_status_t;

typedef struct {
  size_t address;
  bool initiated;
  _i2c_ic_send_receive_data _send_receive_data;
  _i2c_ic_send_data _send_data;
} lis2dw12_driver_t;

/*!
 * @brief Reads the device ID from the LIS2DW12 sensor.
 * @param driver Pointer to the LIS2DW12 driver
 * @param device_id Pointer to the variable where the device ID will be stored
 * @return LIS2DW12_DRIVER_OK if the device ID was read successfully,
 *    LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_write_register_byte(
    lis2dw12_driver_t *driver, uint8_t reg, uint8_t data);

/*!
 * @brief Reads the register byte from the LIS2DW12 sensor.
 * @param driver Pointer to the LIS2DW12 driver
 * @param reg Register address
 * @param data Pointer to the variable where the register byte will be stored
 * @return LIS2DW12_DRIVER_OK if the register byte was read successfully,
 *    LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_read_register_byte(
    lis2dw12_driver_t *driver, uint8_t reg, uint8_t *data);

/*!
 * @brief Initializes the LIS2DW12 driver - configures the IC
 * @param driver Pointer to the LIS2DW12 driver
 * @return LIS2DW12_DRIVER_OK if the driver was initialized successfully,
 *   LIS2DW12_I2C_TRANSACTION_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_init(lis2dw12_driver_t *driver);

/*!
 * @brief Deinitializes the LIS2DW12 driver
 * @param driver Pointer to the LIS2DW12 driver
 * @return LIS2DW12_DRIVER_OK if the driver was deinitialized successfully,
 *   LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_deinit(lis2dw12_driver_t *driver);

/*!
 * @brief Reads the WHO_AM_I register from the LIS2DW12 sensor.
 * @param driver Pointer to the LIS2DW12 driver
 * @param who_am_i Pointer to the variable where the device ID will be stored
 * @return LIS2DW12_DRIVER_OK if the device ID was read successfully,
 *    LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_who_am_i(lis2dw12_driver_t *driver,
                                                  uint8_t *who_am_i);

/*!
 * @brief Reads the samples number from FIFO_STATUS register from the LIS2DW12
 * sensor.
 * @param driver Pointer to the LIS2DW12 driver
 * @param samples_number Pointer to the variable where the samples number will
 * be stored
 * @return LIS2DW12_DRIVER_OK if the samples number was read successfully,
 *   LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_get_fifo_samples_number(
    lis2dw12_driver_t *driver, uint8_t *samples_number);

/*!
 * @brief Reads the FIFO data from the LIS2DW12 sensor.
 * @param driver Pointer to the LIS2DW12 driver
 * @param fifo_data Pointer to the variable where the FIFO data will be stored
 * @return LIS2DW12_DRIVER_OK if the FIFO data was read successfully,
 *    LIS2DW12_DRIVER_ERROR otherwise
 */
lis2dw12_driver_status_t lis2dw12_driver_read_fifo_data(
    lis2dw12_driver_t *driver, lis2dw12_fifo_data_t *fifo_data);
