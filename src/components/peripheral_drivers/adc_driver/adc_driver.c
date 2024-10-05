// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "adc_driver.h"

#define TAG "ADC_DRIVER"

adc_driver_status_t adc_driver_init(adc_driver_t *config) {
  if (config->initialized) {
    return ADC_DRIVER_OK;
  }

  adc_oneshot_unit_handle_t handle = NULL;
  esp_err_t ret = adc_oneshot_new_unit(config->init_config, &handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_INITIALIZATION_ERROR;
  }

  ret =
      adc_oneshot_config_channel(handle, config->channel, config->chan_config);

  *config->handle = handle;
  config->initialized = true;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_deinit(adc_driver_t *config) {
  if (!config->initialized) {
    return ADC_DRIVER_OK;
  }

  esp_err_t ret = adc_oneshot_del_unit(*config->handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }

  config->initialized = false;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_calibration_init(adc_driver_t *config) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }

  adc_cali_scheme_ver_t scheme_mask = ADC_CALI_SCHEME_VER_CURVE_FITTING;

  esp_err_t ret = adc_cali_check_scheme(&scheme_mask);
  if (ret != ESP_OK) {
    config->calibrated = false;
    return ADC_DRIVER_CALIBRATION_UNSUPPORTED;
  }

  adc_cali_handle_t handle = NULL;
  ret = adc_cali_create_scheme_curve_fitting(config->cali_config, &handle);

  printf("%d\n", ret);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }

  *config->cali_handle = handle;
  config->calibrated = true;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_calibration_deinit(adc_driver_t *config) {
  if (!config->initialized) {
    return ADC_DRIVER_OK;
  }

  esp_err_t ret = adc_cali_delete_scheme_curve_fitting(*config->cali_handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }

  config->calibrated = false;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_get_reading(adc_driver_t *config,
                                           int16_t *reading) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }
  int reading_raw;
  esp_err_t ret =
      adc_oneshot_read(*config->handle, config->channel, &reading_raw);
  *reading = (int16_t)reading_raw;
  return ret == ESP_OK ? ADC_DRIVER_OK : ADC_DRIVER_ERROR;
}

adc_driver_status_t adc_driver_get_reading_voltage(adc_driver_t *config,
                                                   int16_t *reading_raw,
                                                   int16_t *reading_voltage) {
  if (!config->calibrated) {
    return ADC_DRIVER_CALIBRATION_UNSUPPORTED;
  }

  int reading;
  esp_err_t ret =
      adc_cali_raw_to_voltage(*config->cali_handle, *reading_raw, &reading);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }

  *reading_voltage = (int16_t)reading;
  return ADC_DRIVER_OK;
}
