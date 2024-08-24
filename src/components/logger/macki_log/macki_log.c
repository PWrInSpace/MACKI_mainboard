// Copyright 2024 MACKI

#include "macki_log.h"

static log_manager_t macki_manager = {
    .num_receivers = 0,
    .receivers = {NULL},
    .log_buffer = {0},
};

#define CONFIG_LOG_MESSAGE_MAX_SIZE 255

log_manager_status_t macki_log(log_level_t level, const char* tag,
                               const char* message, ...) {
  va_list args;
  va_start(args, message);
  char new_message[CONFIG_LOG_MESSAGE_MAX_SIZE];
  vsnprintf(new_message, CONFIG_LOG_MESSAGE_MAX_SIZE, message, args);
  va_end(args);
  return log_manager_log_message(&macki_manager, level, tag, new_message);
}

log_manager_t* get_macki_log_manager_instance() { return &macki_manager; }
