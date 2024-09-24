// Copyright 2022 PWrInSpace

#include "cli_task.h"
#include "cmd_parser.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logger_task.h"
#include "rtc_wrapper.h"

#define TAG "MAIN"

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");
  vTaskDelay(pdMS_TO_TICKS(1000));
  // TODO(Gliwus): To be moved to init procedure
  rtc_wrapper_init();

  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);
  char* data = "Hello world!";

  // create freertos task
  xTaskCreate(logger_task, "logger_task", 8192, NULL, 1, NULL);

  esp_console_config_t console_config = {
      .max_cmdline_args = 8,
      .max_cmdline_length = 256,
  };
  cmd_register_common();
  cli_init(console_config.max_cmdline_length);
  cli_run();

  vTaskDelete(NULL);
}
