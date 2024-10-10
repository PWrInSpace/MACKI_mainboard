// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct __attribute__((packed)) {
  int64_t time_us;
  int16_t load_cell_reading;
} sensor_controller_data_t;

typedef union {
  sensor_controller_data_t data;
  uint8_t raw[sizeof(sensor_controller_data_t)];
} sensor_controller_data_u;

/*!
 * @brief Initializes the sensor controller.
 */
void sensor_controller_init();

/*!
 * @brief Gets the last data from the sensor controller.
 * @return The last data from the sensor controller.
 */
sensor_controller_data_t sensor_controller_get_last_data();

// TODO(Glibus): maybe use ring buffer for this
void read_and_buffer_sensor_data(sensor_controller_data_t* data);
