// Copyright 2022 PWrInSpace

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "usb_cdc_driver.h"
#include "usb_cdc_interface.h"

#include "log_receiver.h"
#include "log_manager.h"

#define TAG "MAIN"

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");
  vTaskDelay(pdMS_TO_TICKS(1000));
  // TODO(Gliwus): To be moved to init procedure

  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);
  char* data = "Hello world!";
  vTaskDelete(NULL);
}
