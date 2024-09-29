// Copyright 2024 PWrInSpace, Kuba
#include "cmd_parser.h"

#include "macki_log.h"
#include "solenoid_implementation.h"

static int cmd_read_data(int argc, char **argv) {
  char buffer[255] = "RZYGANIE";

  CLI_WRITE_OK("%s", buffer);
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
      {.command = "read_data",
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

static int cmd_move_valve(int argc, char **argv) {
  if (argc != 3) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  valve_instance_t valve = atoi(argv[1]);
  bool open_close = atoi(argv[2]);
  if (open_close) {
    solenoid_open(valve);
  } else {
    solenoid_close(valve);
  }

  printf("Valve %d %s\n", valve, open_close ? "opened" : "closed");

  return 0;
}

bool cmd_register_move_valve(void) {
  const esp_console_cmd_t open_cmd[] = {
      {.command = "move_valve",
       .help = "move valve",
       .hint = NULL,
       .func = cmd_move_valve},
  };
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);
  init_solenoid_pins();
  cmd_register_commands(open_cmd, number_of_commands);
  return true;
}

bool cmd_get_data(void) { return true; }
