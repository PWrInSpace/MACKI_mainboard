// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "rtc_wrapper.h"
#include "sdkconfig.h"
#include "shared_i2c_wrapper.h"
#include "unity.h"
#include "vl53l0x_driver.h"

static int64_t g_current_timeout_len_ms = 0;
static int64_t g_timeout_start_ms = 0;

void start_timeout(uint16_t timeout) {
  g_timeout_start_ms = rtc_wrapper_get_time_ms();
  g_current_timeout_len_ms = timeout;
}

bool check_timeout_expired(uint16_t timeout) {
  int64_t current_time = rtc_wrapper_get_time_ms();
  return (current_time - g_timeout_start_ms) > timeout;
}

void delay(size_t delay) { vTaskDelay(pdMS_TO_TICKS(delay)); }

vl53l0x_driver_t vl53l0x_driver = {
    .address = 0x29,
    .initialized = false,
    ._send_data = &i2c_ic_send_data,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._delay = &delay,
    ._start_timeout = &start_timeout,
    ._check_timeout_expired = &check_timeout_expired,
};

TEST_CASE("VL53L0X driver initialization", "[VL53L0X_DRIVER]") {
  init_i2c_driver();
  rtc_wrapper_init();
  TEST_ASSERT_TRUE(init_vl53l0x(&vl53l0x_driver, 1));
}

TEST_CASE("VL53L0X driver single range measurement", "[VL53L0X_DRIVER]") {
  stat_info_t stats;
  TEST_ASSERT_UINT16_WITHIN(
      400, 400, read_range_single_millimeters(&vl53l0x_driver, &stats));
}

TEST_CASE("VL53L0X driver deinitialization", "[VL53L0X_DRIVER]") {
  deinit_i2c_driver();
}
