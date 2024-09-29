// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "motor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "solenoid_implementation.h"

#ifdef __cplusplus
extern "C" {
#endif

void motor_task(void *pvParameters) {
  (void)pvParameters;
  vTaskDelay(pdMS_TO_TICKS(1000));

  int16_t speed = 20000;
  uint8_t i = 0;
  while (1) {
    // if (++i == 10) {
    //   for (uint8_t motor = 0; motor < STEPPER_MOTOR_MAX_NUM; motor++) {
    //     motor_set_speed(speed, motor);
    //   }
    //   i = 0;
    //   speed = -speed;
    // }
    check_limit_switch_state();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


#ifdef __cplusplus
}
#endif
