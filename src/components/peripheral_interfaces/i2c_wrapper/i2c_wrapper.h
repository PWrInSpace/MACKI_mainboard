// Copyright 2024 MACKI

#pragma once

#include "i2c_driver.h"

/*!
 * @brief we only have one I2C instance
 */
typedef enum {
  I2C_INSTANCE_MAIN = 0,
  I2C_MAX_INSTANCES_NUM
} i2c_driver_instances_t;

/*!
 * @brief Initialize I2C wrappers
 *
 * @param[in] driver I2C driver instance
 * @return true if success, false otherwise
 */
bool i2c_wrapper_init();

/*!
 * @brief Get instance wrapper
 *
 * @return pointer to I2C driver instance
 */
i2c_driver_t* i2c_wrapper_get_driver_instance(i2c_driver_instances_t instance);

/*!
 * @brief Deinitialize I2C wrapper
 *
 * @return true if success, false otherwise
 */
bool i2c_wrapper_deinit();
