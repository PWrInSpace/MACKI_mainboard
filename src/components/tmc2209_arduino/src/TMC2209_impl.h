// Copyright NO ONE NO WHERE

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  STEPPER_MOTOR_0,
  // STEPPER_MOTOR_1,
  STEPPER_MOTOR_MAX_NUM
} stepper_motor_instances_t;

typedef enum{
  STEPPER_MOTOR_STATUS_OK,
  STEPPER_MOTOR_MOVING,
  STEPPER_MOTOR_STOPPED,
  STEPPER_MOTOR_HARDWARE_DISABLED,
  STEPPER_MOTOR_NOT_SETUP_AND_COMMUNICATING,
  STEPPER_MOTOR_SOFTWARE_DISABLED,
  STEPPER_MOTOR_STATUS_ERROR
} stepper_motor_status_t;

typedef struct{
  bool can_move_up;
  bool can_move_down;
} stepper_motor_permissions_t;

#ifdef __cplusplus
extern "C" {
#endif

void tmc2209_c_init(stepper_motor_instances_t instance);

void tmc2209_c_enable(stepper_motor_instances_t instance);

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int16_t speed);

void tmc2209_c_disable(stepper_motor_instances_t instance);

void tmc2209_c_stop(stepper_motor_instances_t instance);

stepper_motor_status_t tmc2209_c_get_status(stepper_motor_instances_t instance);

#ifdef __cplusplus
}
#endif
