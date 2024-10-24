// Copyright 2024 NO oNe

#include "TMC2209_impl.h"

#include "TMC2209.h"

#ifdef __cplusplus
extern "C" {
#endif

HardwareSerial& serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 2000;
const int32_t VELOCITY = 20000;
// current values may need to be reduced to prevent overheating depending on
// specific motor and power supply voltage
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t RX_PIN = 5;
const uint8_t TX_PIN = 4;

TMC2209 stepper_drivers[STEPPER_MOTOR_MAX_NUM];

#ifdef EXPERIMENT_BOARD
uint16_t pins[STEPPER_MOTOR_MAX_NUM] = {12, 21};

TMC2209::SerialAddress addresses[STEPPER_MOTOR_MAX_NUM] = {
    TMC2209::SERIAL_ADDRESS_0, TMC2209::SERIAL_ADDRESS_1};

#else
uint16_t pins[STEPPER_MOTOR_MAX_NUM] = {37};

TMC2209::SerialAddress addresses[STEPPER_MOTOR_MAX_NUM] = {
    TMC2209::SERIAL_ADDRESS_3};
#endif

static void _config(void) {
  static bool configured = false;
  if (configured) {
    return;
  }
  initArduino();
  for (uint8_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  configured = true;
}

void tmc2209_c_init(stepper_motor_instances_t instance) {
  _config();
  stepper_drivers[instance].setup(serial_stream, SERIAL_BAUD_RATE,
                                  addresses[instance]);
  delay(2000);
  stepper_drivers[instance].setOperationModeToSerial(addresses[instance]);

  stepper_drivers[instance].setRunCurrent(RUN_CURRENT_PERCENT);
  stepper_drivers[instance].enableCoolStep();
  stepper_drivers[instance].enable();
}

void tmc2209_c_enable(stepper_motor_instances_t instance) {
  digitalWrite(pins[instance], LOW);
}

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int32_t speed) {
  stepper_drivers[instance].moveAtVelocity(speed);
}

void tmc2209_c_set_current(stepper_motor_instances_t instance,
                           uint8_t current_percent) {
  stepper_drivers[instance].setRunCurrent(current_percent);
}

void tmc2209_c_set_microsteps_per_step_pow_2(stepper_motor_instances_t instance,
                                             uint8_t exponent) {
  stepper_drivers[instance].setMicrostepsPerStepPowerOfTwo(exponent);
}

void tmc2209_c_enable_automatic_current_scaling(
    stepper_motor_instances_t instance) {
  stepper_drivers[instance].enableAutomaticCurrentScaling();
}

void tmc2209_c_disable(stepper_motor_instances_t instance) {
  digitalWrite(pins[instance], HIGH);
}

void tmc2209_c_stop(stepper_motor_instances_t instance) {
  stepper_drivers[instance].moveAtVelocity(0);
}

stepper_motor_status_t tmc2209_c_get_status(
    stepper_motor_instances_t instance) {
  bool hardware_disabled = stepper_drivers[instance].hardwareDisabled();
  if (hardware_disabled) {
    return STEPPER_MOTOR_HARDWARE_DISABLED;
  }

  bool communicating = stepper_drivers[instance].isSetupAndCommunicating();
  if (!communicating) {
    return STEPPER_MOTOR_NOT_SETUP_AND_COMMUNICATING;
  }

  TMC2209::Settings settings = stepper_drivers[instance].getSettings();
  if (!settings.software_enabled) {
    return STEPPER_MOTOR_SOFTWARE_DISABLED;
  }

  TMC2209::Status status = stepper_drivers[instance].getStatus();
  if (!status.standstill) {
    return STEPPER_MOTOR_MOVING;
  } else {
    return STEPPER_MOTOR_STOPPED;
  }
  return STEPPER_MOTOR_STATUS_OK;
}

#ifdef __cplusplus
}
#endif
