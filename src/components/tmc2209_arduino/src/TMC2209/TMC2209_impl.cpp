// Copyright 2024 NO oNe

#include "TMC2209_impl.h"

#include "TMC2209.h"


#ifdef __cplusplus
extern "C" {
#endif

struct TMC2209;

HardwareSerial &serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int DELAY = 2000;
const int32_t VELOCITY = 20000;
// current values may need to be reduced to prevent overheating depending on
// specific motor and power supply voltage
const uint8_t RUN_CURRENT_PERCENT = 100;
const uint8_t ENABLE_PIN = 45;
const uint8_t RX_PIN = 5;
const uint8_t TX_PIN = 4;

TMC2209 stepper_drivers[STEPPER_MOTOR_MAX_NUM];

uint16_t pins[STEPPER_MOTOR_MAX_NUM] = {ENABLE_PIN, 0, 0, 0};

static void _config(void) {
  static bool configured = false;
  if (configured) {
    return;
  }
  configured = true;
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  pinMode(ENABLE_PIN, OUTPUT);
}

void tmc2209_c_init(stepper_motor_instances_t instance) {
  _config();
  stepper_drivers[instance] = TMC2209();
  stepper_drivers[instance].setup(serial_stream, SERIAL_BAUD_RATE);
  stepper_drivers[instance].setHardwareEnablePin(ENABLE_PIN);
  stepper_drivers[instance].enable();
  stepper_drivers[instance].setOperationModeToSerial(TMC2209::SERIAL_ADDRESS_2);

}

void tmc2209_c_enable(stepper_motor_instances_t instance) {
    stepper_drivers[instance].enable();
}

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int16_t speed) {
    stepper_drivers[instance].moveAtVelocity(speed);
}

void tmc2209_c_disable(stepper_motor_instances_t instance) {
    stepper_drivers[instance].disable();
}

#ifdef __cplusplus
}
#endif
