// Copyright 2024 MACKI

#include "macki_log.h"

static log_manager_t macki_manager = {
    .num_receivers = 0,
    .receivers = {NULL},
    .log_buffer = {0},
};

log_manager_status_t macki_log(log_level_t level, const char* tag,
                          const char* message) {
  return log_manager_log_message(&macki_manager, level, tag, message);
}

log_manager_t* get_macki_log_manager_instance() { return &macki_manager; }
