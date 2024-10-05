// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "adc_driver.h"
#include "sdkconfig.h"
#include "unity.h"

static adc_oneshot_unit_init_cfg_t adc_init_config = {
    .unit_id = ADC_UNIT_1,
};

static adc_oneshot_chan_cfg_t adc_chan_config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
};

static adc_oneshot_unit_handle_t handle;
static adc_cali_handle_t cali_handle;
static adc_cali_curve_fitting_config_t cali_config = {
    .unit_id = ADC_UNIT_1,
    .chan = ADC_CHANNEL_5,
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
};

static adc_driver_t adc_driver = {
    .handle = &handle,
    .init_config = &adc_init_config,
    .chan_config = &adc_chan_config,
    .cali_handle = &cali_handle,
    .cali_config = &cali_config,
    .channel = ADC_CHANNEL_5,
    .initialized = false,
    .calibrated = false,
};

TEST_CASE("ADC driver initialization", "[adc]") {
  adc_driver_status_t status = adc_driver_init(&adc_driver);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
  TEST_ASSERT_TRUE(adc_driver.initialized);
}

TEST_CASE("ADC driver calibration initialization", "[adc]") {
  adc_driver_status_t status = adc_driver_calibration_init(&adc_driver);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
  TEST_ASSERT_TRUE(adc_driver.calibrated);
}

TEST_CASE("ADC driver get reading", "[adc]") {
  int32_t reading;
  adc_driver_status_t status = adc_driver_get_reading(&adc_driver, &reading);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
}

TEST_CASE("ADC driver get reading voltage", "[adc]") {
  int32_t reading_raw;
  adc_driver_get_reading(&adc_driver, &reading_raw);
  int32_t reading_voltage;
  adc_driver_status_t status = adc_driver_get_reading_voltage(
      &adc_driver, &reading_raw, &reading_voltage);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
  printf("Reading voltage: %ld\n", reading_voltage);
}

TEST_CASE("ADC driver deinitialization", "[adc]") {
  adc_driver_status_t status = adc_driver_calibration_deinit(&adc_driver);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
  status = adc_driver_deinit(&adc_driver);
  TEST_ASSERT_EQUAL(ADC_DRIVER_OK, status);
  TEST_ASSERT_FALSE(adc_driver.initialized);
  TEST_ASSERT_FALSE(adc_driver.calibrated);
}
