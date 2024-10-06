// Copyright

#include "solenoid_implementation.h"

#include "i2c_sensor_defines.h"
#include "pca9574_driver.h"
#include "solenoid_driver.h"
#include "tmc2209_c.h"
#include "gpio_wrapper.h"

#define LIMIT_SWITCH_1_PIN 0U
#define LIMIT_SWITCH_2_PIN 1U

typedef enum {
  VALVE_0 = 0,
  VALVE_1 = 1,
  VALVE_2 = 2,
  VALVE_3 = 5,
} solenoid_pins_t;

// these are connected to IC3 expander on the board
static pca957_driver_t pca9574_driver = {
    .address = 0x21,
    .pin_config = {.pins.pin0 = PCA9574_INPUT,
                   .pins.pin1 = PCA9574_INPUT,
                   .pins.pin2 = PCA9574_INPUT,
                   .pins.pin3 = PCA9574_INPUT,
                   .pins.pin4 = PCA9574_OUTPUT,
                   .pins.pin5 = PCA9574_OUTPUT,
                   .pins.pin6 = PCA9574_OUTPUT,
                   .pins.pin7 = PCA9574_OUTPUT},
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    ._pin_set = &gpio_set_pin,
    .initiated = false,
    .reset_pin = 18,
};

static pca957_driver_t pca9574_driver_lms = {
    .address = 0x20,
    .pin_config = {.pins.pin0 = PCA9574_OUTPUT,
                   .pins.pin1 = PCA9574_OUTPUT,
                   .pins.pin2 = PCA9574_INPUT,
                   .pins.pin3 = PCA9574_INPUT,
                   .pins.pin4 = PCA9574_INPUT,
                   .pins.pin5 = PCA9574_INPUT,
                   .pins.pin6 = PCA9574_INPUT,
                   .pins.pin7 = PCA9574_INPUT},
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    ._pin_set = &gpio_set_pin,
    .initiated = false,
    .reset_pin = 8,
};

static bool gpio_set_pin(uint8_t pin_number, bool value) {
  if (pca9574_driver.initiated == false) {
    pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver);
  }
  pca9574_pin_level_t level = value ? PCA9574_HIGH : PCA9574_LOW;
  pca957_driver_status_t ret =
      pca957_driver_set_level_pin(&pca9574_driver, level, (uint8_t)pin_number);
  return ret == PCA957_DRIVER_OK;
}

static solenoid_driver_t solenoid_drivers[VALVE_INSTANCE_MAX] = {
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_0},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_1},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_2},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_3},
};

static bool initialized_solenoid = false;
static bool solenoids_blocked = false;
static bool motors_blocked = false;
static bool initialized_limit_switch = false;
static bool limit_switch_pressed = false;

static bool check_solenoid_number(valve_instance_t valve) {
  if (valve > VALVE_INSTANCE_MAX) {
    return false;
  }
  if (!initialized_solenoid) {
    init_solenoid_pins();
  }
  return true;
}

static void motors_init() {
  for (uint8_t motor = 0; motor < STEPPER_MOTOR_MAX_NUM; motor++) {
    tmc2209_c_init(motor);
    tmc2209_c_set_speed(motor, 0);
  }
}

void init_solenoid_pins() {
  init_i2c_driver();
  pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver);

  ret = pca957_driver_set_level(&pca9574_driver, PCA9574_LOW);

  init_limit_switch();
  motors_init();
  initialized_solenoid = true;
}

void init_limit_switch() {
  pca957_driver_status_t ret;
  if (pca9574_driver_lms.initiated == false) {
    ret = pca957_driver_init(&pca9574_driver_lms);
  }

  pca957_driver_set_pull(&pca9574_driver_lms, LIMIT_SWITCH_1_PIN,
                             PCA9574_PULL_DOWN);
  pca957_driver_set_pull(&pca9574_driver_lms, LIMIT_SWITCH_2_PIN,
                             PCA9574_PULL_DOWN);
  initialized_limit_switch = true;
}

/// return true if the limit switch is pressed
bool check_limit_switch_state() {
  if (initialized_limit_switch == false) {
    init_limit_switch();
  }
  if (pca9574_driver_lms.initiated == false) {
    pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver_lms);
  }

  pca9574_pin_level_t level_1;
  level_1 = pca957_driver_get_level_pin(
      &pca9574_driver_lms, LIMIT_SWITCH_1_PIN);
  if (level_1 == PCA9574_HIGH) {
    limit_switch_pressed = true;
    block_solenoids();
    block_motors();
  }
  pca9574_pin_level_t level_2;
  level_2 = pca957_driver_get_level_pin(&pca9574_driver_lms,
                                          LIMIT_SWITCH_2_PIN);

  if (level_2 == PCA9574_HIGH) {
    limit_switch_pressed = true;
    block_solenoids();
    block_motors();
  }

  if (level_1 == PCA9574_LOW && level_2 == PCA9574_LOW) {
    limit_switch_pressed = false;
    unblock_solenoids();
    unblock_motors();
  }

  return limit_switch_pressed;
}

bool solenoid_open(valve_instance_t valve) {
  if (!check_solenoid_number(valve)) {
    return false;
  }
  if (solenoids_blocked) {
    return false;
  }
  if (solenoid_drivers[valve].initialized == false) {
    solenoid_driver_init(&solenoid_drivers[valve]);
  }
  return solenoid_driver_open(&solenoid_drivers[valve]) == SOLENOID_DRIVER_OK;
}

bool solenoid_close(valve_instance_t valve) {
  if (!check_solenoid_number(valve)) {
    return false;
  }
  if (solenoids_blocked) {
    return false;
  }
  if (solenoid_drivers[valve].initialized == false) {
    solenoid_driver_init(&solenoid_drivers[valve]);
  }
  return solenoid_driver_close(&solenoid_drivers[valve]) == SOLENOID_DRIVER_OK;
}

void block_solenoids() {
  for (valve_instance_t i = VALVE_INSTANCE_0; i < VALVE_INSTANCE_MAX; i++) {
    solenoid_close(i);
  }
  solenoids_blocked = true;
}

void unblock_solenoids() { solenoids_blocked = false; }

void block_motors() {
  for (uint8_t motor = 0; motor < STEPPER_MOTOR_MAX_NUM; motor++) {
    tmc2209_c_stop(motor);
  }
  motors_blocked = true;
}

void unblock_motors() { motors_blocked = false; }

bool motor_set_speed(int16_t speed, stepper_motor_instances_t motor) {
  if (motors_blocked) {
    return false;
  }
  tmc2209_c_set_speed(motor, speed);
  return true;
}
