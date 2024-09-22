// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "shared_i2c_wrapper.h"

static i2c_driver_instances_t i2c_instance = I2C_INSTANCE_MAIN;

static i2c_driver_t* i2c_driver_instance;

void init_i2c_driver() {
  i2c_wrapper_init();
  i2c_driver_instance = i2c_wrapper_get_driver_instance(i2c_instance);
  TEST_ASSERT_NOT_NULL(i2c_driver_instance);
}

void deinit_i2c_driver() { TEST_ASSERT_TRUE(i2c_wrapper_deinit()); }

bool i2c_ic_send_receive_data(uint8_t* data_out, size_t size_out,
                              uint8_t device_address, uint8_t reg_address) {
  if (i2c_driver_instance == NULL) {
    return false;
  }
  i2c_driver_status_t status = i2c_driver_send_receive_data(
      i2c_driver_instance, data_out, size_out, device_address, reg_address);
  return status == I2C_DRIVER_OK;
}

bool i2c_ic_send_data(uint8_t* data, size_t data_size, uint8_t device_address) {
  if (i2c_driver_instance == NULL) {
    return false;
  }
  i2c_driver_status_t status = i2c_driver_send_data(i2c_driver_instance, data,
                                                    data_size, device_address);
  return status == I2C_DRIVER_OK;
}
