// Copyright 2024 PWrInSpace, Kuba

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "cmd_command_defines.h"

#define CLI_WRITE_OK(format, ...) printf("OK: " format "\n", ##__VA_ARGS__)
#define CLI_WRITE_ERR(format, ...) printf("ERR: " format "\n", ##__VA_ARGS__)
#define CLI_WRITE(format, ...) printf(format "\n", ##__VA_ARGS__)

bool cmd_register_common(void);
