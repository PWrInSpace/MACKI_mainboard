// Copyright

#include "solenoid_implementation.h"

#include "i2c_sensor_defines.h"
#include "pca9574_driver.h"
#include "solenoid_driver.h"

// these are connected to IC3 expander on the board
static pca957_driver_t pca9574_driver = {
    .address = 0x21,
    ._send_receive_data = &i2c_ic_send_receive_data,
    ._send_data = &i2c_ic_send_data,
    .initiated = false,
};

typedef enum {
  VALVE_0 = 0,
  VALVE_1 = 1,
  VALVE_2 = 2,
  VALVE_3 = 5,
} solenoid_pins_t;

static bool initialized = false;

void init_solenoid_pins() {
  init_i2c_driver();
  pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver);
  ret = pca957_driver_set_mode(&pca9574_driver, PCA9574_OUTPUT);

  ret = pca957_driver_set_level(&pca9574_driver, PCA9574_LOW);
  initialized = true;
}

static bool gpio_set_pin(uint8_t pin_number, bool value) {
  if (pca9574_driver.initiated == false) {
    pca957_driver_status_t ret = pca957_driver_init(&pca9574_driver);
  }
  pca9574_pin_level_t level = value ? PCA9574_HIGH : PCA9574_LOW;
  pca957_driver_status_t ret =
      pca957_driver_set_level_pin(&pca9574_driver, level, (uint8_t)pin_number);
  return ret == PCA957_DRIVER_OK;
}

solenoid_driver_t solenoid_drivers[VALVE_INSTANCE_MAX] = {
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_0},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_1},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_2},
    {.initialized = false, ._set_pin = &gpio_set_pin, .gpio_pin = VALVE_3},
};

static bool check_solenoid_number(valve_instance_t valve) {
  if (valve > VALVE_INSTANCE_MAX) {
    return false;
  }
  if (!initialized) {
    init_solenoid_pins();
  }
  return true;
}

void solenoid_open(valve_instance_t valve) {
  if (!check_solenoid_number(valve)) {
    return;
  }
  if (solenoid_drivers[valve].initialized == false) {
    solenoid_driver_init(&solenoid_drivers[valve]);
  }
  solenoid_driver_open(&solenoid_drivers[valve]);
}

void solenoid_close(valve_instance_t valve) {
  if (!check_solenoid_number(valve)) {
    return;
  }
  if (solenoid_drivers[valve].initialized == false) {
    solenoid_driver_init(&solenoid_drivers[valve]);
  }
  solenoid_driver_close(&solenoid_drivers[valve]);
}
