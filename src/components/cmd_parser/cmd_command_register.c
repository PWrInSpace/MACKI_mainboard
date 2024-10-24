// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "cmd_command_register.h"

#include "cmd_parser.h"
#include "cmd_procedure.h"

static bool cmd_register_commands(const esp_console_cmd_t *commands,
                                  size_t commands_count) {
  for (size_t i = 0; i < commands_count; i++) {
    if (esp_console_cmd_register(commands + i) != ESP_OK) {
      return false;
    }
  }

  return true;
}

bool cmd_register_move_valve(void) {
  const esp_console_cmd_t open_cmd[] = {{.command = "move_valve",
                                         .help = "move valve",
                                         .hint = NULL,
                                         .func = cmd_move_valve}};
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);
  cmd_register_commands(open_cmd, number_of_commands);
  return true;
}

bool cmd_register_read_data(void) {
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

bool cmd_register_set_motor_speed(void) {
  const esp_console_cmd_t open_cmd[] = {
      {.command = "set_motor_speed",
       .help = "set motor speed",
       .hint = NULL,
       .func = cmd_set_motor_speed},
  };
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);

  cmd_register_commands(open_cmd, number_of_commands);

  return true;
}

bool cmd_register_procedure(void) {
  const esp_console_cmd_t open_cmd[] = {{.command = "procedure",
                                         .help = "procedure",
                                         .hint = NULL,
                                         .func = cmd_procedure},
                                        {.command = "procedure_check",
                                         .help = "procedure_check",
                                         .hint = NULL,
                                         .func = cmd_procedure_check},
                                        {.command = "procedure_stop",
                                         .help = "procedure_stop",
                                         .hint = NULL,
                                         .func = cmd_procedure_stop}};
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);
  cmd_register_commands(open_cmd, number_of_commands);
  return true;
}
