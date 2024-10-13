// Copyright 2024 MACKI

#pragma once

#include <stddef.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef enum {
  RING_BUFFER_OK = 0,
  RING_BUFFER_FULL,
  RING_BUFFER_EMPTY,
  RING_BUFFER_ERROR
} ring_buffer_status_t;

// TODO(Glibus): in ring_buffer_bugfix branch, refactor this to use statically
// allocated memory
typedef struct {
  size_t size;
  size_t head;
  size_t tail;
  size_t count;
  void* data[CONFIG_MAX_RING_BUFFER_SIZE];
  SemaphoreHandle_t mutex;
} ring_buffer_t;

ring_buffer_status_t ring_buffer_init(ring_buffer_t* buffer, size_t size);

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data);

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data);

ring_buffer_status_t ring_buffer_peek(ring_buffer_t* buffer, void** data);

ring_buffer_status_t ring_buffer_peek_last(ring_buffer_t* buffer, void** data);

ring_buffer_status_t ring_buffer_is_empty(ring_buffer_t* buffer);

ring_buffer_status_t ring_buffer_is_full(ring_buffer_t* buffer);
