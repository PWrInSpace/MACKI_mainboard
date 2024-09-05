// Copyright 2024 MACKI

#include <stdio.h>
#include <string.h>

#include "i2c_wrapper.h"
#include "sdkconfig.h"
#include "tmp1075_driver.h"
#include "unity.h"

#define TEST_ASSERT_TMP1075_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(TMP1075_DRIVER_OK, condition)

static i2c_driver_instances_t i2c_instance = I2C_INSTANCE_MAIN;

static i2c_driver_t* i2c_driver_instance;

static void init_i2c_driver() {
  i2c_wrapper_init();
  i2c_driver_instance = i2c_wrapper_get_driver_instance(i2c_instance);
  TEST_ASSERT_NOT_NULL(i2c_driver_instance);
}

bool i2c_ic_send_receive_data(uint8_t* data_out, size_t size_out,
                              uint8_t device_address, uint8_t reg_address) {
  if (i2c_driver_instance == NULL) {
    return false;
  }
  i2c_driver_status_t status = i2c_driver_send_receive_data(
      i2c_driver_instance, data_out, size_out, device_address, reg_address);
  return status == I2C_DRIVER_OK;
}

bool i2c_ic_send_data(uint8_t* data, size_t data_size, uint8_t device_address) {
  if (i2c_driver_instance == NULL) {
    return false;
  }
  i2c_driver_status_t status = i2c_driver_send_data(i2c_driver_instance, data,
                                                    data_size, device_address);
  return status == I2C_DRIVER_OK;
}

tmp1075_driver_t tmp1075_driver = {
    .address = 0b1001000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
};

TEST_CASE("TMP1075 driver initialization", "[TMP1075_DRIVER]") {
  init_i2c_driver();
  TEST_ASSERT_TMP1075_DRIVER_OK(tmp1075_driver_init(&tmp1075_driver));
}

TEST_CASE("TMP1075 driver read device ID", "[TMP1075_DRIVER]") {
  uint16_t device_id;
  TEST_ASSERT_TMP1075_DRIVER_OK(
      tmp1075_driver_read_device_id(&tmp1075_driver, &device_id));
  TEST_ASSERT_EQUAL_HEX16(0x7500, device_id);
}
