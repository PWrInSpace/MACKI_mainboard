// Copyright 2024 MACKI

#pragma once

#include <stdint.h>

/*!
 * @brief Logs a message
 *
 * @param manager Pointer to the log manager
 * @param level Log level
 * @param tag Log tag
 * @param message Log message
 */
logger_status_t macki_log(log_level_t level,
                          const char* tag, const char* message);

/*!
 * @brief Gets the log manager instance pointer
 *
 * @return Pointer to the log manager
 */
log_manager_t* get_macki_log_manager_instance();

#define MACKI_LOG_TRACE(tag, message) \
  macki_log(LOG_LEVEL_TRACE, tag, message)

#define MACKI_LOG_DEBUG(tag, message) \
    macki_log(LOG_LEVEL_DEBUG, tag, message)

#define MACKI_LOG_INFO(tag, message) \
    macki_log(LOG_LEVEL_INFO, tag, message)

#define MACKI_LOG_WARN(tag, message) \
    macki_log(LOG_LEVEL_WARN, tag, message)