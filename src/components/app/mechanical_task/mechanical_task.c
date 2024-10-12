// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "mechanical_task.h"

#include "macki_log.h"

#define TAG "MECHANICAL_TASK"

void mechanical_task(void* pvParameters) {
  bool ret = mechanical_controller_init();

  if (!ret) {
    MACKI_LOG_ERROR(
        TAG, "Failed to initialize Mechanical controller! Exiting task...");
    vTaskDelete(NULL);
  }
  MACKI_LOG_INFO(TAG, "Mechanical controller initialized");

  while (1) {
    handle_door_limit_switches();
    handle_motor_limit_switches();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
