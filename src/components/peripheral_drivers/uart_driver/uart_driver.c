// Copyright 2024 MACKI

#include "uart_driver.h"

uart_driver_status_t uart_driver_init(uart_driver_config_t *config) {
  esp_err_t ret = ESP_OK;
  ret |= uart_driver_install(config->port, config->rx_buf_size, 0, 0, NULL, 0);
  ret |= uart_param_config(config->port, &config->uart_config);
  ret |= uart_set_pin(config->port, config->tx_pin, config->tx_pin,
                      UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  return ret == ESP_OK ? UART_DRIVER_STATUS_OK : UART_DRIVER_STATUS_ERROR;
}

uart_driver_status_t uart_driver_deinit(uart_port_t port) {
  esp_err_t ret = uart_driver_delete(port);
  return ret == ESP_OK ? UART_DRIVER_STATUS_OK : UART_DRIVER_STATUS_ERROR;
}

uart_driver_status_t uart_driver_read(uart_port_t port, uint8_t *data,
                                      size_t size, uint32_t timeout_ms) {
  int bytes_read =
      uart_read_bytes(port, data, size, timeout_ms / portTICK_PERIOD_MS);
  return bytes_read == size ? UART_DRIVER_STATUS_OK : UART_DRIVER_STATUS_ERROR;
}

uart_driver_status_t uart_driver_write(uart_port_t port, uint8_t *data,
                                       size_t size) {
  int bytes_written = uart_write_bytes(port, (const char *)data, size);
  return bytes_written == size ? UART_DRIVER_STATUS_OK
                               : UART_DRIVER_STATUS_ERROR;
}
