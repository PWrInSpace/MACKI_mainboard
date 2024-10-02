// Copyright 2024 MACKI

#include "tmp1075_driver.h"

#include <stdbool.h>

#include "tmp1075_registers.h"

#define TAG "TMP1075_DRIVER"

#define DATA_OUT_SIZE 2

tmp1075_driver_status_t tmp1075_driver_init(tmp1075_driver_t *driver) {
  if (driver == NULL) {
    return TMP1075_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    return TMP1075_DRIVER_OK;
  }

  static const uint8_t tmp1075_default_config_msb =
      (uint8_t)TMP1075_CFGR_OS_ENABLE | TMP1075_CFGR_CONVERSION_RATE_27_5 |
      TMP1075_CFGR_ALERT_CONSECUTIVE_UNUSED | TMP1075_CFGR_ALERT_POLARITY_LOW |
      TMP1075_CFGR_ALERT_MODE_COMPARATOR | TMP1075_CFGR_CONTINUOUS_CONVERSION;

  static uint8_t tmp1075_default_config[3] = {
      (uint8_t)TMP1075_CFGR_REG, tmp1075_default_config_msb, 0b00000000};

  if (driver->_send_data(tmp1075_default_config, sizeof(tmp1075_default_config),
                         driver->address) == false) {
    return TMP1075_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = true;
  return TMP1075_DRIVER_OK;
}

tmp1075_driver_status_t tmp1075_driver_deinit(tmp1075_driver_t *driver) {
  if (driver == NULL) {
    return TMP1075_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    return TMP1075_DRIVER_OK;
  }
  static const uint8_t tmp1075_shutdown_config_msb = 0b0000000;

  static uint8_t tmp1075_shutdown_config[3] = {(uint8_t)TMP1075_CFGR_REG,
                                               TMP1075_CFGR_OS_DISABLE,
                                               tmp1075_shutdown_config_msb};

  if (driver->_send_data(tmp1075_shutdown_config,
                         sizeof(tmp1075_shutdown_config),
                         driver->address) == false) {
    return TMP1075_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = false;
  return TMP1075_DRIVER_OK;
}

tmp1075_driver_status_t tmp1075_driver_read_device_id(tmp1075_driver_t *driver,
                                                      uint16_t *device_id) {
  if (driver == NULL || device_id == NULL) {
    return TMP1075_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    return TMP1075_DRIVER_UNINITIALIZED;
  }

  uint8_t data_out[DATA_OUT_SIZE] = {0};
  if (driver->_send_receive_data(data_out, DATA_OUT_SIZE, driver->address,
                                 TMP1075_DEVICE_ID_REG) == false) {
    return TMP1075_I2C_TRANSACTION_ERROR;
  }
  *device_id = (uint16_t)(data_out[0] << 8) | data_out[1];

  return TMP1075_DRIVER_OK;
}

tmp1075_driver_status_t tmp1075_driver_read_raw_temperature(
    tmp1075_driver_t *driver, int16_t *temperature) {
  if (driver == NULL || temperature == NULL) {
    return TMP1075_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    return TMP1075_DRIVER_UNINITIALIZED;
  }

  uint8_t data_out[DATA_OUT_SIZE] = {0};
  if (driver->_send_receive_data(data_out, DATA_OUT_SIZE, driver->address,
                                 TMP1075_TEMP_REG) == false) {
    return TMP1075_I2C_TRANSACTION_ERROR;
  }
  uint16_t temperature_u =
      (uint16_t)((data_out[0] << 8U) | data_out[1]) & TMP1075_TEMPERATURE_MASK;

  *temperature = (int16_t)temperature_u;
  return TMP1075_DRIVER_OK;
}

float tmp1075_driver_convert_raw_temperature_to_celsius(
    int16_t raw_temperature) {
  raw_temperature >>= 4;
  return (float)raw_temperature * TMP1075_TEMPERATURE_RESOLUTION_CELSIUS;
}
