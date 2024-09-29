// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stddef.h>
#include <stdint.h>

/*!
 * Safely converts two uint8_t bytes to int16_t
 */
int16_t int16_from_uint8_bytes(uint8_t bytes[2]);
