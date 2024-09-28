// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "uart_console_interface.h"

#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"

#define TAG "UART_CONSOLE_INTERFACE"

static struct config {
  uart_config_t uart_config;
  uart_port_t uart_num;
  esp_console_config_t console_config;
  bool initialized;
} config = {.uart_config =
                {
                    .baud_rate = 115200,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .source_clk = UART_SCLK_DEFAULT,
                },
            .uart_num = CONFIG_ESP_CONSOLE_UART_NUM,
            .console_config =
                {
                    .max_cmdline_args = 8,
                    .max_cmdline_length = 256,
                },
            .initialized = false};

static bool _configure_default_uart() {
  // Drain stdout before reconfiguring it
  fflush(stdout);
  fsync(fileno(stdout));

  // Disable buffering on stdin
  setvbuf(stdin, NULL, _IONBF, 0);

  // Minicom, screen, idf_monitor send CR when ENTER key is pressed
  esp_vfs_dev_uart_port_set_rx_line_endings(config.uart_num,
                                            ESP_LINE_ENDINGS_CR);
  // Move the caret to the beginning of the next line on '\n'
  esp_vfs_dev_uart_port_set_tx_line_endings(config.uart_num,
                                            ESP_LINE_ENDINGS_CRLF);

  // Install UART driver for interrupt-driven reads and writes
  esp_err_t err = uart_driver_install(config.uart_num, 256, 0, 0, NULL, 0);
  if (err != ESP_OK) {
    return false;
  }

  err = uart_param_config(config.uart_num, &config.uart_config);
  if (err != ESP_OK) {
    uart_driver_delete(config.uart_num);
    return false;
  }

  // Tell VFS to use UART driver
  esp_vfs_dev_uart_use_driver(config.uart_num);

  return true;
}

static void _configure_linenoise(uint8_t command_max_len) {
  // Configure linenoise line completion library
  // Enable multiline editing.
  linenoiseSetMultiLine(1);

  // Tell linenoise where to get command completions and hints
  linenoiseSetCompletionCallback(&esp_console_get_completion);
  linenoiseSetHintsCallback((linenoiseHintsCallback*)&esp_console_get_hint);

  // Set command history size
  linenoiseHistorySetMaxLen(100);

  // Set command maximum length
  linenoiseSetMaxLineLen(command_max_len);

  // Don't return empty lines
  linenoiseAllowEmpty(true);
}

uart_console_interface_status_t uart_console_interface_init() {
  if (!_configure_default_uart()) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  if (esp_console_init(&config.console_config) != ESP_OK) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  _configure_linenoise(config.console_config.max_cmdline_length);

  config.initialized = true;
  return UART_CONSOLE_INTERFACE_STATUS_OK;
}

uart_console_interface_status_t uart_console_interface_deinit() {
  if (!config.initialized) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  if (esp_console_deinit() != ESP_OK) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  if (uart_driver_delete(config.uart_num) != ESP_OK) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  config.initialized = false;
  return UART_CONSOLE_INTERFACE_STATUS_OK;
}

uart_console_interface_status_t uart_console_register_commands(
    const esp_console_cmd_t* commands, size_t commands_count) {
  if (esp_console_register_help_command() != ESP_OK) {
    return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }

  for (size_t i = 0; i < commands_count; i++) {
    if (esp_console_cmd_register(commands + i) != ESP_OK) {
      return UART_CONSOLE_INTERFACE_STATUS_ERROR;
    }
  }

  return UART_CONSOLE_INTERFACE_STATUS_OK;
}

bool uart_console_is_initialized() { return config.initialized; }

uart_console_interface_status_t uart_console_parse_line(char* line) {
  int return_code;

  esp_err_t err = esp_console_run(line, &return_code);
  switch (err) {
    case ESP_ERR_NOT_FOUND:
      return UART_CONSOLE_INTERFACE_UNRECOGNIZED_COMMAND;
    case ESP_ERR_INVALID_STATE:
      return UART_CONSOLE_INTERFACE_UNINITIALIZED;
    case ESP_OK:
    case ESP_ERR_INVALID_ARG:
      return UART_CONSOLE_INTERFACE_STATUS_OK;
    default:
      return UART_CONSOLE_INTERFACE_STATUS_ERROR;
  }
}

void uart_console_add_line_to_history(char* line) {
  if (strlen(line) > 0) {
    linenoiseHistoryAdd(line);
  }
}

bool uart_console_check_escape_sequences(void) {
  // Figure out if the terminal supports escape sequences
  int probe_status = linenoiseProbe();
  if (probe_status != 0) {  // zero indicates success
    linenoiseSetDumbMode(1);
    return false;
  }
  return true;
}

char* uart_console_get_line(const char* cli_prompt) {
  return linenoise(cli_prompt);
}

void uart_console_free_line(void* line) { linenoiseFree(line); }

const char* uart_console_status_to_string(
    uart_console_interface_status_t status) {
  switch (status) {
    case UART_CONSOLE_INTERFACE_STATUS_OK:
      return "OK";
    case UART_CONSOLE_INTERFACE_STATUS_ERROR:
      return "ERROR";
    case UART_CONSOLE_INTERFACE_UNRECOGNIZED_COMMAND:
      return "UNRECOGNIZED COMMAND";
    case UART_CONSOLE_INTERFACE_UNINITIALIZED:
      return "UNINITIALIZED";
    default:
      return "UNKNOWN ERROR CODE";
  }
}
