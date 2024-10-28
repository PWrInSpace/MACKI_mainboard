// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lis2dw12_data_structs.h"
#include "ring_buffer.h"

#define SENSOR_DATA_RING_BUFFER_SIZE 64
#define SENSOR_DATA_SD_BUFFER_SIZE 256

typedef int64_t (*sensor_controller_get_procedure_time_ms_cb)(void);

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
  float lis2dw12_acc_x;
  float lis2dw12_acc_y;
  float lis2dw12_acc_z;
  int32_t left_motor_speed;
  int32_t right_motor_speed;
  int64_t procedure_time_ms;
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
bool sensor_controller_get_last_data(char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

/*!
 * @brief Reads and buffers the sensor data in a ring buffer.
 */
void read_and_buffer_sensor_data();

void read_and_save_macus_data();

void single_shot_data_header_to_string(char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

void continuous_data_header_to_string(char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

void single_shot_data_to_string(sensor_controller_single_shot_data_t data,
                                char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

void continuous_data_to_string(sensor_controller_continuous_data_t data,
                               char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

void continuous_data_to_string_all_data(
    sensor_controller_continuous_data_t data,
    char buffer[SENSOR_DATA_SD_BUFFER_SIZE * 2]);

void transmission_data_to_string(sensor_controller_data_transmission_t data,
                                 char buffer[SENSOR_DATA_SD_BUFFER_SIZE]);

void sensor_controller_save_data_to_sd();

void sensor_controller_print_header_on_sd();

size_t sensor_controller_get_ring_buffer_count();

void update_procedure_start_time(int64_t time_ms);

void tare_load_cell();