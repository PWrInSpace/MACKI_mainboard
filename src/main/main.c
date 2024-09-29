// Copyright 2022 PWrInSpace

#include "cli_task.h"
#include "cmd_parser.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_wrapper.h"
#include "logger_task.h"
#include "motor_task.h"
#include "rtc_wrapper.h"
#include "sensor_task.h"

#define TAG "MAIN"

#define STATUS_CHECK(status) \
  if (status != true) {    \
    ESP_LOGE(TAG, "Error");  \
  }

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");
  vTaskDelay(pdMS_TO_TICKS(1000));
  // TODO(Gliwus): To be moved to init procedure
  rtc_wrapper_init();

  // pin_config_t pin_config = {.pin_number = 8, .mode = GPIO_OUTPUT};
  // pin_config_t pin_config2 = {.pin_number = 18, .mode = GPIO_OUTPUT};
  // STATUS_CHECK(gpio_pin_config_output(&pin_config, false, false));
  // STATUS_CHECK(gpio_pin_config_output(&pin_config2, false, false));

  // STATUS_CHECK(gpio_pin_set_level(&pin_config, GPIO_LEVEL_HIGH));
  // STATUS_CHECK(gpio_pin_set_level(&pin_config2, GPIO_LEVEL_HIGH));

  // vTaskDelay(pdMS_TO_TICKS(1000));

  // STATUS_CHECK(gpio_pin_set_level(&pin_config, GPIO_LEVEL_LOW));
  // STATUS_CHECK(gpio_pin_set_level(&pin_config2, GPIO_LEVEL_LOW));

  usb_cdc_interface_init();
  usb_cdc_register_rx_callback(&tinyusb_cdc_line_state_changed_callback);
  char* data = "Hello world!";

  // create freertos task
  xTaskCreate(logger_task, "logger_task", 8192, NULL, 2, NULL);
  // xTaskCreate(adc_sensor_task, "sensor_task", 8192, NULL, 1, NULL);
  xTaskCreate(motor_task, "motor_task", 8192, NULL, 2, NULL);
  // xTaskCreate(i2c_sensors_task, "cli_task", 8192, NULL, 2, NULL);
  cli_run();

  vTaskDelete(NULL);
}
