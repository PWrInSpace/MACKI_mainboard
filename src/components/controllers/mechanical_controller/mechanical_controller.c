// Copyright 2024 MACKI, Krzysztof Gliwiński

// TODO(Glibus): When implementing this later, get inspiration from the
// solenoid_implementation.h file on mvp branch

#include "mechanical_controller.h"

#include <freertos/FreeRTOS.h>

#include "gpio_wrapper.h"
#include "macki_log.h"
#include "solenoid_driver.h"

#define TAG "MECHANICAL_CONTROLLER"

#define MOTOR_BUMP_SPEED -6000

typedef struct {
  solenoid_driver_t solenoid_driver[VALVE_INSTANCE_MAX];
  limit_switch_pair_t motor_limit_switches[STEPPER_MOTOR_MAX_NUM];
  limit_switch_t door_limit_switches[DOOR_LIMIT_SWITCH_MAX];
  stepper_motor_permissions_t motor_permissions[STEPPER_MOTOR_MAX_NUM];
  int32_t motor_speed[STEPPER_MOTOR_MAX_NUM];
} mechanical_controller_drivers_t;

static struct {
  bool initialized;
  bool blocked;
} controller_state = {.initialized = false, .blocked = false};

/// @note Drivers for stepper motors are implemented in TMC2209_impl
static mechanical_controller_drivers_t drivers = {
    .solenoid_driver =
        {
            [VALVE_INSTANCE_0] = {.initialized = false,
                                  ._set_pin = &expander_2_gpio_set,
                                  .gpio_pin = VALVE_1},
            [VALVE_INSTANCE_1] = {.initialized = false,
                                  ._set_pin = &expander_2_gpio_set,
                                  .gpio_pin = VALVE_2},
            [VALVE_INSTANCE_2] = {.initialized = false,
                                  ._set_pin = &expander_2_gpio_set,
                                  .gpio_pin = VALVE_3},
            [VALVE_INSTANCE_3] = {.initialized = false,
                                  ._set_pin = &expander_2_gpio_set,
                                  .gpio_pin = VALVE_4},
        },
    .motor_limit_switches =
        {
            [STEPPER_MOTOR_0] =
                {.top_limit_switch = {.limit_switch_pin_num = LIMIT_SW_1,
                                      .gpio_expander_instance = GPIO_EXPANDER_1,
                                      .state = LIMIT_SWITCH_NOT_PRESSED},
                 .bottom_limit_switch = {.limit_switch_pin_num = LIMIT_SW_2,
                                         .gpio_expander_instance =
                                             GPIO_EXPANDER_1,
                                         .state = LIMIT_SWITCH_NOT_PRESSED}},
#ifdef EXPERIMENT_BOARD
            [STEPPER_MOTOR_1] =
                {.top_limit_switch = {.limit_switch_pin_num = LIMIT_SW_3,
                                      .gpio_expander_instance = GPIO_EXPANDER_1,
                                      .state = LIMIT_SWITCH_NOT_PRESSED},
                 .bottom_limit_switch = {.limit_switch_pin_num = LIMIT_SW_4,
                                         .gpio_expander_instance =
                                             GPIO_EXPANDER_1,
                                         .state = LIMIT_SWITCH_NOT_PRESSED}},
#endif
        },
    .door_limit_switches =
        {
            [DOOR_LIMIT_SWITCH_0] = {.limit_switch_pin_num = LIMIT_SW_5,
                                     .gpio_expander_instance = GPIO_EXPANDER_2,
                                     .state = LIMIT_SWITCH_NOT_PRESSED},
            [DOOR_LIMIT_SWITCH_1] = {.limit_switch_pin_num = LIMIT_SW_6,
                                     .gpio_expander_instance = GPIO_EXPANDER_2,
                                     .state = LIMIT_SWITCH_NOT_PRESSED},
        },
    .motor_permissions =
        {
            [STEPPER_MOTOR_0] = {.can_move_up = true, .can_move_down = true},
#ifdef EXPERIMENT_BOARD
            [STEPPER_MOTOR_1] = {.can_move_up = true, .can_move_down = true},
#endif
        },
};

static portMUX_TYPE motor_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*!
 * @brief Sets the motor speed without checking for permissions
 */
static mechanical_controller_status_t set_motor_speed_with_override(
    int32_t speed, stepper_motor_instances_t motor) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return MECHANICAL_CONTROLLER_NOT_INITIALIZED;
  }
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  taskENTER_CRITICAL(&motor_spinlock);
  set_motor_speed_with_override(motor, speed);
  drivers.motor_speed[motor] = speed;
  taskEXIT_CRITICAL(&motor_spinlock);
  return MECHANICAL_CONTROLLER_OK;
}

bool mechanical_controller_init() {
  bool ret = gpio_wrapper_init();
  if (!ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize GPIO wrapper");
    return false;
  }

  ret = init_gpio_expanders();
  if (!ret) {
    MACKI_LOG_ERROR(TAG, "Failed to initialize GPIO expanders");
    return false;
  }

  solenoid_driver_status_t solenoid_ret;
  for (size_t i = 0; i < VALVE_INSTANCE_MAX; i++) {
    solenoid_ret = solenoid_driver_init(&drivers.solenoid_driver[i]);
    if (solenoid_ret != SOLENOID_DRIVER_OK) {
      MACKI_LOG_ERROR(TAG, "Failed to initialize solenoid driver %d", i);
      return false;
    }
    solenoid_ret = solenoid_driver_close(&drivers.solenoid_driver[i]);
    if (solenoid_ret != SOLENOID_DRIVER_OK) {
      MACKI_LOG_ERROR(TAG, "Failed to close solenoid %d", i);
      return false;
    }
  }

  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; ++i) {
    tmc2209_c_init(i);
    tmc2209_c_set_current(i, 100);
    tmc2209_c_enable_automatic_current_scaling(i);
  }

  stepper_motor_status_t motor_ret;
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    motor_ret = tmc2209_c_get_status(i);
    if (motor_ret > STEPPER_MOTOR_HARDWARE_DISABLED) {
      MACKI_LOG_ERROR(TAG, "Something wrong with motor %d: status %d", i,
                      motor_ret);
      return false;
    }
  }
  controller_state.initialized = true;
  return true;
}

void block_mechanics() {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return;
  }
  if (controller_state.blocked) {
    return;
  }
  for (size_t i = 0; i < VALVE_INSTANCE_MAX; i++) {
    solenoid_driver_close(&drivers.solenoid_driver[i]);
  }
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    tmc2209_c_stop(i);
  }
  MACKI_LOG_INFO(TAG, "Mechanical controller blocked");
  controller_state.blocked = true;
}

void unblock_mechanics() {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return;
  }
  if (!controller_state.blocked) {
    return;
  }
  MACKI_LOG_INFO(TAG, "Mechanical controller unblocked");
  controller_state.blocked = false;
}

limit_switch_state_t check_door_limit_switches() {
  limit_switch_state_t level = check_limit_switch_state(
      &drivers.door_limit_switches[DOOR_LIMIT_SWITCH_0]);
  if (level == LIMIT_SWITCH_NOT_PRESSED) {
    return LIMIT_SWITCH_NOT_PRESSED;
  }

  level = check_limit_switch_state(
      &drivers.door_limit_switches[DOOR_LIMIT_SWITCH_1]);
  return level;
}

void handle_door_limit_switches() {
  limit_switch_state_t level = check_door_limit_switches();
  // We block the controller if any of the limit switches is not pressed
  if (level == LIMIT_SWITCH_NOT_PRESSED) {
    block_mechanics();
  } else {
    unblock_mechanics();
  }
}

bool check_motor_limit_switches() {
  bool ret = false;

  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // Limit switches will have their state updated in the check function
    limit_switch_state_t top_level = check_limit_switch_state(
        &drivers.motor_limit_switches[i].top_limit_switch);
    limit_switch_state_t bottom_level = check_limit_switch_state(
        &drivers.motor_limit_switches[i].bottom_limit_switch);

    MACKI_LOG_INFO(TAG, "Motor %d top level: %d, bottom level: %d", i,
                   top_level, bottom_level);

    if (top_level == LIMIT_SWITCH_PRESSED ||
        bottom_level == LIMIT_SWITCH_PRESSED) {
      ret = true;
    }
  }
  return ret;
}

bool bump_motor_from_limit_switch(stepper_motor_instances_t motor,
                                  limit_switch_t* limit_switch) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return false;
  }
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return false;
  }
  controller_state.blocked = true;

  int32_t motor_speed = MOTOR_BUMP_SPEED;
  if (drivers.motor_limit_switches[motor].top_limit_switch.state ==
      LIMIT_SWITCH_PRESSED) {
    // we need to go down
    motor_speed = -motor_speed;
  }
  taskENTER_CRITICAL(&motor_spinlock);
  // Both motors need to coordinate here
  for (uint8_t i = 0; i < STEPPER_MOTOR_MAX_NUM; ++i) {
    set_motor_speed_with_override(i, motor_speed);
  }
  taskEXIT_CRITICAL(&motor_spinlock);

  do {
    vTaskDelay(pdMS_TO_TICKS(10));
  } while (check_limit_switch_state(limit_switch) == LIMIT_SWITCH_PRESSED);

  taskENTER_CRITICAL(&motor_spinlock);
  for (uint8_t i = 0; i < STEPPER_MOTOR_MAX_NUM; ++i) {
    tmc2209_c_stop(i);
  }
  taskEXIT_CRITICAL(&motor_spinlock);

  controller_state.blocked = false;
  return true;
}

void handle_motor_limit_switches() {
  bool ret = check_motor_limit_switches();
  // None of the limit switches are pressed, the motors can move wherever
  if (!ret) {
    for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
      drivers.motor_permissions[i].can_move_up = true;
      drivers.motor_permissions[i].can_move_down = true;
    }
    return;
  }
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // Check top level limit switch
    limit_switch_state_t top_level =
        drivers.motor_limit_switches[i].top_limit_switch.state;
    if (top_level == LIMIT_SWITCH_PRESSED) {
      drivers.motor_permissions[i].can_move_up = false;
      tmc2209_c_stop(i);
      bump_motor_from_limit_switch(
          i, &drivers.motor_limit_switches[i].top_limit_switch);
      drivers.motor_permissions[i].can_move_up = true;
    } else {
      drivers.motor_permissions[i].can_move_up = true;
    }

    // Check bottom level limit switch
    limit_switch_state_t bottom_level =
        drivers.motor_limit_switches[i].bottom_limit_switch.state;
    if (bottom_level == LIMIT_SWITCH_PRESSED) {
      drivers.motor_permissions[i].can_move_down = false;
      tmc2209_c_stop(i);
      bump_motor_from_limit_switch(
          i, &drivers.motor_limit_switches[i].bottom_limit_switch);
      drivers.motor_permissions[i].can_move_down = true;
    } else {
      drivers.motor_permissions[i].can_move_down = true;
    }
  }
}

mechanical_controller_status_t solenoid_open(valve_instance_t valve) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return MECHANICAL_CONTROLLER_NOT_INITIALIZED;
  }
  if (controller_state.blocked) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller blocked");
    return MECHANICAL_CONTROLLER_BLOCKED;
  }
  if (valve >= VALVE_INSTANCE_MAX) {
    MACKI_LOG_ERROR(TAG, "Invalid valve instance");
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  solenoid_driver_status_t ret =
      solenoid_driver_open(&drivers.solenoid_driver[valve]);
  if (ret != SOLENOID_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to open solenoid %d", valve);
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  return MECHANICAL_CONTROLLER_OK;
}

mechanical_controller_status_t solenoid_close(valve_instance_t valve) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return MECHANICAL_CONTROLLER_NOT_INITIALIZED;
  }
  if (controller_state.blocked) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller blocked");
    return MECHANICAL_CONTROLLER_BLOCKED;
  }
  if (valve >= VALVE_INSTANCE_MAX) {
    MACKI_LOG_ERROR(TAG, "Invalid valve instance");
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  solenoid_driver_status_t ret =
      solenoid_driver_close(&drivers.solenoid_driver[valve]);
  if (ret != SOLENOID_DRIVER_OK) {
    MACKI_LOG_ERROR(TAG, "Failed to close solenoid %d", valve);
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  return MECHANICAL_CONTROLLER_OK;
}

static bool check_motor_permissions(int32_t speed,
                                    stepper_motor_instances_t motor) {
  if (!drivers.motor_permissions[motor].can_move_up && speed > 0) {
    MACKI_LOG_ERROR(TAG, "Motor %d cannot move up", motor);
    return false;
  }
  if (!drivers.motor_permissions[motor].can_move_down && speed < 0) {
    MACKI_LOG_ERROR(TAG, "Motor %d cannot move down", motor);
    return false;
  }
  return true;
}

mechanical_controller_status_t motor_set_speed(
    int32_t speed, stepper_motor_instances_t motor) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return MECHANICAL_CONTROLLER_NOT_INITIALIZED;
  }
  if (controller_state.blocked) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller blocked");
    return MECHANICAL_CONTROLLER_BLOCKED;
  }
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  bool permissions_status = check_motor_permissions(speed, motor);
  if (!permissions_status) {
    return MECHANICAL_CONTROLLER_DRIVER_ERROR;
  }
  taskENTER_CRITICAL(&motor_spinlock);
  set_motor_speed_with_override(motor, speed);
  taskEXIT_CRITICAL(&motor_spinlock);
  return MECHANICAL_CONTROLLER_OK;
}

mechanical_controller_status_t motor_set_speed_all_motors(int32_t speed) {
  if (!controller_state.initialized) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller not initialized");
    return MECHANICAL_CONTROLLER_NOT_INITIALIZED;
  }
  if (controller_state.blocked) {
    MACKI_LOG_ERROR(TAG, "Mechanical controller blocked");
    return MECHANICAL_CONTROLLER_BLOCKED;
  }
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    bool permissions_status = check_motor_permissions(speed, i);
    if (!permissions_status) {
      return MECHANICAL_CONTROLLER_DRIVER_ERROR;
    }
  }
  taskENTER_CRITICAL(&motor_spinlock);
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    set_motor_speed_with_override(i, speed);
  }
  taskEXIT_CRITICAL(&motor_spinlock);
  return MECHANICAL_CONTROLLER_OK;
}
