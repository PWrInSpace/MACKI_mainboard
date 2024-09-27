// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "solenoid_driver.h"

#include "macki_log.h"

#define TAG "SOLENOID_DRIVER"

solenoid_driver_status_t solenoid_driver_init(solenoid_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "Driver is NULL");
    return SOLENOID_DRIVER_ERROR;
  }

  driver->initialized = true;
  driver->current_state = SOLENOID_UNKNOWN;
  solenoid_driver_close(driver);
  return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_open(solenoid_driver_t *driver) {
  if (driver == NULL || !driver->initialized) {
    MACKI_LOG_ERROR(TAG, "Driver is not initialized");
    return SOLENOID_DRIVER_ERROR;
  }

  if (!driver->_set_pin(driver->gpio_pin, true)) {
    MACKI_LOG_ERROR(TAG, "Failed to open solenoid");
    return SOLENOID_DRIVER_ERROR;
  }
  driver->current_state = SOLENOID_OPEN;
  return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_close(solenoid_driver_t *driver) {
  if (driver == NULL || !driver->initialized) {
    MACKI_LOG_ERROR(TAG, "Driver is not initialized");
    return SOLENOID_DRIVER_ERROR;
  }

  if (!driver->_set_pin(driver->gpio_pin, false)) {
    MACKI_LOG_ERROR(TAG, "Failed to close solenoid");
    return SOLENOID_DRIVER_ERROR;
  }
  driver->current_state = SOLENOID_CLOSED;
  return SOLENOID_DRIVER_OK;
}

solenoid_state_t solenoid_driver_get_state(solenoid_driver_t *driver) {
  if (driver == NULL || !driver->initialized) {
    MACKI_LOG_ERROR(TAG, "Driver is not initialized");
    return SOLENOID_UNKNOWN;
  }

  return driver->current_state;
}
