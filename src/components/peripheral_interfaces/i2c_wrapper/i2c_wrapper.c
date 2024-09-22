// Copyright 2024 MACKI

#include "i2c_wrapper.h"

#include "macki_log.h"

#define TAG "I2C_WRAPPER"

static i2c_driver_t driver[I2C_MAX_INSTANCES_NUM] = {
    [I2C_INSTANCE_MAIN] = {
        .config =
            {
                .mode = I2C_MODE_MASTER,
                .sda_io_num = CONFIG_I2C_SDA_PIN_NUM,
                .scl_io_num = CONFIG_I2C_SCL_PIN_NUM,
                .sda_pullup_en = GPIO_PULLUP_ENABLE,
                .scl_pullup_en = GPIO_PULLUP_ENABLE,
                .master.clk_speed = 100000,
            },
        .port = I2C_NUM_0,
        .rx_buf_setting = I2C_BUF_DISABLE,
        .tx_buf_setting = I2C_BUF_DISABLE,
    }};

static bool i2c_driver_initialized = false;

bool i2c_wrapper_init() {
  if (i2c_driver_initialized) {
    MACKI_LOG_WARN(TAG, "I2C driver already initialized");
    return true;
  }
  for (uint8_t i = 0; i < I2C_MAX_INSTANCES_NUM; ++i) {
    if (i2c_driver_init(&driver[i]) != I2C_DRIVER_OK) {
      return false;
    }
  }
  i2c_driver_initialized = true;
  MACKI_LOG_INFO(TAG, "I2C wrapper initialized, number of instances: %d",
                 (uint8_t)I2C_MAX_INSTANCES_NUM);
  return true;
}

i2c_driver_t *i2c_wrapper_get_driver_instance(i2c_driver_instances_t instance) {
  if (instance >= I2C_MAX_INSTANCES_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid I2C instance %d", instance);
    return NULL;
  }
  if(driver[instance].initiated == false) {
    MACKI_LOG_WARN(TAG, "I2C driver instance %d not initialized", instance);
    return NULL;
  }
  return &driver[instance];
}

bool i2c_wrapper_deinit() {
  if (!i2c_driver_initialized) {
    MACKI_LOG_WARN(TAG, "I2C driver already deinitialized");
    return true;
  }
  for (uint8_t i = 0; i < I2C_MAX_INSTANCES_NUM; ++i) {
    if (i2c_driver_deinit(&driver[i]) != I2C_DRIVER_OK) {
      return false;
    }
  }
  i2c_driver_initialized = false;
  MACKI_LOG_INFO(TAG, "I2C wrapper deinitialized");
  return true;
}
