// Copyright 2024 MACKI

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pca9574_driver.h"
#include "pca9574_registers.h"
#include "sdkconfig.h"
#include "shared_gpio_wrapper.h"
#include "shared_i2c_wrapper.h"
#include "unity.h"

#define TEST_ASSERT_PCA9574_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, condition)

typedef enum {
  PCA9574_IC2 = 0,
  PCA9574_IC3,
  NUM_PCA9574_DRIVERS
} pca9574_driver_index_t;

void delay_ms(size_t delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); }

pca957_driver_t pca957_drivers[2] = {
    [PCA9574_IC2] =
        {
            .address = 0x20,
            .pin_config = {.pins.pin0 = PCA9574_INPUT,
                           .pins.pin1 = PCA9574_INPUT,
                           .pins.pin2 = PCA9574_INPUT,
                           .pins.pin3 = PCA9574_INPUT,
                           .pins.pin4 = PCA9574_OUTPUT,
                           .pins.pin5 = PCA9574_OUTPUT,
                           .pins.pin6 = PCA9574_OUTPUT,
                           .pins.pin7 = PCA9574_OUTPUT},
            .reset_pin = 18,
            ._pin_set = &esp_gpio_set_pin,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            ._delay = &delay_ms,
            .initiated = false,
        },
    [PCA9574_IC3] =
        {
            .address = 0x21,
            ._pin_set = &esp_gpio_set_pin,
            .pin_config = {.pins.pin0 = PCA9574_OUTPUT,
                           .pins.pin1 = PCA9574_OUTPUT,
                           .pins.pin2 = PCA9574_OUTPUT,
                           .pins.pin3 = PCA9574_OUTPUT,
                           .pins.pin4 = PCA9574_OUTPUT,
                           .pins.pin5 = PCA9574_OUTPUT,
                           .pins.pin6 = PCA9574_INPUT,
                           .pins.pin7 = PCA9574_INPUT},
            .reset_pin = 8,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
            ._delay = &delay_ms,
            .initiated = false,
        },
};

TEST_CASE("PCA9574 driver IC3 initialization", "[PCA9574_DRIVER]") {
  init_i2c_driver();
  init_all_pins();

  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_init(&pca957_drivers[PCA9574_IC3]));
}

TEST_CASE("PCA9574 driver IC2 initialization", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_init(&pca957_drivers[PCA9574_IC2]));
}

static void test_driver_set_mode(pca9574_driver_index_t driver_index,
                                 pca9574_config_reg_t mode) {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_set_mode(&pca957_drivers[driver_index], mode));
}

TEST_CASE("PCA9574 driver IC3 set mode", "[PCA9574_DRIVER]") {
  pca9574_config_reg_t reg = {
      .pins.pin0 = PCA9574_OUTPUT,
      .pins.pin1 = PCA9574_OUTPUT,
      .pins.pin2 = PCA9574_OUTPUT,
      .pins.pin3 = PCA9574_OUTPUT,
      .pins.pin4 = PCA9574_OUTPUT,
      .pins.pin5 = PCA9574_OUTPUT,
      .pins.pin6 = PCA9574_OUTPUT,
      .pins.pin7 = PCA9574_OUTPUT,
  };
  test_driver_set_mode(PCA9574_IC3, reg);

  pca9574_config_reg_t reg_2 = {
      .pins.pin0 = PCA9574_INPUT,
      .pins.pin1 = PCA9574_INPUT,
      .pins.pin2 = PCA9574_INPUT,
      .pins.pin3 = PCA9574_INPUT,
      .pins.pin4 = PCA9574_INPUT,
      .pins.pin5 = PCA9574_INPUT,
      .pins.pin6 = PCA9574_INPUT,
      .pins.pin7 = PCA9574_INPUT,
  };
  test_driver_set_mode(PCA9574_IC3, reg_2);
  test_driver_set_mode(PCA9574_IC2, reg);
  test_driver_set_mode(PCA9574_IC2, reg_2);
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

TEST_CASE("PCA9574 driver IC3 reset", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_reset_ic(&pca957_drivers[PCA9574_IC3]));
}

TEST_CASE("PCA9574 driver IC2 reset", "[PCA9574_DRIVER]") {
  TEST_ASSERT_PCA9574_DRIVER_OK(
      pca957_driver_reset_ic(&pca957_drivers[PCA9574_IC2]));
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
