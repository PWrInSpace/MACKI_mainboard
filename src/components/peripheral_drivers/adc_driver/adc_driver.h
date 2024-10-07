// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

typedef enum {
  ADC_DRIVER_OK = 0,
  ADC_DRIVER_ERROR,
  ADC_DRIVER_INITIALIZATION_ERROR,
  ADC_DRIVER_CALIBRATION_UNSUPPORTED,
  ADC_DRIVER_TIMEOUT_ERROR,
} adc_driver_status_t;

typedef struct {
  adc_oneshot_unit_handle_t *handle;
  adc_oneshot_unit_init_cfg_t *init_config;
  adc_oneshot_chan_cfg_t *chan_config;
  adc_channel_t channel;
  adc_cali_handle_t *cali_handle;
  adc_cali_curve_fitting_config_t *cali_config;
  bool initialized;
  bool calibrated;
} adc_driver_t;

adc_driver_status_t adc_driver_init(adc_driver_t *config);

adc_driver_status_t adc_driver_deinit(adc_driver_t *config);

adc_driver_status_t adc_driver_calibration_init(adc_driver_t *config);

adc_driver_status_t adc_driver_calibration_deinit(adc_driver_t *config);

adc_driver_status_t adc_driver_get_reading(adc_driver_t *config,
                                           int16_t *reading);

adc_driver_status_t adc_driver_get_reading_voltage(adc_driver_t *config,
                                                   int16_t *reading_raw,
                                                   int16_t *reading_voltage);
