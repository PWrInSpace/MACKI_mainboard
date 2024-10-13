// Copyright 2024 PWrInSpace, Kuba

// Code based on example from esp-idf documentation
// https://github.com/espressif/esp-idf/blob/v5.2/examples/system/console/advanced/main/console_example_main.c

#include "cli_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise/linenoise.h"
#include "macki_log.h"
#include "uart_console_interface.h"

#define TAG "CLI"

static struct {
  TaskHandle_t task_handle;
} gb;

static void _cli_task(void* arg) {
  char* line;

  bool ret = uart_console_check_escape_sequences();
  if (ret == false) {
    MACKI_LOG_ERROR(TAG, "Terminal doesn't support escape sequences");
    CLI_WRITE_ERR("Terminal doesn't support escape sequences");
  }

  while (true) {
    // The line is returned when ENTER is pressed, so this functions blocks the
    // task
    line = uart_console_get_line(CLI_PROMPT);
    if (line != NULL) {
      MACKI_LOG_INFO(TAG, "Received line: %s/TU KONIEC LINII", line);
      uart_console_add_line_to_history(line);
      uart_console_interface_status_t ret = uart_console_parse_line(line);
      if (ret != UART_CONSOLE_INTERFACE_STATUS_OK) {
        CLI_WRITE_ERR("Error during command parsing %s",
                      uart_console_status_to_string(ret));
        MACKI_LOG_ERROR(TAG, "Error during command parsing %s",
                        uart_console_status_to_string(ret));
      }
    } else {
      CLI_WRITE_ERR("Error during line reading");
      MACKI_LOG_ERROR(TAG, "Error during line reading");
    }

    uart_console_free_line((void*)line);  // allocates line on the heap
  }
}

bool cli_run(void) {
  uart_console_interface_status_t ret = uart_console_interface_init();
  if (ret != UART_CONSOLE_INTERFACE_STATUS_OK) {
    MACKI_LOG_ERROR(TAG, "Error during uart console interface initialization");
    return false;
  }

  cmd_register_common();
  cmd_register_dummy();

  if (gb.task_handle != NULL) {
    MACKI_LOG_ERROR(TAG, "Task already running");
    return false;
  }

  if (esp_console_register_help_command() != ESP_OK) {
    MACKI_LOG_ERROR(TAG, "esp_console_init wasn't called");
    return false;
  }

  xTaskCreatePinnedToCore(_cli_task, "CLItask", CLI_TASK_STACK_DEPTH, NULL,
                          CLI_TASK_PRIORITY, &gb.task_handle, CLI_TASK_CPU_NUM);

  if (gb.task_handle == NULL) {
    return false;
  }

  return true;
}

bool cli_deinit(void) {
  uart_console_interface_status_t ret = uart_console_interface_deinit();
  if (ret != UART_CONSOLE_INTERFACE_STATUS_OK) {
    MACKI_LOG_ERROR(TAG,
                    "Error during uart console interface deinitialization");
    return false;
  }

  vTaskDelete(gb.task_handle);
  gb.task_handle = NULL;

  return true;
}
