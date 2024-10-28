// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "macki_log.h"
#include "macus_helper.h"
#include "sd_card_wrapper.h"
#include "sensor_controller.h"

#define TAG "SENSOR_TASK"

#define SENSOR_SAVE_ON_SD_THRESHOLD 16

void sensor_task(void* pvParameters) {
  bool ret = sensor_controller_init();

  sensor_controller_print_header_on_sd();

  if (!ret) {
    MACKI_LOG_ERROR(TAG,
                    "Failed to initialize Sensor controller! Exiting task...");
    vTaskDelete(NULL);
  }
  while (1) {
    read_and_buffer_sensor_data();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void sensor_save_task(void* pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(1000));
  while (1) {
    size_t buffer_count = sensor_controller_get_ring_buffer_count();
    if (buffer_count >
        SENSOR_SAVE_ON_SD_THRESHOLD) {
      sensor_controller_save_data_to_sd();
    }
    if(buffer_count == SENSOR_DATA_RING_BUFFER_SIZE) {
      MACKI_LOG_ERROR(TAG, "Sensor data buffer is full! Data overwritten");
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void macus_task(void* pvParameters) {
  macus_status_t ret = macus_init();
  if (ret != MACUS_STATUS_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize MACUS! Exiting task...");
    vTaskDelete(NULL);
  }

  while (1) {
    read_and_save_macus_data();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
