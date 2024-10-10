// Copyright 2017 https://github.com/michael-betz, pololu/vl53l0x-arduino.
// Adjusted by Krzysztof Gliwinski

#include "vl53l0x_driver.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "safe_bitwise_shifts.h"

uint8_t g_i2c_addr = ADDRESS_DEFAULT;
uint16_t g_io_timeout = 100;
uint8_t g_is_timeout = 0;
uint16_t g_timeout_start_ms;
uint8_t g_stop_variable;
uint32_t g_meas_tim_bud_us;

bool get_spad_info(vl53l0x_driver_t* driver, uint8_t* count,
                   bool* type_is_aperture);
void get_sequence_step_enables(vl53l0x_driver_t* driver,
                               sequence_step_enables* enables);
void get_sequence_step_timeouts(vl53l0x_driver_t* driver,
                                sequence_step_enables const* enables,
                                sequence_step_timeouts* timeouts);
bool perform_single_ref_calibration(vl53l0x_driver_t* driver,
                                    uint8_t vhv_init_byte);
static uint16_t decode_timeout(uint16_t value);
static uint16_t encode_timeout(uint16_t timeout_mclks);
static uint32_t timeout_mclks_to_microseconds(uint16_t timeout_period_mclks,
                                              uint8_t vcsel_period_pclks);
static uint32_t timeout_microseconds_to_mclks(uint32_t timeout_period_us,
                                              uint8_t vcsel_period_pclks);
void write_reg(vl53l0x_driver_t* driver, uint8_t reg, uint8_t value) {
  uint8_t data_to_send[2] = {reg, value};
  driver->_send_data(data_to_send, 2, driver->address);
}

void write_reg_16bit(vl53l0x_driver_t* driver, uint8_t reg, uint16_t value) {
  uint8_t data_to_send[3] = {reg, (value >> 8) & 0xFF, value & 0xFF};
  driver->_send_data(data_to_send, 3, driver->address);
}

void write_reg_32bit(vl53l0x_driver_t* driver, uint8_t reg, uint32_t value) {
  uint8_t data_to_send[5] = {reg, (value >> 24) & 0xFF, (value >> 16) & 0xFF,
                             (value >> 8) & 0xFF, value & 0xFF};
  driver->_send_data(data_to_send, 5, driver->address);
}

uint8_t read_reg(vl53l0x_driver_t* driver, uint8_t reg) {
  uint8_t data_out;
  driver->_send_receive_data(&data_out, 1, driver->address, reg);
  return data_out;
}

uint16_t read_reg_16bit(vl53l0x_driver_t* driver, uint8_t reg) {
  uint8_t data_out[2];
  driver->_send_receive_data(data_out, 2, driver->address, reg);
  return uint16_from_uint8_bytes(data_out);
}

uint32_t read_reg_32bit(vl53l0x_driver_t* driver, uint8_t reg) {
  uint8_t data_out[4];
  driver->_send_receive_data(data_out, 4, driver->address, reg);
  return uint32_from_uint8_bytes(data_out);
}

void write_multi(vl53l0x_driver_t* driver, uint8_t reg, uint8_t const* src,
                 uint8_t count) {
  uint8_t* data_to_send = (uint8_t*)malloc(count + 1);
  data_to_send[0] = reg;
  for (uint8_t i = 0; i < count; i++) {
    data_to_send[i + 1] = src[i];
  }
  driver->_send_data(data_to_send, count + 1, driver->address);
}

void read_multi(vl53l0x_driver_t* driver, uint8_t reg, uint8_t* dst,
                uint8_t count) {
  driver->_send_receive_data(dst, count, driver->address, reg);
}

void set_address(vl53l0x_driver_t* driver, uint8_t new_addr) {
  write_reg(driver, I2C_SLAVE_DEVICE_ADDRESS, (new_addr >> 1) & 0x7F);
  driver->address = new_addr;
}

uint8_t get_address(vl53l0x_driver_t* driver) { return driver->address; }

bool init_vl53l0x(vl53l0x_driver_t* driver, uint8_t io_2v8) {
  if (io_2v8) {
    write_reg(driver, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
              read_reg(driver, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01);
  }

  write_reg(driver, 0x88, 0x00);

  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);
  g_stop_variable = read_reg(driver, 0x91);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x00);

  write_reg(driver, MSRC_CONFIG_CONTROL,
            read_reg(driver, MSRC_CONFIG_CONTROL) | 0x12);

  set_signal_rate_limit(driver, 0.25);

  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0xFF);

  uint8_t spad_count;
  bool spad_type_is_aperture;
  if (!get_spad_info(driver, &spad_count, &spad_type_is_aperture)) {
    return false;
  }

  uint8_t ref_spad_map[6];
  read_multi(driver, GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
  write_reg(driver, DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

  uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0;
  uint8_t spads_enabled = 0;

  for (uint8_t i = 0; i < 48; i++) {
    if (i < first_spad_to_enable || spads_enabled == spad_count) {
      ref_spad_map[i / 8] &= ~(1 << (i % 8));
    } else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1) {
      spads_enabled++;
    }
  }

  write_multi(driver, GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x09, 0x00);
  write_reg(driver, 0x10, 0x00);
  write_reg(driver, 0x11, 0x00);

  write_reg(driver, 0x24, 0x01);
  write_reg(driver, 0x25, 0xFF);
  write_reg(driver, 0x75, 0x00);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x4E, 0x2C);
  write_reg(driver, 0x48, 0x00);
  write_reg(driver, 0x30, 0x20);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x30, 0x09);
  write_reg(driver, 0x54, 0x00);
  write_reg(driver, 0x31, 0x04);
  write_reg(driver, 0x32, 0x03);
  write_reg(driver, 0x40, 0x83);
  write_reg(driver, 0x46, 0x25);
  write_reg(driver, 0x60, 0x00);
  write_reg(driver, 0x27, 0x00);
  write_reg(driver, 0x50, 0x06);
  write_reg(driver, 0x51, 0x00);
  write_reg(driver, 0x52, 0x96);
  write_reg(driver, 0x56, 0x08);
  write_reg(driver, 0x57, 0x30);
  write_reg(driver, 0x61, 0x00);
  write_reg(driver, 0x62, 0x00);
  write_reg(driver, 0x64, 0x00);
  write_reg(driver, 0x65, 0x00);
  write_reg(driver, 0x66, 0xA0);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x22, 0x32);
  write_reg(driver, 0x47, 0x14);
  write_reg(driver, 0x49, 0xFF);
  write_reg(driver, 0x4A, 0x00);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x7A, 0x0A);
  write_reg(driver, 0x7B, 0x00);
  write_reg(driver, 0x78, 0x21);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x23, 0x34);
  write_reg(driver, 0x42, 0x00);
  write_reg(driver, 0x44, 0xFF);
  write_reg(driver, 0x45, 0x26);
  write_reg(driver, 0x46, 0x05);
  write_reg(driver, 0x40, 0x40);
  write_reg(driver, 0x0E, 0x06);
  write_reg(driver, 0x20, 0x1A);
  write_reg(driver, 0x43, 0x40);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x34, 0x03);
  write_reg(driver, 0x35, 0x44);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x31, 0x04);
  write_reg(driver, 0x4B, 0x09);
  write_reg(driver, 0x4C, 0x05);
  write_reg(driver, 0x4D, 0x04);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x44, 0x00);
  write_reg(driver, 0x45, 0x20);
  write_reg(driver, 0x47, 0x08);
  write_reg(driver, 0x48, 0x28);
  write_reg(driver, 0x67, 0x00);
  write_reg(driver, 0x70, 0x04);
  write_reg(driver, 0x71, 0x01);
  write_reg(driver, 0x72, 0xFE);
  write_reg(driver, 0x76, 0x00);
  write_reg(driver, 0x77, 0x00);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x0D, 0x01);

  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0x01, 0xF8);

  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x8E, 0x01);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x00);

  write_reg(driver, SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
  write_reg(driver, GPIO_HV_MUX_ACTIVE_HIGH,
            read_reg(driver, GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10);
  write_reg(driver, SYSTEM_INTERRUPT_CLEAR, 0x01);

  g_meas_tim_bud_us = get_measurement_timing_budget(driver);

  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0xE8);

  set_measurement_timing_budget(driver, g_meas_tim_bud_us);

  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0x01);
  if (!perform_single_ref_calibration(driver, 0x40)) {
    return false;
  }

  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0x02);
  if (!perform_single_ref_calibration(driver, 0x00)) {
    return false;
  }

  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0xE8);

  return true;
}

bool set_signal_rate_limit(vl53l0x_driver_t* driver, float limit_mcps) {
  if (limit_mcps < 0 || limit_mcps > 511.99) {
    return false;
  }

  write_reg_16bit(driver, FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT,
                  limit_mcps * (1 << 7));
  return true;
}

float get_signal_rate_limit(vl53l0x_driver_t* driver) {
  return (float)read_reg_16bit(driver,
                               FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT) /
         (1 << 7);
}

bool set_measurement_timing_budget(vl53l0x_driver_t* driver,
                                   uint32_t budget_us) {
  sequence_step_enables enables;
  sequence_step_timeouts timeouts;

  uint16_t const start_overhead = 1320;
  uint16_t const end_overhead = 960;
  uint16_t const msrc_overhead = 660;
  uint16_t const tcc_overhead = 590;
  uint16_t const dss_overhead = 690;
  uint16_t const pre_range_overhead = 660;
  uint16_t const final_range_overhead = 550;

  uint32_t const min_timing_budget = 20000;

  if (budget_us < min_timing_budget) {
    return false;
  }

  uint32_t used_budget_us = start_overhead + end_overhead;

  get_sequence_step_enables(driver, &enables);
  get_sequence_step_timeouts(driver, &enables, &timeouts);

  if (enables.tcc) {
    used_budget_us += (timeouts.msrc_dss_tcc_us + tcc_overhead);
  }

  if (enables.dss) {
    used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + dss_overhead);
  } else if (enables.msrc) {
    used_budget_us += (timeouts.msrc_dss_tcc_us + msrc_overhead);
  }

  if (enables.pre_range) {
    used_budget_us += (timeouts.pre_range_us + pre_range_overhead);
  }

  if (enables.final_range) {
    used_budget_us += final_range_overhead;

    if (used_budget_us > budget_us) {
      return false;
    }

    uint32_t final_range_timeout_us = budget_us - used_budget_us;

    uint16_t final_range_timeout_mclks = timeout_microseconds_to_mclks(
        final_range_timeout_us, timeouts.final_range_vcsel_period_pclks);

    if (enables.pre_range) {
      final_range_timeout_mclks += timeouts.pre_range_mclks;
    }

    write_reg_16bit(driver, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                    encode_timeout(final_range_timeout_mclks));

    g_meas_tim_bud_us = budget_us;
  }
  return true;
}
uint32_t get_measurement_timing_budget(vl53l0x_driver_t* driver) {
  sequence_step_enables enables;
  sequence_step_timeouts timeouts;

  uint16_t const start_overhead = 1910;
  uint16_t const end_overhead = 960;
  uint16_t const msrc_overhead = 660;
  uint16_t const tcc_overhead = 590;
  uint16_t const dss_overhead = 690;
  uint16_t const pre_range_overhead = 660;
  uint16_t const final_range_overhead = 550;

  uint32_t budget_us = start_overhead + end_overhead;

  get_sequence_step_enables(driver, &enables);
  get_sequence_step_timeouts(driver, &enables, &timeouts);

  if (enables.tcc) {
    budget_us += (timeouts.msrc_dss_tcc_us + tcc_overhead);
  }

  if (enables.dss) {
    budget_us += 2 * (timeouts.msrc_dss_tcc_us + dss_overhead);
  } else if (enables.msrc) {
    budget_us += (timeouts.msrc_dss_tcc_us + msrc_overhead);
  }

  if (enables.pre_range) {
    budget_us += (timeouts.pre_range_us + pre_range_overhead);
  }

  if (enables.final_range) {
    budget_us += (timeouts.final_range_us + final_range_overhead);
  }

  g_meas_tim_bud_us = budget_us;
  return budget_us;
}

bool set_vcsel_pulse_period(vl53l0x_driver_t* driver, vcsel_period_type type,
                            uint8_t period_pclks) {
  uint8_t vcsel_period_reg = encode_vcsel_period(period_pclks);

  sequence_step_enables enables;
  sequence_step_timeouts timeouts;

  get_sequence_step_enables(driver, &enables);
  get_sequence_step_timeouts(driver, &enables, &timeouts);

  if (type == vcsel_period_pre_range) {
    switch (period_pclks) {
      case 12:
        write_reg(driver, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
        break;

      case 14:
        write_reg(driver, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
        break;

      case 16:
        write_reg(driver, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
        break;

      case 18:
        write_reg(driver, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
        break;

      default:
        return false;
    }
    write_reg(driver, PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

    write_reg(driver, PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    uint16_t new_pre_range_timeout_mclks =
        timeout_microseconds_to_mclks(timeouts.pre_range_us, period_pclks);

    write_reg_16bit(driver, PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                    encode_timeout(new_pre_range_timeout_mclks));

    uint16_t new_msrc_timeout_mclks =
        timeout_microseconds_to_mclks(timeouts.msrc_dss_tcc_us, period_pclks);

    write_reg(
        driver, MSRC_CONFIG_TIMEOUT_MACROP,
        (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

  } else if (type == vcsel_period_final_range) {
    switch (period_pclks) {
      case 8:
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);
        write_reg(driver, GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
        write_reg(driver, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
        write_reg(driver, 0xFF, 0x01);
        write_reg(driver, ALGO_PHASECAL_LIM, 0x30);
        write_reg(driver, 0xFF, 0x00);
        break;

      case 10:
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);
        write_reg(driver, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        write_reg(driver, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
        write_reg(driver, 0xFF, 0x01);
        write_reg(driver, ALGO_PHASECAL_LIM, 0x20);
        write_reg(driver, 0xFF, 0x00);
        break;

      case 12:
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);
        write_reg(driver, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        write_reg(driver, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
        write_reg(driver, 0xFF, 0x01);
        write_reg(driver, ALGO_PHASECAL_LIM, 0x20);
        write_reg(driver, 0xFF, 0x00);
        break;

      case 14:
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
        write_reg(driver, FINAL_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);
        write_reg(driver, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        write_reg(driver, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
        write_reg(driver, 0xFF, 0x01);
        write_reg(driver, ALGO_PHASECAL_LIM, 0x20);
        write_reg(driver, 0xFF, 0x00);
        break;

      default:
        return false;
    }

    write_reg(driver, FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    uint16_t new_final_range_timeout_mclks =
        timeout_microseconds_to_mclks(timeouts.final_range_us, period_pclks);

    if (enables.pre_range) {
      new_final_range_timeout_mclks += timeouts.pre_range_mclks;
    }

    write_reg_16bit(driver, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                    encode_timeout(new_final_range_timeout_mclks));

  } else {
    return false;
  }

  set_measurement_timing_budget(driver, g_meas_tim_bud_us);

  uint8_t sequence_config = read_reg(driver, SYSTEM_SEQUENCE_CONFIG);
  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, 0x02);
  perform_single_ref_calibration(driver, 0x0);
  write_reg(driver, SYSTEM_SEQUENCE_CONFIG, sequence_config);

  return true;
}

uint8_t get_vcsel_pulse_period(vl53l0x_driver_t* driver,
                               vcsel_period_type type) {
  if (type == vcsel_period_pre_range) {
    return decode_vcsel_period(read_reg(driver, PRE_RANGE_CONFIG_VCSEL_PERIOD));
  } else if (type == vcsel_period_final_range) {
    return decode_vcsel_period(
        read_reg(driver, FINAL_RANGE_CONFIG_VCSEL_PERIOD));
  } else {
    return 255;
  }
}
void start_continuous(vl53l0x_driver_t* driver, uint32_t period_ms) {
  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);
  write_reg(driver, 0x91, g_stop_variable);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x00);

  if (period_ms != 0) {
    uint16_t osc_calibrate_val = read_reg_16bit(driver, OSC_CALIBRATE_VAL);

    if (osc_calibrate_val != 0) {
      period_ms *= osc_calibrate_val;
    }

    write_reg_32bit(driver, SYSTEM_INTERMEASUREMENT_PERIOD, period_ms);

    write_reg(driver, SYSRANGE_START, 0x04);
  } else {
    write_reg(driver, SYSRANGE_START, 0x02);
  }
}

void stop_continuous(vl53l0x_driver_t* driver) {
  write_reg(driver, SYSRANGE_START, 0x01);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);
  write_reg(driver, 0x91, 0x00);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
}

uint16_t read_range_continuous_millimeters(vl53l0x_driver_t* driver,
                                           stat_info_t* extra_stats) {
  uint8_t temp_buffer[12];
  uint16_t temp;
  driver->_start_timeout(g_io_timeout);
  while ((read_reg(driver, RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
    if (driver->_check_timeout_expired(g_io_timeout)) {
      g_is_timeout = true;
      return 65535;
    }
  }
  if (extra_stats == 0) {
    temp = read_reg_16bit(driver, RESULT_RANGE_STATUS + 10);
  } else {
    read_multi(driver, 0x14, temp_buffer, 12);
    extra_stats->range_status = temp_buffer[0x00] >> 3;
    extra_stats->spad_cnt = (temp_buffer[0x02] << 8) | temp_buffer[0x03];
    extra_stats->signal_cnt = (temp_buffer[0x06] << 8) | temp_buffer[0x07];
    extra_stats->ambient_cnt = (temp_buffer[0x08] << 8) | temp_buffer[0x09];
    temp = (temp_buffer[0x0A] << 8) | temp_buffer[0x0B];
    extra_stats->raw_distance = temp;
  }
  write_reg(driver, SYSTEM_INTERRUPT_CLEAR, 0x01);
  return temp;
}

uint16_t read_range_single_millimeters(vl53l0x_driver_t* driver,
                                       stat_info_t* extra_stats) {
  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);
  write_reg(driver, 0x91, g_stop_variable);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x00);
  write_reg(driver, SYSRANGE_START, 0x01);
  driver->_start_timeout(g_io_timeout);
  while (read_reg(driver, SYSRANGE_START) & 0x01) {
    if (driver->_check_timeout_expired(g_io_timeout)) {
      g_is_timeout = true;
      return 65535;
    }
  }
  return read_range_continuous_millimeters(driver, extra_stats);
}

void set_timeout(vl53l0x_driver_t* driver, uint16_t timeout) {
  g_io_timeout = timeout;
}

uint16_t get_timeout(vl53l0x_driver_t* driver) { return g_io_timeout; }

bool get_spad_info(vl53l0x_driver_t* driver, uint8_t* count,
                   bool* type_is_aperture) {
  uint8_t tmp;

  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x00);

  write_reg(driver, 0xFF, 0x06);
  write_reg(driver, 0x83, read_reg(driver, 0x83) | 0x04);
  write_reg(driver, 0xFF, 0x07);
  write_reg(driver, 0x81, 0x01);
  write_reg(driver, 0x80, 0x01);
  write_reg(driver, 0x94, 0x6b);
  write_reg(driver, 0x83, 0x00);
  driver->_start_timeout(g_io_timeout);
  while (read_reg(driver, 0x83) == 0x00) {
    if (driver->_check_timeout_expired(g_io_timeout)) {
      return false;
    }
  }
  write_reg(driver, 0x83, 0x01);
  tmp = read_reg(driver, 0x92);
  *count = tmp & 0x7f;
  *type_is_aperture = (tmp >> 7) & 0x01;
  write_reg(driver, 0x81, 0x00);
  write_reg(driver, 0xFF, 0x06);
  write_reg(driver, 0x83, read_reg(driver, 0x83) & ~0x04);
  write_reg(driver, 0xFF, 0x01);
  write_reg(driver, 0x00, 0x01);
  write_reg(driver, 0xFF, 0x00);
  write_reg(driver, 0x80, 0x00);
  return true;
}

void get_sequence_step_enables(vl53l0x_driver_t* driver,
                               sequence_step_enables* enables) {
  uint8_t sequence_config = read_reg(driver, SYSTEM_SEQUENCE_CONFIG);

  enables->tcc = (sequence_config >> 4) & 0x1;
  enables->dss = (sequence_config >> 3) & 0x1;
  enables->msrc = (sequence_config >> 2) & 0x1;
  enables->pre_range = (sequence_config >> 6) & 0x1;
  enables->final_range = (sequence_config >> 7) & 0x1;
}

void get_sequence_step_timeouts(vl53l0x_driver_t* driver,
                                sequence_step_enables const* enables,
                                sequence_step_timeouts* timeouts) {
  timeouts->pre_range_vcsel_period_pclks =
      get_vcsel_pulse_period(driver, vcsel_period_pre_range);

  timeouts->msrc_dss_tcc_mclks =
      read_reg(driver, MSRC_CONFIG_TIMEOUT_MACROP) + 1;
  timeouts->msrc_dss_tcc_us = timeout_mclks_to_microseconds(
      timeouts->msrc_dss_tcc_mclks, timeouts->pre_range_vcsel_period_pclks);

  timeouts->pre_range_mclks = decode_timeout(
      read_reg_16bit(driver, PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
  timeouts->pre_range_us = timeout_mclks_to_microseconds(
      timeouts->pre_range_mclks, timeouts->pre_range_vcsel_period_pclks);

  timeouts->final_range_vcsel_period_pclks =
      get_vcsel_pulse_period(driver, vcsel_period_final_range);

  timeouts->final_range_mclks = decode_timeout(
      read_reg_16bit(driver, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

  if (enables->pre_range) {
    timeouts->final_range_mclks -= timeouts->pre_range_mclks;
  }

  timeouts->final_range_us = timeout_mclks_to_microseconds(
      timeouts->final_range_mclks, timeouts->final_range_vcsel_period_pclks);
}

uint16_t decode_timeout(uint16_t reg_val) {
  return (uint16_t)((reg_val & 0x00FF) << (uint16_t)((reg_val & 0xFF00) >> 8)) +
         1;
}

uint16_t encode_timeout(uint16_t timeout_mclks) {
  uint32_t ls_byte = 0;
  uint16_t ms_byte = 0;

  if (timeout_mclks > 0) {
    ls_byte = timeout_mclks - 1;

    while ((ls_byte & 0xFFFFFF00) > 0) {
      ls_byte >>= 1;
      ms_byte++;
    }

    return (ms_byte << 8) | (ls_byte & 0xFF);
  } else {
    return 0;
  }
}

uint32_t timeout_mclks_to_microseconds(uint16_t timeout_period_mclks,
                                       uint8_t vcsel_period_pclks) {
  uint32_t macro_period_ns = calc_macro_period(vcsel_period_pclks);

  return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) /
         1000;
}

uint32_t timeout_microseconds_to_mclks(uint32_t timeout_period_us,
                                       uint8_t vcsel_period_pclks) {
  uint32_t macro_period_ns = calc_macro_period(vcsel_period_pclks);

  return (((timeout_period_us * 1000) + (macro_period_ns / 2)) /
          macro_period_ns);
}

bool perform_single_ref_calibration(vl53l0x_driver_t* driver,
                                    uint8_t vhv_init_byte) {
  write_reg(driver, SYSRANGE_START, 0x01 | vhv_init_byte);
  driver->_start_timeout(g_io_timeout);
  while ((read_reg(driver, RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
    if (driver->_check_timeout_expired(g_io_timeout)) {
      return false;
    }
  }

  write_reg(driver, SYSTEM_INTERRUPT_CLEAR, 0x01);

  write_reg(driver, SYSRANGE_START, 0x00);

  return true;
}
