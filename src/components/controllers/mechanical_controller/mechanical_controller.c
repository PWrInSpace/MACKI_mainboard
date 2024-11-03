// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "mechanical_controller.h"

#include <freertos/FreeRTOS.h>

#include "gpio_wrapper.h"
#include "macki_log.h"
#include "solenoid_driver.h"

#define TAG "MECHANICAL_CONTROLLER"

#define MOTOR_DIRECTION_UP -1
#define MOTOR_BUMP_SPEED 20000 * MOTOR_DIRECTION_UP
#define MOTOR_SET_POSITION_SPEED 60000 * MOTOR_DIRECTION_UP

typedef struct {
  solenoid_driver_t solenoid_driver[VALVE_INSTANCE_MAX];
  limit_switch_pair_t motor_limit_switches[STEPPER_MOTOR_MAX_NUM];
  limit_switch_t door_limit_switches[DOOR_LIMIT_SWITCH_MAX];
  stepper_motor_permissions_t motor_permissions[STEPPER_MOTOR_MAX_NUM];
  int32_t motor_speed[STEPPER_MOTOR_MAX_NUM];
  motor_overheat_status_t motor_overheat_status[STEPPER_MOTOR_MAX_NUM];
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
  tmc2209_c_set_speed(motor, speed);
  taskEXIT_CRITICAL(&motor_spinlock);
  drivers.motor_speed[motor] = speed;
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
  }

  stepper_motor_status_t motor_ret;
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    motor_ret = tmc2209_c_get_status(i);
    if (motor_ret > STEPPER_MOTOR_HARDWARE_DISABLED) {
      MACKI_LOG_ERROR(TAG, "Something wrong with motor %d: status %d", i,
                      motor_ret);
      return false;
    }
    drivers.motor_speed[i] = 0;
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

void log_motor_status() {
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    stepper_motor_status_t ret = tmc2209_c_get_status(i);
    uint8_t ifcnt = tmc2209_c_get_ifcnt(i);
    bool overtemperature_warning = tmc2209_c_is_overtempretature_warning(i);
    bool overtemperature_shutdown = tmc2209_c_is_overtempretature_shut_down(i);
    MACKI_LOG_INFO(TAG,
                   "Motor %d; status: %s, ifcnt: %d, overtemperature warning: "
                   "%s, overtemperature shutdown: %s",
                   i, stepper_motor_status_to_string(ret), ifcnt,
                   overtemperature_warning ? "true" : "false",
                   overtemperature_shutdown ? "true" : "false");
  }
}

limit_switch_state_t check_door_limit_switches() {
  limit_switch_state_t level = check_limit_switch_state(
      &drivers.door_limit_switches[DOOR_LIMIT_SWITCH_0]);

  if (level == LIMIT_SWITCH_NOT_PRESSED) {
    return LIMIT_SWITCH_NOT_PRESSED;
  }

  level = check_limit_switch_state(
      &drivers.door_limit_switches[DOOR_LIMIT_SWITCH_1]);
  if (level == LIMIT_SWITCH_NOT_PRESSED) {
    return LIMIT_SWITCH_NOT_PRESSED;
  }

  return level;
}

void handle_door_limit_switches_and_overheat() {
  limit_switch_state_t level = check_door_limit_switches();
  bool overheat_shutdown = check_motor_overheat_status();
  // We block the controller if any of the limit switches is not pressed
  if ((level == LIMIT_SWITCH_NOT_PRESSED) || overheat_shutdown) {
    block_mechanics();
  } else {
    unblock_mechanics();
  }
}

bool check_motor_overheat_status() {
  bool ret = false;
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    motor_overheat_status_t overheat_status = read_motor_overheat_status(i);
    drivers.motor_overheat_status[i] = overheat_status;
    if (overheat_status == MOTOR_OVERHEAT_SHUTDOWN) {
      MACKI_LOG_ERROR(TAG, "Motor %d is in overheat shutdown state", i);
      ret = true;
    } else if (overheat_status == MOTOR_OVERHEAT_WARNING) {
      MACKI_LOG_ERROR(TAG, "Motor %d is in overheat warning state", i);
    }
  }
  return ret;
}

bool check_motor_limit_switches() {
  bool ret = false;

  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // Limit switches will have their state updated in the check function
    limit_switch_state_t top_level = check_limit_switch_state(
        &drivers.motor_limit_switches[i].top_limit_switch);
    limit_switch_state_t bottom_level = check_limit_switch_state(
        &drivers.motor_limit_switches[i].bottom_limit_switch);

    MACKI_LOG_DEBUG(TAG, "Motor %d top level: %d, bottom level: %d", i,
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
    motor_speed = motor_speed * (-1);
  }
  taskENTER_CRITICAL(&motor_spinlock);
  // Both motors need to coordinate here
  for (uint8_t i = 0; i < STEPPER_MOTOR_MAX_NUM; ++i) {
    set_motor_speed_with_override(motor_speed, i);
  }
  taskEXIT_CRITICAL(&motor_spinlock);
  uint8_t retries = 0;
  do {
    vTaskDelay(pdMS_TO_TICKS(100));
    retries++;
    if (retries >= 5) {
      MACKI_LOG_WARN(TAG,
                     "Bumping motor %d from limit switch retry count achieved",
                     motor);
      break;
    }
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

  motor_set_speed_all_motors(0);
  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // Check top level limit switch
    limit_switch_state_t top_level =
        drivers.motor_limit_switches[i].top_limit_switch.state;
    if (top_level == LIMIT_SWITCH_PRESSED) {
      drivers.motor_permissions[i].can_move_up = false;
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
  if (!drivers.motor_permissions[motor].can_move_up && speed < 0) {
    MACKI_LOG_ERROR(TAG, "Motor %d cannot move up", motor);
    return false;
  }
  if (!drivers.motor_permissions[motor].can_move_down && speed > 0) {
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
  set_motor_speed_with_override(speed, motor);
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
    set_motor_speed_with_override(speed, i);
  }
  taskEXIT_CRITICAL(&motor_spinlock);
  return MECHANICAL_CONTROLLER_OK;
}

mechanical_controller_status_t set_all_motors_in_starting_point() {
  mechanical_controller_status_t ret =
      motor_set_speed_all_motors(MOTOR_SET_POSITION_SPEED);
  // The motors will be stopped by the limit switches and then the speed will be
  // set to 0 automatically
  // We just need to wait for a bit
  vTaskDelay(pdMS_TO_TICKS(2000));
  return ret;
}

bool is_mechanical_controller_blocked() { return controller_state.blocked; }

int32_t get_motor_speed(stepper_motor_instances_t motor) {
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return 0;
  }
  return drivers.motor_speed[motor];
}

motor_overheat_status_t read_motor_overheat_status(
    stepper_motor_instances_t motor) {
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return MOTOR_NO_OVERHEAT;
  }
  bool overheat_shutdown = tmc2209_c_is_overtempretature_shut_down(motor);
  if (overheat_shutdown) {
    return MOTOR_OVERHEAT_SHUTDOWN;
  }

  bool overheat_warning = tmc2209_c_is_overtempretature_warning(motor);
  if (overheat_warning) {
    return MOTOR_OVERHEAT_WARNING;
  }
  return MOTOR_NO_OVERHEAT;
}

motor_overheat_status_t get_motor_overheat_status(
    stepper_motor_instances_t motor) {
  if (motor >= STEPPER_MOTOR_MAX_NUM) {
    MACKI_LOG_ERROR(TAG, "Invalid motor instance");
    return MOTOR_NO_OVERHEAT;
  }
  return drivers.motor_overheat_status[motor];
}

void motor_controller_data_header_to_string(
    char buffer[MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE]) {
  snprintf(
      buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE,
      "Valve 0;;Motor 0;Motor 1;Door 0;Door 1;Motor 0 top limit "
      "switch;Motor 0 bottom limit switch;Motor 1 top limit switch;Motor 1 "
      "bottom limit switch;Blocked\n");
}

void motor_controller_data_to_string(
    char buffer[MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE]) {
  for (size_t i = 0; i < VALVE_INSTANCE_MAX; i++) {
    // Valve state
    snprintf(buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE, "%d;", i,
             drivers.solenoid_driver[i].current_state);
  }

  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // motor speed
    snprintf(buffer + strlen(buffer),
             MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE - strlen(buffer), "%ld;", i,
             drivers.motor_speed[i]);
  }

  for (size_t i = 0; i < DOOR_LIMIT_SWITCH_MAX; i++) {
    // door limit switch
    snprintf(buffer + strlen(buffer),
             MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE - strlen(buffer), "%d;", i,
             drivers.door_limit_switches[i].state);
  }

  for (size_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    // Top limit switch
    snprintf(buffer + strlen(buffer),
             MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE - strlen(buffer), "%d;", i,
             drivers.motor_limit_switches[i].top_limit_switch.state);
    // Bottoms limit switch
    snprintf(buffer + strlen(buffer),
             MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE - strlen(buffer), "%d;", i,
             drivers.motor_limit_switches[i].bottom_limit_switch.state);
  }

  // Last if it's blocked
  snprintf(buffer + strlen(buffer),
           MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE - strlen(buffer), "%d;",
           controller_state.blocked);
}
