// Copyright 2024 PWrInSpace, Kuba
#include "cmd_parser.h"

#include "cmd_command_register.h"
#include "cmd_defines.h"
#include "freertos/FreeRTOS.h"
#include "macki_log.h"
#include "mechanical_controller.h"
#include "procedure_parser.h"
#include "sensor_controller.h"

#define MINIMUM_ARGS_NUM_PROCEDURE 4

int cmd_read_data(int argc, char **argv) {
  sensor_controller_data_u data;
  data.data = sensor_controller_get_last_data();

  CLI_PUT(CLI_ACK);
  for (size_t i = 0; i < sizeof(data.raw); i++) {
    CLI_PUT("%c", data.raw[i]);
  }
  CLI_PUT(CLI_EOL);

  return 0;
}

int cmd_move_valve(int argc, char **argv) {
  if (argc != 3) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  valve_instance_t valve = atoi(argv[1]);
  bool open_close = atoi(argv[2]);
  mechanical_controller_status_t ret;
  if (open_close == true) {
    ret = solenoid_open(valve);
  } else if (open_close == false) {
    ret = solenoid_close(valve);
  } else {
    CLI_WRITE_ERR("Invalid argument");
    return 1;
  }

  if (ret == MECHANICAL_CONTROLLER_OK) {
    CLI_WRITE_OK("Valve %d %s\n", valve, open_close ? "opened" : "closed");
  } else {
    CLI_WRITE_ERR("Failed to %s valve %d, they're blocked\n",
                  open_close ? "open" : "close", valve);
  }

  return 0;
}

int cmd_procedure(int argc, char **argv) {
  if (argc < MINIMUM_ARGS_NUM_PROCEDURE) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }
  return 0;
}

int cmd_procedure_check(int argc, char **argv) {
  if (argc < MINIMUM_ARGS_NUM_PROCEDURE) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  procedure_t procedure;
  procedure_status_t status =
      parse_and_verify_procedure(argv + 1, argc - 1, &procedure);
  if (status != PROCEDURE_OK) {
    CLI_WRITE_ERR("Failed to parse and verify procedure, reason: %s",
                  procedure_status_to_string(status));
    return 1;
  }

  CLI_WRITE_OK("Procedure parsed and verified successfully");

  return 0;
}

int cmd_set_motor_speed(int argc, char **argv) {
  if (argc != 3) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  stepper_motor_instances_t motor = atoi(argv[1]);
  int32_t speed = atoi(argv[2]);

  mechanical_controller_status_t ret = motor_set_speed(speed, motor);
  if (ret != MECHANICAL_CONTROLLER_OK) {
    CLI_WRITE_ERR("Failed to set motor %d speed to %d", motor, speed);
    return 1;
  }

  CLI_WRITE_OK("Motor %d speed set to %d", motor, speed);

  return 0;
}

int cmd_set_both_motors_speed(int argc, char **argv) {
  if (argc != 2) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  int32_t speed = atoi(argv[1]);

  mechanical_controller_status_t ret = motor_set_speed_all_motors(speed);
  if (ret != MECHANICAL_CONTROLLER_OK) {
    CLI_WRITE_ERR("Failed to set both motors speed to %d", speed);
    return 1;
  }

  CLI_WRITE_OK("Both motors speed set to %d", speed);

  return 0;
}
