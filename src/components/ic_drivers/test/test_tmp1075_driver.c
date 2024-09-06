// Copyright 2024 MACKI

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "shared_i2c_wrapper.h"
#include "tmp1075_driver.h"
#include "unity.h"

#define TEST_ASSERT_TMP1075_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(TMP1075_DRIVER_OK, condition)

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
    // TODO(GLibus): Uncomment address test when correct sensor is connected
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

// TODO(Glibus): Uncomment this test when correct sensor
// TEST_CASE("TMP1075 driver U7 initialization", "[TMP1075_DRIVER]") {
//   TEST_ASSERT_TMP1075_DRIVER_OK(
//       tmp1075_driver_init(&tmp1075_drivers[TMP1075_U7]));
// }

static void test_device_id_read(tmp1075_driver_index_t driver_index) {
  uint16_t device_id;
  TEST_ASSERT_TMP1075_DRIVER_OK(tmp1075_driver_read_device_id(
      &tmp1075_drivers[driver_index], &device_id));
  TEST_ASSERT_EQUAL_HEX16(0x7500, device_id);
}

TEST_CASE("TMP1075 driver U6 read device ID", "[TMP1075_DRIVER]") {
  test_device_id_read(TMP1075_U6);
}

// TODO(Glibus): Uncomment this test when correct sensor
// TEST_CASE("TMP1075 driver U7 read device ID", "[TMP1075_DRIVER]") {
//   test_device_id_read(TMP1075_U7);
// }

static void test_temperature_read(tmp1075_driver_index_t driver_index) {
  int16_t temperature;
  TEST_ASSERT_TMP1075_DRIVER_OK(tmp1075_driver_read_raw_temperature(
      &tmp1075_drivers[driver_index], &temperature));

  TEST_ASSERT_INT16_WITHIN(300, 7700, temperature);

  float temperature_celsius =
      tmp1075_driver_convert_raw_temperature_to_celsius(temperature);

  TEST_ASSERT_FLOAT_WITHIN(10., 25.0, temperature_celsius);
}

// TODO(Glibus): Uncomment this test when correct sensor
TEST_CASE("TMP1075 driver U6 read raw temperature", "[TMP1075_DRIVER]") {
  test_temperature_read(TMP1075_U6);
}

// TEST_CASE("TMP1075 driver U7 read raw temperature", "[TMP1075_DRIVER]") {
//   test_temperature_read(TMP1075_U7);
// }

// TODO(Glibus): Uncomment this test when correct sensor
TEST_CASE("TMP1075 driver deinitialization", "[TMP1075_DRIVER]") {
  TEST_ASSERT_TMP1075_DRIVER_OK(
      tmp1075_driver_deinit(&tmp1075_drivers[TMP1075_U6]));
  // TEST_ASSERT_TMP1075_DRIVER_OK(
  //     tmp1075_driver_deinit(&tmp1075_drivers[TMP1075_U7]));

  /// @note Also deinit wrapper as this is the last test
  deinit_i2c_driver();
}
