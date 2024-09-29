// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "motor_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

void motor_task(void *pvParameters) {
  (void)pvParameters;
    int16_t speed = 20000;
  tmc2209_c_init(STEPPER_MOTOR_1);
  tmc2209_c_enable(STEPPER_MOTOR_1);
  tmc2209_c_set_speed(STEPPER_MOTOR_1, 20000);
  vTaskDelay(pdMS_TO_TICKS(2000));

  while(1) {
    tmc2209_c_set_speed(STEPPER_MOTOR_1, speed);
    speed = -speed;
    printf("Speed: %d\n", speed);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#ifdef __cplusplus
}
#endif