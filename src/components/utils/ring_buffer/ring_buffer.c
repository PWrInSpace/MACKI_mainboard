// Copyright 2024 MACKI

#include "ring_buffer.h"

ring_buffer_status_t ring_buffer_init(ring_buffer_t* buffer, size_t size){
    if(size > CONFIG_MAX_RING_BUFFER_SIZE){
        return RING_BUFFER_ERROR;
    }
    buffer->size = size;
    buffer->head = 0;
    buffer->tail = 0;
    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_push(ring_buffer_t* buffer, void* data){
    if(buffer->head == buffer->tail){
        return RING_BUFFER_FULL;
    }
    buffer->data[buffer->head] = data;
    buffer->head = (buffer->head + 1) % buffer->size;
    return RING_BUFFER_OK;
}

ring_buffer_status_t ring_buffer_pop(ring_buffer_t* buffer, void** data){
    if(buffer->head == buffer->tail){
        return RING_BUFFER_EMPTY;
    }
    *data = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % buffer->size;
    return RING_BUFFER_OK;
}