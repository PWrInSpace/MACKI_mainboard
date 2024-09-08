// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ads1115_registers.h"
#include "ic_function_defines.h"

typedef enum {
  ADS1115_DRIVER_OK,
  ADS1115_I2C_TRANSACTION_ERROR,
  ADS1115_DRIVER_UNINITIALIZED,
  ADS1115_DRIVER_ERROR
} ads1115_driver_status_t;

typedef struct {
  uint8_t address;
  bool initiated;
  _i2c_ic_send_receive_data _i2c_send_receive;
  _i2c_ic_send_data _i2c_send;
} ads1115_driver_t;

/*!
 * @brief Writes the register byte to the ADS1115 sensor.
 * @param driver Pointer to the ADS1115 driver
 * @param reg Register address
 * @param data Data to write - as the registers are 16-bit, the data should be
 *   an array of 2 bytes
 * @return ADS1115_DRIVER_OK if the register byte was written successfully,
 *    ADS1115_DRIVER_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_write_reg(ads1115_driver_t *driver,
                                                 uint8_t reg,
                                                 const uint8_t data[2]);

/*!
 * @brief Reads the register byte from the ADS1115 sensor.
 * @param driver Pointer to the ADS1115 driver
 * @param reg Register address
 * @param data Pointer to the variable where the register byte will be stored
 * @param data_size Size of the data array
 * @return ADS1115_DRIVER_OK if the register byte was read successfully,
 *    ADS1115_DRIVER_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_read_reg(ads1115_driver_t *driver,
                                                uint8_t reg, uint8_t *data,
                                                size_t data_size);

/*!
 * @brief Initializes the ADS1115 driver - configures the IC
 * @param driver Pointer to the ADS1115 driver
 * @return ADS1115_DRIVER_OK if the driver was initialized successfully,
 *   ADS1115_I2C_TRANSACTION_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_init(ads1115_driver_t *driver);

/*!
 * @brief Deinitializes the ADS1115 driver
 * @param driver Pointer to the ADS1115 driver
 * @return ADS1115_DRIVER_OK if the driver was deinitialized successfully,
 *   ADS1115_DRIVER_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_deinit(ads1115_driver_t *driver);

/*!
 * @brief Selects the analog pin to read from
 * @param driver Pointer to the ADS1115 driver
 * @param pin Analog pin to read from
 * @return ADS1115_DRIVER_OK if the pin was selected successfully,
 *    ADS1115_DRIVER_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_select_pin(
    ads1115_driver_t *driver, ads1115_select_analog_pin_t pin);

/*!
 * @brief Reads the conversion data from the ADS1115 sensor.
 * @param driver Pointer to the ADS1115 driver
 * @param data Pointer to the variable where the conversion data will be stored
 * @return ADS1115_DRIVER_OK if the conversion data was read successfully,
 *    ADS1115_DRIVER_ERROR otherwise
 */
ads1115_driver_status_t ads1115_driver_get_conversion_data(
    ads1115_driver_t *driver, int16_t *data);
