// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "macus_helper.h"

#include "uart_wrapper.h"

uart_comm_driver_config_t *macus_uart_driver;

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

macus_status_t macus_get_data(sensor_controller_macus_data_t *data) {
  if (!initialized) {
    return MACUS_STATUS_ERROR;
  }

  uint8_t macus_data[MACUS_DATA_SIZE];

  uart_comm_driver_status_t ret = uart_comm_driver_read(
      macus_uart_driver, macus_data, MACUS_DATA_SIZE, 1000);
  if (ret != UART_COMM_DRIVER_STATUS_OK) {
    return MACUS_STATUS_ERROR;
  }
  for (uint8_t i = 0; i < MACUS_SYNC_SIZE; i++) {
    if (data->data_points[i] != expected_sync[i]) {
      return MACUS_STATUS_NO_SYNC_ERROR;
    }
  }
  for (uint8_t i = MACUS_SYNC_SIZE; i < MACUS_DATA_POINTS_SIZE; i++) {
    data->data_points[i - MACUS_SYNC_SIZE] = macus_data[i];
  }
  return MACUS_STATUS_OK;
}
