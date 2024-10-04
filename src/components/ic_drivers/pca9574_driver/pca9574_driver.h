// Copyright 2024 MACKI

#pragma once

#include <stdint.h>

#include "ic_function_defines.h"
#include "pca9574_registers.h"

typedef enum {
  PCA957_DRIVER_OK,
  PCA957_I2C_TRANSACTION_ERROR,
  PCA957_DRIVER_UNINITIALIZED,
  PCA957_DRIVER_OUT_OF_PIN_RANGE,
  PCA957_DRIVER_ERROR
} pca957_driver_status_t;

typedef enum {
  PCA9574_LOW = 0,
  PCA9574_HIGH,
  PCA9574_UNKNOWN
} pca9574_pin_level_t;

typedef struct {
  uint8_t address;
  bool initiated;
  pca9574_config_reg_t pin_config;
  uint8_t reset_pin;
  _i2c_ic_send_receive_data _send_receive_data;
  _i2c_ic_send_data _send_data;
  _gpio_pin_set _pin_set;
  _delay_ms _delay;
} pca957_driver_t;

/*!
 * @brief Writes a byte to the PCA957 driver
 * @param driver Pointer to the PCA957 driver
 * @return PCA957_DRIVER_OK if the byte was written successfully,
 *    PCA957_I2C_TRANSACTION_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_write_byte(pca957_driver_t *driver,
                                                uint8_t reg, uint8_t data);

/*!
 * @brief Reads a byte from the PCA957 driver
 * @param driver Pointer to the PCA957 driver
 * @return PCA957_DRIVER_OK if the byte was read successfully,
 *    PCA957_I2C_TRANSACTION_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_read_byte(pca957_driver_t *driver,
                                               uint8_t reg, uint8_t *data);

/*!
 * @brief Initializes the PCA957 driver - configures the IC
 * @param driver Pointer to the PCA957 driver
 * @return PCA957_DRIVER_OK if the driver was initialized successfully,
 *   PCA957_I2C_TRANSACTION_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_init(pca957_driver_t *driver);

/*!
 * @brief Resets the PCA957 driver through the reset pin
 * @param driver Pointer to the PCA957 driver
 * @note The driver will be deinitiated after the reset
 */
pca957_driver_status_t pca957_driver_reset_ic(pca957_driver_t *driver);

/*!
 * @brief Deinitializes the PCA957 driver
 * @param driver Pointer to the PCA957 driver
 * @return PCA957_DRIVER_OK if the driver was deinitialized successfully,
 *   PCA957_DRIVER_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_deinit(pca957_driver_t *driver);

/*!
 * @brief Sets the mode of the PCA957
 * @param driver Pointer to the PCA957 driver
 * @param mode mode value to set
 * @return PCA957_DRIVER_OK if the output was set successfully,
 *    PCA957_DRIVER_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_set_mode(pca957_driver_t *driver,
                                              pca9574_config_reg_t mode);

/*!
 * @brief Sets the level of the PCA957
 * @param driver Pointer to the PCA957 driver
 * @param level level value to set
 * @return PCA957_DRIVER_OK if the output was set successfully,
 *    PCA957_DRIVER_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_set_level(pca957_driver_t *driver,
                                               pca9574_pin_level_t level);

/*!
 * @brief Sets the level of the PCA957 pin
 * @param driver Pointer to the PCA957 driver
 * @param level level value to set
 * @param pin pin to set
 * @return PCA957_DRIVER_OK if the output was set successfully,
 *    PCA957_DRIVER_ERROR otherwise
 */
pca957_driver_status_t pca957_driver_set_level_pin(pca957_driver_t *driver,
                                                   pca9574_pin_level_t level,
                                                   pca9574_pins_num_t pin);

/*!
 * @brief Get the output level of the PCA957
 * @param driver Pointer to the PCA957 driver
 * @param output_level output level value to get
 * @return PCA957_DRIVER_OK if the output was set successfully,
 */
pca957_driver_status_t pca957_driver_get_output_level(
    pca957_driver_t *driver, pca9574_config_reg_t *output_level);

/*!
 * @brief Get the input level of the PCA957
 * @param driver Pointer to the PCA957 driver
 * @param output_level input level value to get
 * @return PCA957_DRIVER_OK if the output was set successfully,
 */
pca957_driver_status_t pca957_driver_get_input_level(
    pca957_driver_t *driver, pca9574_config_reg_t *output_level);

/*!
 * @brief Get the output level of the PCA957 pin
 * @param driver Pointer to the PCA957 driver
 * @param pin pin to get
 * @return PCA9574_LOW if the pin is low, PCA9574_HIGH if the pin is high,
 *    PCA9574_UNKNOWN otherwise
 */
pca9574_pin_level_t pca957_driver_get_level_pin(pca957_driver_t *driver,
                                                pca9574_pins_num_t pin);
