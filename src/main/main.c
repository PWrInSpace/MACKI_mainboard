// Copyright 2022 PWrInSpace

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#define TAG "MAIN"

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");
  vTaskDelay(pdMS_TO_TICKS(1000));

  vTaskDelete(NULL);
}
