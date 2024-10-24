// Copyright 2024 PWrInSpace, Kuba
#include "cmd_procedure.h"

#include "cmd_defines.h"
#include "macki_log.h"
#include "procedure_parser.h"

#define MINIMUM_ARGS_NUM_PROCEDURE 4
#define TAG "CMD_PROCEDURE"

static struct {
  bool initialized;
  procedure_task_context_t *pd;
  _get_procedure_task_context_cb procedure_task_cb;
} procedure_context = {
    .initialized = false,
    .pd = NULL,
    .procedure_task_cb = NULL,
};

bool register_get_procedure_task_context_cb(_get_procedure_task_context_cb cb) {
  procedure_context.procedure_task_cb = cb;
  return true;
}

void init_cmd_procedure(void) {
  if (procedure_context.procedure_task_cb != NULL) {
    procedure_context.procedure_task_cb();
    procedure_context.initialized = true;
  }
}

int cmd_procedure(int argc, char **argv) {
  if (argc < MINIMUM_ARGS_NUM_PROCEDURE) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }
  if (procedure_context.initialized == false) {
    CLI_WRITE_ERR("Procedure task not initialized");
    return 1;
  }

  procedure_t procedure;
  procedure_status_t status =
      parse_and_verify_procedure(argv + 1, argc - 1, &procedure);
  if (status != PROCEDURE_OK) {
    CLI_WRITE_ERR("Failed to parse and verify procedure, reason: %s",
                  procedure_status_to_string(status));
    return 1;
  }

  if (xQueueSend(procedure_context.pd->queue_handle, &procedure, 0) != pdPASS) {
    CLI_WRITE_ERR("Failed to send procedure to the queue");
    return 1;
  }

  CLI_WRITE_OK("Procedure sent to the queue");

  return 0;
}

int cmd_procedure_check(int argc, char **argv) {
  if (argc < MINIMUM_ARGS_NUM_PROCEDURE) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }
  if (procedure_context.initialized == false) {
    CLI_WRITE_ERR("Procedure task not initialized");
    return 1;
  }

  char* argv_copy[CONFIG_PROCEDURE_MAX_EVENTS];
  for (int i = 1; i < argc; i++) {
    argv_copy[i] = argv[i];
    printf("argv_copy[%d]: %s\n", i, argv_copy[i]);
  }

  procedure_t procedure = {0};
  procedure_status_t status =
      parse_and_verify_procedure(argv_copy, argc - 1, &procedure);
  if (status != PROCEDURE_OK) {
    CLI_WRITE_ERR("Failed to parse and verify procedure, reason: %s",
                  procedure_status_to_string(status));
    return 1;
  }

  CLI_WRITE_OK("Procedure parsed and verified successfully");

  return 0;
}

int cmd_procedure_stop(int argc, char **argv) {
  if (argc != 1) {
    CLI_WRITE_ERR("Invalid number of arguments");
    return 1;
  }
  if (procedure_context.initialized == false) {
    CLI_WRITE_ERR("Procedure task not initialized");
    return 1;
  }
  MACKI_LOG_INFO(TAG, "Sending stop notification to procedure task");
  xTaskNotify(procedure_context.pd->task_handle, STOP_NOTIFICATION, eSetBits);

  return 0;
}
