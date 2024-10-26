// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MACUS_DATA_SIZE 32

typedef enum {
  MACUS_STATUS_OK,
  MACUS_STATUS_ERROR,
  MACUS_STATUS_EMPTY_DATA_ERROR,
} macus_status_t;

typedef struct {
  uint8_t data_points[MACUS_DATA_SIZE];
} sensor_controller_macus_data_t;

/*!
 * @brief Initialize the MACUS sensor controller.
 *
 * @return Status of the initialization.
 */
macus_status_t macus_init();

/*!
 * @brief Deinitialize the MACUS sensor controller.
 *
 * @return Status of the deinitialization.
 */
macus_status_t macus_deinit();

/*!
 * @brief Get MACUS data.
 *
 * @param[out] data Pointer to the buffer where the data will be stored.
 * @return Status of the operation.
 */
macus_status_t macus_get_single_data_array(
    sensor_controller_macus_data_t *data);
