// Copyright 2024 MACKI

#include "logger_task.h"
#include "esp_log.h"

#define TAG "LOGGER_TASK"

static struct logger_task_data {
  log_receiver_t log_receivers[LOG_RECEIVER_MAX_NUM];
  log_manager_t* log_manager;
} logger_task_data;

static void log_components_init(void) {
  logger_init(logger_task_data.log_receivers, LOG_RECEIVER_MAX_NUM);
  logger_task_data.log_manager = get_log_manager_instance();
}

static void usb_cdc_receiver_init(void) {
  logger_task_data.log_receivers[LOG_RECEIVER_USB].instance = LOG_RECEIVER_USB;
  logger_task_data.log_receivers[LOG_RECEIVER_USB].process_log = usb_cdc_on_log_received;
}

void logger_task(void* pvParameters) {
  (void)pvParameters;
  usb_cdc_receiver_init();
  
  log_components_init();

  while(1){
    ESP_LOG_INFO(TAG, "Creating log");
    macki_log(0, TAG, "Hello World!\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOG_INFO(TAG, "Saving logs");
    save_logs();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}