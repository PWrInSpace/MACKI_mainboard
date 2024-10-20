// Copyright NO ONE NO WHERE

#include <stdbool.h>
#include <stdint.h>

#define EXPERIMENT_BOARD

typedef enum {
  STEPPER_MOTOR_0,
#ifdef EXPERIMENT_BOARD
  STEPPER_MOTOR_1,
#endif
  STEPPER_MOTOR_MAX_NUM
} stepper_motor_instances_t;

typedef enum {
  STEPPER_MOTOR_STATUS_OK,
  STEPPER_MOTOR_MOVING,
  STEPPER_MOTOR_STOPPED,
  STEPPER_MOTOR_HARDWARE_DISABLED,
  STEPPER_MOTOR_NOT_SETUP_AND_COMMUNICATING,
  STEPPER_MOTOR_SOFTWARE_DISABLED,
  STEPPER_MOTOR_STATUS_ERROR
} stepper_motor_status_t;

typedef struct {
  bool can_move_up;
  bool can_move_down;
} stepper_motor_permissions_t;

#ifdef __cplusplus
extern "C" {
#endif

void tmc2209_c_init(stepper_motor_instances_t instance);

void tmc2209_c_enable(stepper_motor_instances_t instance);

void tmc2209_c_set_speed(stepper_motor_instances_t instance, int32_t speed);

void tmc2209_c_set_current(stepper_motor_instances_t instance,
                           uint8_t current_percent);

void tmc2209_c_set_microsteps_per_step_pow_2(stepper_motor_instances_t instance,
                                             uint8_t exponent);

void tmc2209_c_enable_automatic_current_scaling(
    stepper_motor_instances_t instance);

void tmc2209_c_disable(stepper_motor_instances_t instance);

void tmc2209_c_stop(stepper_motor_instances_t instance);

stepper_motor_status_t tmc2209_c_get_status(stepper_motor_instances_t instance);

#ifdef __cplusplus
}
#endif
