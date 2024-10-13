// Copyright 2024 MACKI

#include "uart_comm_driver.h"

#define DEFAULT_TX_BUF_SIZE 0
#define DEFAULT_QUEUE_SIZE 0
#define DEFAULT_INTR_FLAGS 0

uart_comm_driver_status_t uart_comm_driver_init(
    uart_comm_driver_config_t *config) {
  esp_err_t ret = ESP_OK;
  ret |= uart_driver_install(config->port, config->rx_buf_size,
                             DEFAULT_TX_BUF_SIZE, DEFAULT_QUEUE_SIZE, NULL,
                             DEFAULT_INTR_FLAGS);
  ret |= uart_param_config(config->port, &config->uart_config);
  ret |= uart_set_pin(config->port, config->tx_pin, config->tx_pin,
                      UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (ret == ESP_OK) {
    config->initialized = true;
  }
  return ret == ESP_OK ? UART_COMM_DRIVER_STATUS_OK
                       : UART_COMM_DRIVER_STATUS_ERROR;
}

uart_comm_driver_status_t uart_comm_driver_deinit(
    uart_comm_driver_config_t *config) {
  esp_err_t ret = uart_driver_delete(config->port);
  if (ret == ESP_OK) {
    config->initialized = false;
  }
  return ret == ESP_OK ? UART_COMM_DRIVER_STATUS_OK
                       : UART_COMM_DRIVER_STATUS_ERROR;
}

uart_comm_driver_status_t uart_comm_driver_read(
    uart_comm_driver_config_t *config, uint8_t *data, size_t size,
    uint32_t timeout_ms) {
  int bytes_read = uart_read_bytes(config->port, data, size,
                                   timeout_ms / portTICK_PERIOD_MS);
  return bytes_read == size ? UART_COMM_DRIVER_STATUS_OK
                            : UART_COMM_DRIVER_ERROR_INVALID_NUMBER_IN_BUFFER;
}

uart_comm_driver_status_t uart_comm_driver_write(
    uart_comm_driver_config_t *config, uint8_t *data, size_t size) {
  int bytes_written = uart_write_bytes(config->port, (const char *)data, size);
  return bytes_written == size ? UART_COMM_DRIVER_STATUS_OK
                               : UART_COMM_DRIVER_STATUS_ERROR;
}

bool uart_comm_driver_is_initialized(uart_comm_driver_config_t *config) {
  return uart_is_driver_installed(config->port) && config->initialized;
}
