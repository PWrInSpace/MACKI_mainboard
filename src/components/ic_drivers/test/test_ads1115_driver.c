// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "ads1115_driver.h"
#include "shared_i2c_wrapper.h"

#define TEST_ASSERT_ADS1115_DRIVER_OK(condition) \
  TEST_ASSERT_EQUAL(ADS1115_DRIVER_OK, condition)

ads1115_driver_t ads1115_driver = {
    .address = 0b1001001,
    ._i2c_send_receive = &i2c_ic_send_receive_data,
    ._i2c_send = &i2c_ic_send_data,
    .initiated = false,
};

TEST_CASE("ADS1115 driver initialization", "[ADS1115_DRIVER]") {
  init_i2c_driver();

  TEST_ASSERT_ADS1115_DRIVER_OK(ads1115_driver_init(&ads1115_driver));
  TEST_ASSERT_TRUE(ads1115_driver.initiated);
}

static void test_select_and_read_pin(ads1115_select_analog_pin_t pin) {
  TEST_ASSERT_ADS1115_DRIVER_OK(
      ads1115_driver_select_pin(&ads1115_driver, pin));
  vTaskDelay(pdMS_TO_TICKS(50));
  uint16_t value;
  TEST_ASSERT_ADS1115_DRIVER_OK(
      ads1115_driver_get_conversion_data(&ads1115_driver, &value));
  printf("Pin %d value: %d\n", pin, value);
}

TEST_CASE("ADS1115 driver read A0 pin", "[ADS1115_DRIVER]") {
  test_select_and_read_pin(MUX_AIN0_GND);
}

TEST_CASE("ADS1115 driver read A1 pin", "[ADS1115_DRIVER]") {
  test_select_and_read_pin(MUX_AIN1_GND);
}

TEST_CASE("ADS1115 driver read A2 pin", "[ADS1115_DRIVER]") {
  test_select_and_read_pin(MUX_AIN2_GND);
}

TEST_CASE("ADS1115 driver read A3 pin", "[ADS1115_DRIVER]") {
  test_select_and_read_pin(MUX_AIN3_GND);
}

TEST_CASE("ADS1115 driver deinitialization", "[ADS1115_DRIVER]") {
  TEST_ASSERT_ADS1115_DRIVER_OK(ads1115_driver_deinit(&ads1115_driver));
  /// @note as this is the last test, deinit the i2c driver
  deinit_i2c_driver();
}
