// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sdkconfig.h"
#include "uart_comm_driver.h"
#include "unity.h"

#define TEST_ASSERT_UART_COMM_DRIVER_STATUS_OK(result) \
  TEST_ASSERT_EQUAL(UART_COMM_DRIVER_STATUS_OK, result)

static uart_comm_driver_config_t driver_config = {
    .port = UART_NUM_1,
    .rx_buf_size = 1024,
    .uart_config =
        {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        },
    .tx_pin = 5,
    .rx_pin = 6,
    .initialized = false,
};

TEST_CASE("UART Driver init test", "[uart_comm_driver]") {
  uart_comm_driver_status_t status = uart_comm_driver_init(&driver_config);
  TEST_ASSERT_UART_COMM_DRIVER_STATUS_OK(status);
  TEST_ASSERT_TRUE(driver_config.initialized);
}

TEST_CASE("UART Driver read test", "[uart_comm_driver]") {
  uint8_t data[10];
  uart_comm_driver_status_t status =
      uart_comm_driver_read(&driver_config, data, 10, 10);
  // Kind of special case but we expect this in the test :D
  TEST_ASSERT_EQUAL(UART_COMM_DRIVER_ERROR_INVALID_NUMBER_IN_BUFFER, status);
}

TEST_CASE("UART Driver write test", "[uart_comm_driver]") {
  uint8_t data[10];
  uart_comm_driver_status_t status =
      uart_comm_driver_write(&driver_config, data, 10);
  TEST_ASSERT_UART_COMM_DRIVER_STATUS_OK(status);
}

TEST_CASE("UART Driver deinit test", "[uart_comm_driver]") {
  uart_comm_driver_status_t status = uart_comm_driver_deinit(&driver_config);
  TEST_ASSERT_UART_COMM_DRIVER_STATUS_OK(status);
  TEST_ASSERT_FALSE(driver_config.initialized);
}
