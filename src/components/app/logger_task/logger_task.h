// Copyright 2024 MACKI

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "log_manager.h"
#include "usb_cdc_interface.h"

typedef enum {
  LOG_RECEIVER_USB = 0,
  LOG_RECEIVER_MAX_NUM = CONFIG_LOG_RECEIVERS_COUNT
} log_receiver_instance_t;

/*!
 * @brief Initializes and runs the logger task
 * @param pvParameters unused
 */
void logger_task(void* pvParameters);
