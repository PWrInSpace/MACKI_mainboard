// Copyright 2024 MACKI, Krzysztof Gliwinski
#pragma once

#include <stdint.h>

typedef enum {
  LIS2DW12_STATUS_REG = 0x27U,
  LIS2DW12_CTRL1_REG = 0x20U,
  LIS2DW12_CTRL2_REG = 0x21U,
  LIS2DW12_CTRL3_REG = 0x22U,
  LIS2DW12_CTRL4_REG = 0x23U,
  LIS2DW12_CTRL5_REG = 0x24U,
  LIS2DW12_CTRL6_REG = 0x25U,
  LIS2DW12_CTRL7_REG = 0x3FU,
  LIS2DW12_OUT_T_REG = 0x26U,
  LIS2DW12_OUT_T_L_REG = 0x0DU,
  LIS2DW12_OUT_T_H_REG = 0x0EU,
  LIS2DW12_OUT_X_L_REG = 0x28U,
  LIS2DW12_OUT_X_H_REG = 0x29U,
  LIS2DW12_OUT_Y_L_REG = 0x2AU,
  LIS2DW12_OUT_Y_H_REG = 0x2BU,
  LIS2DW12_OUT_Z_L_REG = 0x2CU,
  LIS2DW12_OUT_Z_H_REG = 0x2DU,
  LIS2DW12_FIFO_CTRL_REG = 0x2EU,
  LIS2DW12_FIFO_SAMPLES_REG = 0x2FU,
  LIS2DW12_STATUS_DUP_REG = 0x37U,
  LIS2DW12_WAKE_UP_THS_REG = 0x5BU,
  LIS2DW12_WAKE_UP_DUR_REG = 0x5CU,
  LIS2DW12_FREE_FALL_REG = 0x5DU,
  LIS2DW12_STATUS_WAKE_UP_REG = 0x35U,
  LIS2DW12_STATUS_FREE_FALL_REG = 0x36U,
  LIS2DW12_STATUS_TAP_REG = 0x3AU,
  LIS2DW12_TAP_THS_6D_REG = 0x59U,
  LIS2DW12_INT_DUR_REG = 0x5EU,
  LIS2DW12_WAKE_UP_SRC_REG = 0x1BU,
  LIS2DW12_TAP_SRC_REG = 0x1CU,
  LIS2DW12_FREE_FALL_SRC_REG = 0x1DU,
  LIS2DW12_MD1_CFG_REG = 0x5EU,
  LIS2DW12_MD2_CFG_REG = 0x5FU,
  LIS2DW12_WHO_AM_I_REG = 0x0FU,
} ls2dw12_registers_t;

/*!
 * @brief Apply this mask on the FIFO_SAMPLES register to get the number of
 * samples
 */
#define FIFO_SAMPLES_MASK 0b111111U

/*!
 * @brief LIS2DW12 CTRL1 register configuration
 *  - ODR = 25Hz - 0b0011 << 4
 *  - MODE = High performance - 0b01 << 2
 *  - LP_MODE = not relevant - 0b00
 */

const uint8_t CTRL1_DEFAULT_CONFIG = 0b0011U << 4U | 0b01 << 2U;

/*!
 * @brief LIS2DW12 CTRL2 register configuration
 *  - BDU = Block data update - 0b1 << 3
 */
const uint8_t CTRL2_DEFAULT_CONFIG = 0b1U << 3U;

// TODO(Glibus): when interrupt is enabled
/*!
 * @brief LIS2DW12 CTRL3 register configuration
 */
const uint8_t CTRL3_DEFAULT_CONFIG = 0;

// TODO(Glibus): when interrupt is enabled
/*!
 * @brief LIS2DW12 CTRL4 register configuration
 */
const uint8_t CTRL4_DEFAULT_CONFIG = 0;

/*!
 * @brief LIS2DW12 CTRL5 register configuration
 * Unused - related to INT2 pad
 */
const uint8_t CTRL5_DEFAULT_CONFIG = 0;

/*!
 * @brief LIS2DW12 CTRL6 register configuration
 *  - BW = ODR/2 - 0b00 << 6
 *  - FS = 2g - 0b00 << 4
 */
const uint8_t CTRL6_DEFAULT_CONFIG = 0b00 << 6U | 0b00 << 4U | 0b00 << 2U;

// TODO(Glibus): when interrupt is enabled
/*!
 * @brief LIS2DW12 CTRL7 register configuration
 */
const uint8_t CTRL7_DEFAULT_CONFIG = 0;

/*!
 * @brief LIS2DW12 FIFO_CTRL register configuration
 *  - FMode = Continuous mode - 0b110 << 5
 *  - FTH = 31 - 0b11111
 */
const uint8_t FIFO_CTRL_DEFAULT_CONFIG = 0b110 << 5U | 0b11111;

// TODO(Glibus): when free fall is explored
/*!
 * @brief LIS2DW12 FREE_FALL register configuration
 */
const uint8_t FREE_FALL_DEFAULT_CONFIG = 0;
