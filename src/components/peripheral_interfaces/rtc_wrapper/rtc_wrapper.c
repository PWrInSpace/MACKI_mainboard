// Copyright 2024 MACKI

#include "rtc_wrapper.h"

static rtc_wrapper_t rtc_wrapper = {.time_base_us = 0};

void rtc_wrapper_init() { rtc_wrapper.time_base_us = 0; }

int64_t rtc_wrapper_get_time_us() {
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec - rtc_wrapper.time_base_us;
}

int64_t rtc_wrapper_get_time_ms() { return rtc_wrapper_get_time_us() / 1000; }

void rtc_wrapper_reset_time_base() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  rtc_wrapper.time_base_us = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}
