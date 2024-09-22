// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "safe_bitwise_shifts.h"

int16_t int16_from_uint8_bytes(uint8_t bytes[2]) {
  uint16_t result = (bytes[1] << 8) | bytes[0];
  return (int16_t)result;
}
