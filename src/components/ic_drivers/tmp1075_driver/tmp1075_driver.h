// Copyright 2024 MACKI

#pragma once

#include "ic_function_defines.h"
#include "macki_log.h"

#define TMP1075_TEMPERATURE_RESOLUTION_CELSIUS 0.0625f
#define TMP1075_TEMPERATURE_MASK 0xFFF0U

typedef enum {
  TMP1075_DRIVER_OK,
  TMP1075_I2C_TRANSACTION_ERROR,
  TMP1075_DRIVER_UNINITIALIZED,
  TMP1075_DRIVER_ERROR
} tmp1075_driver_status_t;

typedef struct {
  size_t address;
  bool initiated;
  _i2c_ic_send_receive_data _send_receive_data;
  _i2c_ic_send_data _send_data;
} tmp1075_driver_t;

/*!
 * @brief Initializes the TMP1075 driver - configures the IC
 * @param driver Pointer to the TMP1075 driver
 * @return TMP1075_DRIVER_OK if the driver was initialized successfully,
 *   TMP1075_I2C_TRANSACTION_ERROR otherwise
 */
tmp1075_driver_status_t tmp1075_driver_init(tmp1075_driver_t *driver);

/*!
 * @brief Deinitializes the TMP1075 driver
 * @param driver Pointer to the TMP1075 driver
 * @return TMP1075_DRIVER_OK if the driver was deinitialized successfully,
 *   TMP1075_DRIVER_ERROR otherwise
 */
tmp1075_driver_status_t tmp1075_driver_deinit(tmp1075_driver_t *driver);

/*!
 * @brief Reads the device ID from the TMP1075 sensor.
 * @param driver Pointer to the TMP1075 driver
 * @param device_id Pointer to the variable where the device ID will be stored
 * @return TMP1075_DRIVER_OK if the device ID was read successfully,
 *    TMP1075_DRIVER_ERROR otherwise
 */
tmp1075_driver_status_t tmp1075_driver_read_device_id(tmp1075_driver_t *driver,
                                                      uint16_t *device_id);

/*!
 * @brief Reads the raw temperature from the TMP1075 sensor.
 * @param driver Pointer to the TMP1075 driver
 * @param temperature Pointer to the variable where the raw temperature will be
 * stored
 * @return TMP1075_DRIVER_OK if the raw temperature was read successfully,
 *    TMP1075_DRIVER_ERROR otherwise
 */
tmp1075_driver_status_t tmp1075_driver_read_raw_temperature(
    tmp1075_driver_t *driver, int16_t *temperature);

/*!
 * @brief Converts the raw temperature to Celsius.
 * @param raw_temperature The raw temperature read from the TMP1075 sensor
 * @return The temperature in Celsius
 */
float tmp1075_driver_convert_raw_temperature_to_celsius(
    int16_t raw_temperature);
