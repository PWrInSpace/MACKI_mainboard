// Copyright 2024 MACKI

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pca9574_driver.h"
#include "pca9574_registers.h"
#include "sdkconfig.h"
#include "shared_i2c_wrapper.h"
#include "unity.h"

#define TEST_ASSERT_PCA9574_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, condition)

typedef enum {
  PCA9574_IC2 = 0,
  PCA9574_IC3,
  NUM_PCA9574_DRIVERS
} pca9574_driver_index_t;

pca957_driver_t pca957_drivers[2] = {
    [PCA9574_IC2] =
        {
            .address = 0x20,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            .initiated = false,
        },
    [PCA9574_IC3] =
        {
            .address = 0x21,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            .initiated = false,
        },
};

TEST_CASE("PCA9574 driver IC3 initialization", "[PCA9574_DRIVER]") {
  init_i2c_driver();

  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_init(&pca957_drivers[PCA9574_IC3]));
}

TEST_CASE("PCA9574 driver IC2 initialization", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_init(&pca957_drivers[PCA9574_IC2]));
}

static void test_driver_set_mode(pca9574_driver_index_t driver_index,
                                 pca9574_pin_mode_t mode) {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_set_mode(&pca957_drivers[driver_index], mode));
}

TEST_CASE("PCA9574 driver IC3 set mode", "[PCA9574_DRIVER]") {
  test_driver_set_mode(PCA9574_IC3, PCA9574_OUTPUT);
  test_driver_set_mode(PCA9574_IC3, PCA9574_INPUT);
}

TEST_CASE("PCA9574 driver IC2 set mode", "[PCA9574_DRIVER]") {
  test_driver_set_mode(PCA9574_IC2, PCA9574_OUTPUT);
  test_driver_set_mode(PCA9574_IC2, PCA9574_INPUT);
}

static void test_driver_set_mode_pin(pca9574_driver_index_t driver_index,
                                     uint8_t pin, pca9574_pin_mode_t mode) {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_set_mode_pin(&pca957_drivers[driver_index], pin, mode));
}

TEST_CASE("PCA9574 driver IC3 set mode pin", "[PCA9574_DRIVER]") {
  test_driver_set_mode_pin(PCA9574_IC3, 0, PCA9574_OUTPUT);
  test_driver_set_mode_pin(PCA9574_IC3, 0, PCA9574_INPUT);
}

TEST_CASE("PCA9574 driver IC2 set mode pin", "[PCA9574_DRIVER]") {
  test_driver_set_mode_pin(PCA9574_IC2, 0, PCA9574_OUTPUT);
  test_driver_set_mode_pin(PCA9574_IC2, 0, PCA9574_INPUT);
}

static void test_driver_set_level(pca9574_driver_index_t driver_index,
                                  pca9574_pin_level_t level) {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_set_level(&pca957_drivers[driver_index], level));
}

TEST_CASE("PCA9574 driver IC3 set level", "[PCA9574_DRIVER]") {
  test_driver_set_level(PCA9574_IC3, PCA9574_LOW);
  test_driver_set_level(PCA9574_IC3, PCA9574_HIGH);
}

TEST_CASE("PCA9574 driver IC2 set level", "[PCA9574_DRIVER]") {
  test_driver_set_level(PCA9574_IC2, PCA9574_LOW);
  test_driver_set_level(PCA9574_IC2, PCA9574_HIGH);
}

static void test_driver_set_level_pin(pca9574_driver_index_t driver_index,
                                      uint8_t pin, pca9574_pin_level_t level) {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_set_level_pin(&pca957_drivers[driver_index], pin, level));
}

TEST_CASE("PCA9574 driver IC3 set level pin", "[PCA9574_DRIVER]") {
  test_driver_set_level_pin(PCA9574_IC3, 0, PCA9574_LOW);
  test_driver_set_level_pin(PCA9574_IC3, 0, PCA9574_HIGH);
}

TEST_CASE("PCA9574 driver IC2 set level pin", "[PCA9574_DRIVER]") {
  test_driver_set_level_pin(PCA9574_IC2, 0, PCA9574_LOW);
  test_driver_set_level_pin(PCA9574_IC2, 0, PCA9574_HIGH);
}

TEST_CASE("PCA9574 driver IC3 deinitialization", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_deinit(&pca957_drivers[PCA9574_IC3]));
}

TEST_CASE("PCA9574 driver IC2 deinitialization", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_deinit(&pca957_drivers[PCA9574_IC2]));
  /// @brief as this is the last, deinit the i2c driver
  deinit_i2c_driver();
}
