// Copyright 2024 MACKI, Krzysztof Gliwiński

#include "mechanical_task.h"

#include "macki_log.h"
#include "sd_card_wrapper.h"

#define TAG "MECHANICAL_TASK"

void mechanical_task(void* pvParameters) {
  bool ret = mechanical_controller_init();

  if (!ret) {
    MACKI_LOG_ERROR(
        TAG, "Failed to initialize Mechanical controller! Exiting task...");
    vTaskDelete(NULL);
  }

  while (1) {
    handle_door_limit_switches_and_overheat();
    handle_motor_limit_switches();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void mechanical_sd_task(void* pvParameters) {
  char buffer[MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE];
  motor_controller_data_header_to_string(buffer);
  sd_card_on_motor_controller_data_received(
      buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE);

  vTaskDelay(pdMS_TO_TICKS(1000));
  while (1) {
    motor_controller_data_to_string(buffer);
    sd_card_on_motor_controller_data_received(
        buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
