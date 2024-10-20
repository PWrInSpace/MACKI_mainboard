// Copyright 2024 MACKI, Krzysztof Gliwinski

#include "procedure_task.h"
#include "macki_log.h"

#define TAG "PROCEDURE_TASK"

// TODO(Glibus): consider using event loop instead of task notification
void procedure_task(void* pvParameters) {
  // TODO(Glibus): this task will take notify from procedure command if the 
  // procedure parsing is successful. It will then execute the procedure until another
  // notification is given to stop the procedure.
    MACKI_LOG_INFO(TAG, "Procedure task started");
    while (1) {
        // Wait for the notification from the procedure parser
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        MACKI_LOG_INFO(TAG, "Procedure task received notification");
        // Execute the procedure
        // TODO(Glibus): implement the procedure execution
    }
}