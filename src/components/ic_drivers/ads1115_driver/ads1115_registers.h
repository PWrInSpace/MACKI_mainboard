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

typedef enum {
  ADS1115_DRIVER_CONTINUOUS_MODE = 0x00,
  ADS1115_DRIVER_SINGLE_SHOT_MODE = 0x01,
} ads1115_driver_operation_mode_t;

typedef enum {
  MUX_AIN0_GND = 0x04,
  MUX_AIN1_GND = 0x05,
  MUX_AIN2_GND = 0x06,
  MUX_AIN3_GND = 0x07,
} ads1115_select_analog_pin_t;

typedef union {
  struct {
    uint8_t os : 1;
    ads1115_select_analog_pin_t mux : 3;
    uint8_t pga : 3;
    ads1115_driver_operation_mode_t mode : 1;
    uint8_t dr : 3;
    uint8_t comp_mode : 1;
    uint8_t comp_pol : 1;
    uint8_t comp_lat : 1;
    uint8_t comp_que : 2;
  };
  uint16_t raw;
} ads1115_config_reg_t;

#define ADS1115_CONFIG_RESET 0x8583
