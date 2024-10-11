// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "ads1115_driver.h"

#define TAG "ADS1115_DRIVER"

static ads1115_driver_status_t ads1115_driver_write_reg(
    ads1115_driver_t *driver, uint8_t reg, uint16_t data) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  uint8_t data_to_send[3];
  data_to_send[0] = reg;
  data_to_send[2] = (uint8_t)(data & 0xFF);
  data_to_send[1] = (uint8_t)(data >> 8) & 0xFF;
  if (driver->_i2c_send(data_to_send, 3, driver->address) == false) {
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

  uint8_t data_to_send[1] = {reg};
  if (driver->_i2c_send(data_to_send, 1, driver->address) == false) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  uint8_t out_data[2];
  if (driver->_i2c_send_receive(out_data, 2, driver->address, reg) == false) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  *data = (out_data[0] << 8) | out_data[1];
  return ADS1115_DRIVER_OK;
}

static float ads1115_driver_pga_to_float(ads1115_config_pga_t pga) {
  switch (pga) {
    case ADS1115_PGA_6_144V:
      return 6.144f;
    case ADS1115_PGA_4_096V:
      return 4.096f;
    case ADS1115_PGA_2_048V:
      return 2.048f;
    case ADS1115_PGA_1_024V:
      return 1.024f;
    case ADS1115_PGA_0_512V:
      return 0.512f;
    case ADS1115_PGA_0_256V:
      return 0.256f;
    default:
      return 0.0f;
  }
}

ads1115_driver_status_t ads1115_driver_init(ads1115_driver_t *driver) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    return ADS1115_DRIVER_OK;
  }
  ads1115_config_reg_t reg_setting;
  ads1115_driver_read_reg(driver, ADS1115_REG_CONFIG, &reg_setting.raw);

  ads1115_config_reg_t ads1115_config = {.os = 0,
                                         .mux = MUX_AIN0_GND,
                                         .pga = driver->pga,
                                         .mode = driver->mode,
                                         .dr = 0b100,
                                         .comp_mode = 0,
                                         .comp_pol = 0,
                                         .comp_lat = 0,
                                         .comp_que = 0b11};

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

ads1115_driver_status_t ads1115_driver_start_continuous_conversion(
    ads1115_driver_t *driver) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  ads1115_config_reg_t reg_setting;
  ads1115_driver_status_t ret =
      ads1115_driver_read_reg(driver, ADS1115_REG_CONFIG, &reg_setting.raw);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  reg_setting.os = 1;

  ret = ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, reg_setting.raw);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  return ADS1115_DRIVER_OK;
}

ads1115_driver_status_t ads1115_driver_stop_continuous_conversion(
    ads1115_driver_t *driver) {
  if (driver == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  ads1115_config_reg_t reg_setting;
  ads1115_driver_status_t ret =
      ads1115_driver_read_reg(driver, ADS1115_REG_CONFIG, &reg_setting.raw);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  reg_setting.os = 0;

  ret = ads1115_driver_write_reg(driver, ADS1115_REG_CONFIG, reg_setting.raw);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

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

ads1115_driver_status_t ads1115_driver_get_conversion_data_millivolts(
    ads1115_driver_t *driver, float *data) {
  if (driver == NULL || data == NULL) {
    return ADS1115_DRIVER_ERROR;
  }

  uint16_t raw_conversion_data;
  ads1115_driver_status_t ret =
      ads1115_driver_get_conversion_data(driver, &raw_conversion_data);
  if (ret != ADS1115_DRIVER_OK) {
    return ADS1115_I2C_TRANSACTION_ERROR;
  }

  float pga = ads1115_driver_pga_to_float(driver->pga);

  *data = (float)raw_conversion_data * pga / 32768.0f;

  return ADS1115_DRIVER_OK;
}
