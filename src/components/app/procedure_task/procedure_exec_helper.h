// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include "procedure_parser.h"

typedef enum {
  PROCEDURE_EXECUTION_OK = 0,
  PROCEDURE_LOADING_ERROR,
  PROCEDURE_EXECUTION_ERROR,
} procedure_exec_status_t;

/*!
 * @brief Structure representing a single event in the procedure
 * @param event_type Type of the event
 * @param extra_data Extra data for the event - i.e speed for motor
 * @param time_ms Time in milliseconds to wait before the next event is executed
 */
typedef struct {
  procedure_t procedure;
  uint16_t current_step;
  uint32_t time_differences[CONFIG_PROCEDURE_MAX_EVENTS];
} procedure_exec_t;

procedure_exec_status_t load_procedure(procedure_exec_t* procedure_exec,
                                       procedure_t* procedure);

procedure_exec_status_t execute_next_procedure_step(
    procedure_exec_t* procedure_exec, uint32_t* duration_ms);
