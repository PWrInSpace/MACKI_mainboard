// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  PCA_9574_REG_INPUT_PORT = 0x00,
  PCA_9574_REG_POLARITY_INV = 0x01,
  PCA_9574_REG_BUS_PULL_UP_DOWN_ENABLE = 0x02,
  PCA_9574_REG_PULL_UP_DOWN_SELECT = 0x03,
  PCA_9574_REG_CONFIGURATION = 0x04,
  PCA_9574_REG_OUTPUT_PORT = 0x05,
  PCA_9574_REG_INTERRUPT_MASK = 0x06,
  PCA_9574_REG_INTERRUPT_STATUS = 0x07
} pca9574_registers_t;

typedef enum {
  PCA9574_OUTPUT = 0,
  PCA9574_INPUT,
  PCA9574_PIN_MODE_UNKNOWN
} pca9574_pin_mode_t;

typedef enum {
  PCA9574_PIN0 = 0,
  PCA9574_PIN1,
  PCA9574_PIN2,
  PCA9574_PIN3,
  PCA9574_PIN4,
  PCA9574_PIN5,
  PCA9574_PIN6,
  PCA9574_PIN7,
} pca9574_pins_num_t;

/*!
 * @brief Setting the pin modes (input/output) for the PCA9574
 *        Also used for reading pin levels
 */
typedef union {
  struct {
    uint8_t pin0 : 1;
    uint8_t pin1 : 1;
    uint8_t pin2 : 1;
    uint8_t pin3 : 1;
    uint8_t pin4 : 1;
    uint8_t pin5 : 1;
    uint8_t pin6 : 1;
    uint8_t pin7 : 1;
  } pins;
  uint8_t raw;
} pca9574_config_reg_t;
