// Copyright

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lis2dw12_data_structs.h"

#define ADC_EXPANDER_MUX_NUM 4

typedef struct __packed__ {
  lis2dw12_out_data_t lis2dw12_data;
  uint16_t adc_data[ADC_EXPANDER_MUX_NUM];
  float temperature_data;
} sensor_out_data_t;

QueueHandle_t* get_sensor_data_queue();
