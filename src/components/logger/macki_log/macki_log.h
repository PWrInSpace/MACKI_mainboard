// Copyright 2024 MACKI

#pragma once

#include <stdarg.h>
#include <stdint.h>

#include "log_manager.h"

/*!
 * @brief Logs a message
 *
 * @param manager Pointer to the log manager
 * @param level Log level
 * @param tag Log tag
 * @param message Log message
 */
log_manager_status_t macki_log(log_level_t level, const char* tag,
                               const char* message, ...);

/*!
 * @brief Gets the log manager instance pointer
 *
 * @return Pointer to the log manager
 */
log_manager_t* get_macki_log_manager_instance();

#define MACKI_LOG_TRACE(tag, message, ...) \
  macki_log(LOG_LEVEL_TRACE, tag, message, ##__VA_ARGS__)

#define MACKI_LOG_DEBUG(tag, message, ...) \
  macki_log(LOG_LEVEL_DEBUG, tag, message, ##__VA_ARGS__)

#define MACKI_LOG_INFO(tag, message, ...) \
  macki_log(LOG_LEVEL_INFO, tag, message, ##__VA_ARGS__)

#define MACKI_LOG_WARN(tag, message, ...) \
  macki_log(LOG_LEVEL_WARN, tag, message, ##__VA_ARGS__)

#define MACKI_LOG_ERROR(tag, message, ...) \
  macki_log(LOG_LEVEL_ERROR, tag, message, ##__VA_ARGS__)
