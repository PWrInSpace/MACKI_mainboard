// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "adc_wrapper.h"

#include "macki_log.h"

#define TAG "ADC_WRAPPER"

#define STRAIN_GAUGE_CHANNEL ADC_CHANNEL_5
#define READINGS_NUM 256

enum adc_channels_idx { ADC_CHANNEL_STRAIN_GAUGE = 0, ADC_CHANNEL_NUM = 1 };

static adc_channel_t adc_channels[ADC_CHANNEL_NUM] = {STRAIN_GAUGE_CHANNEL};

static adc_continuous_handle_t adc_handle = NULL;

static adc_driver_t adc_driver = {
    .handle = &adc_handle,
    .buffer_size = 1024,
    .frame_size = 256,
    .conv_mode = ADC_CONV_SINGLE_UNIT_1,
    .measurement_freq_hz = 20,
    .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    .atten = ADC_ATTEN_DB_0,
    .unit = ADC_UNIT_1,
    .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH,
    .channel_num = ADC_CHANNEL_NUM,
    .channel = adc_channels,
    .initialized = false,
};

bool adc_wrapper_init() {
  adc_driver_status_t status = adc_driver_continuous_init(&adc_driver);
  if (status != ADC_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Error during ADC driver initialization");
    return false;
  }
  return true;
}

bool adc_wrapper_register_callback(_adc_wrapper_conversion_done_cb cbs) {
  adc_continuous_evt_cbs_t adc_cbs = {
      .on_conv_done = cbs,
  };
  adc_driver_status_t status = adc_continuous_register_event_callbacks(
      *adc_driver.handle, &adc_cbs, NULL);
  if (status != ADC_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Error during ADC driver callback registration");
    return false;
  }
  return true;
}

bool adc_wrapper_start() {
  adc_driver_status_t status = adc_driver_continuous_start(&adc_driver);
  if (status != ADC_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Error during ADC driver start");
    return false;
  }
  return true;
}

bool adc_wrapper_get_readings(uint32_t *readings, size_t readings_num,
                              uint32_t *readings_done) {
  uint8_t buf[READINGS_NUM] = {0};
  uint32_t readings_out[READINGS_NUM] = {0};
  uint32_t out_length = 0;

  uint32_t actual_readings = 0;
  uint32_t channel = 0;
  while (1) {
    adc_driver_status_t status =
        adc_driver_get_reading(&adc_driver, buf, READINGS_NUM, &out_length, 0);

    if (status == ADC_DRIVER_OK) {
      for (uint32_t i = 0; i < out_length; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_driver_convert_reading(&adc_driver, &buf[i], &readings_out[i],
                                   &channel);
        if (channel < SOC_ADC_CHANNEL_NUM(adc_driver.unit)) {
          readings[actual_readings] = readings_out[i];
          actual_readings++;
        } else {
          MACKI_LOG_WARN(TAG, "Invalid channel number %d", channel);
        }
      }
    } else if (status == ADC_DRIVER_TIMEOUT_ERROR) {
      break;
    }
  }

  *readings_done = actual_readings;
  return true;
}

adc_driver_t *adc_wrapper_get_instance() { return &adc_driver; }

bool adc_wrapper_deinit() {
  adc_driver_status_t status = adc_driver_continuous_deinit(&adc_driver);
  if (status != ADC_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Error during ADC driver deinitialization");
    return false;
  }
  return true;
}
