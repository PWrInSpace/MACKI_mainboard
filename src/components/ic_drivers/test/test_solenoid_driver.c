// Copyright 2024 MACKI, Krzysztof Gliwiński

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pca9574_driver.h"
#include "sdkconfig.h"
#include "shared_i2c_wrapper.h"
#include "solenoid_driver.h"
#include "unity.h"

#define TEST_ASSERT_SOLENOID_OK(status) \
  TEST_ASSERT_EQUAL(SOLENOID_DRIVER_OK, status)
#define TEST_ASSERT_SOLENOID_OPEN(status) \
  TEST_ASSERT_EQUAL(SOLENOID_OPEN, status)
#define TEST_ASSERT_SOLENOID_CLOSED(status) \
  TEST_ASSERT_EQUAL(SOLENOID_CLOSED, status)

// these are connected to IC3 expander on the board
pca957_driver_t pca9574_driver = {
    .address = 0x21,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

bool gpio_set_pin(uint8_t pin_number, bool value) {
  pca9574_pin_level_t level = value ? PCA9574_HIGH : PCA9574_LOW;
  pca957_driver_status_t ret =
      pca957_driver_set_level_pin(&pca9574_driver, level, (uint8_t)pin_number);
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, ret);
  return ret == PCA957_DRIVER_OK;
}

typedef enum {
  VALVE_INSTANCE_1,
  VALVE_INSTANCE_2,
  VALVE_INSTANCE_3,
  VALVE_INSTANCE_4,
  VALVE_INSTANCE_MAX
} valve_instance_t;

typedef enum {
  VALVE_1 = 0,
  VALVE_2 = 1,
  VALVE_3 = 2,
  VALVE_4 = 5,
} solenoid_pins_t;

solenoid_driver_t solenoid_drivers[VALVE_INSTANCE_MAX] = {
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_1},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_2},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_3},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_4},
};

void init_expander() {
  init_i2c_driver();
  pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver);
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, ret);
  ret = pca957_driver_set_mode(&pca9574_driver, PCA9574_OUTPUT);
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, ret);
  ret = pca957_driver_set_level(&pca9574_driver, PCA9574_LOW);
  TEST_ASSERT_EQUAL(PCA957_DRIVER_OK, ret);
}

void deinit_expanders() {
  pca957_driver_deinit(&pca9574_driver);
  deinit_i2c_driver();
}

void test_solenoid_driver_init(solenoid_driver_t* driver) {
  solenoid_driver_status_t status = solenoid_driver_init(driver);
  TEST_ASSERT_SOLENOID_OK(status);
  TEST_ASSERT_TRUE(driver->initialized);
  TEST_ASSERT_SOLENOID_CLOSED(driver->current_state);
}

TEST_CASE("Solenoid driver initialization", "[SOLENOID_DRIVER]") {
  init_expander();
  for (valve_instance_t i = VALVE_INSTANCE_1; i < VALVE_INSTANCE_MAX; i++) {
    test_solenoid_driver_init(&solenoid_drivers[i]);
  }
}

void test_solenoid_open_close(solenoid_driver_t* driver) {
  solenoid_driver_status_t status = solenoid_driver_open(driver);
  TEST_ASSERT_SOLENOID_OK(status);
  TEST_ASSERT_SOLENOID_OPEN(driver->current_state);
  vTaskDelay(pdMS_TO_TICKS(1000));

  status = solenoid_driver_close(driver);
  TEST_ASSERT_SOLENOID_OK(status);
  TEST_ASSERT_SOLENOID_CLOSED(driver->current_state);
}

TEST_CASE("Solenoid driver open close test", "[SOLENOID_DRIVER]") {
  for (valve_instance_t i = VALVE_INSTANCE_1; i < VALVE_INSTANCE_MAX; i++) {
    test_solenoid_open_close(&solenoid_drivers[i]);
  }
  deinit_expanders();
}

// TEST_CASE("Solenoid driver deinit test", "[SOLENOID_DRIVER]"){

// }
