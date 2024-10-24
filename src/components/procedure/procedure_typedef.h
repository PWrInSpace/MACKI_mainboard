// Copyright 2024 MACKI, Krzysztof Gliwinski

#pragma once
#include "sdkconfig.h"  // for CONFIG_PROCEDURE_MAX_EVENTS

// TODO(Glibus): move this to sdkconfig.h
#define CONFIG_PROCEDURE_MAX_EVENTS 128
#define CONFIG_PROCEDURE_DELIMITER ";"
#define CONFIG_PROCEDURE_MIN_EVENTS 3

#define CONFIG_PROCEDURE_MIN_TIME_MS 0
#define CONFIG_PROCEDURE_MAX_TIME_MS 100000

#define CONFIG_LAST_MOTOR_EVENT_FROM_END 3
#define CONFIG_VOPEN_EVT_POS_FROM_END 2
#define CONFIG_VCLOSE_EVT_POS_FROM_END 1

typedef enum {
  PROCEDURE_OK = 0,
  PROCEDURE_INVALID_TIME,
  PROCEDURE_MOTOR_ACTIONS_CONCURRENT_ERROR,
  PROCEDURE_LAST_MOTOR_ACTION_NOT_STOP,
  PROCEDURE_ERROR
} procedure_status_t;

typedef enum {
  PROCEDURE_VALVE_ACTION_OPEN = 0b001,
  PROCEDURE_VALVE_ACTION_CLOSE = 0b010,
  PROCEDURE_MOTOR_ACTION_SET_SPEED = 0b100,
  PROCEDURE_MOTOR_AND_VALVE_OPEN_ACTION =
      PROCEDURE_VALVE_ACTION_OPEN | PROCEDURE_MOTOR_ACTION_SET_SPEED,
  PROCEDURE_MOTOR_AND_VALVE_CLOSE_ACTION =
      PROCEDURE_VALVE_ACTION_CLOSE | PROCEDURE_MOTOR_ACTION_SET_SPEED,
  PROCEDURE_ACTION_ERASED = 0b000,
} procedure_event_type_t;


/*!
 * @brief Structure representing a single event in the procedure
 * @param event_type Type of the event
 * @param extra_data Extra data for the event - i.e speed for motor
 * @param time_ms Time in milliseconds when the event should be executed
 */
typedef struct {
  procedure_event_type_t event_type;
  int32_t extra_data;  // only used for motor speed
  uint32_t time_ms;
} procedure_event_t;

typedef struct {
  procedure_event_t events[CONFIG_PROCEDURE_MAX_EVENTS];
  uint16_t num_events;
} procedure_t;