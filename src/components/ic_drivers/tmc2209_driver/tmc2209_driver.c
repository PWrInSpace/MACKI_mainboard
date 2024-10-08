// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "tmc2209_driver.h"

#include "tmc2209_datagram.h"
#include "tmc2209_registers.h"

#define TAG "TMC2209_DRIVER"

#define TMC2209_STATUS_CHECK(status) \
  if (status != TMC2209_DRIVER_OK) { \
    return status;                   \
  }

#define TMC2209_WRITE_BIT 1
#define TMC2209_READ_BIT 0
#define TMC2209_WRITE_ACCESS_CONSTANT 0x80

static tmc2209_driver_status_t _tmc2209_check_driver(tmc2209_driver_t *driver) {
  if (driver == NULL) {
    return TMC2209_DRIVER_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_init(tmc2209_driver_t *driver) {
  if (driver == NULL) {
    return TMC2209_DRIVER_ERROR;
  }

  tmc2209_reg_gstat_t gstat;
  gstat.raw = 0;
  gstat.drv_err = 1;

  TMC2209_STATUS_CHECK(
      tmc2209_write_register(driver, TMC_2209_REG_GSTAT, gstat.raw));

  tmc2209_reg_gconf_t gconf;
  gconf.raw = 0;
  gconf.I_scale_analog = 0;
  gconf.pdn_disable = 1;
  gconf.mstep_reg_select = 1;
  gconf.multistep_filt = 1;
  tmc2209_reg_ifcnt_t ifcnt;

  TMC2209_STATUS_CHECK(
      tmc2209_write_register(driver, TMC_2209_REG_GCONF, gconf.raw));

  TMC2209_STATUS_CHECK(tmc2209_driver_disable(driver));

  TMC2209_STATUS_CHECK(
      tmc2209_read_register(driver, TMC_2209_REG_GSTAT, &gstat.raw));
  TMC2209_STATUS_CHECK(
      tmc2209_write_register(driver, TMC_2209_REG_GSTAT, gstat.raw));

  TMC2209_STATUS_CHECK(
      tmc2209_read_register(driver, TMC_2209_REG_GCONF, &gconf.raw));

  TMC2209_STATUS_CHECK(
      tmc2209_read_register(driver, TMC_2209_REG_IFCNT, &ifcnt.raw));

  uint8_t if_cnt = (uint8_t)ifcnt.raw;

  gconf.pdn_disable = 1;
  gconf.mstep_reg_select = 1;
  TMC2209_STATUS_CHECK(
      tmc2209_write_register(driver, TMC_2209_REG_GCONF, gconf.raw));

  TMC2209_STATUS_CHECK(
      tmc2209_read_register(driver, TMC_2209_REG_IFCNT, &ifcnt.raw));

  tmc2209_driver_status_t ret = ((uint8_t)ifcnt.raw == if_cnt + 2)
                                    ? TMC2209_DRIVER_OK
                                    : TMC2209_DRIVER_ERROR;

  // TMC2209_STATUS_CHECK(tmc2209_set_speed(0));
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }
  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_enable(tmc2209_driver_t *driver) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  // Enable pin is active low
  if (driver->_enable_pin_set(driver->en_pin, false) == false) {
    return TMC2209_DRIVER_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_driver_disable(tmc2209_driver_t *driver) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  // Enable pin is active low
  if (driver->_enable_pin_set(driver->en_pin, true) == false) {
    return TMC2209_DRIVER_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_write_register(tmc2209_driver_t *driver,
                                               uint8_t reg_address,
                                               uint32_t reg_value) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }
  uint8_t data[TMC2209_DATAGRAM_SIZE_BYTES];
  tmc2209_datagram_t datagram = tmc2209_create_datagram(
      driver->address, reg_address, &reg_value, TMC2209_WRITE_BIT);

  tmc2209_datagram_serialize(&datagram, data);

  if (driver->_send_data(data, TMC2209_DATAGRAM_SIZE_BYTES) == false) {
    return TMC2209_UART_TRANSACTION_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

static tmc2209_driver_status_t tmc2209_driver_send_read_command(
    tmc2209_driver_t *driver, uint8_t reg_address) {
  uint8_t data[TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES];
  tmc2209_datagram_read_command_t read_command_datagram =
      tmc2209_create_datagram_read_command(driver->address, reg_address,
                                           TMC2209_READ_BIT);

  tmc2209_datagram_read_command_serialize(&read_command_datagram, data);

  if (driver->_send_data(data, TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES) ==
      false) {
    return TMC2209_UART_TRANSACTION_ERROR;
  }

  return TMC2209_DRIVER_OK;
}

static tmc2209_driver_status_t tmc2209_driver_read_register_after_command(
    tmc2209_driver_t *driver, uint8_t reg_address, uint32_t *reg_value) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  uint8_t received_data[TMC2209_DATAGRAM_SIZE_BYTES];
  if (driver->_receive_data(received_data, TMC2209_DATAGRAM_SIZE_BYTES) ==
      false) {
    return TMC2209_UART_TRANSACTION_ERROR;
  }

  tmc2209_datagram_t received_datagram;
  tmc2209_datagram_deserialize(received_data, &received_datagram);

  uint32_t data_out = received_datagram.data;
  *reg_value = data_out;

  return TMC2209_DRIVER_OK;
}

tmc2209_driver_status_t tmc2209_read_register(tmc2209_driver_t *driver,
                                              uint8_t reg_address,
                                              uint32_t *reg_value) {
  tmc2209_driver_status_t ret = _tmc2209_check_driver(driver);
  if (ret != TMC2209_DRIVER_OK) {
    return ret;
  }

  if (tmc2209_driver_send_read_command(driver, reg_address) !=
      TMC2209_DRIVER_OK) {
    return TMC2209_UART_TRANSACTION_ERROR;
  }

  driver->_delay(driver->uart_read_delay_ms);

  ret = tmc2209_driver_read_register_after_command(driver, reg_address,
                                                   reg_value);
  if (ret != TMC2209_DRIVER_OK) {
    return TMC2209_UART_TRANSACTION_ERROR;
  }

  return TMC2209_DRIVER_OK;
}
