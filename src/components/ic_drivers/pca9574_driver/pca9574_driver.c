// Copyright 2024 MACKI

#include "pca9574_driver.h"

#include "pca9574_registers.h"
#include "macki_log.h"

#define TAG "PCA9574_DRIVER"

pca957_driver_status_t pca957_driver_write_byte(pca957_driver_t *driver,
                                                uint8_t reg, uint8_t data) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver write byte failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }

  uint8_t data_to_send[2] = {reg, data};
  if (driver->_send_data(data_to_send, sizeof(data_to_send), driver->address) ==
      false) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver write byte failed, I2C communication error");
    return PCA957_I2C_TRANSACTION_ERROR;
  }

  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_read_byte(pca957_driver_t *driver,
                                               uint8_t reg, uint8_t *data) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver read byte failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }

  if (driver->_send_receive_data(data, 1, driver->address, reg) == false) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver read byte failed, I2C communication error");
    return PCA957_I2C_TRANSACTION_ERROR;
  }

  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_init(pca957_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver initialization failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver already initialized");
    return PCA957_DRIVER_OK;
  }

  uint8_t input_port = 0;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_INPUT_PORT, &input_port);

  return ret;
}

pca957_driver_status_t pca957_driver_deinit(pca957_driver_t *driver) {
  (void)driver;
  // TODO(Glibus): Implement this function
  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_set_mode(pca957_driver_t *driver,
                                              pca9574_pin_mode_t mode) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver set mode failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }

  uint8_t reg = 0x00;
  if (mode == PCA9574_OUTPUT) {
    reg = 0x00;
  } else if (mode == PCA9574_INPUT) {
    reg = 0xFF;
  } else {
    MACKI_LOG_ERROR(TAG, "PCA957 driver set mode failed, invalid mode");
    return PCA957_DRIVER_ERROR;
  }

  pca957_driver_status_t ret =
      pca957_driver_write_byte(driver, PCA_9574_REG_CONFIGURATION, reg);

  // disable high impedance on pin
  if (mode == PCA9574_OUTPUT) {
    reg = 0x00;
    ret |= pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);
  }

  return ret;
}

pca957_driver_status_t pca957_driver_set_mode_pin(pca957_driver_t *driver,
                                                  pca9574_pin_mode_t mode,
                                                  uint8_t pin) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver set mode pin failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_OUTPUT_PORT, &reg);

  if (mode == PCA9574_OUTPUT) {
    reg &= ~(mode << pin);
  } else {
    reg |= (mode << pin);
  }

  ret |= pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);

  return ret;
}

pca957_driver_status_t pca957_driver_set_level(pca957_driver_t *driver,
                                               pca9574_pin_level_t level) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver set level failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  if (level == PCA9574_HIGH) {
    reg = 0xFF;
  }

  pca957_driver_status_t ret =
      pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);

  return ret;
}

pca957_driver_status_t pca957_driver_set_level_pin(pca957_driver_t *driver,
                                                   pca9574_pin_level_t level,
                                                   uint8_t pin) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "PCA957 driver set level pin failed, invalid args");
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_OUTPUT_PORT, &reg);

  if (level == PCA9574_LOW) {
    reg &= ~(1 << pin);
  } else {
    reg |= (1 << pin);
  }

  ret |= pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);

  return ret;
}
