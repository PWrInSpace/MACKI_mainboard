// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "gpio_wrapper.h"
#include "ic_function_defines.h"
#include "pca9574_driver.h"
#include "unity.h"

/*!
 * @brief Initializes ESP GPIO driver in input mode
 *
 * @param pin_number GPIO pin number
 */
void init_esp_gpio_output(uint64_t pin_number);

/*!
 * @brief Sets the value of the GPIO pin
 *
 * @param pin_number GPIO pin number
 * @param value Value to set
 */
void esp_gpio_set_pin(uint64_t pin_number, bool value);

/*!
 * @brief Initializes PCA9574 drivers
 *
 * @param driver Pointer to the PCA9574 driver
 * @param pin_number GPIO pin number
 */
void init_expanders();

/*!
 * @brief Deinitializes PCA9574 driver
 */
void deinit_expanders();

/*!
 * @brief Initializes ESP GPIO driver in input mode
 *
 * @param pin_number GPIO pin number
 */
void init_expander_gpio_output(uint64_t pin_number);

/*!
 * @brief Sets the value of the GPIO pin
 *
 * @param pin_number GPIO pin number
 * @param value Value to set
 */
void expander_gpio_set_pin(uint64_t pin_number, bool value);
