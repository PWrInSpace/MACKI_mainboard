// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "sensor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_sensor_defines.h"
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

  lis2dw12_fifo_data_t fifo_data = {
      .current_samples_number = 0,
      .samples = {},
  };

  int16_t temperature = 0;
  float temperature_celsius = 0.0f;
  int16_t adc_data;

  lis2dw12_driver_init(&lis2dw12_driver);
  tmp1075_driver_init(&tmp1075_driver);
  ads1115_driver_init(&ads1115_driver);
  vTaskDelay(pdMS_TO_TICKS(1000));

  while (1) {
    lis2dw12_driver_read_fifo_data(&lis2dw12_driver, &fifo_data);
    printf("Sample %d: x=%d, y=%d, z=%d\n", 0, fifo_data.samples[0].x,
           fifo_data.samples[0].y, fifo_data.samples[0].z);
    tmp1075_driver_read_raw_temperature(&tmp1075_driver, &temperature);
    temperature_celsius =
        tmp1075_driver_convert_raw_temperature_to_celsius(temperature);
    printf("Temperature: %f\n", temperature_celsius);
    for (int i = 0; i < 4; i++) {
      ads1115_select_analog_pin_t pin = MUX_AIN0_GND + i;
      ads1115_driver_select_pin(&ads1115_driver, pin);
      // This delay here to allow the old conversion to go through before
      // starting a new one on a different pin
      vTaskDelay(pdMS_TO_TICKS(100));
      ads1115_driver_get_conversion_data(&ads1115_driver, &adc_data);
      printf("ADC data: %d\n", adc_data);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
