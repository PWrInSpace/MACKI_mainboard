// Copyright


#define ADC_EXPANDER_MUX_NUM 4

#include "lis2dw12_data_structs.h"

typedef struct __packed__ {
  lis2dw12_out_data_t lis2dw12_data;
  int16_t adc_data[ADC_EXPANDER_MUX_NUM];
  float temperature_data;
} sensor_out_data_t;