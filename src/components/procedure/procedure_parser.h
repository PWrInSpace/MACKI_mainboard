// Copyright 2024 MACKI, Krzysztof Gliwinski

#include <stdbool.h>
#include <stdint.h>

#include "procedure_typedef.h"

procedure_status_t parse_and_verify_procedure(
    char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS], uint16_t num_events,
    procedure_t* procedure);
