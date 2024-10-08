// Copyright 2024 MACKI

#include "pca9574_driver.h"

#define TAG "PCA9574_DRIVER"

// TODO(Glibus): In all functions before they just write like 1 bit, they should
//  first read the register and mask the bit they want to change, then write the
//  register back

static pca9574_pin_mode_t _pca957_get_pins_pin_mode(
    pca9574_config_reg_t *config, pca9574_pins_num_t pin) {
  if (config == NULL) {
    return PCA9574_PIN_MODE_UNKNOWN;
  }

  return (config->raw & (1 << pin)) ? PCA9574_INPUT : PCA9574_OUTPUT;
}

pca957_driver_status_t pca957_driver_write_byte(pca957_driver_t *driver,
                                                uint8_t reg, uint8_t data) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }

  uint8_t data_to_send[2] = {reg, data};
  if (driver->_send_data(data_to_send, sizeof(data_to_send), driver->address) ==
      false) {
    return PCA957_I2C_TRANSACTION_ERROR;
  }

  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_read_byte(pca957_driver_t *driver,
                                               uint8_t reg, uint8_t *data) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }

  if (driver->_send_receive_data(data, 1, driver->address, reg) == false) {
    return PCA957_I2C_TRANSACTION_ERROR;
  }

  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_init(pca957_driver_t *driver) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    return PCA957_DRIVER_OK;
  }

  // Set the reset pin to high
  driver->_pin_set(driver->reset_pin, true);

  uint8_t input_port = 0;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_INPUT_PORT, &input_port);

  if (ret != PCA957_DRIVER_OK) {
    return ret;
  }

  ret = pca957_driver_set_mode(driver, driver->pin_config);
  if (ret != PCA957_DRIVER_OK) {
    return ret;
  }

  ret = pca957_driver_set_level(driver, PCA9574_LOW);
  driver->initiated = true;

  return ret;
}

pca957_driver_status_t pca957_driver_reset_ic(pca957_driver_t *driver) {
  driver->_pin_set(driver->reset_pin, false);
  driver->_delay(50);
  driver->_pin_set(driver->reset_pin, true);

  driver->initiated = false;
  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_deinit(pca957_driver_t *driver) {
  driver->initiated = false;
  return PCA957_DRIVER_OK;
}

pca957_driver_status_t pca957_driver_set_mode(pca957_driver_t *driver,
                                              pca9574_config_reg_t mode) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }

  pca957_driver_status_t ret =
      pca957_driver_write_byte(driver, PCA_9574_REG_CONFIGURATION, mode.raw);

  return ret;
}

pca957_driver_status_t pca957_driver_set_level(pca957_driver_t *driver,
                                               pca9574_pin_level_t level) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_OUTPUT_PORT, &reg);

  // We need to check which pins are output and which are input
  for (int i = 0; i < 8; i++) {
    if (_pca957_get_pins_pin_mode(&driver->pin_config, i) == PCA9574_OUTPUT) {
      if (level == PCA9574_LOW) {
        reg &= ~(1 << i);
      } else if (level == PCA9574_HIGH) {
        reg |= (1 << i);
      } else {
        return PCA957_DRIVER_OUT_OF_PIN_RANGE;
      }
    }
  }

  ret = pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);

  return ret;
}

pca957_driver_status_t pca957_driver_set_level_pin(pca957_driver_t *driver,
                                                   pca9574_pin_level_t level,
                                                   pca9574_pins_num_t pin) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_OUTPUT_PORT, &reg);

  if (ret != PCA957_DRIVER_OK) {
    return ret;
  }

  if (level == PCA9574_LOW) {
    reg &= ~(1 << pin);
  } else if (level == PCA9574_HIGH) {
    reg |= (1 << pin);
  } else {
    return PCA957_DRIVER_OUT_OF_PIN_RANGE;
  }

  ret |= pca957_driver_write_byte(driver, PCA_9574_REG_OUTPUT_PORT, reg);

  return ret;
}

pca957_driver_status_t pca957_driver_get_output_level(
    pca957_driver_t *driver, pca9574_config_reg_t *output_level) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_OUTPUT_PORT, &reg);

  if (ret != PCA957_DRIVER_OK) {
    return PCA957_DRIVER_ERROR;
  }

  output_level->raw = reg;
  return ret;
}

pca957_driver_status_t pca957_driver_get_input_level(
    pca957_driver_t *driver, pca9574_config_reg_t *input_level) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }
  uint8_t reg = 0x00;
  pca957_driver_status_t ret =
      pca957_driver_read_byte(driver, PCA_9574_REG_INPUT_PORT, &reg);

  if (ret != PCA957_DRIVER_OK) {
    return PCA957_DRIVER_ERROR;
  }

  input_level->raw = reg;
  return ret;
}

pca9574_pin_level_t pca957_driver_get_level_pin(pca957_driver_t *driver,
                                                pca9574_pins_num_t pin) {
  if (driver == NULL) {
    return PCA9574_PIN_MODE_UNKNOWN;
  }

  pca9574_pin_mode_t mode = _pca957_get_pins_pin_mode(&driver->pin_config, pin);
  if (mode == PCA9574_PIN_MODE_UNKNOWN) {
    return PCA9574_PIN_MODE_UNKNOWN;
  }

  pca9574_config_reg_t reg;
  reg.raw = 0x00;

  if (mode == PCA9574_OUTPUT) {
    pca957_driver_get_output_level(driver, &reg);
  } else {
    pca957_driver_get_input_level(driver, &reg);
  }

  return (reg.raw & (1 << pin)) ? PCA9574_HIGH : PCA9574_LOW;
}

pca957_driver_status_t pca957_driver_pull_enable(pca957_driver_t *driver) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }

  uint8_t reg = 0x00;
  pca957_driver_status_t ret = pca957_driver_read_byte(
      driver, PCA_9574_REG_BUS_PULL_UP_DOWN_ENABLE, &reg);

  if (ret != PCA957_DRIVER_OK) {
    return ret;
  }

  // Mask the register in the E0.1 bit
  reg |= 0x02;
  ret = pca957_driver_write_byte(driver, PCA_9574_REG_BUS_PULL_UP_DOWN_ENABLE,
                                 reg);

  return ret;
}

pca957_driver_status_t pca957_driver_set_pull(pca957_driver_t *driver,
                                              pca9574_pull_mode_t pull_mode,
                                              uint8_t pin_mask) {
  if (driver == NULL) {
    return PCA957_DRIVER_ERROR;
  }
  pca9574_config_reg_t reg;
  pca957_driver_status_t ret = pca957_driver_read_byte(
      driver, PCA_9574_REG_BUS_PULL_UP_DOWN_ENABLE, &reg.raw);

  if (ret != PCA957_DRIVER_OK) {
    return ret;
  }

  if (pull_mode == PCA9574_PULL_UP) {
    reg.raw |= pin_mask;
  } else if (pull_mode == PCA9574_PULL_DOWN) {
    reg.raw &= ~pin_mask;
  } else {
    return PCA957_DRIVER_ERROR;
  }
  ret = pca957_driver_write_byte(driver, PCA_9574_REG_BUS_PULL_UP_DOWN_ENABLE,
                                 reg.raw);

  return ret;
}
