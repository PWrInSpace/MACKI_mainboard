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
  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);

  if (sd_card_wrapper_init() != SD_CARD_WRAPPER_OK) {
    ESP_LOGE(TAG, "Failed to initialize SD card wrapper");
    esp_restart();
  }
  uart_wrapper_init();
  init_shared_i2c_wrapper();

  // create freertos tasks
  xTaskCreatePinnedToCore(logger_task, "logger_task", 8192, NULL, 3, NULL, 1);

  xTaskCreatePinnedToCore(mechanical_task, "mechanical_task", 8192, NULL, 4,
                          NULL, 0);

  vTaskDelay(pdMS_TO_TICKS(1000));

  xTaskCreatePinnedToCore(sensor_task, "sensor_task", 16384, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(sensor_save_task, "sensor_save_task", 8192, NULL, 3,
                          NULL, 1);

  xTaskCreatePinnedToCore(mechanical_sd_task, "mechaical_sd_task", 8192, NULL,
                          2, NULL, 0);
  xTaskCreatePinnedToCore(procedure_task, "procedure_task", 8192, NULL, 2, NULL,
                          0);
  xTaskCreatePinnedToCore(macus_task, "macus_task", 8192, NULL, 2, NULL, 0);

  cli_run();

  vTaskDelete(NULL);
}
