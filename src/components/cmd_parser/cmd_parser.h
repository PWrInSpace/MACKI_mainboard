// Copyright 2024 PWrInSpace, Kuba

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "cmd_command_defines.h"

#define CLI_EOL "\n"
#define CLI_ACK "OK: "
#define CLI_NACK "ERR: "

#define CLI_WRITE_OK(format, ...) printf(CLI_ACK format CLI_EOL, ##__VA_ARGS__)
#define CLI_WRITE_ERR(format, ...) printf(CLI_NACK format CLI_EOL, ##__VA_ARGS__)
#define CLI_WRITE(format, ...) printf(format CLI_EOL, ##__VA_ARGS__)

#define CLI_PUT(format, ...) printf(format, ##__VA_ARGS__)
