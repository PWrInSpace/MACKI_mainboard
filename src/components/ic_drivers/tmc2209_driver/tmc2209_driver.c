// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "tmc2209_driver.h"

#include "macki_log.h"
#include "tmc2209_registers.h"

#define TAG "TMC2209_DRIVER"

static tmc2209_driver_status_t _tmc2209_check_driver(tmc2209_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "Driver is NULL");
    return TMC2209_DRIVER_ERROR;
  }

  if (driver->initialized == false) {
    MACKI_LOG_ERROR(TAG, "Driver is not initialized");
    return TMC2209_DRIVER_UNINITIALIZED;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_init(tmc2209_driver_t *driver) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  driver->initialized = true;
  // TODO(Gliwus): Add initialization code - writing registers etc.
  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_enable(tmc2209_driver_t *driver) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  // Enable pin is active low
  if (driver->_enable_pin_set(driver->address, false) == false) {
    MACKI_LOG_ERROR(TAG, "Error during setting enable pin");
    return TMC2209_DRIVER_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_disable(tmc2209_driver_t *driver) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  // Enable pin is active low
  if (driver->_enable_pin_set(driver->address, true) == false) {
    MACKI_LOG_ERROR(TAG, "Error during setting enable pin");
    return TMC2209_DRIVER_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_write_register(tmc2209_driver_t *driver,
                                               uint8_t reg_address,
                                               uint32_t reg_value) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }
//   uint8_t data[5];
//   data[0] = reg_address | TMC2209_WRITE_BIT;
//   data[1] = (reg_value >> 0) & 0xFF;
//   data[2] = (reg_value >> 8) & 0xFF;
//   data[3] = (reg_value >> 16) & 0xFF;
//   data[4] = (reg_value >> 24) & 0xFF;

//   if (driver->_send_data(data, 5) == false) {
//     MACKI_LOG_ERROR(TAG, "Error during sending data");
//     return TMC2209_I2C_TRANSACTION_ERROR;
//   }

  return TMC2209_DRIVER_OK;
}
