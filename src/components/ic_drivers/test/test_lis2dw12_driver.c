// Copyright 2024 MACKI

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lis2dw12_driver.h"
#include "sdkconfig.h"
#include "shared_i2c_wrapper.h"
#include "unity.h"

#define TEST_ASSERT_LIS2DW12_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(LIS2DW12_DRIVER_OK, condition)

lis2dw12_driver_t lis2dw12_driver = {
    .address = 0b0011000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

TEST_CASE("LIS2DW12 driver initialization", "[LIS2DW12_DRIVER]") {
  init_i2c_driver();

  TEST_ASSERT_LIS2DW12_DRIVER_OK(lis2dw12_driver_init(&lis2dw12_driver));
}

TEST_CASE("LIS2DW12 driver read who am i", "[LIS2DW12_DRIVER]") {
  uint8_t who_am_i;
  TEST_ASSERT_LIS2DW12_DRIVER_OK(
      lis2dw12_driver_who_am_i(&lis2dw12_driver, &who_am_i));
  TEST_ASSERT_EQUAL_HEX8(0x44, who_am_i);
}

TEST_CASE("LIS2DW12 driver read fifo samples", "[LIS2DW12_DRIVER]") {
  uint8_t samples_number;
  TEST_ASSERT_LIS2DW12_DRIVER_OK(lis2dw12_driver_get_fifo_samples_number(
      &lis2dw12_driver, &samples_number));
  TEST_ASSERT_LESS_OR_EQUAL_INT16(MAX_FIFO_SAMPLES_NUM, samples_number);
}

TEST_CASE("LIS2DW12 driver read whole fifo", "[LIS2DW12_DRIVER]") {
  lis2dw12_fifo_data_t fifo_data = {
      .current_samples_number = 0,
      .samples = {},
  };
  TEST_ASSERT_LIS2DW12_DRIVER_OK(
      lis2dw12_driver_read_fifo_data(&lis2dw12_driver, &fifo_data));

  printf("FIFO samples:\n");
  for (int i = 0; i < fifo_data.current_samples_number; i++) {
    printf("Sample %d: x=%d, y=%d, z=%d\n", i, fifo_data.samples[i].x,
          fifo_data.samples[i].y, fifo_data.samples[i].z);
  }
}

TEST_CASE("LIS2DW12 driver deinitialization", "[LIS2DW12_DRIVER]") {
  TEST_ASSERT_LIS2DW12_DRIVER_OK(lis2dw12_driver_deinit(&lis2dw12_driver));
  /// @note as this is the last test, deinit the i2c driver
  deinit_i2c_driver();
}
