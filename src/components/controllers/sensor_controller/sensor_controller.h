// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lis2dw12_data_structs.h"
#include "ring_buffer.h"

typedef struct {
  int64_t time_us;
  float load_cell_reading;
  float tmp1075_temperature;
  float pressure_sensor_1;
  float pressure_sensor_2;
  uint16_t distance;
} sensor_controller_single_shot_data_t;

typedef struct {
  int64_t time_us;
  lis2dw12_fifo_data_t accelerometer_data;
} sensor_controller_continuous_data_t;

typedef struct {
  sensor_controller_single_shot_data_t single_shot_data;
  sensor_controller_continuous_data_t continuous_data;
} sensor_controller_data_t;

typedef struct __attribute__((packed)) {
  // Single shot data
  int64_t time_us;
  float load_cell_reading;
  float tmp1075_temperature;
  float pressure_sensor_1;
  float pressure_sensor_2;
  uint16_t distance;
  // Continuous data
  // TODO(Glibus): change this to float acceleration
  int16_t lis2dw12_acc_x;
  int16_t lis2dw12_acc_y;
  int16_t lis2dw12_acc_z;
} sensor_controller_data_transmission_t;

typedef union {
  sensor_controller_data_transmission_t data;
  uint8_t raw[sizeof(sensor_controller_data_transmission_t)];
} sensor_controller_data_u;

/*!
 * @brief Initializes the sensor controller.
 */
bool sensor_controller_init();

/*!
 * @brief Gets the last data from the sensor controller.
 * @return The last data from the sensor controller.
 */
sensor_controller_data_transmission_t sensor_controller_get_last_data();

/*!
 * @brief Reads and buffers the sensor data in a ring buffer.
 */
void read_and_buffer_sensor_data();

void single_shot_data_header_to_string(char buffer[256]);

void continuous_data_header_to_string(char buffer[256]);

void single_shot_data_to_string(sensor_controller_single_shot_data_t data,
                                char buffer[256]);

void continuous_data_to_string(sensor_controller_continuous_data_t data, char buffer[256]);
