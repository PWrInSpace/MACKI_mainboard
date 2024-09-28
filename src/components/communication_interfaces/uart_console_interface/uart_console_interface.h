// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "driver/uart.h"
#include "esp_console.h"

typedef enum {
  UART_CONSOLE_INTERFACE_STATUS_OK,
  UART_CONSOLE_INTERFACE_STATUS_ERROR,
  UART_CONSOLE_INTERFACE_UNRECOGNIZED_COMMAND,
  UART_CONSOLE_INTERFACE_UNINITIALIZED,
} uart_console_interface_status_t;

uart_console_interface_status_t uart_console_interface_init();

uart_console_interface_status_t uart_console_interface_deinit();

uart_console_interface_status_t uart_console_register_commands(
    const esp_console_cmd_t* commands, size_t commands_count);

bool uart_console_is_initialized();

uart_console_interface_status_t uart_console_parse_line(char* line);

void uart_console_add_line_to_history(char* line);

bool uart_console_check_escape_sequences(void);

char* uart_console_get_line(const char* cli_prompt);

void uart_console_free_line(void* line);

const char* uart_console_status_to_string(
    uart_console_interface_status_t status);
