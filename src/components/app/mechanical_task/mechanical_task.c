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

  char buffer[MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE];
  motor_controller_data_header_to_string(buffer);
  sd_card_on_motor_controller_data_received(
      buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE);

  uint8_t ctr = 0;

  while (1) {
    handle_door_limit_switches();
    handle_motor_limit_switches();
    // handle_motor_problems();
    // if (++ctr == 20) {
    //   MACKI_LOG_INFO(TAG, "BUFFERED MECHANICAL DATA");
    //   motor_controller_data_to_string(buffer);
    //   sd_card_on_motor_controller_data_received(
    //       buffer, MOTOR_CONTROLLER_DATA_SD_BUFFER_SIZE);
    //   ctr = 0;
    // }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
