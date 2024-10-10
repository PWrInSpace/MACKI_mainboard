// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "sensor_controller.h"

#include "macki_log.h"
#include "shared_i2c_wrapper.h"
#include "timeout_utility.h"
#include "vl53l0x_driver.h"

#define TAG "SENSOR_CONTROLLER"

typedef struct {
  vl53l0x_driver_t* vl53l0x_driver;
} sensor_controller_drivers_t;

void delay(size_t delay) { vTaskDelay(pdMS_TO_TICKS(delay)); }

static vl53l0x_driver_t distance_sensor = {
    .address = 0x29,
    .initialized = false,
    ._send_data = &i2c_ic_send_data,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._delay = &delay,
    ._start_timeout = &start_timeout,
    ._check_timeout_expired = &check_timeout_expired,
};

static sensor_controller_drivers_t sensor_controller_drivers = {
    .vl53l0x_driver = &distance_sensor,
};

void sensor_controller_init() {
  rtc_wrapper_init();
  init_shared_i2c_wrapper();
  init_vl53l0x(sensor_controller_drivers.vl53l0x_driver, 1);
}

sensor_controller_data_t sensor_controller_get_last_data() {
  static int16_t load_cell_reading = 0;
  sensor_controller_data_t data = {
      .time_us = rtc_wrapper_get_time_us(),
      .load_cell_reading = load_cell_reading++,
  };
  return data;
}

void read_and_buffer_sensor_data(sensor_controller_data_t* data) {
  // read single vl53l0x range measurement
  (void)data;
  stat_info_t stats;
  // data->time_us = rtc_wrapper_get_time_us();
  uint16_t distance = read_range_single_millimeters(
      sensor_controller_drivers.vl53l0x_driver, &stats);
  MACKI_LOG_INFO(TAG, "Distance: %d", distance);
  printf("Distance: %d\n", distance);
}
