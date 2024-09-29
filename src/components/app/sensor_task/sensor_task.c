// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "ads1115_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "macki_log.h"


#define READINGS_NUM 512
#define TAG "SENSOR_TASK"

static struct {
  TaskHandle_t adc_task_handle;
} pd_context;

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle,
                                     const adc_continuous_evt_data_t *edata,
                                     void *user_data) {
  BaseType_t mustYield = pdFALSE;
  // Notify that ADC continuous driver has done enough number of conversions
  vTaskNotifyGiveFromISR(pd_context.adc_task_handle, &mustYield);

  return (mustYield == pdTRUE);
}

void adc_sensor_task(void *pvParameters) {
  pd_context.adc_task_handle = xTaskGetCurrentTaskHandle();

  uint32_t readings_buf[READINGS_NUM] = {0};
  uint32_t readings_done = 0;
  adc_wrapper_init();
  adc_wrapper_register_callback(s_conv_done_cb);
  adc_wrapper_start();

  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    adc_wrapper_get_readings(readings_buf, READINGS_NUM, &readings_done);
    MACKI_LOG_TRACE(TAG, "Readings done: %d", readings_done);
    vTaskDelay(1);
  }
}

void i2c_sensors_task(void *pvParameters) {
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
