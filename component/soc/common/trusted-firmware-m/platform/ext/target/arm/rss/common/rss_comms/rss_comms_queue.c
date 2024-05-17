/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_queue.h"

#include <stdbool.h>
#include <stddef.h>

#define QUEUE_SIZE (RSS_COMMS_MAX_CONCURRENT_REQ + 1)

struct queue_t {
    void *buf[QUEUE_SIZE];
    size_t head;
    size_t tail;
};

static struct queue_t queue;

/* Advance head or tail */
static size_t advance(size_t index)
{
    if (++index == QUEUE_SIZE) {
        index = 0;
    }
    return index;
}

static inline bool is_empty(void)
{
    return queue.head == queue.tail;
}

static inline bool is_full(void)
{
    return advance(queue.head) == queue.tail;
}

int32_t queue_enqueue(void *entry)
{
    if (is_full()) {
        return -1;
    }

    queue.buf[queue.head] = entry;
    queue.head = advance(queue.head);

    return 0;
}

int32_t queue_dequeue(void **entry)
{
    if (is_empty()) {
        return -1;
    }

    *entry = queue.buf[queue.tail];
    queue.tail = advance(queue.tail);

    return 0;
}
