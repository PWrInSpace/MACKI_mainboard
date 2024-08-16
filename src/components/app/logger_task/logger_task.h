// Copyright 2024 MACKI

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "usb_cdc_interface.h"
#include "macki_log.h"
#include "esp_log.h"

typedef enum {
  LOG_RECEIVER_USB = 0,
  LOG_RECEIVER_MAX_NUM = 1
} log_receiver_instance_t;

/*!
 * @brief Initializes and runs the logger task
 * @param pvParameters unused
 */
void logger_task(void* pvParameters);
