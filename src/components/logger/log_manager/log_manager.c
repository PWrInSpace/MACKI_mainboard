// Copyright 2024 MACKI

#include "log_manager.h"

#define TAG "LOG_MANAGER"
#define LOG_MESSAGE_SIZE_OFFSET 6

static const char* macki_log_level_to_string[] = {"TRACE", "DEBUG", "INFO",
                                                  "WARN", "ERROR"};

logger_status_t logger_init(log_manager_t* manager, uint8_t num_receivers) {
  if (manager == NULL || num_receivers == 0) {
    ESP_LOGE(TAG, "Logger init fail");
    return LOGGER_ERROR;
  }

  manager->num_receivers = num_receivers;

  ring_buffer_init(&manager->log_buffer, CONFIG_LOG_BUFFER_SIZE);
  return LOGGER_OK;
}

logger_status_t add_receiver(log_manager_t* manager, log_receiver_t* receiver) {
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

logger_status_t log_message(log_manager_t* manager, log_level_t level,
                            const char* tag, const char* message) {
  if (manager == NULL || tag == NULL || message == NULL) {
    return LOGGER_ERROR;
  }

  if (level >= LOG_LEVEL_MAX_NUM) {
    level = LOG_LEVEL_MAX_NUM - 1;
  }

  size_t message_len = strlen(macki_log_level_to_string[level]) + strlen(tag) +
                       strlen(message) + LOG_MESSAGE_SIZE_OFFSET;

  char* new_message = malloc(message_len * sizeof(char));
  if (new_message == NULL) {
    return LOGGER_ERROR;
  }

  strcpy(new_message, macki_log_level_to_string[level]);
  strcat(new_message, "; ");
  strcat(new_message, tag);
  strcat(new_message, ": ");
  strcat(new_message, message);
  strcat(new_message, "\r\n");

  // ESP_LOGI(TAG, "%s", new_message);

  log_string_t log_string = {.message = new_message, .length = message_len};

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

logger_status_t save_logs(log_manager_t* manager) {
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
    ESP_LOGI(TAG, "%s, %d", new_data->message, new_data->length);
    for (uint8_t i = 0; i < manager->num_receivers; i++) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      manager->receivers[i]->process_log(new_data->message, new_data->length);
    }
    free(data);
  }
  return LOGGER_OK;
}
