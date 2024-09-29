#include "data_queues.h"

static QueueHandle_t sensor_data_queue = NULL;

QueueHandle_t* get_sensor_data_queue() {
  if (sensor_data_queue == NULL) {
    sensor_data_queue = xQueueCreate(10, sizeof(sensor_out_data_t));
  }
  return &sensor_data_queue;
}
