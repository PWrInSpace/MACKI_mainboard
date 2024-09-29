// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdint.h>

#include "esp_console.h"

bool cmd_register_common(void);

bool cmd_register_dummy(void);

bool cmd_register_get_data(void);

bool cmd_register_move_valve(void);

bool cmd_register_set_motor_speed(void);