// Copyright 2024 MACKI

#include "log_manager.h"

#define TAG "LOG_MANAGER"

logger_status_t logger_init(log_manager_t *manager, log_receiver_t** receivers,
                     uint8_t num_receivers) {
  if (manager == NULL || receivers == NULL || num_receivers == 0) {
    ESP_LOG_ERROR(TAG, "Logger init fail, %s, %s", __FILE__, __LINE__);
    return LOGGER_ERROR;
  }
  manager.num_receivers = num_receivers;
  for (uint8_t i = 0; i < num_receivers; i++) {
    manager.receivers[i] = receivers[i];
  }
  ring_buffer_init(&manager.log_buffer, CONFIG_LOG_BUFFER_SIZE);
  return LOGGER_OK;
}

logger_status_t add_receiver(log_manager_t *manager, log_receiver_t* receiver) {
  if (manager == NULL || receiver == NULL) {
    return LOGGER_ERROR;
  }
  if (manager.num_receivers >= CONFIG_LOG_RECEIVERS_COUNT) {
    ESP_LOG_ERROR(TAG, "Receivers are full, %s, %s", __FILE__, __LINE__);
    return LOGGER_FULL_RECEIVERS;
  }
  manager.receivers[manager.num_receivers] = receiver;
  manager.num_receivers++;
  return LOGGER_OK;
}

logger_status_t save_logs(log_manager_t *manager) {
  if (manager == NULL) {
    return LOGGER_ERROR;
  }
  if (ring_buffer_is_empty(&manager.log_buffer) == RING_BUFFER_EMPTY) {
    ESP_LOG_WARN(TAG, "Ring buffer is empty, %s, %s", __FILE__, __LINE__);
    return LOGGER_EMPTY_BUFFER;
  }
  void* data;
  while (ring_buffer_pop(&manager.log_buffer, &data) == RING_BUFFER_OK) {
    for (uint8_t i = 0; i < manager.num_receivers; i++) {
      manager.receivers[i]->process_log("TAG",
                                         (const char*)data);
    }
  }
  return LOGGER_OK;
}