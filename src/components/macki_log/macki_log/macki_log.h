// Copyright 2024 MACKI

#pragma once

#include <stdarg.h>
#include <stdint.h>

#include "log_manager.h"
#include "sdkconfig.h"

typedef enum {
  LOG_LEVEL_TRACE = 0,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_MAX_NUM
} log_level_t;

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

// TODO(Glibus): add log levels per log receiver to make things more complicated :)
#if CONFIG_MACKI_LOG_LEVEL <= 0
#define MACKI_LOG_TRACE(tag, message, ...) \
  macki_log(LOG_LEVEL_TRACE, tag, message, ##__VA_ARGS__)
#else
#define MACKI_LOG_TRACE(tag, message, ...)
#endif

#if CONFIG_MACKI_LOG_LEVEL <= 1
#define MACKI_LOG_DEBUG(tag, message, ...) \
  macki_log(LOG_LEVEL_DEBUG, tag, message, ##__VA_ARGS__)
#else
#define MACKI_LOG_DEBUG(tag, message, ...)
#endif

#if CONFIG_MACKI_LOG_LEVEL <= 2
#define MACKI_LOG_INFO(tag, message, ...) \
  macki_log(LOG_LEVEL_INFO, tag, message, ##__VA_ARGS__)
#else
#define MACKI_LOG_INFO(tag, message, ...)
#endif

#if CONFIG_MACKI_LOG_LEVEL <= 3
#define MACKI_LOG_WARN(tag, message, ...) \
  macki_log(LOG_LEVEL_WARN, tag, message, ##__VA_ARGS__)
#else
#define MACKI_LOG_WARN(tag, message, ...)
#endif

#if CONFIG_MACKI_LOG_LEVEL <= 4
#define MACKI_LOG_ERROR(tag, message, ...) \
  macki_log(LOG_LEVEL_ERROR, tag, message, ##__VA_ARGS__)
#else
#define MACKI_LOG_ERROR(tag, message, ...)
#endif
