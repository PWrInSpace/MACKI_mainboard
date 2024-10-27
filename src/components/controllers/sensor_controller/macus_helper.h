// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MACUS_SYNC_SIZE 4
#define MACUS_DATA_POINTS_SIZE 32
#define MACUS_DATA_BYTES_PER_POINT 3
#define MACUS_RAW_DATA_POINTS_SIZE \
  MACUS_DATA_POINTS_SIZE * MACUS_DATA_BYTES_PER_POINT

// Macus data size - 4 bytes for the sync + 32 * 3 bytes
#define MACUS_DATA_WHOLE_FRAME_SIZE MACUS_SYNC_SIZE + MACUS_RAW_DATA_POINTS_SIZE

#define MACUS_DATA_STRING_SIZE 512

typedef enum {
  MACUS_STATUS_OK,
  MACUS_STATUS_ERROR,
  MACUS_STATUS_EMPTY_DATA_ERROR,
  MACUS_STATUS_NO_SYNC_ERROR,
} macus_status_t;

typedef struct {
  int64_t timestamp;
  uint32_t data_points[MACUS_DATA_POINTS_SIZE];
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
macus_status_t macus_get_data(sensor_controller_macus_data_t* data);

/*!
 * @brief Get the number of buffered frames on uart RX.
 *
 * @param[out] frames_count Number of buffered frames.
 * @return Status of the operation.
 */
macus_status_t macus_get_buffered_frames(uint8_t* frames_count);

/*!
 * @brief Convert MACUS data to string.
 *
 * @param[in] data MACUS data.
 * @param[out] buffer Buffer where the string will be stored.
 */
void macus_data_to_string(sensor_controller_macus_data_t data,
                          char buffer[MACUS_DATA_STRING_SIZE]);

const char* macus_status_to_string(macus_status_t status);
