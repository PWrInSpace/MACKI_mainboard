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
  LOG_LEVEL_TRACE = 0,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_MAX_NUM
} log_level_t;

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
} log_manager_t;

typedef struct {
  const char* message;
  size_t length;
  const char* tag;
  log_level_t level;
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
                                             log_level_t level, const char* tag,
                                             const char* message);

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
char* log_manager_concatenate_log_string(log_string_t log_string);
