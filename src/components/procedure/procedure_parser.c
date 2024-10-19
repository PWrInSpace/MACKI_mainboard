// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_parser.h"

#include <string.h>

#include "macki_log.h"

#define TAG "PROCEDURE_PARSER"

procedure_status_t parse_procedure(
    char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS], uint16_t num_events,
    procedure_t* procedure) {
  if (num_events > CONFIG_PROCEDURE_MAX_EVENTS) {
    MACKI_LOG_ERROR(TAG, "Too many events in the procedure");
    return PROCEDURE_ERROR;
  }
  if (num_events < CONFIG_PROCEDURE_MIN_EVENTS) {
    MACKI_LOG_ERROR(TAG, "Too few events in the procedure");
    return PROCEDURE_ERROR;
  }
  procedure_status_t ret_val = PROCEDURE_OK;
  // First we would have num_events-2 strings, which describe the
  // motor_set_speed events They are in the form of
  // 'speed(int)<delimiter>time(int)'
  for (uint16_t i = 0; i < num_events - CONFIG_VOPEN_EVT_POS_FROM_END; i++) {
    ret_val =
        read_and_verify_motor_event(procedure_str[i], &procedure->events[i]);
    if (ret_val != PROCEDURE_OK) {
      MACKI_LOG_ERROR(TAG, "Error parsing motor event: %s", procedure_str[i]);
      return ret_val;
    }
  }

  // The last two events are the valve open and close events
  // Which just give time first for open and then for close

  // Open event
  ret_val = read_and_verify_valve_event(
      procedure_str[num_events - CONFIG_VOPEN_EVT_POS_FROM_END],
      &procedure->events[num_events - CONFIG_VOPEN_EVT_POS_FROM_END]);
  if (ret_val != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error parsing valve open event: %s",
                    procedure_str[num_events - CONFIG_VOPEN_EVT_POS_FROM_END]);
    return ret_val;
  }

  // Close event
  ret_val = read_and_verify_valve_event(
      procedure_str[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END],
      &procedure->events[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END]);
  if (ret_val != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error parsing valve close event: %s",
                    procedure_str[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END]);
    return ret_val;
  }

  procedure->num_events = num_events;

  return PROCEDURE_OK;
}

bool verify_time_range(int32_t time) {
  return (time > CONFIG_PROCEDURE_MIN_TIME_MS) &&
         (time < CONFIG_PROCEDURE_MAX_TIME_MS);
}

procedure_status_t read_and_verify_valve_event(char* instruction,
                                               procedure_event_t* event) {
  if (instruction == NULL) {
    MACKI_LOG_ERROR(TAG, "Invalid event string: %s", instruction);
    return PROCEDURE_ERROR;
  }
  int32_t time = atol(instruction);
  if (!verify_time_range(time)) {
    MACKI_LOG_ERROR(TAG, "Invalid time range: %d", time);
    return PROCEDURE_INVALID_TIME;
  }
  event->time_ms = atoi(instruction);
  event->extra_data = 0;
  return PROCEDURE_OK;
}

procedure_status_t read_and_verify_motor_event(char* instruction,
                                               procedure_event_t* event) {
  char* token = strtok(instruction, CONFIG_PROCEDURE_DELIMITER);
  if (token == NULL) {
    MACKI_LOG_ERROR(TAG, "Invalid event string: %s", instruction);
    return PROCEDURE_ERROR;
  }
  event->event_type = PROCEDURE_MOTOR_ACTION_SET_SPEED;
  int32_t time = atol(token);
  if (!verify_time_range(time)) {
    MACKI_LOG_ERROR(TAG, "Invalid time range: %d", time);
    return PROCEDURE_ERROR;
  }
  event->time_ms = (uint32_t)time;

  token = strtok(NULL, CONFIG_PROCEDURE_DELIMITER);
  if (token == NULL) {
    MACKI_LOG_ERROR(TAG, "Invalid event string: %s", instruction);
    return PROCEDURE_ERROR;
  }
  event->extra_data = atoi(token);
}
