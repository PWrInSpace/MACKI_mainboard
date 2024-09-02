// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdint.h>

#include "driver/i2c.h"

#define NONE_ALLOC_FLAGS 0
#define I2C_TIMEOUT_MS 1000 / portTICK_PERIOD_MS

#define I2C_BUF_SIZE 2

typedef enum { I2C_DRIVER_OK = 0, I2C_DRIVER_ERROR } i2c_driver_status_t;

typedef enum { I2C_BUF_DISABLE = 0, I2C_BUF_ENABLE } i2c_buf_settings_t;

/*!
 * @brief For I2C master config - buf setting shall be set to DISABLE
 */
typedef struct {
  i2c_config_t config;
  i2c_port_t port;
  i2c_buf_settings_t rx_buf_setting;
  i2c_buf_settings_t tx_buf_setting;
} i2c_driver_t;

/*!
 * @brief Initialize I2C driver
 *
 * @param driver I2C driver structure
 * @return i2c_driver_status_t
 */
i2c_driver_status_t i2c_driver_init(i2c_driver_t *driver);

/*!
 * @brief Send byte to I2C device
 *
 * @param driver I2C driver structure
 * @param data Data to send
 * @param device_address Device address
 * @param reg_address Register address
 * @return i2c_driver_status_t
 */
i2c_driver_status_t i2c_driver_send_byte(i2c_driver_t *driver,
                                         const uint8_t data,
                                         uint8_t device_address,
                                         const uint8_t reg_address);

/*!
 * @brief Receive byte from I2C device
 *
 * @param driver I2C driver structure
 * @param data Data to receive
 * @param device_address Device address
 * @param reg_address Register address
 * @return i2c_driver_status_t
 */
i2c_driver_status_t i2c_driver_receive_byte(i2c_driver_t *driver, uint8_t *data,
                                            uint8_t device_address,
                                            uint8_t reg_address);

/*!
 * @brief Send and receive data from I2C device
 *
 * @param driver I2C driver structure
 * @param data_out Data to send
 * @param size_out Size of data to send
 * @param device_address Device address
 * @param reg_address Register address
 * @return i2c_driver_status_t
 */
i2c_driver_status_t i2c_driver_send_receive_data(i2c_driver_t *driver,
                                                 uint8_t *data_out,
                                                 size_t size_out,
                                                 uint8_t device_address,
                                                 uint8_t reg_address);

/*!
 * @brief Deinitialize I2C driver
 *
 * @param driver I2C driver structure
 * @return i2c_driver_status_t
 */
i2c_driver_status_t i2c_driver_deinit(i2c_driver_t *driver);
