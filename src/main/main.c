// Copyright 2022 PWrInSpace

#include "cli_task.h"
#include "cmd_parser.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logger_task.h"
#include "rtc_wrapper.h"
#include "sensor_task.h"
#include "sensor_controller.h"

#define TAG "MAIN"

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);
  char* data = "Hello world!";

  // create freertos task
  xTaskCreate(logger_task, "logger_task", 8192, NULL, 1, NULL);
  xTaskCreate(sensor_task, "sensor_task", 8192, NULL, 1, NULL);
  
  cli_run();

  vTaskDelete(NULL);
}
