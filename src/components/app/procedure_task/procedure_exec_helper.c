// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_exec_helper.h"

#include <string.h>

#include "macki_log.h"
#include "mechanical_controller.h"

#define TAG "PROCEDURE_EXEC_HELPER"

procedure_exec_status_t load_procedure(procedure_exec_t* procedure_exec,
                                       procedure_t* procedure) {
  if (procedure == NULL || procedure_exec == NULL) {
    MACKI_LOG_ERROR(TAG, "Invalid arguments");
    return PROCEDURE_LOADING_ERROR;
  }

  for (size_t i = 0; i < procedure->num_events - 1; i++) {
    procedure_exec->time_differences[i] =
        procedure->events[i + 1].time_ms - procedure->events[i].time_ms;
  }
  procedure_exec->time_differences[procedure->num_events - 1] = 0;

  memcpy(&procedure_exec->procedure, procedure, sizeof(procedure_t));
  procedure_exec->current_step = 0;

  return PROCEDURE_EXECUTION_OK;
}

procedure_exec_status_t execute_next_procedure_step(
    procedure_exec_t* procedure_exec, uint32_t* duration_ms) {
  if (procedure_exec == NULL) {
    MACKI_LOG_ERROR(TAG, "Invalid arguments");
    return PROCEDURE_EXECUTION_ERROR;
  }

  mechanical_controller_status_t status = MECHANICAL_CONTROLLER_OK;

  if (procedure_exec->current_step >= procedure_exec->procedure.num_events) {
    MACKI_LOG_INFO(TAG, "Procedure finished, setting motors in starting point");
    status = set_all_motors_in_starting_point();
    MACKI_LOG_INFO(TAG, "Motors set in starting point");
    if (status != MECHANICAL_CONTROLLER_OK) {
      MACKI_LOG_ERROR(TAG, "Error while setting motors in starting point");
      return PROCEDURE_EXECUTION_ERROR;
    }
    procedure_exec->current_step = 0;
  }

  procedure_event_t* event =
      &procedure_exec->procedure.events[procedure_exec->current_step];

  switch (event->event_type) {
    case PROCEDURE_VALVE_ACTION_OPEN:
      MACKI_LOG_INFO(TAG, "Opening valve");
      status |= solenoid_open(VALVE_INSTANCE_0);
      break;
    case PROCEDURE_VALVE_ACTION_CLOSE:
      MACKI_LOG_INFO(TAG, "Closing valve");
      status |= solenoid_close(VALVE_INSTANCE_0);
      break;
    case PROCEDURE_MOTOR_ACTION_SET_SPEED:
      MACKI_LOG_INFO(TAG, "Setting motor speed to %d", event->extra_data);
      status |= motor_set_speed_all_motors(event->extra_data);
      break;
    case PROCEDURE_MOTOR_AND_VALVE_OPEN_ACTION:
      MACKI_LOG_INFO(TAG, "Setting motor speed to %d and opening valve", event->extra_data);
      status |= motor_set_speed_all_motors(event->extra_data);
      status |= solenoid_open(VALVE_INSTANCE_0);
      break;
    case PROCEDURE_MOTOR_AND_VALVE_CLOSE_ACTION:
      MACKI_LOG_INFO(TAG, "Setting motor speed to %d and closing valve", event->extra_data);
      status |= motor_set_speed_all_motors(event->extra_data);
      status |= solenoid_close(VALVE_INSTANCE_0);
      break;
    case PROCEDURE_ACTION_ERASED:
      MACKI_LOG_ERROR(TAG, "Erased action");
      return PROCEDURE_EXECUTION_ERROR;
      break;
    default:
      MACKI_LOG_ERROR(TAG, "Unknown action");
      return PROCEDURE_EXECUTION_ERROR;
  }
  
  *duration_ms = procedure_exec->time_differences[procedure_exec->current_step];

  procedure_exec->current_step++;
  return status == MECHANICAL_CONTROLLER_OK ? PROCEDURE_EXECUTION_OK
                                            : PROCEDURE_EXECUTION_ERROR;
}
