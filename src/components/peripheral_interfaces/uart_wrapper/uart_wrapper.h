// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "uart_comm_driver.h"

/*!
 * @brief we only have one uart instance
 */
typedef enum {
  UART_INSTANCE_MACUS = 0,
  UART_MAX_INSTANCES_NUM
} uart_comm_driver_instances_t;

/*!
 * @brief Initialize uart wrappers
 *
 * @param[in] driver uart driver instance
 * @return true if success, false otherwise
 */
bool uart_wrapper_init();

/*!
 * @brief Get instance wrapper
 *
 * @return pointer to uart driver instance
 */
uart_comm_driver_config_t* uart_wrapper_get_driver_instance(
    uart_comm_driver_instances_t instance);

/*!
 * @brief Deinitialize uart wrapper
 *
 * @return true if success, false otherwise
 */
bool uart_wrapper_deinit();
