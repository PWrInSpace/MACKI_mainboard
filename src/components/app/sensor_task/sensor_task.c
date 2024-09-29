// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "data_queues.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_sensor_defines.h"
#include "macki_log.h"

#define READINGS_NUM 512
#define TAG "SENSOR_TASK"

static struct {
  TaskHandle_t adc_task_handle;
  QueueHandle_t *sensor_data_queue;
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

static lis2dw12_driver_t lis2dw12_driver = {
    .address = 0b0011000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

static tmp1075_driver_t tmp1075_driver = {
    .address = 0b1001000,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

static ads1115_driver_t ads1115_driver = {
    .address = 0b1001001,
    .initiated = false,
    ._i2c_send_receive = &i2c_ic_send_receive_data,
    ._i2c_send = &i2c_ic_send_data,
};

void i2c_sensors_task(void *pvParameters) {
  init_i2c_driver();
  pd_context.sensor_data_queue = get_sensor_data_queue();

  lis2dw12_fifo_data_t fifo_data = {
      .current_samples_number = 0,
      .samples = {},
  };

  int16_t temperature = 0;
  float temperature_celsius = 0.0f;
  uint16_t adc_data;

  lis2dw12_driver_init(&lis2dw12_driver);
  tmp1075_driver_init(&tmp1075_driver);
  ads1115_driver_init(&ads1115_driver);
  vTaskDelay(pdMS_TO_TICKS(1000));

  while (1) {
    sensor_out_data_t sensor_data = {
        .lis2dw12_data = {0},
        .adc_data = {0},
        .temperature_data = 0.0f,
    };
    lis2dw12_driver_read_fifo_data(&lis2dw12_driver, &fifo_data);
    printf("Sample %d: x=%d, y=%d, z=%d\n", 0, fifo_data.samples[0].x,
           fifo_data.samples[0].y, fifo_data.samples[0].z);
    sensor_data.lis2dw12_data = fifo_data.samples[0];

    tmp1075_driver_read_raw_temperature(&tmp1075_driver, &temperature);
    temperature_celsius =
        tmp1075_driver_convert_raw_temperature_to_celsius(temperature);
    sensor_data.temperature_data = temperature_celsius;
    printf("Temperature: %f\n", temperature_celsius);
    for (int i = 0; i < ADC_EXPANDER_MUX_NUM; i++) {
      ads1115_select_analog_pin_t pin = MUX_AIN0_GND + i;
      ads1115_driver_select_pin(&ads1115_driver, pin);
      // This delay here to allow the old conversion to go through before
      // starting a new one on a different pin
      vTaskDelay(pdMS_TO_TICKS(100));
      ads1115_driver_get_conversion_data(&ads1115_driver, &adc_data);
      printf("ADC data[%d]: %d\n", i, adc_data);
      sensor_data.adc_data[i] = adc_data;
    }

    xQueueSend(*pd_context.sensor_data_queue, &sensor_data, portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
