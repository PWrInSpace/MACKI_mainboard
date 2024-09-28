// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "tmc2209_datagram.h"

#include "macki_log.h"

#define TAG "TMC2209_DATAGRAM"

#define TMC2209_SYNC 0b1010
#define TMC2209_RESERVED 0x00

static uint8_t _tmc2209_crc8(uint8_t *data, size_t size) {
  uint8_t crc = 0xFF;
  for (size_t i = 0; i < size; i++) {
    crc ^= data[i];
    for (size_t j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

tmc2209_datagram_t tmc2209_create_datagram(uint8_t node_address,
                                           uint8_t reg_address, uint32_t *data,
                                           bool rw) {
  static uint8_t datagram_serialized[TMC2209_DATAGRAM_SIZE_BYTES];

  tmc2209_sync_reserved_t sync_reserved;
  sync_reserved.sync = TMC2209_SYNC;
  sync_reserved.reserved = TMC2209_RESERVED;

  tmc2209_datagram_t datagram;
  datagram.sync_reserved = sync_reserved.raw;
  datagram.node_address = node_address;
  datagram.address = reg_address;
  datagram.data = *data;

  tmc2209_datagram_serialize(&datagram, datagram_serialized);

  datagram.crc =
      _tmc2209_crc8(datagram_serialized, sizeof(tmc2209_datagram_t) - 1);

  return datagram;
}

tmc2209_datagram_read_command_t tmc2209_create_read_command_datagram(
    uint8_t node_address, uint8_t reg_address, bool rw) {
  tmc2209_sync_reserved_t sync_reserved;
  sync_reserved.sync = TMC2209_SYNC;
  sync_reserved.reserved = TMC2209_RESERVED;

  tmc2209_datagram_read_command_t datagram;
  datagram.sync_reserved = sync_reserved.raw;
  datagram.node_address = node_address;
  datagram.address = reg_address;
  datagram.crc = _tmc2209_crc8((uint8_t *)&datagram,
                               sizeof(tmc2209_datagram_read_command_t) - 1);

  return datagram;
}

void tmc2209_datagram_serialize(
    tmc2209_datagram_t *datagram,
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES]) {
  if (datagram == NULL) {
    MACKI_LOG_ERROR(TAG, "Datagram is NULL");
    return;
  }

  if (serialized_datagram == NULL) {
    MACKI_LOG_ERROR(TAG, "Buffer is NULL");
    return;
  }

  serialized_datagram[0] = datagram->sync_reserved;
  serialized_datagram[1] = datagram->node_address;
  serialized_datagram[2] = datagram->address;
  serialized_datagram[3] = (datagram->data >> 24U) & 0xFF;
  serialized_datagram[4] = (datagram->data >> 16U) & 0xFF;
  serialized_datagram[5] = (datagram->data >> 8U) & 0xFF;
  serialized_datagram[6] = datagram->data & 0xFF;
  serialized_datagram[7] = datagram->crc;
}

void tmc2209_datagram_deserialize(
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES],
    tmc2209_datagram_t *datagram) {
  if (serialized_datagram == NULL) {
    MACKI_LOG_ERROR(TAG, "Serialized datagram is NULL");
    return;
  }

  if (datagram == NULL) {
    MACKI_LOG_ERROR(TAG, "Datagram is NULL");
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
    MACKI_LOG_ERROR(TAG, "Datagram is NULL");
    return;
  }

  if (buffer == NULL) {
    MACKI_LOG_ERROR(TAG, "Buffer is NULL");
    return;
  }

  buffer[0] = datagram->sync_reserved;
  buffer[1] = datagram->node_address;
  buffer[2] = datagram->address;
  buffer[3] = datagram->crc;
}
