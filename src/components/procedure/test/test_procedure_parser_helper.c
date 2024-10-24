// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_parser_helper.h"
#include "sdkconfig.h"
#include "unity.h"

char* std_procedure_str[CONFIG_PROCEDURE_MAX_EVENTS] = {
    "1000;1000;", "2000;1000;", "3000;1000;", "4000;1000;",
    "5000;0;",    "500",       "5500"};

procedure_t expected_std_procedure = {
  .events = {{PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 1000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 2000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 3000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 4000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 0, 5000},
               {PROCEDURE_VALVE_ACTION_OPEN, 0, 500},
               {PROCEDURE_VALVE_ACTION_CLOSE, 0, 5500}},
    .num_events = 7};

procedure_t expected_sorted_std_procedure = {
    .events = {{PROCEDURE_VALVE_ACTION_OPEN, 0, 500},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 1000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 2000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 3000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 1000, 4000},
               {PROCEDURE_MOTOR_ACTION_SET_SPEED, 0, 5000},
               {PROCEDURE_VALVE_ACTION_CLOSE, 0, 5500}},
    .num_events = 7};

void test_assert_equal_procedures(procedure_t* expected, procedure_t* actual) {
  TEST_ASSERT_EQUAL(expected->num_events, actual->num_events);
  for (uint16_t i = 0; i < expected->num_events; i++) {
    TEST_ASSERT_EQUAL(expected->events[i].event_type,
                      actual->events[i].event_type);
    TEST_ASSERT_EQUAL(expected->events[i].extra_data,
                      actual->events[i].extra_data);
    TEST_ASSERT_EQUAL(expected->events[i].time_ms, actual->events[i].time_ms);
  }
}

void test_parse_procedure(char* procedure_str[CONFIG_PROCEDURE_MAX_EVENTS],
                          procedure_t* expected_procedure,
                          procedure_t* expected_sorted_procedure) {
  procedure_t procedure;
  procedure_status_t status = parse_procedure(procedure_str, 7, &procedure);
  TEST_ASSERT_EQUAL(PROCEDURE_OK, status);
  test_assert_equal_procedures(expected_procedure, &procedure);

  status = sort_procedures_by_time(&procedure);
  TEST_ASSERT_EQUAL(PROCEDURE_OK, status);
  test_assert_equal_procedures(expected_sorted_procedure, &procedure);
}

TEST_CASE("Procedure_parser test 1: standard procedure", "[procedure_parser]") {
  test_parse_procedure(std_procedure_str, &expected_std_procedure,
                       &expected_sorted_std_procedure);
}
