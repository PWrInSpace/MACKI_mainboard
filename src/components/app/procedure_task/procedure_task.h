// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "cmd_procedure.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "procedure_parser.h"

procedure_task_context_t* get_procedure_task_context();

void procedure_task(void* pvParameters);
