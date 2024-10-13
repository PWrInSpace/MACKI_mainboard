// Copyright 2024 PWrInSpace, Kuba
#include "cmd_parser.h"

#include "macki_log.h"
#include "sensor_controller.h"
#include "mechanical_controller.h"
#include "freertos/FreeRTOS.h"


static int cmd_read_data(int argc, char **argv) {
  sensor_controller_data_u data;
  data.data = sensor_controller_get_last_data();

  CLI_PUT(CLI_ACK);
  for (size_t i = 0; i < sizeof(data.raw); i++) {
    CLI_PUT("%c", data.raw[i]);
  }
  CLI_PUT(CLI_EOL);
  sensor_controller_clear_buffer();

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

static int cmd_move_valve(int argc, char **argv) {
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


static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

static void stepper_motor_move(int16_t speed) {
  taskENTER_CRITICAL(&my_spinlock);
  tmc2209_c_set_speed(STEPPER_MOTOR_0, speed);
  tmc2209_c_set_speed(STEPPER_MOTOR_1, -speed);
  taskEXIT_CRITICAL(&my_spinlock);
}


static int cmd_procedure(int argc, char **argv) {
  CLI_WRITE_OK("Procedure starts");
  int16_t speed = 30000;

  CLI_WRITE("Moving motors");
  stepper_motor_move(-speed);
  vTaskDelay(pdMS_TO_TICKS(3000));


  CLI_WRITE("Stopping motors");
  stepper_motor_move(0);
  vTaskDelay(pdMS_TO_TICKS(500));

  CLI_WRITE("Pressurization");
  solenoid_open(VALVE_INSTANCE_0);
  vTaskDelay(pdMS_TO_TICKS(3000));

  CLI_WRITE("Moving motors");
  stepper_motor_move(speed);
  vTaskDelay(pdMS_TO_TICKS(3000));

  CLI_WRITE("Stopping motors");
  stepper_motor_move(0);
  vTaskDelay(pdMS_TO_TICKS(500));

  CLI_WRITE("Depresurization");
  solenoid_close(VALVE_INSTANCE_0);
  vTaskDelay(pdMS_TO_TICKS(5000));

  return 0;
}

bool cmd_register_move_valve(void) {
  const esp_console_cmd_t open_cmd[] = {
      {.command = "move_valve",
       .help = "move valve",
       .hint = NULL,
       .func = cmd_move_valve},
       {
        .command = "procedure",
        .help = "run procedure",
        .hint = NULL,
        .func = cmd_procedure
      }
  };
  size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);
  cmd_register_commands(open_cmd, number_of_commands);
  return true;
}

static int cmd_set_motor_speed(int argc, char **argv) {
  if (argc != 3) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }

  stepper_motor_instances_t motor = atoi(argv[1]);
  int16_t speed = atoi(argv[2]);

  mechanical_controller_status_t ret = motor_set_speed(speed, motor);
  if (ret != MECHANICAL_CONTROLLER_OK) {
    CLI_WRITE_ERR("Failed to set motor %d speed to %d", motor, speed);
    return 1;
  }

  CLI_WRITE_OK("Motor %d speed set to %d", motor, speed);

  return 0;
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
