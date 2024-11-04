// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_task.h"

#include "macki_log.h"
#include "mechanical_controller.h"
#include "procedure_exec_helper.h"
#include "procedure_parser.h"
#include "procedure_typedef.h"
#include "rtc_wrapper.h"
#include "sensor_controller.h"

#define TAG "PROCEDURE_TASK"

static procedure_task_context_t pd;

procedure_task_context_t* get_procedure_task_context(void) { return &pd; }

void procedure_task(void* pvParameters) {
  // Initialize the queue
  pd.queue_handle = xQueueCreate(5, sizeof(procedure_t));
  pd.task_handle = xTaskGetCurrentTaskHandle();

  register_get_procedure_task_context_cb(get_procedure_task_context);

  // We must call it after queue and task handle initialization
  init_cmd_procedure();

  uint32_t receivedNotification;

  MACKI_LOG_INFO(TAG, "Procedure task started");
  procedure_t procedure;
  procedure_exec_t procedure_exec;
  uint32_t duration_ms;
  int64_t start_procedure_time;
  while (1) {
    if (xQueueReceive(pd.queue_handle, &procedure, portMAX_DELAY)) {
      MACKI_LOG_INFO(TAG, "Procedure task received notification");

      if (load_procedure(&procedure_exec, &procedure) ==
          PROCEDURE_EXECUTION_OK) {
        MACKI_LOG_INFO(TAG, "Procedure loaded successfully");
      } else {
        MACKI_LOG_ERROR(TAG, "Error while loading procedure");
        break;
      }
      // First we need to set motors in starting point
      MACKI_LOG_INFO(
          TAG, "Setting motors in starting point before starting procedure");
      if (set_all_motors_in_starting_point() != MECHANICAL_CONTROLLER_OK) {
        MACKI_LOG_ERROR(TAG, "Error while setting motors in starting point");
      }
      MACKI_LOG_INFO(TAG, "Motors set in starting point, starting procedure");
      start_procedure_time = rtc_wrapper_get_time_ms();
      tare_load_cell();
      update_procedure_start_time(start_procedure_time);
      while (1) {
        procedure_exec_status_t status =
            execute_next_procedure_step(&procedure_exec, &duration_ms);
        if (status == PROCEDURE_EXECUTION_ERROR) {
          MACKI_LOG_ERROR(TAG, "Error while executing procedure step");
        }
        if(status == PROCEDURE_EXECUTION_OK_ENTERING_NEW_LOOP){
          // We need to reset the start time of the procedure
          start_procedure_time = rtc_wrapper_get_time_ms();
          update_procedure_start_time(start_procedure_time);
        }
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        if (xTaskNotifyWait(STOP_NOTIFICATION, STOP_NOTIFICATION,
                            &receivedNotification, 0) == pdPASS) {
          if (receivedNotification & STOP_NOTIFICATION) {
            MACKI_LOG_INFO(TAG,
                           "Stop notification received, stopping procedure");

            // Stop the procedure and the motors
            motor_set_speed_all_motors(0);
            solenoid_close(VALVE_INSTANCE_0);
            // Break out of the procedure execution loop
            break;
          }
        }
      }
    }
  }
}
