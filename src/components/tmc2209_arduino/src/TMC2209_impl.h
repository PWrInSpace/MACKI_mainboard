// Copyright NO ONE NO WHERE

#include <stdint.h>

typedef enum {
  STEPPER_MOTOR_1,
  STEPPER_MOTOR_2,
  STEPPER_MOTOR_3,
  STEPPER_MOTOR_4,
  STEPPER_MOTOR_MAX_NUM
} stepper_motor_instances_t;

#ifdef __cplusplus
extern "C" {
#endif

void tmc2209_c_init(stepper_motor_instances_t instance);

void tmc2209_c_enable(stepper_motor_instances_t instance);

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int16_t speed);

void tmc2209_c_disable(stepper_motor_instances_t instance);

#ifdef __cplusplus
}
#endif
