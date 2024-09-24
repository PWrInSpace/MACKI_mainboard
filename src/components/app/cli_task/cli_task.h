// Copyright 2024 PWrInSpace, Kuba

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_console.h"
#include "cmd_parser.h"

#define CLI_PROMPT ""
#define CLI_TASK_STACK_DEPTH 4096
#define CLI_TASK_PRIORITY 0
#define CLI_TASK_CPU_NUM 1

bool cli_init(uint8_t command_max_len);

bool cli_run(void);

bool cli_deinit(void);

