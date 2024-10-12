// Copyright 2024 MACKI

#include "log_manager.h"

#define TAG "LOG_MANAGER"
#define CONCATENATION_FAILED "LOG_STRING_CONCATENATION_FAILED"

log_manager_status_t log_manager_init(log_manager_t* manager) {
  if (manager == NULL) {
    ESP_LOGE(TAG, "Logger init fail");
    return LOGGER_ERROR;
  }
  manager->initalized = true;

  ring_buffer_init(&manager->log_buffer, CONFIG_LOG_BUFFER_SIZE);
  return LOGGER_OK;
}

log_manager_status_t log_manager_add_receiver(log_manager_t* manager,
                                              log_receiver_t* receiver) {
  if (manager == NULL || receiver == NULL) {
    return LOGGER_ERROR;
  }
  if (manager->num_receivers >= CONFIG_LOG_RECEIVERS_COUNT) {
    ESP_LOGE(TAG, "Receivers are full");
    return LOGGER_FULL_RECEIVERS;
  }
  manager->receivers[manager->num_receivers] = receiver;
  manager->num_receivers++;
  return LOGGER_OK;
}

log_manager_status_t log_manager_log_message(log_manager_t* manager,
                                             const char* tag, char* message) {
  if (manager == NULL || tag == NULL || message == NULL ||
      !manager->initalized) {
    return LOGGER_ERROR;
  }

  if (ring_buffer_is_full(&manager->log_buffer) == RING_BUFFER_FULL) {
    ESP_LOGE(TAG, "Log buffer is full %s", message);
    return LOGGER_FULL_BUFFER;
  }

  log_string_t log_string = {
      .timestamp = rtc_wrapper_get_time_ms(),
  };

  strncpy(log_string.tag, tag, CONFIG_LOG_TAG_CHAR_BUFFER_LEN);
  strncpy(log_string.message, message, CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN);

  void* data = malloc(sizeof(log_string_t));
  if (data == NULL) {
    return LOGGER_ERROR;
  }
  memcpy(data, &log_string, sizeof(log_string_t));

  if (ring_buffer_push(&manager->log_buffer, data) != RING_BUFFER_OK) {
    ESP_LOGE(TAG, "Log buffer is full%s", message);
    return LOGGER_FULL_BUFFER;
  }

  return LOGGER_OK;
}

log_manager_status_t log_manager_save_logs(log_manager_t* manager) {
  if (manager == NULL || !manager->initalized) {
    return LOGGER_ERROR;
  }
  if (ring_buffer_is_empty(&manager->log_buffer) == RING_BUFFER_EMPTY) {
    return LOGGER_EMPTY_BUFFER;
  }
  void* data;
  while (ring_buffer_pop(&manager->log_buffer, &data) == RING_BUFFER_OK) {
    log_string_t* new_data = ((log_string_t*)data);
    char* new_message = log_manager_concatenate_log_string(*new_data);

    for (uint8_t i = 0; i < manager->num_receivers; i++) {
      manager->receivers[i]->process_log(new_message, strlen(new_message));
      printf("Log: %s", new_message);
    }
    free(new_message);
    free(data);
  }
  return LOGGER_OK;
}

char* log_manager_concatenate_log_string(log_string_t log_string) {
  char* new_message = (char*)malloc(CONFIG_LOG_TOTAL_CHAR_BUFFER_LEN);

  int16_t result = snprintf(new_message, CONFIG_LOG_TOTAL_CHAR_BUFFER_LEN,
                            "(%" PRId64 ") %s; %s\r\n", log_string.timestamp,
                            log_string.tag, log_string.message);

  if (result < 0) {
    free(new_message);
    return CONCATENATION_FAILED;
  }

  return new_message;
}
