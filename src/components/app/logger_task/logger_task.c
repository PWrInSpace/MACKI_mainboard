// Copyright 2024 MACKI

#include "logger_task.h"

#define TAG "LOGGER_TASK"

static struct logger_task_data {
  log_receiver_t log_receivers[LOG_RECEIVER_MAX_NUM];
  log_manager_t* log_manager;
} logger_task_data;

static void log_components_init(void) {
  logger_task_data.log_manager = get_macki_log_manager_instance();
  logger_task_data.log_manager->num_receivers = (uint8_t)LOG_RECEIVER_MAX_NUM;
  log_manager_init(logger_task_data.log_manager);
}

static void usb_cdc_receiver_init(void) {
  logger_task_data.log_receivers[LOG_RECEIVER_USB].instance = LOG_RECEIVER_USB;
  logger_task_data.log_receivers[LOG_RECEIVER_USB].process_log =
      usb_cdc_on_log_received;
  log_manager_add_receiver(logger_task_data.log_manager,
               &(logger_task_data.log_receivers[LOG_RECEIVER_USB]));
}

void logger_task(void* pvParameters) {
  (void)pvParameters;
  usb_cdc_receiver_init();

  log_components_init();

  while (1) {
    // ESP_LOG_INFO(TAG, "Creating log");
    MACKI_LOG_ERROR("LOGGER_TASK", "This is an error message");
    vTaskDelay(pdMS_TO_TICKS(1000));
    // ESP_LOG_INFO(TAG, "Saving logs");
    log_manager_save_logs(logger_task_data.log_manager);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
