// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "ads1115_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "macki_log.h"

#define TAG "SENSOR_TASK"

static struct {
  TaskHandle_t adc_task_handle;
} pd_context;

void sensor_task(void *pvParameters) {
  pd_context.adc_task_handle = xTaskGetCurrentTaskHandle();

  bool ret = adc_wrapper_init();
  if (!ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize ADC wrapper");
    printf("Failed to initialize ADC wrapper\n");
    return;
  }
  int16_t reading_voltage;
  while (1) {
    if (adc_wrapper_get_reading(ADC_STRAIN_GAUGE, &reading_voltage)) {
      MACKI_LOG_INFO(TAG, "Reading: %d", reading_voltage);
      printf("Reading: %d\n", reading_voltage);
    } else {
      MACKI_LOG_ERROR(TAG, "Failed to get reading");
      printf("Failed to get reading\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void i2c_sensors_task(void *pvParameters) {
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
