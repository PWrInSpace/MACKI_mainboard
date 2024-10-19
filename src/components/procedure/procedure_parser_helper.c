// Copyright 2024 MACKI, Krzysztof Gliwinski

#include <string.h>

#include "macki_log.h"
#include "procedure_parser_helper.h"

#define TAG "PROCEDURE_PARSER_HELPER"

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
  // Set event type to open
  procedure->events[num_events - CONFIG_VOPEN_EVT_POS_FROM_END].event_type =
      PROCEDURE_VALVE_ACTION_OPEN;
  if (ret_val != PROCEDURE_OK) {
    MACKI_LOG_ERROR(TAG, "Error parsing valve open event: %s",
                    procedure_str[num_events - CONFIG_VOPEN_EVT_POS_FROM_END]);
    return ret_val;
  }

  // Close event
  ret_val = read_and_verify_valve_event(
      procedure_str[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END],
      &procedure->events[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END]);
  // Set event type to close
  procedure->events[num_events - CONFIG_VCLOSE_EVT_POS_FROM_END].event_type =
      PROCEDURE_VALVE_ACTION_CLOSE;
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
  return PROCEDURE_OK;
}

void handle_combined_events(procedure_event_t* valve_event,
                                   procedure_event_t* motor_event) {
  valve_event->extra_data = motor_event->extra_data;
  valve_event->event_type =
      valve_event->event_type | PROCEDURE_MOTOR_ACTION_SET_SPEED;
  motor_event->event_type = PROCEDURE_ACTION_ERASED;
}

procedure_status_t sort_procedures_by_time(procedure_t* procedure) {
  if (procedure == NULL) {
    MACKI_LOG_ERROR(TAG, "Procedure is NULL");
    return PROCEDURE_ERROR;
  }

  for (uint16_t i = 0; i < procedure->num_events; i++) {
    for (uint16_t j = i + 1; j < procedure->num_events; j++) {
      if (procedure->events[i].time_ms == procedure->events[j].time_ms) {
        // handling events that happend at the same time
        // let's first check if it is a combined valve and motor event
        if ((procedure->events[i].event_type <= PROCEDURE_VALVE_ACTION_CLOSE) &&
            (procedure->events[j].event_type ==
             PROCEDURE_MOTOR_ACTION_SET_SPEED)) {
          // we have a combined event
          handle_combined_events(&procedure->events[i], &procedure->events[j]);
        } else if ((procedure->events[j].event_type <=
                        PROCEDURE_VALVE_ACTION_CLOSE &&
                    procedure->events[i].event_type ==
                        PROCEDURE_MOTOR_ACTION_SET_SPEED)) {
          // we have a combined event
          handle_combined_events(&procedure->events[j], &procedure->events[i]);
        } else if (procedure->events[i].event_type == PROCEDURE_ACTION_ERASED ||
                   procedure->events[j].event_type == PROCEDURE_ACTION_ERASED) {
          MACKI_LOG_TRACE(TAG, "Event already erased");
        } else {
          MACKI_LOG_ERROR(TAG, "Concurrent motor events at the same time!");
          return PROCEDURE_MOTOR_ACTIONS_CONCURRENT_ERROR;
        }
      }

      if (procedure->events[i].time_ms > procedure->events[j].time_ms) {
        procedure_event_t temp = procedure->events[i];
        procedure->events[i] = procedure->events[j];
        procedure->events[j] = temp;
      }
    }
  }
  return PROCEDURE_OK;
}

procedure_status_t remove_erased_events(procedure_t* procedure) {
  if (procedure == NULL) {
    MACKI_LOG_ERROR(TAG, "Procedure is NULL");
    return PROCEDURE_ERROR;
  }

  uint16_t num_events = 0;
  for (uint16_t i = 0; i < procedure->num_events; i++) {
    if (procedure->events[i].event_type != PROCEDURE_ACTION_ERASED) {
      procedure->events[num_events] = procedure->events[i];
      num_events++;
    }
  }
  procedure->num_events = num_events;
  return PROCEDURE_OK;
}
