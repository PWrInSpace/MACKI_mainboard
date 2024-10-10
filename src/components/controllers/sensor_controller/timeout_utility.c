// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "timeout_utility.h"
#include "rtc_wrapper.h"

static int64_t g_current_timeout_len_ms = 0;
static int64_t g_timeout_start_ms = 0;

void start_timeout(uint16_t timeout) {
  g_timeout_start_ms = rtc_wrapper_get_time_ms();
  g_current_timeout_len_ms = timeout;
}

bool check_timeout_expired(uint16_t timeout) {
  int64_t current_time = rtc_wrapper_get_time_ms();
  return (current_time - g_timeout_start_ms) > timeout;
}