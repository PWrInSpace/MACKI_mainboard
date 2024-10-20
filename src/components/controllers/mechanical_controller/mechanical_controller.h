// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>

#include "limit_switch_wrapper.h"
#include "tmc2209_wrapper_c.h"

typedef enum {
  VALVE_INSTANCE_0 = 0,
  VALVE_INSTANCE_1,
  VALVE_INSTANCE_2,
  VALVE_INSTANCE_3,
  VALVE_INSTANCE_MAX
} valve_instance_t;

typedef enum {
  DOOR_LIMIT_SWITCH_0 = 0,
  DOOR_LIMIT_SWITCH_1,
  DOOR_LIMIT_SWITCH_MAX
} door_limit_switch_instances_t;

typedef enum {
  MECHANICAL_CONTROLLER_OK = 0,
  MECHANICAL_CONTROLLER_BLOCKED,
  MECHANICAL_CONTROLLER_NOT_INITIALIZED,
  MECHANICAL_CONTROLLER_DRIVER_ERROR
} mechanical_controller_status_t;

/*!
 * @brief Initializes the mechanical controller.
 */
bool mechanical_controller_init();

/*!
 * @brief Blocks the mechanical controller. Stops and blocks all the motors.
 * Closes and blocks all the solenoids.
 */
void block_mechanics();

/*!
 * @brief Unblocks the mechanical controller. Unblocks all the motors and
 * solenoids.
 */
void unblock_mechanics();

/*!
 * @brief Checks the limit switches of the doors.
 * @return LIMIT_SWITCH_NOT_PRESSED if ANY of the limit switches is not pressed,
 * LIMIT_SWITCH_PRESSED otherwise.
 */
limit_switch_state_t check_door_limit_switches();

/*!
 * @brief Handles the limit switches of the doors.
 * It will block all mechanics if the limit switch connected to it is pressed.
 */
void handle_door_limit_switches();

/*!
 * @brief Checks the limit switches of the motors.
 * @return True if the any of the limit switches are pressed, false otherwise.
 */
bool check_motor_limit_switches();

/*!
 * @brief Handled the situation, when one of the motors is blocked by the limit
 * switch. It will bump the motor from the limit switch.
 * @note For the time of execution, it will also block the motor for other
 * users.
 */
bool bump_motor_from_limit_switch(stepper_motor_instances_t motor,
                                  limit_switch_t* limit_switch);

/*!
 * @brief Handles the limit switches of the motors.
 * It will stop the motor if the limit switch connected to it is pressed.
 * Will also block the motor to move in a particular direction if the limit
 * switch is pressed.
 */
void handle_motor_limit_switches();

/*!
 * @brief Opens the valve. Checks for the blocked state first.
 * @param valve The valve to open.
 * @return True if the valve was opened successfully, false otherwise.
 */
mechanical_controller_status_t solenoid_open(valve_instance_t valve);

/*!
 * @brief Closes the valve. Checks for the blocked state first.
 * @param valve The valve to close.
 * @return True if the valve was closed successfully, false otherwise.
 */
mechanical_controller_status_t solenoid_close(valve_instance_t valve);

/*!
 * @brief Sets the speed of the motor.
 * @param speed The speed to set.
 * @param motor The motor to set the speed of.
 * @return True if the speed was set successfully, false otherwise.
 */
mechanical_controller_status_t motor_set_speed(int32_t speed,
                                               stepper_motor_instances_t motor);
