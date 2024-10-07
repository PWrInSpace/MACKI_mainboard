// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "safe_bitwise_shifts.h"

int16_t int16_from_uint8_bytes(uint8_t bytes[2]) {
  uint16_t result = (bytes[1] << 8) | bytes[0];
  return (int16_t)result;
}

uint16_t uint16_from_uint8_bytes(uint8_t bytes[2]) {
  return (bytes[1] << 8) | bytes[0];
}

void uint8_from_uint16(uint16_t data, uint8_t bytes[2]) {
  bytes[0] = (uint8_t)(data & 0xFF);
  bytes[1] = (uint8_t)(data >> 8) & 0xFF;
}
