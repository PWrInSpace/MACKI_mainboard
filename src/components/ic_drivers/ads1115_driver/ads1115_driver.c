// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "ads1115_driver.h"

#include "ads1115_registers.h"
#include "macki_log.h"
#include "safe_bitwise_shifts.h"

#define TAG "ADS1115_DRIVER"

ads1115_driver_status_t ads1115_driver_write_reg(ads1115_driver_t *driver,
                                                 uint8_t reg,
                                                 const uint8_t data[2]) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver write register failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }

  uint8_t data_to_send[3] = {reg, data[0], data[1]};
  if (driver->_i2c_send(data_to_send, sizeof(data_to_send), driver->address) ==
      false) {
    MACKI_LOG_ERROR(
        TAG, "ADS1115 driver write register failed, I2C communication error");
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_read_reg(ads1115_driver_t *driver,
                                                uint8_t reg, uint8_t *data,
                                                size_t data_size) {
  if (driver == NULL || data == NULL) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver read register failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->_i2c_send_receive(data, data_size, driver->address, reg) ==
      false) {
    MACKI_LOG_ERROR(
        TAG, "ADS1115 driver read register failed, I2C communication error");
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_init(ads1115_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver initialization failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver already initialized");
    return ADS1115_DRIVER_OK;
  }

  ads1115_driver_status_t ret = ads1115_driver_write_reg(
      driver, ADS1115_REG_CONFIG, ads1115_config_default);
  if (ret != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "ADS1115 driver initialization failed, I2C communication error");
    driver->initiated = false;
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = true;
  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_deinit(ads1115_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "ADS1115 driver deinitialization failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver already deinitialized");
    return ADS1115_DRIVER_OK;
  }

  ads1115_driver_status_t ret =
      ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, ads1115_power_down);
  if (ret != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "ADS1115 driver deinitialization failed, I2C communication error");
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = false;
  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_select_pin(
    ads1115_driver_t *driver, ads1115_select_analog_pin_t pin) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "ADS1115 driver select pin failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }
  uint8_t read_value[2];
  ads1115_driver_status_t ret =
      ads1115_driver_read_reg(driver, ADS1115_REG_CONFIG, read_value, 2);

  const uint8_t select_pin_config[2] = {0x00 | ((uint8_t)(pin << 4U)),
                                        read_value[1]};

  ret = ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, select_pin_config);
  if (ret != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "ADS1115 driver select pin failed, I2C communication error");
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_get_conversion_data(
    ads1115_driver_t *driver, int16_t *data) {
  if (driver == NULL || data == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "ADS1115 driver get conversion data failed, invalid args");
    return ADS1115_DRIVER_ERROR;
  }

  uint8_t conversion_data[2] = {0};
  ads1115_driver_status_t ret = ads1115_driver_read_reg(
      driver, ADS1115_REG_CONVERSION, conversion_data, 2);
  if (ret != ADS1115_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG,
        "ADS1115 driver get conversion data failed, I2C communication error");
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  *data = int16_from_uint8_bytes(conversion_data);
  return ADS1115_DRIVER_OK;
}
