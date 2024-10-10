// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define TMC2209_DATAGRAM_SIZE_BYTES 8
#define TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES 4

typedef union {
  struct {
    uint8_t sync : 4;
    uint8_t reserved : 4;
  };
  uint8_t raw;
} tmc2209_sync_reserved_t;

typedef union {
  struct {
    uint8_t reg_address : 7;
    uint8_t rw : 1;
  };
  uint8_t raw;
} tmc2209_reg_address_rw_t;

typedef union {
  struct {
    uint8_t sync_reserved;
    uint8_t node_address;
    uint8_t address;
    uint32_t data;
    uint8_t crc;
  };
  uint8_t raw[TMC2209_DATAGRAM_SIZE_BYTES];
} tmc2209_datagram_t;

typedef union {
  struct {
    uint8_t sync_reserved;
    uint8_t node_address;
    uint8_t address;
    uint8_t crc;
  };
  uint8_t raw[TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES];
} tmc2209_datagram_read_command_t;

/*!
 * @brief Creates a TMC2209 datagram
 *
 * @param node_address Address of the node
 * @param reg_address Address of the register
 * @param data Data to be written to the register
 * @param rw Read/Write flag
 * @returns tmc2209_datagram_t Created datagram
 */
tmc2209_datagram_t tmc2209_create_datagram(uint8_t node_address,
                                           uint8_t reg_address, uint32_t *data,
                                           bool rw);

/*!
 * @brief Creates a TMC2209 read command datagram
 *
 * @param node_address Address of the node
 * @param reg_address Address of the register
 * @returns tmc2209_datagram_read_command_t Created datagram
 */
tmc2209_datagram_read_command_t tmc2209_create_datagram_read_command(
    uint8_t node_address, uint8_t reg_address, bool rw);

/*!
 * @brief Serializes the TMC2209 datagram
 *
 * @param datagram Pointer to the datagram structure
 * @returns uint8_t* Serialized datagram
 */
void tmc2209_datagram_serialize(
    tmc2209_datagram_t *datagram,
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES]);

/*!
 * @brief Deserializes the TMC2209 datagram
 *
 * @param serialized_datagram Serialized datagram
 * @param datagram Pointer to the datagram structure
 */
void tmc2209_datagram_deserialize(
    uint8_t serialized_datagram[TMC2209_DATAGRAM_SIZE_BYTES],
    tmc2209_datagram_t *datagram);

/*!
 * @brief Serializes the TMC2209 read command datagram
 *
 * @param datagram Pointer to the datagram structure
 * @returns uint8_t* Serialized datagram
 */
void tmc2209_datagram_read_command_serialize(
    tmc2209_datagram_read_command_t *datagram,
    uint8_t serialized_datagram[TMC2209_DATAGRAM_READ_COMMAND_SIZE_BYTES]);
