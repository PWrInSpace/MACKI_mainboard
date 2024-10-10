// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "tmc2209_datagram.h"

#define TAG "TMC2209_DATAGRAM"

#define TMC2209_SYNC 0b0101
#define TMC2209_RESERVED 0x00

static uint8_t _tmc2209_crc8(uint8_t *data, size_t size) {
  uint8_t crc = 0x00;
  uint8_t current_byte;
  for (size_t i = 0; i < size; i++) {
    current_byte = data[i];
    for (size_t j = 0; j < 8; j++) {
      if ((crc >> 7) ^ (current_byte & 0x01)) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc = crc << 1;
      }
      current_byte = current_byte >> 1;
    }
  }
  return crc;
}

static uint32_t _tmc2209_reverse_data(uint32_t data) {
  static uint8_t DATA_SIZE = sizeof(data);
  static uint8_t BITS_PER_BYTE = 8;
  static uint8_t BYTE_MAX_VALUE = 0xFF;

  uint32_t reversed_data = 0;
  uint8_t right_shift;
  uint8_t left_shift;
  for (uint8_t i = 0; i < DATA_SIZE; ++i) {
    right_shift = (DATA_SIZE - i - 1) * BITS_PER_BYTE;
    left_shift = i * BITS_PER_BYTE;
    reversed_data |= ((data >> right_shift) & BYTE_MAX_VALUE) << left_shift;
  }
  return reversed_data;
}

tmc2209_datagram_t tmc2209_create_datagram(uint8_t node_address,
                                           uint8_t reg_address, uint32_t *data,
                                           bool rw) {
  tmc2209_sync_reserved_t sync_reserved;
  sync_reserved.sync = TMC2209_SYNC;
  sync_reserved.reserved = TMC2209_RESERVED;

  tmc2209_reg_address_rw_t reg_address_rw;
  reg_address_rw.reg_address = reg_address;
  reg_address_rw.rw = rw;

  tmc2209_datagram_t datagram;
  datagram.sync_reserved = sync_reserved.raw;
  datagram.node_address = node_address;
  datagram.address = reg_address_rw.raw;
  datagram.data = _tmc2209_reverse_data(*data);

  datagram.crc = _tmc2209_crc8(datagram.raw, TMC2209_DATAGRAM_SIZE_BYTES - 1);

  return datagram;
}

tmc2209_datagram_read_command_t tmc2209_create_datagram_read_command(
    uint8_t node_address, uint8_t reg_address, bool rw) {
  tmc2209_sync_reserved_t sync_reserved;
  sync_reserved.sync = TMC2209_SYNC;
  sync_reserved.reserved = TMC2209_RESERVED;

  tmc2209_reg_address_rw_t reg_address_rw;
  reg_address_rw.reg_address = reg_address;
  reg_address_rw.rw = rw;

  tmc2209_datagram_read_command_t datagram;
  datagram.sync_reserved = sync_reserved.raw;
  datagram.node_address = node_address;
  datagram.address = reg_address_rw.raw;
  datagram.crc =
      _tmc2209_crc8(datagram.raw, TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES - 1);

  return datagram;
}

void tmc2209_datagram_serialize(
    tmc2209_datagram_t *datagram,
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES]) {
  if (datagram == NULL) {
    return;
  }

  if (serialized_datagram == NULL) {
    return;
  }

  serialized_datagram[7] = datagram->sync_reserved;
  serialized_datagram[6] = datagram->node_address;
  serialized_datagram[5] = datagram->address;
  serialized_datagram[4] = (datagram->data >> 24U) & 0xFF;
  serialized_datagram[3] = (datagram->data >> 16U) & 0xFF;
  serialized_datagram[2] = (datagram->data >> 8U) & 0xFF;
  serialized_datagram[1] = datagram->data & 0xFF;
  serialized_datagram[0] = datagram->crc;
}

void tmc2209_datagram_deserialize(
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES],
    tmc2209_datagram_t *datagram) {
  if (serialized_datagram == NULL) {
    return;
  }

  if (datagram == NULL) {
    return;
  }

  datagram->sync_reserved = serialized_datagram[0];
  datagram->node_address = serialized_datagram[1];
  datagram->address = serialized_datagram[2];
  datagram->data = (serialized_datagram[3] << 24U) |
                   (serialized_datagram[4] << 16U) |
                   (serialized_datagram[5] << 8U) | serialized_datagram[6];
  datagram->crc = serialized_datagram[7];
}

void tmc2209_datagram_read_command_serialize(
    tmc2209_datagram_read_command_t *datagram,
    uint8_t buffer[TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES]) {
  if (datagram == NULL) {
    return;
  }

  if (buffer == NULL) {
    return;
  }

  buffer[3] = datagram->sync_reserved;
  buffer[2] = datagram->node_address;
  buffer[1] = datagram->address;
  buffer[0] = datagram->crc;
}
