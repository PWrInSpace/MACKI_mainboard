// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "ads1115_driver.h"

#include "safe_bitwise_shifts.h"

#define TAG "ADS1115_DRIVER"

static ads1115_driver_status_t ads1115_driver_write_reg(
    ads1115_driver_t *driver, uint8_t reg, uint16_t data) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  uint8_t data_to_send[2];
  uint8_from_uint16(data, data_to_send);
  if (driver->_i2c_send(data_to_send, sizeof(data_to_send), driver->address) ==
      false) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

static ads1115_driver_status_t ads1115_driver_read_reg(ads1115_driver_t *driver,
                                                       uint8_t reg,
                                                       uint16_t *data) {
  if (driver == NULL || data == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  uint8_t out_data[2];
  if (driver->_i2c_send_receive(out_data, 2, driver->address, reg) == false) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  *data = int16_from_uint8_bytes(out_data);
  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_init(ads1115_driver_t *driver) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    return ADS1115_DRIVER_OK;
  }

  ads1115_config_reg_t ads1115_config = {.os = 0,
                                         .mux = MUX_AIN0_GND,
                                         .pga = 0,
                                         .mode = driver->mode,
                                         .dr = 0,
                                         .comp_mode = 0,
                                         .comp_pol = 0,
                                         .comp_lat = 0,
                                         .comp_que = 0};

  ads1115_driver_status_t ret =
      ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, ads1115_config.raw);
  if (ret != ADS1115_DRIVER_OK) {
    driver->initiated = false;
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = true;
  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_deinit(ads1115_driver_t *driver) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    return ADS1115_DRIVER_OK;
  }

  ads1115_driver_status_t ret = ads1115_driver_write_reg(
      driver, ADS1115_REG_CONFIG, ADS1115_CONFIG_RESET);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = false;
  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_select_pin(
    ads1115_driver_t *driver, ads1115_select_analog_pin_t pin) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  ads1115_config_reg_t reg_setting;
  ads1115_driver_status_t ret =
      ads1115_driver_read_reg(driver, ADS1115_REG_CONFIG, &reg_setting.raw);

  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  reg_setting.mux = pin;

  ret = ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, reg_setting.raw);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_get_conversion_data(
    ads1115_driver_t *driver, uint16_t *data) {
  if (driver == NULL || data == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  ads1115_driver_status_t ret =
      ads1115_driver_read_reg(driver, ADS1115_REG_CONVERSION, data);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}
