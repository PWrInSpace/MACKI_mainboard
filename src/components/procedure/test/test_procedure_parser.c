// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_parser.h"
#include "sdkconfig.h"
#include "unity.h"

char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS] = {
    "1000;1000", "1000;2000", "1000;3000", "1000;4000",
    "1000;5000", "500",       "5500"};

procedure_t expected_procedure = {
    .events = {{PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 1000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 2000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 3000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 4000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 5000},
               {PROCEDURE_VALVE_ACTION_OPEN, 0, 500},
               {PROCEDURE_VALVE_ACTION_CLOSE, 0, 5500}},
    .num_events = 7};

void test_parse_procedure() {
  procedure_t procedure;
  procedure_status_t status = parse_procedure(procedure_str, 7, &procedure);
  TEST_ASSERT_EQUAL(PROCEDURE_OK, status);
  TEST_ASSERT_EQUAL_MEMORY(&expected_procedure, &procedure,
                           sizeof(procedure_t));
}
