// Copyright 2017 https://github.com/michael-betz, pololu/vl53l0x-arduino.
// Adjusted by Krzysztof Gliwinski

#pragma once

#include "ic_function_defines.h"

#define ADDRESS_DEFAULT 0b01010010

#define decode_vcsel_period(reg_val) (((reg_val) + 1) << 1)

#define encode_vcsel_period(period_pclks) (((period_pclks) >> 1) - 1)

#define calc_macro_period(vcsel_period_pclks) \
  ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

enum reg_addr {
  SYSRANGE_START = 0x00,
  SYSTEM_THRESH_HIGH = 0x0C,
  SYSTEM_THRESH_LOW = 0x0E,
  SYSTEM_SEQUENCE_CONFIG = 0x01,
  SYSTEM_RANGE_CONFIG = 0x09,
  SYSTEM_INTERMEASUREMENT_PERIOD = 0x04,
  SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A,
  GPIO_HV_MUX_ACTIVE_HIGH = 0x84,
  SYSTEM_INTERRUPT_CLEAR = 0x0B,
  RESULT_INTERRUPT_STATUS = 0x13,
  RESULT_RANGE_STATUS = 0x14,
  RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN = 0xBC,
  RESULT_CORE_RANGING_TOTAL_EVENTS_RTN = 0xC0,
  RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF = 0xD0,
  RESULT_CORE_RANGING_TOTAL_EVENTS_REF = 0xD4,
  RESULT_PEAK_SIGNAL_RATE_REF = 0xB6,
  ALGO_PART_TO_PART_RANGE_OFFSET_MM = 0x28,
  I2C_SLAVE_DEVICE_ADDRESS = 0x8A,
  MSRC_CONFIG_CONTROL = 0x60,
  PRE_RANGE_CONFIG_MIN_SNR = 0x27,
  PRE_RANGE_CONFIG_VALID_PHASE_LOW = 0x56,
  PRE_RANGE_CONFIG_VALID_PHASE_HIGH = 0x57,
  PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT = 0x64,
  FINAL_RANGE_CONFIG_MIN_SNR = 0x67,
  FINAL_RANGE_CONFIG_VALID_PHASE_LOW = 0x47,
  FINAL_RANGE_CONFIG_VALID_PHASE_HIGH = 0x48,
  FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,
  PRE_RANGE_CONFIG_SIGMA_THRESH_HI = 0x61,
  PRE_RANGE_CONFIG_SIGMA_THRESH_LO = 0x62,
  PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50,
  PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51,
  PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x52,
  SYSTEM_HISTOGRAM_BIN = 0x81,
  HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT = 0x33,
  HISTOGRAM_CONFIG_READOUT_CTRL = 0x55,
  FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70,
  FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71,
  FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x72,
  CROSSTALK_COMPENSATION_PEAK_RATE_MCPS = 0x20,
  MSRC_CONFIG_TIMEOUT_MACROP = 0x46,
  SOFT_RESET_GO2_SOFT_RESET_N = 0xBF,
  IDENTIFICATION_MODEL_ID = 0xC0,
  IDENTIFICATION_REVISION_ID = 0xC2,
  OSC_CALIBRATE_VAL = 0xF8,
  GLOBAL_CONFIG_VCSEL_WIDTH = 0x32,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_1 = 0xB1,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_2 = 0xB2,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_3 = 0xB3,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_4 = 0xB4,
  GLOBAL_CONFIG_SPAD_ENABLES_REF_5 = 0xB5,
  GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6,
  DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E,
  DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F,
  POWER_MANAGEMENT_GO1_POWER_FORCE = 0x80,
  VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89,
  ALGO_PHASECAL_LIM = 0x30,
  ALGO_PHASECAL_CONFIG_TIMEOUT = 0x30,
};

typedef enum {
  vcsel_period_pre_range,
  vcsel_period_final_range
} vcsel_period_type;

typedef struct {
  uint16_t raw_distance;
  uint16_t signal_cnt;
  uint16_t ambient_cnt;
  uint16_t spad_cnt;
  uint8_t range_status;
} stat_info_t;

typedef struct {
  _delay_ms _delay;
  _i2c_ic_send_data _send_data;
  _i2c_ic_send_receive_data _send_receive_data;
  _start_timeout _start_timeout;
  _check_timeout_expired _check_timeout_expired;
  uint8_t address;
  bool initialized;
} vl53l0x_driver_t;

void set_address(vl53l0x_driver_t* driver, uint8_t new_addr);
uint8_t get_address(vl53l0x_driver_t* driver);
bool init_vl53l0x(vl53l0x_driver_t* driver, uint8_t io_2v8);
bool set_signal_rate_limit(vl53l0x_driver_t* driver, float limit_mcps);
float get_signal_rate_limit(vl53l0x_driver_t* driver);
bool set_measurement_timing_budget(vl53l0x_driver_t* driver,
                                   uint32_t budget_us);
uint32_t get_measurement_timing_budget(vl53l0x_driver_t* driver);
bool set_vcsel_pulse_period(vl53l0x_driver_t* driver, vcsel_period_type type,
                            uint8_t period_pclks);
uint8_t get_vcsel_pulse_period(vl53l0x_driver_t* driver,
                               vcsel_period_type type);
void start_continuous(vl53l0x_driver_t* driver, uint32_t period_ms);
void stop_continuous(vl53l0x_driver_t* driver);
uint16_t read_range_continuous_millimeters(vl53l0x_driver_t* driver,
                                           stat_info_t* extra_stats);
uint16_t read_range_single_millimeters(vl53l0x_driver_t* driver,
                                       stat_info_t* extra_stats);
void set_timeout(vl53l0x_driver_t* driver, uint16_t timeout);
uint16_t get_timeout(vl53l0x_driver_t* driver);

void write_reg(vl53l0x_driver_t* driver, uint8_t reg, uint8_t value);
void write_reg_16bit(vl53l0x_driver_t* driver, uint8_t reg, uint16_t value);
void write_reg_32bit(vl53l0x_driver_t* driver, uint8_t reg, uint32_t value);
uint8_t read_reg(vl53l0x_driver_t* driver, uint8_t reg);
uint16_t read_reg_16bit(vl53l0x_driver_t* driver, uint8_t reg);
uint32_t read_reg_32bit(vl53l0x_driver_t* driver, uint8_t reg);
void write_multi(vl53l0x_driver_t* driver, uint8_t reg, uint8_t const* src,
                 uint8_t count);
void read_multi(vl53l0x_driver_t* driver, uint8_t reg, uint8_t* dst,
                uint8_t count);

typedef struct {
  uint8_t tcc, msrc, dss, pre_range, final_range;
} sequence_step_enables;

typedef struct {
  uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;
  uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
  uint32_t msrc_dss_tcc_us, pre_range_us, final_range_us;
} sequence_step_timeouts;
