// Copyright 2024 MACKI, Krzysztof Gliwinski

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Tasks
#include "cli_task.h"
#include "logger_task.h"
#include "mechanical_task.h"
#include "procedure_task.h"
#include "sensor_task.h"
#include "shared_i2c_wrapper.h"

// Wrappers
#include "sd_card_wrapper.h"
#include "uart_wrapper.h"
#include "usb_cdc_interface.h"

#define TAG "MAIN"

void app_main(void) {
  vTaskDelay(pdMS_TO_TICKS(1000));

  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);

  sd_card_wrapper_init();
  uart_wrapper_init();
  init_shared_i2c_wrapper();

  // create freertos tasks
  xTaskCreatePinnedToCore(logger_task, "logger_task", 8192, NULL, 1, NULL, 0);

  vTaskDelay(pdMS_TO_TICKS(1000));

  xTaskCreatePinnedToCore(sensor_task, "sensor_task", 16384, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(mechanical_task, "mechanical_task", 8192, NULL, 1,
                          NULL, 1);
  xTaskCreatePinnedToCore(procedure_task, "procedure_task", 8192, NULL, 1, NULL,
                          1);
  cli_run();

  vTaskDelete(NULL);
}
