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
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data) {
  if (ring_buffer_is_full(buffer) == RING_BUFFER_FULL) {
    return RING_BUFFER_FULL;
  }
  buffer->count++;
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % buffer->size;
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  buffer->count--;
  *data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % buffer->size;
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  *data = buffer->data[buffer->tail];
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_empty(ring_buffer_t* buffer) {
  if (buffer->count == 0) {
    return RING_BUFFER_EMPTY;
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_full(ring_buffer_t* buffer) {
  if (buffer->count == buffer->size) {
    return RING_BUFFER_FULL;
  }
  return RING_BUFFER_OK;
}
