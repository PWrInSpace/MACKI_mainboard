// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "i2c_driver.h"

i2c_driver_status_t i2c_driver_init(i2c_driver_t *driver) {
  if (driver == NULL) {
    return I2C_DRIVER_ERROR;
  }

  i2c_param_config(driver->port, &driver->config);
  return i2c_driver_install(
             driver->port, driver->config.mode, (uint8_t)driver->rx_buf_setting,
             (uint8_t)driver->tx_buf_setting, NONE_ALLOC_FLAGS) == ESP_OK
             ? I2C_DRIVER_OK
             : I2C_DRIVER_ERROR;
}

i2c_driver_status_t i2c_driver_send_data(i2c_driver_t *driver,
                                         const uint8_t *data, size_t size,
                                         uint8_t device_address) {
  if (driver == NULL || data == NULL || size == 0) {
    return I2C_DRIVER_SEND_ERROR_INVALID_ARGS;
  }
  esp_err_t ret = i2c_master_write_to_device(driver->port, device_address, data,
                                             size, I2C_TIMEOUT_MS);
  return ret == ESP_OK ? I2C_DRIVER_OK : I2C_DRIVER_ERROR;
}

i2c_driver_status_t i2c_driver_receive_byte(i2c_driver_t *driver, uint8_t *data,
                                            uint8_t device_address,
                                            uint8_t reg_address) {
  if (driver == NULL || data == NULL) {
    return I2C_DRIVER_ERROR;
  }

  esp_err_t ret = i2c_master_read_from_device(driver->port, device_address,
                                              data, 1, I2C_TIMEOUT_MS);

  return ret == ESP_OK ? I2C_DRIVER_OK : I2C_DRIVER_ERROR;
}

i2c_driver_status_t i2c_driver_send_receive_data(i2c_driver_t *driver,
                                                 uint8_t *data_out,
                                                 size_t size_out,
                                                 uint8_t device_address,
                                                 uint8_t reg_address) {
  if (driver == NULL || data_out == NULL || size_out == 0) {
    return I2C_DRIVER_SEND_ERROR_INVALID_ARGS;
  }

  esp_err_t ret =
      i2c_master_write_read_device(driver->port, device_address, &reg_address,
                                   1, data_out, size_out, I2C_TIMEOUT_MS);

  return ret == ESP_OK ? I2C_DRIVER_OK : I2C_DRIVER_ERROR;
}

i2c_driver_status_t i2c_driver_deinit(i2c_driver_t *driver) {
  if (driver == NULL) {
    return I2C_DRIVER_ERROR;
  }

  return i2c_driver_delete(driver->port) == ESP_OK ? I2C_DRIVER_OK
                                                   : I2C_DRIVER_ERROR;
}
