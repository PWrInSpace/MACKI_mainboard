// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "macus_helper.h"

#include <string.h>

#include "macki_log.h"
#include "rtc_wrapper.h"
#include "uart_wrapper.h"

#define TAG "MACUS"

uart_comm_driver_config_t* macus_uart_driver;

static bool initialized = false;

static uint8_t expected_sync[MACUS_SYNC_SIZE] = {0x01, 0x02, 0x03, 0x04};

macus_status_t macus_init() {
  macus_uart_driver = uart_wrapper_get_driver_instance(UART_INSTANCE_MACUS);
  if (macus_uart_driver == NULL) {
    return MACUS_STATUS_ERROR;
  }
  initialized = true;
  return MACUS_STATUS_OK;
}

macus_status_t macus_deinit() {
  if (!initialized) {
    return MACUS_STATUS_ERROR;
  }
  initialized = false;
  return MACUS_STATUS_OK;
}

macus_status_t macus_get_data(sensor_controller_macus_data_t* data) {
  if (!initialized) {
    return MACUS_STATUS_ERROR;
  }

  uint8_t macus_data[MACUS_DATA_WHOLE_FRAME_SIZE];

  uart_comm_driver_status_t ret = uart_comm_driver_read(
      macus_uart_driver, macus_data, MACUS_DATA_WHOLE_FRAME_SIZE, 10);

  // // print the whole received raw data to buffer
  // char buffer[1024];
  // snprintf(buffer, 1024, "Received data: ");
  // for (uint8_t i = 0; i < MACUS_DATA_WHOLE_FRAME_SIZE; i++) {
  //   snprintf(buffer + strlen(buffer), 1024 - strlen(buffer), "%02X ",
  //            macus_data[i]);
  // }

  // MACKI_LOG_INFO(TAG, "%s", buffer);

  if (ret != UART_COMM_DRIVER_STATUS_OK) {
    return MACUS_STATUS_ERROR;
  }
  for (uint8_t i = 0; i < MACUS_SYNC_SIZE; i++) {
    if (macus_data[i] != expected_sync[i]) {
      MACKI_LOG_INFO(TAG, "Sync received: %d, expected: %d",
                     data->data_points[i], expected_sync[i]);
      return MACUS_STATUS_NO_SYNC_ERROR;
    }
  }

  // We need to concatenate MACUS_DATA_BYTES_PER_POINT bytes into one uint32_t
  for (uint8_t i = 0; i < MACUS_DATA_POINTS_SIZE; i++) {
    data->data_points[i] = 0;
    for (uint8_t j = 0; j < MACUS_DATA_BYTES_PER_POINT; j++) {
      data->data_points[i] |=
          macus_data[i * MACUS_DATA_BYTES_PER_POINT + j + MACUS_SYNC_SIZE]
          << (8 * (2 - j));
    }
  }

  data->timestamp = rtc_wrapper_get_time_ms();
  return MACUS_STATUS_OK;
}

macus_status_t macus_get_buffered_frames(uint8_t* frames_count) {
  if (!initialized) {
    return MACUS_STATUS_ERROR;
  }
  size_t size = 0;
  uart_comm_driver_status_t ret =
      uart_comm_driver_get_buffered_data_len(macus_uart_driver, &size);
  if (ret != UART_COMM_DRIVER_STATUS_OK) {
    return MACUS_STATUS_ERROR;
  }

  *frames_count = (uint8_t)((size_t)size / (size_t)(MACUS_DATA_WHOLE_FRAME_SIZE));
  // MACKI_LOG_INFO(TAG, "Buffered size: %lu, frames_count: %d", size, *frames_count);

  return MACUS_STATUS_OK;
}

void macus_data_to_string(sensor_controller_macus_data_t data,
                          char buffer[MACUS_DATA_STRING_SIZE]) {
  snprintf(buffer, MACUS_DATA_STRING_SIZE, "%lld;", data.timestamp);
  for (uint8_t i = 0; i < MACUS_DATA_POINTS_SIZE; i++) {
    snprintf(buffer + strlen(buffer), MACUS_DATA_STRING_SIZE - strlen(buffer),
             "%lu;", data.data_points[i]);
  }
}

const char* macus_status_to_string(macus_status_t status) {
  switch (status) {
    case MACUS_STATUS_OK:
      return "MACUS_STATUS_OK";
    case MACUS_STATUS_ERROR:
      return "MACUS_STATUS_ERROR";
    case MACUS_STATUS_EMPTY_DATA_ERROR:
      return "MACUS_STATUS_EMPTY_DATA_ERROR";
    case MACUS_STATUS_NO_SYNC_ERROR:
      return "MACUS_STATUS_NO_SYNC_ERROR";
    default:
      return "UNKNOWN";
  }
}
