// Copyright 2024 MACKI

#pragma once

#include <string.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include "esp_log.h"
#include "log_receiver.h"
#include "ring_buffer.h"
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
} logger_status_t;

typedef uint32_t (*get_time)();

typedef struct {
  get_time get_timestamp;
  log_receiver_t* receivers[CONFIG_LOG_RECEIVERS_COUNT];
  uint8_t num_receivers;
  ring_buffer_t log_buffer;
} log_manager_t;

typedef struct {
  char* message;
  size_t length;
} log_string_t;

/*!
 * @brief Initializes the log manager
 *
 * @param manager Pointer to the log manager
 * @param receivers Array of log receivers
 * @param num_receivers Number of log receivers
 */
logger_status_t logger_init(log_manager_t* manager,
                            uint8_t num_receivers);

/*!
 * @brief Adds a receiver to the log manager
 *
 * @param manager Pointer to the log manager
 * @param receiver Pointer to the log receiver
 */
logger_status_t add_receiver(log_manager_t* manager, log_receiver_t* receiver);

/*!
 * @brief Logs a message
 *
 * @param manager Pointer to the log manager
 * @param level Log level
 * @param tag Log tag
 * @param message Log message
 */
logger_status_t log_message(log_manager_t* manager, log_level_t level,
                            const char* tag, const char* message);

/*!
 * @brief Saves all logs
 *
 * @param manager Pointer to the log manager
 */
logger_status_t save_logs(log_manager_t* manager);
