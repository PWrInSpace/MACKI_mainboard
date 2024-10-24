// Copyright 2024 PWrInSpace, Kuba

#pragma once

#include <stdbool.h>
#include <stdint.h>

int cmd_read_data(int argc, char **argv);

int cmd_move_valve(int argc, char **argv);

int cmd_set_motor_speed(int argc, char **argv);

int cmd_set_both_motors_speed(int argc, char **argv);
