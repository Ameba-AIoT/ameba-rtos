/*
 * Copyright (c) 2025 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMEBA_RINGBUFFER_H
#define AMEBA_RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE:
 * 1. The RingBuffer is only for SPSC model
 * 2. Since the ring_buffer_header structure may be shared
 * across CPU cores. It need be aligned to the cache line size.
 * We select the max cache line size among all cores to ensure
 * correct behavior.
 * Some typical CPU cache line sizes:
 *   KM4/KR4: 32 byte
 *   DSP: 128 byte
 *   CA32: 64 byte
 */
#define MAX_CACHE_LINE_SIZE 128
#define CACHE_ALIGNED __attribute__((aligned(MAX_CACHE_LINE_SIZE)))

typedef enum ring_buffer_type {
    RINGBUFFER_IPC,
    RINGBUFFER_LOCAL,
} ring_buffer_type;

typedef struct ring_buffer_header {
    void *buffer;
    uint32_t capacity;
    uint32_t mask;
    uint32_t buffer_id;
    uint32_t type;
    uint32_t rsvd0[(MAX_CACHE_LINE_SIZE - 5 * 4) / 4];

    volatile uint32_t head;
    uint32_t rsvd1[(MAX_CACHE_LINE_SIZE - 4) / 4];

    volatile uint32_t tail;
    uint32_t rsvd2[(MAX_CACHE_LINE_SIZE - 4) / 4];
} CACHE_ALIGNED ring_buffer_header;

typedef struct ring_buffer {
    bool owns_data;
    struct ring_buffer_header* header;

    uint32_t (*capacity)(const struct ring_buffer *rb);
    uint32_t (*space)(const struct ring_buffer *rb);
    uint32_t (*available)(const struct ring_buffer *rb);

    uint32_t (*write)(struct ring_buffer *rb, const void *data, uint32_t count);
    uint32_t (*read)(struct ring_buffer *rb, void *data, uint32_t count);

    void     (*reset)(struct ring_buffer *rb);
} ring_buffer;

struct ring_buffer *ring_buffer_create(uint32_t capacity, enum ring_buffer_type type);
struct ring_buffer *ring_buffer_create_by_header(ring_buffer_header* header);
void ring_buffer_destroy(struct ring_buffer *rb);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_RINGBUFFER_H
