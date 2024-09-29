// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "shared_uart_wrapper.h"

static uart_comm_driver_instances_t uart_instance = UART_INSTANCE_MAIN;

static uart_comm_driver_config_t* uart_comm_driver_instance;

void init_uart_comm_driver() {
  uart_wrapper_init();
  uart_comm_driver_instance = uart_wrapper_get_driver_instance(uart_instance);
  TEST_ASSERT_NOT_NULL(uart_comm_driver_instance);
}

void deinit_uart_comm_driver() { TEST_ASSERT_TRUE(uart_wrapper_deinit()); }

bool uart_ic_receive_data(uint8_t* data_out, size_t size_out) {
  if (uart_comm_driver_instance == NULL) {
    return false;
  }
  uart_comm_driver_status_t status = uart_comm_driver_read(
      uart_comm_driver_instance, data_out, size_out, portMAX_DELAY);

  return status == UART_COMM_DRIVER_STATUS_OK;
}

bool uart_ic_send_data(uint8_t* data, size_t data_size) {
  if (uart_comm_driver_instance == NULL) {
    return false;
  }
  uart_comm_driver_status_t status =
      uart_comm_driver_write(uart_comm_driver_instance, data, data_size);
  return status == UART_COMM_DRIVER_STATUS_OK;
}
