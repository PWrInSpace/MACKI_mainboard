// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "uart_comm_driver.h"

// TODO(Glibus): Finish this component when adding communication with lil macus
/*!
 * @brief Initializes the UART interface.
 *
 * @param config Pointer to the configuration structure.
 * @return Status of the initialization.
 */
bool uart_interface_init(uart_comm_driver_config_t *config);
