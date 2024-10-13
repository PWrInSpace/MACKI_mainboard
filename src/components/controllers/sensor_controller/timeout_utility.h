// Copyright 2024 MACKI, Krzysztof Gliwinski

#include <stdint.h>
#include <stdbool.h>

/// @note ONLY use for vl53l0x_driver

void start_timeout(uint16_t timeout);

bool check_timeout_expired(uint16_t timeout);
