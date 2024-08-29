// Copyright 2024 MACKI

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct {
  int64_t time_base_us;
} rtc_wrapper_t;

/*!
 * @brief Initializes the RTC wrapper. Sets the time base to 0.
 */
void rtc_wrapper_init();

/*!
 * @brief Gets the current time in microseconds.
 * @return The current time in microseconds.
 */
int64_t rtc_wrapper_get_time_us();

/*!
 * @brief Gets the current time in milliseconds.
 * @return The current time in milliseconds.
 */
int64_t rtc_wrapper_get_time_ms();

/*!
 * @brief Gets the current time in seconds.
 * @return The current time in seconds.
 */
void rtc_wrapper_reset_time_base();
