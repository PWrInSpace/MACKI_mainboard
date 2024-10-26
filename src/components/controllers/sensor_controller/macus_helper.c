// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "macus_helper.h"

#include "uart_wrapper.h"

uart_comm_driver_config_t *macus_uart_driver;

static bool initialized = false;

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
  
  // TODO(glibus): idk but kinda sorta sync needed i think D:
  return uart_comm_driver_read(macus_uart_driver, data->data_points,
                               MACUS_DATA_SIZE, 1000);
}
