// Copyright 2024 MACKI, Krzysztof Gliwiński

/*!
 * @brief Because the API from ST is this way and not another
 * I have to include the platform file here
 */

/// THIS IS A TEMPORARY SOLUTION SO IT WILL WORK FOR MONDAY :DDDDDDDDDDD
/// Thats why so ugly

#include "vl53l1_platform.h"

#include "i2c_driver.h"
#include "i2c_wrapper.h"

i2c_driver_t* i2c_driver;

VL53L1_Error VL53L1_CommsInitialise(VL53L1_Dev_t* pdev, uint8_t comms_type,
                                    uint16_t comms_speed_khz) {
  i2c_driver = i2c_wrapper_get_driver_instance(I2C_INSTANCE_MAIN);
  return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_CommsClose(VL53L1_Dev_t* pdev) { return VL53L1_ERROR_NONE; }

VL53L1_Error VL53L1_WriteMulti(VL53L1_Dev_t* pdev, uint16_t index,
                               uint8_t* pdata, uint32_t count) {
  return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrByte(VL53L1_Dev_t* pdev, uint16_t index, uint8_t data) {
  i2c_driver_status_t ret =
      i2c_driver_send_data(i2c_driver, &data, 1, pdev->I2cDevAddr);
  if (ret == I2C_DRIVER_STATUS_OK) {
    return VL53L1_ERROR_NONE;
  } else {
    return VL53L1_ERROR_CONTROL_INTERFACE;
  }
  return ret;
}
