// Copyright 2024 MACKI

#include "macki_log.h"


static log_manager_t macki_manager = {
    .num_receivers = 0,
    .receivers = {NULL},
    .log_buffer = {0},
};

logger_status_t macki_log(log_level_t level,
                          const char* tag, const char* message) {
  if (macki_manager == NULL || tag == NULL || message == NULL) {
    return LOGGER_ERROR;
  }
  if (ring_buffer_push(&macki_manager.log_buffer, (void*)message) !=
      RING_BUFFER_OK) {
        ESP_LOG_ERROR(TAG, "Log buffer is full, %s, %s", __FILE__, __LINE__);
    return LOGGER_FULL_BUFFER;
  }
  return LOGGER_OK;
}


log_manager_t* get_macki_log_manager_instance(){
  return &macki_manager;
}
