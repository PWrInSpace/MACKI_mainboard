// Copyright 2024 MACKI

#include "log_manager.h"

#define TAG "LOG_MANAGER"

static const char* macki_log_level_to_string[] = {"TRACE", "DEBUG", "INFO",
                                                  "WARN", "ERROR"};

log_manager_status_t log_manager_init(log_manager_t* manager,
                                      uint8_t num_receivers) {
  if (manager == NULL || num_receivers == 0) {
    ESP_LOGE(TAG, "Logger init fail");
    return LOGGER_ERROR;
  }

  manager->num_receivers = num_receivers;

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
                                             log_level_t level, const char* tag,
                                             const char* message) {
  if (manager == NULL || tag == NULL || message == NULL) {
    return LOGGER_ERROR;
  }

  if (level >= LOG_LEVEL_MAX_NUM) {
    level = LOG_LEVEL_MAX_NUM - 1;
  }

  if(ring_buffer_is_full(&manager->log_buffer) == RING_BUFFER_FULL) {
    ESP_LOGE(TAG, "Log buffer is full");
    return LOGGER_FULL_BUFFER;
  }

  // ESP_LOGI(TAG, "%s", new_message);

  log_string_t log_string = {
      .message = message,
      .length = strlen(message),
      .tag = tag,
      .level = level,
      .timestamp = rtc_wrapper_get_time_ms(),
  };

  void* data = malloc(sizeof(log_string_t));
  if (data == NULL) {
    return LOGGER_ERROR;
  }
  memcpy(data, &log_string, sizeof(log_string_t));

  if (ring_buffer_push(&manager->log_buffer, data) != RING_BUFFER_OK) {
    ESP_LOGE(TAG, "Log buffer is full");
    return LOGGER_FULL_BUFFER;
  }

  return LOGGER_OK;
}

log_manager_status_t log_manager_save_logs(log_manager_t* manager) {
  if (manager == NULL) {
    return LOGGER_ERROR;
  }
  if (ring_buffer_is_empty(&manager->log_buffer) == RING_BUFFER_EMPTY) {
    ESP_LOGW(TAG, "Ring buffer is empty");
    return LOGGER_EMPTY_BUFFER;
  }
  void* data;
  while (ring_buffer_pop(&manager->log_buffer, &data) == RING_BUFFER_OK) {
    log_string_t* new_data = ((log_string_t*)data);
    char* new_message = log_manager_concatenate_log_string(*new_data);
    ESP_LOGI(TAG, "%s, %d", new_message, strlen(new_message));
    for (uint8_t i = 0; i < manager->num_receivers; i++) {
      manager->receivers[i]->process_log(new_message, strlen(new_message));
    }
    free(data);
    free(new_message);
  }
  return LOGGER_OK;
}

char* log_manager_concatenate_log_string(log_string_t log_string) {
  static const size_t log_manager_log_message_size_offset = 11;

  size_t timestamp_length = snprintf(NULL, 0, "%d", log_string.timestamp);

  size_t message_len = timestamp_length +
                       strlen(macki_log_level_to_string[log_string.level]) +
                       strlen(log_string.tag) + strlen(log_string.message) +
                       log_manager_log_message_size_offset;

  char* new_message = malloc(message_len * sizeof(char));
  if (new_message == NULL) {
    return "LOG_STRING_CONCATENATION_FAILED";
  }

  snprintf(new_message, message_len, "(%" PRId64 ") %s; %s: %s\r\n",
           log_string.timestamp, macki_log_level_to_string[log_string.level],
           log_string.tag, log_string.message);

  return new_message;
}
