// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "sensor_controller.h"

#include "rtc_wrapper.h"

void sensor_controller_init() { rtc_wrapper_init(); }

sensor_controller_data_t sensor_controller_get_last_data() {
  static int16_t load_cell_reading = 0;
  sensor_controller_data_t data = {
      .time_us = rtc_wrapper_get_time_us(),
      .load_cell_reading = load_cell_reading++,
  };
  return data;
}
