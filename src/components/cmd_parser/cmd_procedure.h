// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define STOP_NOTIFICATION (1UL << 0)  // Define a bit for stop notification

typedef struct {
  QueueHandle_t queue_handle;
  TaskHandle_t task_handle;
} procedure_task_context_t;

typedef procedure_task_context_t *(*_get_procedure_task_context_cb)(void);

bool register_get_procedure_task_context_cb(_get_procedure_task_context_cb cb);

void init_cmd_procedure(void);

int cmd_procedure(int argc, char **argv);

int cmd_procedure_check(int argc, char **argv);

int cmd_procedure_stop(int argc, char **argv);
