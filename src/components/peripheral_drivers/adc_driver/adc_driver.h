// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "esp_adc/adc_continuous.h"

typedef enum {
  ADC_DRIVER_OK = 0,
  ADC_DRIVER_ERROR,
  ADC_DRIVER_INITIALIZATION_ERROR,
  ADC_DRIVER_TIMEOUT_ERROR,
} adc_driver_status_t;

typedef struct {
  adc_continuous_handle_t *handle;
  size_t buffer_size;
  size_t frame_size;
  adc_digi_convert_mode_t conv_mode;
  size_t measurement_freq_hz;
  adc_digi_output_format_t format;
  adc_atten_t atten;
  adc_unit_t unit;
  uint8_t bit_width;
  uint8_t channel_num;
  adc_channel_t *channel;
  bool initialized;
} adc_driver_t;

adc_driver_status_t adc_driver_continuous_init(adc_driver_t *config);

adc_driver_status_t adc_driver_continuous_start(adc_driver_t *config);

adc_driver_status_t adc_driver_get_reading(adc_driver_t *config, uint8_t *buf,
                                           uint32_t length_max,
                                           uint32_t *out_length,
                                           uint32_t timeout_ms);

adc_driver_status_t adc_driver_convert_reading(adc_driver_t *config,
                                               uint8_t *result,
                                               uint32_t *converted_result,
                                               uint32_t *converted_channel);

adc_driver_status_t adc_driver_continuous_deinit(adc_driver_t *config);
