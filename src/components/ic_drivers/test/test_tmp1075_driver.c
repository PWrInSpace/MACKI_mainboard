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
  printf("status: %d\n", status);
  return status == I2C_DRIVER_OK;
}

typedef enum {
  TMP1075_U6 = 0,
  TMP1075_U7,
  NUM_TMP1075_DRIVERS
} tmp1075_driver_index_t;

tmp1075_driver_t tmp1075_drivers[NUM_TMP1075_DRIVERS] = {
    [TMP1075_U6] =
        {
            .address = 0b1001000,
            ._send_receive_data = &i2c_ic_send_receive_data,
            ._send_data = &i2c_ic_send_data,
        },
    // TODO(GLibus): Fix address test when correct sensor is connected
    [TMP1075_U7] = {
        .address = 0b1001001,
        ._send_receive_data = &i2c_ic_send_receive_data,
        ._send_data = &i2c_ic_send_data,
    }};

TEST_CASE("TMP1075 driver U6 initialization", "[TMP1075_DRIVER]") {
  init_i2c_driver();

  TEST_ASSERT_TMP1075_DRIVER_OK(
      tmp1075_driver_init(&tmp1075_drivers[TMP1075_U6]));
}

TEST_CASE("TMP1075 driver U7 initialization", "[TMP1075_DRIVER]") {
  TEST_ASSERT_TMP1075_DRIVER_OK(
      tmp1075_driver_init(&tmp1075_drivers[TMP1075_U7]));
}

TEST_CASE("TMP1075 driver U6 read device ID", "[TMP1075_DRIVER]") {
  uint16_t device_id;
  TEST_ASSERT_TMP1075_DRIVER_OK(
      tmp1075_driver_read_device_id(&tmp1075_drivers[TMP1075_U6], &device_id));
  TEST_ASSERT_EQUAL_HEX16(0x7500, device_id);
}

// TODO(Glibus): Fix this test when correct sensor
//  TEST_CASE("TMP1075 driver U7 read device ID", "[TMP1075_DRIVER]") {
//    uint16_t device_id;
//    TEST_ASSERT_TMP1075_DRIVER_OK(
//        tmp1075_driver_read_device_id(&tmp1075_drivers[TMP1075_U7],
//        &device_id));
//    TEST_ASSERT_EQUAL_HEX16(0x7500, device_id);
//  }

TEST_CASE("TMP1075 driver U6 read raw temperature", "[TMP1075_DRIVER]") {
  int16_t temperature;
  TEST_ASSERT_TMP1075_DRIVER_OK(tmp1075_driver_read_raw_temperature(
      &tmp1075_drivers[TMP1075_U6], &temperature));

  TEST_ASSERT_INT16_WITHIN(100, 7900, temperature);

  float temperature_celsius =
      tmp1075_driver_convert_raw_temperature_to_celsius(temperature);

  TEST_ASSERT_FLOAT_WITHIN(5, 30.0, temperature_celsius);
}
