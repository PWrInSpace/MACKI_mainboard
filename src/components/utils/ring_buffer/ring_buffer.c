// Copyright 2024 MACKI

#include "ring_buffer.h"

ring_buffer_status_t ring_buffer_init(ring_buffer_t* buffer, size_t size) {
  if (size > CONFIG_MAX_RING_BUFFER_SIZE) {
    return RING_BUFFER_ERROR;
  }
  buffer->size = size;
  buffer->head = 0;
  buffer->tail = 0;
  buffer->count = 0;
  buffer->mutex = xSemaphoreCreateBinary();
  if (buffer->mutex == NULL) {
    return RING_BUFFER_ERROR;
  }
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data) {
  if (ring_buffer_is_full(buffer) == RING_BUFFER_FULL) {
    return RING_BUFFER_FULL;
  }
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  buffer->count++;
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % buffer->size;
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  buffer->count--;
  *data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % buffer->size;
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  *data = buffer->data[buffer->tail];
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek_last(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  *data = buffer->data[buffer->head - 1];
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_empty(ring_buffer_t* buffer) {
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  bool ret = buffer->count == 0;
  xSemaphoreGive(buffer->mutex);
  if (ret) {
    return RING_BUFFER_EMPTY;
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_full(ring_buffer_t* buffer) {
  xSemaphoreTake(buffer->mutex, portMAX_DELAY);
  bool ret = buffer->count == buffer->size;
  xSemaphoreGive(buffer->mutex);
  if (ret) {
    return RING_BUFFER_FULL;
  }
  return RING_BUFFER_OK;
}
