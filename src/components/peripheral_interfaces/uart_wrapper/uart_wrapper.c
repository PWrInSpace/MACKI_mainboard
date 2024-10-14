// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "uart_wrapper.h"

#include "macki_log.h"

#define TAG "UART_WRAPPER"

static uart_comm_driver_config_t driver[UART_MAX_INSTANCES_NUM] = {
    [UART_INSTANCE_MACUS] = {
        .uart_config =
            {
                .baud_rate = 115200,
                .data_bits = UART_DATA_8_BITS,
                .parity = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                .source_clk = UART_SCLK_DEFAULT,
            },
        /// IMPORTANT - UART 2 is used for motors in TMC2209_impl.h
        .port = UART_NUM_1,
        .rx_pin = 2,
        .tx_pin = 1,
        .rx_buf_size = 1024,
        .initialized = false,
    }};

static bool uart_wrapper_initialized = false;

bool uart_wrapper_init() {
  if (uart_wrapper_initialized) {
    MACKI_LOG_WARN(TAG, "UART driver already initialized");
    return true;
  }
  for (uint8_t i = 0; i < UART_MAX_INSTANCES_NUM; ++i) {
    if (uart_comm_driver_init(&driver[i]) != UART_COMM_DRIVER_STATUS_OK) {
      return false;
    }
  }
  uart_wrapper_initialized = true;
  MACKI_LOG_INFO(TAG, "UART wrapper initialized, number of instances: %d",
                 (uint8_t)UART_MAX_INSTANCES_NUM);
  return true;
}

uart_comm_driver_config_t *uart_wrapper_get_driver_instance(
    uart_comm_driver_instances_t instance) {
  if (instance >= UART_MAX_INSTANCES_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid UART instance %d", instance);
    return NULL;
  }
  return &driver[instance];
}

bool uart_wrapper_deinit() {
  if (!uart_wrapper_initialized) {
    MACKI_LOG_WARN(TAG, "UART driver already deinitialized");
    return true;
  }
  for (uint8_t i = 0; i < UART_MAX_INSTANCES_NUM; ++i) {
    if (uart_comm_driver_deinit(&driver[i]) != UART_COMM_DRIVER_STATUS_OK) {
      return false;
    }
  }
  uart_wrapper_initialized = false;
  return true;
}
