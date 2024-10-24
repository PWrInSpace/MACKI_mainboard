// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_parser.h"

#include "macki_log.h"
#include "procedure_parser_helper.h"

#define TAG "PROCEDURE_PARSER"

procedure_status_t parse_and_verify_procedure(
    char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS], uint16_t num_events,
    procedure_t* procedure) {
  procedure_status_t status =
      parse_procedure(procedure_str, num_events, procedure);
  if (status != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error parsing procedure");
    return status;
  }

  status = sort_procedures_by_time(procedure);
  if (status != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error sorting procedure");
    return status;
  }

  status = remove_erased_events(procedure);
  if (status != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error removing erased events");
    return status;
  }
  
  return PROCEDURE_OK;
}

const char* procedure_status_to_string(procedure_status_t status){
  switch (status) {
    case PROCEDURE_OK:
      return "PROCEDURE_OK";
    case PROCEDURE_INVALID_TIME:
      return "PROCEDURE_INVALID_TIME";
    case PROCEDURE_MOTOR_ACTIONS_CONCURRENT_ERROR:
      return "PROCEDURE_MOTOR_ACTIONS_CONCURRENT_ERROR";
    case PROCEDURE_LAST_MOTOR_ACTION_NOT_STOP:
      return "PROCEDURE_LAST_MOTOR_ACTION_NOT_STOP";
    case PROCEDURE_ERROR:
      return "PROCEDURE_ERROR";
    default:
      return "UNKNOWN PROCEDURE STATUS";
  }
}