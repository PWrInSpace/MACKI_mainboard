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