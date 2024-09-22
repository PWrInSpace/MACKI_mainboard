// Copyright 2024 MACKI

#include "uart_driver.h"

uart_driver_status_t uart_driver_init(uart_driver_config_t *config) {
  esp_err_t ret = ESP_OK;
  ret |= uart_driver_install(config->port, config->rx_buf_size, 0, 0, NULL, 0);
  ret |= uart_param_config(config->port, config->uart_config);
  ret |= uart_set_pin(config->port, config->tx_pin, config->tx_pin,
                      UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  return ret == ESP_OK ? UART_DRIVER_STATUS_OK : UART_DRIVER_STATUS_ERROR;
}
