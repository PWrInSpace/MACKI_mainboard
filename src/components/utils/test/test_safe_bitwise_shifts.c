// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "safe_bitwise_shifts.h"
#include "unity.h"

TEST_CASE("int16_from_uint8_bytes", "[SAFE_BITWISE_SHIFTS]") {
  uint8_t bytes[2] = {0xF1U, 0xF2U};
  int16_t result = int16_from_uint8_bytes(bytes);
  TEST_ASSERT_EQUAL_INT16(0xF2F1, result);
}
