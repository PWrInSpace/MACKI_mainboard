// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "procedure_typedef.h"

procedure_status_t parse_procedure(
    char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS], uint16_t num_events,
    procedure_t* procedure);

bool verify_time_range(int32_t time);

procedure_status_t read_and_verify_valve_event(char* instruction,
                                               procedure_event_t* event);

procedure_status_t read_and_verify_motor_event(char* instruction,
                                               procedure_event_t* event);

procedure_status_t sort_procedures_by_time(procedure_t* procedure);

procedure_status_t remove_erased_events(procedure_t* procedure);

void handle_combined_events(procedure_event_t* valve_event,
                            procedure_event_t* motor_event);
