// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "adc_driver.h"

#include "macki_log.h"

#define TAG "ADC_DRIVER"

adc_driver_status_t adc_driver_continuous_init(adc_driver_t *config) {
  adc_continuous_handle_t handle = NULL;

  if (config->initialized) {
    return ADC_DRIVER_OK;
  }

  if (config->channel_num == 0 ||
      sizeof(*config->channel) / sizeof(adc_channel_t) != config->channel_num) {
    return ADC_DRIVER_ERROR;
  }

  adc_continuous_handle_cfg_t adc_config = {
      .max_store_buf_size = config->buffer_size,
      .conv_frame_size = config->frame_size,
  };
  esp_err_t ret = adc_continuous_new_handle(&adc_config, &handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_INITIALIZATION_ERROR;
  }

  adc_continuous_config_t dig_cfg = {
      .sample_freq_hz = config->measurement_freq_hz,
      .conv_mode = config->conv_mode,
      .format = config->format,
  };

  adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
  dig_cfg.pattern_num = config->channel_num;
  for (int i = 0; i < config->channel_num; i++) {
    adc_pattern[i].atten = config->atten;
    adc_pattern[i].channel = config->channel[i] & 0x7;
    adc_pattern[i].unit = config->unit;
    adc_pattern[i].bit_width = config->bit_width;

    MACKI_LOG_INFO(TAG, "adc_pattern[%d].atten is :%" PRIx8, i,
                   adc_pattern[i].atten);
    MACKI_LOG_INFO(TAG, "adc_pattern[%d].channel is :%" PRIx8, i,
                   adc_pattern[i].channel);
    MACKI_LOG_INFO(TAG, "adc_pattern[%d].unit is :%" PRIx8, i,
                   adc_pattern[i].unit);
  }
  dig_cfg.adc_pattern = adc_pattern;
  ret = adc_continuous_config(handle, &dig_cfg);
  if (ret != ESP_OK) {
    return ADC_DRIVER_INITIALIZATION_ERROR;
  }

  config->initialized = true;
  *config->handle = handle;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_continuous_start(adc_driver_t *config) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }

  esp_err_t ret = adc_continuous_start(*config->handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_get_reading(adc_driver_t *config, uint8_t *buf,
                                           uint32_t length_max,
                                           uint32_t *out_length,
                                           uint32_t timeout_ms) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }

  esp_err_t ret = adc_continuous_read(*config->handle, buf, length_max,
                                      out_length, timeout_ms);
  if (ret == ESP_ERR_TIMEOUT) {
    return ADC_DRIVER_TIMEOUT_ERROR;
  } else if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_convert_reading(adc_driver_t *config,
                                               uint8_t *result,
                                               uint32_t *converted_result,
                                               uint32_t *converted_channel) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }

  adc_digi_output_data_t *p = (adc_digi_output_data_t *)result;
  uint32_t data = p->type2.data;
  uint32_t channel = p->type2.channel;
  *converted_result = data;
  *converted_channel = channel;
  return ADC_DRIVER_OK;
}

adc_driver_status_t adc_driver_continuous_deinit(adc_driver_t *config) {
  if (!config->initialized) {
    return ADC_DRIVER_ERROR;
  }

  esp_err_t ret = adc_continuous_deinit(*config->handle);
  if (ret != ESP_OK) {
    return ADC_DRIVER_ERROR;
  }
  return ADC_DRIVER_OK;
}
