// Copyright 2024 MACKI

#pragma once

#include "i2c_ic_function_defines.h"
#include "macki_log.h"

typedef enum {
  TMP1075_TEMP_REG = 0x00U,
  TMP1075_CFGR_REG = 0x01U,
  TMP1075_DEVICE_ID_REG = 0x0FU,
} tmp1075_registers_t;

#define TMP1075_CFGR_OS_ENABLE 1U << 7U
#define TMP1075_CFGR_OS_DISABLE 0U << 7U

#define TMP1075_CFGR_CONVERSION_RATE_27_5 0U << 6U | 0U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_55 0U << 6U | 1U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_110 1U << 6U | 0U << 5U
#define TMP1075_CFGR_CONVERSION_RATE_220 1U << 6U | 1U << 5U

#define TMP1075_CFGR_ALERT_CONSECUTIVE_UNUSED 0U << 4U | 0U << 3U

#define TMP1075_CFGR_ALERT_POLARITY_LOW 0U << 2U
#define TMP1075_CFGR_ALERT_POLARITY_HIGH 1U << 2U

#define TMP1075_CFGR_ALERT_MODE_COMPARATOR 0U << 1U
#define TMP1075_CFGR_ALERT_MODE_INTERRUPT 1U << 1U

#define TMP1075_CFGR_CONTINUOUS_CONVERSION 0U << 0U
#define TMP1075_CFGR_SHUTDOWN_MODE 1U << 0U

#define TMP1075_TEMPERATURE_MASK 0xFFF0U

#define TMP1075_TEMPERATURE_RESOLUTION_CELSIUS 0.0625f

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
