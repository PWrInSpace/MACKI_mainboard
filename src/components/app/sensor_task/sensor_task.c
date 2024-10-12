// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "macki_log.h"
#include "sensor_controller.h"

#define TAG "SENSOR_TASK"

static struct {
  TaskHandle_t adc_task_handle;
} pd_context;

void sensor_task(void *pvParameters) {
  pd_context.adc_task_handle = xTaskGetCurrentTaskHandle();
  bool ret = sensor_controller_init();

  if (!ret) {
    MACKI_LOG_ERROR(TAG,
                    "Failed to initialize Sensor controller! Exiting task...");
    vTaskDelete(NULL);
  }
  while (1) {
    read_and_buffer_sensor_data();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
