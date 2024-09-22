// Copyright 2024 MACKI, Krzysztof Gliwinski
#pragma once

#include <stdint.h>

#define MAX_FIFO_SAMPLES_NUM 32

/*!
 * @brief Structure to store the output data from the LIS2DW12 sensor
 * @param x - X-axis data
 * @param y - Y-axis data
 * @param z - Z-axis data
 * @note Data is 16-bit signed integer, but the sensor returns 12-bit or 14-bit
 * data depending on the low/high power mode
 */
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} lis2dw12_out_data_t;

/*!
 * @brief Structure to store the FIFO data from the LIS2DW12 sensor
 * @param current_samples_number - On fifo reading the current_samples_number is
 * updated with the number of samples read from the FIFO
 * @param samples - Array of samples read from the FIFO
 */
typedef struct {
  uint8_t current_samples_number;
  lis2dw12_out_data_t samples[MAX_FIFO_SAMPLES_NUM];
} lis2dw12_fifo_data_t;

#define LIS2DW12_LOW_POWER_MASK 0x0FFF
#define LIS2DW12_HIGH_POWER_MASK 0x3FFF
