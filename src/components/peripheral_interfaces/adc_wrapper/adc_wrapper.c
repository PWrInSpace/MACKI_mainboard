// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "adc_wrapper.h"

#include "macki_log.h"

#define TAG "ADC_WRAPPER"

static adc_oneshot_unit_init_cfg_t adc_init_config[ADC_MAX_NUM] = {
    [ADC_STRAIN_GAUGE] =
        {
            .unit_id = ADC_UNIT_1,
        },
};

static adc_oneshot_chan_cfg_t adc_chan_config[ADC_MAX_NUM] = {
    [ADC_STRAIN_GAUGE] =
        {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_12,
        },
};

static adc_oneshot_unit_handle_t handle[ADC_MAX_NUM];
static adc_cali_handle_t cali_handle[ADC_MAX_NUM];
static adc_cali_curve_fitting_config_t cali_config[ADC_MAX_NUM] = {
    [ADC_STRAIN_GAUGE] =
        {
            .unit_id = ADC_UNIT_1,
            .chan = ADC_CHANNEL_5,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        },
};

static adc_driver_t adc_driver[ADC_MAX_NUM] = {
    [ADC_STRAIN_GAUGE] =
        {
            .handle = &handle[ADC_STRAIN_GAUGE],
            .init_config = &adc_init_config[ADC_STRAIN_GAUGE],
            .chan_config = &adc_chan_config[ADC_STRAIN_GAUGE],
            .cali_handle = &cali_handle[ADC_STRAIN_GAUGE],
            .cali_config = &cali_config[ADC_STRAIN_GAUGE],
            .channel = ADC_CHANNEL_5,
            .initialized = false,
            .calibrated = false,
        },
};

bool adc_wrapper_init() {
  for (int i = 0; i < ADC_MAX_NUM; i++) {
    adc_driver_status_t status = adc_driver_init(&adc_driver[i]);
    if (status != ADC_DRIVER_OK) {
      return false;
    }
    status = adc_driver_calibration_init(&adc_driver[i]);
    if (status != ADC_DRIVER_OK) {
      return false;
    }
  }
  return true;
}

bool adc_wrapper_get_reading(adc_wrapper_instance_t instance,
                             int16_t *reading) {
  if (instance >= ADC_MAX_NUM) {
    return false;
  }

  int16_t reading_raw;
  adc_driver_status_t status =
      adc_driver_get_reading(&adc_driver[instance], &reading_raw);
  if (status != ADC_DRIVER_OK) {
    return false;
  }

  status = adc_driver_get_reading_voltage(&adc_driver[instance], &reading_raw,
                                          reading);
  if (status != ADC_DRIVER_OK) {
    return false;
  }

  return true;
}

adc_driver_t *adc_wrapper_get_instance(adc_wrapper_instance_t instance) {
  if (instance >= ADC_MAX_NUM) {
    return NULL;
  }

  return &adc_driver[instance];
}

bool adc_wrapper_deinit() {
  for (int i = 0; i < ADC_MAX_NUM; i++) {
    adc_driver_status_t status = adc_driver_deinit(&adc_driver[i]);
    if (status != ADC_DRIVER_OK) {
      return false;
    }
  }
  return true;
}
