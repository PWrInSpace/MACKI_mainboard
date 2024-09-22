// Copyright 2024 MACKI

#include "lis2dw12_driver.h"

#include "lis2dw12_registers.h"
#include "macki_log.h"
#include "safe_bitwise_shifts.h"

#define TAG "LIS2DW12_DRIVER"

lis2dw12_driver_status_t lis2dw12_driver_write_register_byte(
    lis2dw12_driver_t *driver, uint8_t reg, uint8_t data) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver write register byte failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  uint8_t data_to_send[2] = {reg, data};
  if (driver->_send_data(data_to_send, sizeof(data_to_send), driver->address) ==
      false) {
    MACKI_LOG_ERROR(
        TAG,
        "LIS2DW12 driver write register byte failed, I2C communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_read_register_byte(
    lis2dw12_driver_t *driver, uint8_t reg, uint8_t *data) {
  if (driver == NULL || data == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver read register byte failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  if (driver->_send_receive_data(data, 1, driver->address, reg) == false) {
    MACKI_LOG_ERROR(
        TAG,
        "LIS2DW12 driver read register byte failed, I2C communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_init(lis2dw12_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG, "LIS2DW12 driver initialization failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  if (driver->initiated == true) {
    MACKI_LOG_ERROR(TAG, "LIS2DW12 driver already initialized");
    return LIS2DW12_DRIVER_OK;
  }

  lis2dw12_driver_status_t ret = LIS2DW12_DRIVER_OK;
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL1_REG,
                                             CTRL1_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL2_REG,
                                             CTRL2_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL3_REG,
                                             CTRL3_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL4_REG,
                                             CTRL4_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL5_REG,
                                             CTRL5_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL6_REG,
                                             CTRL6_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL7_REG,
                                             CTRL7_DEFAULT_CONFIG);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_FIFO_CTRL_REG,
                                             FIFO_CTRL_DEFAULT_CONFIG);

  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver initialization failed, I2C communication error");
    driver->initiated = false;
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = true;
  MACKI_LOG_TRACE(TAG, "LIS2DW12 driver initialization successful");
  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_deinit(lis2dw12_driver_t *driver) {
  if (driver == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver deinitialization failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  if (driver->initiated == false) {
    MACKI_LOG_ERROR(TAG, "LIS2DW12 driver already deinitialized");
    return LIS2DW12_DRIVER_OK;
  }

  lis2dw12_driver_status_t ret = LIS2DW12_DRIVER_OK;
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL1_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL2_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL3_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL4_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL5_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL6_REG, 0x00);
  ret |= lis2dw12_driver_write_register_byte(driver, LIS2DW12_CTRL7_REG, 0x00);
  ret |=
      lis2dw12_driver_write_register_byte(driver, LIS2DW12_FIFO_CTRL_REG, 0x00);

  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG,
        "LIS2DW12 driver deinitialization failed, I2C communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  driver->initiated = false;
  MACKI_LOG_TRACE(TAG, "LIS2DW12 driver deinitialization successful");
  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_who_am_i(lis2dw12_driver_t *driver,
                                                  uint8_t *who_am_i) {
  if (driver == NULL || who_am_i == NULL) {
    MACKI_LOG_ERROR(TAG, "LIS2DW12 driver read device ID failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  lis2dw12_driver_status_t ret = lis2dw12_driver_read_register_byte(
      driver, LIS2DW12_WHO_AM_I_REG, who_am_i);
  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read device ID failed, I2C communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  MACKI_LOG_TRACE(TAG, "LIS2DW12 driver read who_am_is successful %d",
                  *who_am_i);
  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_get_fifo_samples_number(
    lis2dw12_driver_t *driver, uint8_t *samples_number) {
  if (driver == NULL || samples_number == NULL) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read FIFO samples number failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }

  if (!driver->initiated) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver read FIFO samples number failed, driver "
                    "uninitialized");
    return LIS2DW12_DRIVER_UNINITIALIZED;
  }

  uint8_t fifo_status = 0;
  lis2dw12_driver_status_t ret = lis2dw12_driver_read_register_byte(
      driver, LIS2DW12_FIFO_SAMPLES_REG, &fifo_status);
  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver read FIFO samples number failed, I2C "
                    "communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }

  fifo_status &= FIFO_SAMPLES_MASK;
  if (fifo_status > MAX_FIFO_SAMPLES_NUM) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver read FIFO samples number failed, invalid "
                    "samples number");
    return LIS2DW12_DRIVER_TOO_MANY_SAMPLES;
  }

  *samples_number = fifo_status;
  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_get_fifo_sample(
    lis2dw12_driver_t *driver, lis2dw12_out_data_t *sample) {
  if (driver == NULL || sample == NULL) {
    MACKI_LOG_ERROR(TAG,
                    "LIS2DW12 driver read FIFO sample failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }
  if (!driver->initiated) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read FIFO sample failed, driver uninitialized");
    return LIS2DW12_DRIVER_UNINITIALIZED;
  }

  lis2dw12_driver_status_t ret = LIS2DW12_DRIVER_OK;
  // Read the samples from the FIFO
  uint8_t x[2] = {0};
  uint8_t y[2] = {0};
  uint8_t z[2] = {0};
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_X_L_REG, &x[0]);
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_X_H_REG, &x[1]);
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_Y_L_REG, &y[0]);
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_Y_H_REG, &y[1]);
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_Z_L_REG, &z[0]);
  ret |=
      lis2dw12_driver_read_register_byte(driver, LIS2DW12_OUT_Z_H_REG, &z[1]);
  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read FIFO data failed, I2C communication error");
    return LIS2DW12_I2C_TRANSACTION_ERROR;
  }
  sample->x = (int16_t)int16_from_uint8_bytes(x);
  sample->y = (int16_t)int16_from_uint8_bytes(y);
  sample->z = (int16_t)int16_from_uint8_bytes(z);
  
  return LIS2DW12_DRIVER_OK;
}

lis2dw12_driver_status_t lis2dw12_driver_read_fifo_data(
    lis2dw12_driver_t *driver, lis2dw12_fifo_data_t *fifo_data) {
  if (driver == NULL || fifo_data == NULL) {
    MACKI_LOG_ERROR(TAG, "LIS2DW12 driver read FIFO data failed, invalid args");
    return LIS2DW12_DRIVER_ERROR;
  }
  if (!driver->initiated) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read FIFO data failed, driver uninitialized");
    return LIS2DW12_DRIVER_UNINITIALIZED;
  }
  // Read the number of samples in the FIFO
  uint8_t samples_number = 0;
  lis2dw12_driver_status_t ret =
      lis2dw12_driver_get_fifo_samples_number(driver, &samples_number);
  if (ret != LIS2DW12_DRIVER_OK) {
    MACKI_LOG_ERROR(
        TAG, "LIS2DW12 driver read FIFO data failed, get samples number error");
    return ret;
  }

  // Read the samples from the FIFO
  uint8_t x[2] = {0};
  uint8_t y[2] = {0};
  uint8_t z[2] = {0};

  for (uint8_t i = 0; i < samples_number; i++) {
    ret = lis2dw12_driver_get_fifo_sample(driver, &fifo_data->samples[i]);
    if (ret != LIS2DW12_DRIVER_OK) {
      MACKI_LOG_ERROR(
          TAG,
          "LIS2DW12 driver read FIFO data failed, I2C communication error");
      return LIS2DW12_I2C_TRANSACTION_ERROR;
    }
  }

  fifo_data->current_samples_number = samples_number;
  return LIS2DW12_DRIVER_OK;
}
