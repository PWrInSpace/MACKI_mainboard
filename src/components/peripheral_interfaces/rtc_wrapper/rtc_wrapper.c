// Copyright 2024 MACKI

#include "rtc_wrapper.h"

#define TV_SEC_CONSTANT 1000000L
#define MS_TO_US 1000

static rtc_wrapper_t rtc_wrapper = {.time_base_us = 0};

void rtc_wrapper_init() { rtc_wrapper.time_base_us = 0; }

int64_t rtc_wrapper_get_time_us() {
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (int64_t)tv_now.tv_sec * TV_SEC_CONSTANT + (int64_t)tv_now.tv_usec -
         rtc_wrapper.time_base_us;
}

int64_t rtc_wrapper_get_time_ms() {
  return rtc_wrapper_get_time_us() / MS_TO_US;
}

void rtc_wrapper_reset_time_base() {
  rtc_wrapper.time_base_us = rtc_wrapper_get_time_us();
}
