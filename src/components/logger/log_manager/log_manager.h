// Copyright 2024 MACKI

#pragma once

#include "log_receiver.h"
#include "ring_buffer.h"
#include "sdkconfig.h"

typedef enum{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

typedef uint32_t (*get_time)();

typedef struct{
    get_time get_timestamp;
    log_receiver_t receivers[CONFIG_LOG_RECEIVERS_COUNT];
    uint8_t num_receivers;
    ring_buffer_t log_queue;
} log_manager_t;

void init(log_manager_t* manager, log_receiver_t* receivers, uint8_t num_receivers);

void macki_log(log_manager_t* manager, log_level_t level, const char* tag, const char* message);

void add_receiver(log_manager_t* manager, log_receiver_t* receiver);

void save_logs(log_manager_t* manager);

