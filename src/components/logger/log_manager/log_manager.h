// Copyright 2024 MACKI

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "log_receiver.h"
#include "ring_buffer.h"
#include "rtc_wrapper.h"
#include "sdkconfig.h"

typedef enum {
  LOGGER_OK = 0,
  LOGGER_FULL_BUFFER,
  LOGGER_EMPTY_BUFFER,
  LOGGER_FULL_RECEIVERS,
  LOGGER_EMPTY_RECEIVERS,
  LOGGER_ERROR
} log_manager_status_t;

typedef uint32_t (*get_time)();

typedef struct {
  get_time get_timestamp;
  log_receiver_t* receivers[CONFIG_LOG_RECEIVERS_COUNT];
  uint8_t num_receivers;
  ring_buffer_t log_buffer;
  bool initalized;
} log_manager_t;

// Message is pre-processed with log level
typedef struct {
  char message[CONFIG_LOG_MESSAGE_CHAR_BUFFER_LEN];
  char tag[CONFIG_LOG_TAG_CHAR_BUFFER_LEN];
  int64_t timestamp;
} log_string_t;

/*!
 * @brief Initializes the log manager
 *
 * @param manager Pointer to the log manager
 */
log_manager_status_t log_manager_init(log_manager_t* manager);

/*!
 * @brief Adds a receiver to the log manager
 *
 * @param manager Pointer to the log manager
 * @param receiver Pointer to the log receiver
 */
log_manager_status_t log_manager_add_receiver(log_manager_t* manager,
                                              log_receiver_t* receiver);

/*!
 * @brief Logs a message
 *
 * @param manager Pointer to the log manager
 * @param level Log level
 * @param tag Log tag
 * @param message Log message
 */
log_manager_status_t log_manager_log_message(log_manager_t* manager,
                                             const char* tag, char* message);

/*!
 * @brief Saves all logs
 *
 * @param manager Pointer to the log manager
 */
log_manager_status_t log_manager_save_logs(log_manager_t* manager);

/*!
 * @brief Concatenates a log string
 *
 * @param log_string Log string
 * @return Concatenated log string
 */
log_manager_status_t log_manager_concatenate_log_string(
    log_string_t log_string, char buffer[CONFIG_LOG_TOTAL_CHAR_BUFFER_LEN]);
