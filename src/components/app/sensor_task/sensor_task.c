// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "macki_log.h"
#include "sensor_controller.h"

#define TAG "SENSOR_TASK"

#define SENSOR_SAVE_ON_SD_THRESHOLD SENSOR_DATA_RING_BUFFER_SIZE / 2

void sensor_task(void *pvParameters) {
  bool ret = sensor_controller_init();

  sensor_controller_print_header_on_sd();

  if (!ret) {
    MACKI_LOG_ERROR(TAG,
                    "Failed to initialize Sensor controller! Exiting task...");
    vTaskDelete(NULL);
  }
  while (1) {
    read_and_buffer_sensor_data();
    vTaskDelay(pdMS_TO_TICKS(20));
    if (sensor_controller_get_ring_buffer_count() >
        SENSOR_SAVE_ON_SD_THRESHOLD) {
      sensor_controller_save_data_to_sd();
    }
  }
}
