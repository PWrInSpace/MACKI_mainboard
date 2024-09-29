// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include "tmc2209_c.h"
#include <stdbool.h>

typedef enum {
  VALVE_INSTANCE_0 = 0,
  VALVE_INSTANCE_1,
  VALVE_INSTANCE_2,
  VALVE_INSTANCE_3,
  VALVE_INSTANCE_MAX
} valve_instance_t;

void init_solenoid_pins();

void init_limit_switch();

bool check_limit_switch_state();

bool solenoid_open(valve_instance_t valve);

bool solenoid_close(valve_instance_t valve);

void block_motors();

void unblock_motors();

bool motor_set_speed(int16_t speed, stepper_motor_instances_t motor);

void block_solenoids();

void unblock_solenoids();