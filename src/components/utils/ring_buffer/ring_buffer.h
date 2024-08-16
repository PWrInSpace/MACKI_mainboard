// Copyright 2024 MACKI

#pragma once

#include <stddef.h>

#include "sdkconfig.h"

typedef enum {
  RING_BUFFER_OK = 0,
  RING_BUFFER_FULL,
  RING_BUFFER_EMPTY,
  RING_BUFFER_ERROR
} ring_buffer_status_t;

typedef struct {
  size_t size;
  size_t head;
  size_t tail;
  size_t count;
  void* data[CONFIG_MAX_RING_BUFFER_SIZE];
} ring_buffer_t;

ring_buffer_status_t ring_buffer_init(ring_buffer_t* buffer, size_t size);

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data);

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data);

ring_buffer_status_t ring_buffer_is_empty(ring_buffer_t* buffer);
