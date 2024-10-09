// Copyright 2024 PWrInSpace, Kuba
#include "cmd_parser.h"

#include "macki_log.h"
#include "sensor_controller.h"


static int cmd_read_data(int argc, char **argv) {
  sensor_controller_data_u data;
  data.data = sensor_controller_get_last_data();

  CLI_PUT(CLI_ACK);
  for (size_t i = 0; i < sizeof(data.raw); i++) {
    CLI_PUT("%c", data.raw[i]);
  }
  CLI_PUT(CLI_EOL);

  return 0;
}

static bool cmd_register_commands(const esp_console_cmd_t *commands,
                                  size_t commands_count) {
  for (size_t i = 0; i < commands_count; i++) {
    if (esp_console_cmd_register(commands + i) != ESP_OK) {
      return false;
    }
  }

  return true;
}

bool cmd_register_common(void) {
  const esp_console_cmd_t open_cmd[] = {
      {.command = "data",
       .help = "read data",
       .hint = NULL,
       .func = cmd_read_data},
  };
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);

  cmd_register_commands(open_cmd, number_of_commands);

  return true;
}

static int cmd_dummy(int argc, char **argv) {
  CLI_WRITE_OK("Dummy command");
  return 0;
}

bool cmd_register_dummy(void) {
  const esp_console_cmd_t open_cmd[] = {
      {.command = "dummy",
       .help = "dummy command",
       .hint = NULL,
       .func = cmd_dummy},
  };
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);

  cmd_register_commands(open_cmd, number_of_commands);

  return true;
}
