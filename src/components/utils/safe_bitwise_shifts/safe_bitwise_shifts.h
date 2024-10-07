// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stddef.h>
#include <stdint.h>

/*!
 * @brief Safely converts two uint8_t bytes to int16_t
 */
int16_t int16_from_uint8_bytes(uint8_t bytes[2]);

/*!
 * @brief Safely converts two uint8_t bytes to uint16_t
 */
uint16_t uint16_from_uint8_bytes(uint8_t bytes[2]);

/*!
 * @brief Safely converts uint16_t to uint8_t[2]
 */
void uint8_from_uint16(uint16_t data, uint8_t bytes[2]);
