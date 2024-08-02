// Copyright 2024 MACKI

#pragma once

#include <stdint.h>

#include "sdkconfig.h"

typedef void (*_on_log_received)(const char* tag, const char* message);

typedef struct {
  uint8_t instance;
  _on_log_received process_log;
} log_receiver_t;
