// Copyright 2024 MACKI

#include "macki_log.h"

static log_manager_t macki_manager = {
    .num_receivers = 0,
    .receivers = {NULL},
    .log_buffer = {0},
};

static const char* char_macki_log_level_to_string[] = {"TRACE", "DEBUG", "INFO",
                                                       "WARN", "ERROR"};


static const char* macki_log_level_to_string(log_level_t level) {
  if (level >= LOG_LEVEL_MAX_NUM) {
    return "UNKNOWN";
  }
  return char_macki_log_level_to_string[level];
}

log_manager_status_t macki_log(log_level_t level, const char* tag,
                               const char* message, ...) {
  char message_with_log_level[CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN];
  snprintf(message_with_log_level, CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN, "%s: %s", macki_log_level_to_string(level),
           message);
  va_list args;
  va_start(args, message);
  char new_message[CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN];
  vsnprintf(new_message, CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN, message_with_log_level, args);
  va_end(args);

  return log_manager_log_message(&macki_manager, tag, new_message);
}

log_manager_t* get_macki_log_manager_instance() { return &macki_manager; }

