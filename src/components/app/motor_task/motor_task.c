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
  for (uint8_t motor = 0; motor < STEPPER_MOTOR_MAX_NUM; motor++) {
    tmc2209_c_init(motor);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }

  while (1) {
    for (uint8_t motor = 0; motor < STEPPER_MOTOR_MAX_NUM; motor++) {
      tmc2209_c_set_speed(motor, speed);
      printf(tmc2209_c_get_status(motor));
    }
    speed = -speed;
    printf("Speed: %d\n", speed);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#ifdef __cplusplus
}
#endif
