// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_console.h"

bool cmd_register_move_valve(void);

bool cmd_register_set_motor_speed(void);

bool cmd_register_read_data(void);

bool cmd_register_procedure(void);

bool cmd_register_reset_esp(void);