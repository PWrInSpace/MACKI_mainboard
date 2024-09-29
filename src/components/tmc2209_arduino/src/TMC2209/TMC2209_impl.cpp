// Copyright 2024 NO oNe

#include "TMC2209_impl.h"

#include "TMC2209.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TMC2209;

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

uint16_t pins[STEPPER_MOTOR_MAX_NUM] = {45};

TMC2209::SerialAddress addresses[STEPPER_MOTOR_MAX_NUM] = {
    TMC2209::SERIAL_ADDRESS_2};

static void _config(void) {
  static bool configured = false;
  if (configured) {
    return;
  }
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  for (uint8_t i = 0; i < STEPPER_MOTOR_MAX_NUM; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
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

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int16_t speed) {
  stepper_drivers[instance].moveAtVelocity(speed);
}

void tmc2209_c_disable(stepper_motor_instances_t instance) {
  digitalWrite(pins[instance], HIGH);
}

const char* tmc2209_c_get_status(stepper_motor_instances_t instance) {
  bool hardware_disabled = stepper_drivers[instance].hardwareDisabled();
  if (hardware_disabled) {
    return "Hardware disabled\n";
  }

  bool communicating = stepper_drivers[instance].isSetupAndCommunicating();
  if (!communicating) {
    return "Not setup and communicating\n";
  }

  TMC2209::Settings settings = stepper_drivers[instance].getSettings();
  if (!settings.software_enabled) {
    return "Software disabled\n";
  }

  TMC2209::Status status = stepper_drivers[instance].getStatus();
  if (!status.standstill) {
    return "Moving\n";
  } else {
    return "Stopped, somethin wrong\n";
  }
  return "Okej okej\n";
}

#ifdef __cplusplus
}
#endif
