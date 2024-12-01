#include "circular_buffer.h"
#include <stdbool.h>
#include <string.h>

CircularBuffer_t *circular_buffer_init(size_t max_size) {
    CircularBuffer_t *buffer = (CircularBuffer_t *)malloc(sizeof(CircularBuffer_t));
    if (!buffer) return NULL;
    buffer->buffer = (uint8_t *)malloc(max_size * sizeof(uint8_t));
    if (!buffer->buffer) {
        free(buffer);
        return NULL;
    }
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = 0;
    buffer->max_size = max_size;
    return buffer;
}

void circular_buffer_free(CircularBuffer_t *buffer) {
    if (buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

size_t circular_buffer_used(CircularBuffer_t *buffer) {
    return buffer->size;
}

bool circular_buffer_empty(CircularBuffer_t *buffer) {
    return (buffer->size == 0);
}

bool circular_buffer_full(CircularBuffer_t *buffer) {
    return (buffer->size == buffer->max_size);
}

void circular_buffer_push(CircularBuffer_t *buffer, uint8_t data) {
    if (circular_buffer_full(buffer)) {
        // Optionally handle overflow
        buffer->overflow = true;
        return;
    }
    buffer->buffer[buffer->head] = data;
    buffer->head = (buffer->head + 1) % buffer->max_size;
    buffer->size++;
}

uint8_t circular_buffer_pop(CircularBuffer_t *buffer) {
    if (circular_buffer_empty(buffer)) {
        // Optionally handle underflow
        return 0;
    }
    uint8_t data = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % buffer->max_size;
    buffer->size--;
    return data;
}