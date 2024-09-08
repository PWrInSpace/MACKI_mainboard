// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum {
  ADS1115_REG_CONVERSION = 0x00,  // Read only
  ADS1115_REG_CONFIG = 0x01,      // Read/Write
  ADS1115_REG_LO_THRESH = 0x02,   // Read/Write
  ADS1115_REG_HI_THRESH = 0x03,   // Read/Write
} ads1115_register_t;

const uint8_t ads1115_select_analog_pin_mask = 0b111U << 4U;

/*!
 * @brief Default configuration for the ADS1115 REG CONFIG register
 * By default the MUX AIN0 AIN1 is selected. The device is in the continuous
 * mode, the data rate is 128 SPS, the comparator mode is traditional
 */
const uint8_t ads1115_config_default[2] = {0b00000000, 0b00000000};

/*!
 * @brief ADS1115 REG CONFIG register configuration for the power down mode
 */
const uint8_t ads1115_power_down[2] = {0x01, 0x00};
