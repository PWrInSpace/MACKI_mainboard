// Copyright 2024 MACKI

#include "logger_task.h"

static struct logger_task_data {
  log_receiver_t log_receivers[LOG_RECEIVER_MAX_NUM];
  log_manager_t log_manager;
} logger_task_data;

static void usb_cdc_receiver_init(void) {
  logger_task_data.log_receivers[LOG_RECEIVER_USB].instance = LOG_RECEIVER_USB;
  logger_task_data.log_receivers[LOG_RECEIVER_USB].process_log = usb_cdc_on_log_received;
}

void logger_task(void* pvParameters) {
  (void)pvParameters;
  usb_cdc_receiver_init();

  while(1){
    macki_log(&logger_task_data.log_manager, log_manager, "logger_task", 0, "Hello World!");
    save_logs();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}