// Copyright 2024 MACKI

#include "ring_buffer.h"

#include <memory.h>

static size_t get_index_to_address(ring_buffer_t* buffer, size_t index) {
  return index * buffer->single_data_size;
}

static size_t get_head_address(ring_buffer_t* buffer) {
  return get_index_to_address(buffer, buffer->head);
}

static size_t get_tail_address(ring_buffer_t* buffer) {
  return get_index_to_address(buffer, buffer->tail);
}

ring_buffer_status_t ring_buffer_init(ring_buffer_t* buffer, size_t size,
                                      size_t single_data_size,
                                      bool is_rollovelable) {
  if (size > CONFIG_MAX_RING_BUFFER_SIZE) {
    return RING_BUFFER_ERROR;
  }
  buffer->size = size;
  buffer->single_data_size = single_data_size;
  buffer->head = 0;
  buffer->tail = 0;
  buffer->count = 0;
  buffer->data = malloc(size * single_data_size);
  if (buffer->data == NULL) {
    return RING_BUFFER_ERROR;
  }
  buffer->is_rollovelable = is_rollovelable;
  buffer->mutex = xSemaphoreCreateBinary();
  if (buffer->mutex == NULL) {
    return RING_BUFFER_ERROR;
  }
  xSemaphoreGive(buffer->mutex);
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data) {
  bool is_full = ring_buffer_is_full(buffer) == RING_BUFFER_FULL;
  if ((is_full) && (!buffer->is_rollovelable)) {
    return RING_BUFFER_FULL;
  }

  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    if (!is_full) {
      // if the buffer is full, we don't want to increment the total count, as
      // tail is being overwritten
      buffer->count++;
    } else {
      // Need to update the tail
      buffer->tail = (buffer->tail + 1) % buffer->size;
    }
    memcpy(buffer->data + get_head_address(buffer), data,
           buffer->single_data_size);
    buffer->head = (buffer->head + 1) % buffer->size;
    xSemaphoreGive(buffer->mutex);
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    buffer->count--;
    memcpy(data, buffer->data + get_tail_address(buffer),
           buffer->single_data_size);
    buffer->tail = (buffer->tail + 1) % buffer->size;
    xSemaphoreGive(buffer->mutex);
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    memcpy(data, buffer->data + get_tail_address(buffer),
           buffer->single_data_size);
    xSemaphoreGive(buffer->mutex);
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_peek_last(ring_buffer_t* buffer, void** data) {
  if (ring_buffer_is_empty(buffer) == RING_BUFFER_EMPTY) {
    return RING_BUFFER_EMPTY;
  }
  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    size_t last = (buffer->head == 0) ? (buffer->size - 1) : (buffer->head - 1);
    memcpy(data, buffer->data + get_index_to_address(buffer, last),
           buffer->single_data_size);
    xSemaphoreGive(buffer->mutex);
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_empty(ring_buffer_t* buffer) {
  bool ret = true;
  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    ret = buffer->count == 0;
    xSemaphoreGive(buffer->mutex);
  }
  if (ret) {
    return RING_BUFFER_EMPTY;
  }
  return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_is_full(ring_buffer_t* buffer) {
  bool ret = true;
  if (xSemaphoreTake(buffer->mutex, portMAX_DELAY) == pdTRUE) {
    ret = (buffer->count == buffer->size);
    xSemaphoreGive(buffer->mutex);
  }
  if (ret) {
    return RING_BUFFER_FULL;
  }
  return RING_BUFFER_OK;
}
