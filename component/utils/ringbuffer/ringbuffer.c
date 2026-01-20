/*
 * Copyright (c) 2021 Realtek, LLC.
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

#define LOG_TAG "RingBuffer"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os_wrapper.h"
#include "ameba_soc.h"
#include "ringbuffer.h"

#define RB_BYTE_ALIGNMENT 128

RingBuffer *RingBuffer_Create(void *data, uint32_t size, int32_t type, int32_t owns)
{
	RingBuffer *rb = (RingBuffer *)rtos_mem_malloc(sizeof(RingBuffer));
	if (!rb) {
		RTK_LOGE(LOG_TAG, "alloc struct fail.\n");
		return NULL;
	}
	if (owns) {
		rb->start = rtos_mem_malloc(size);
		if (!rb->start) {
			RTK_LOGE(LOG_TAG, "alloc data fail.\n");
			rtos_mem_free(rb);
			return NULL;
		}
	} else {
		rb->start = (uint8_t *)data;
	}

	rb->size = size;
	rb->wptr = rb->start;
	rb->rptr = rb->start;
	rb->owns = owns;
	rb->type = type;
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Clean((uint32_t)rb, sizeof(RingBuffer));
	}
	return rb;
}

int32_t RingBuffer_Destroy(RingBuffer *rb)
{
	if (rb->owns) {
		if (rb->start) {
			rtos_mem_free(rb->start);
		}
	}
	rtos_mem_free(rb);

	return 0;
}

uint32_t RingBuffer_Size(RingBuffer *rb)
{
	return rb->size;
}

uint32_t RingBuffer_Space(RingBuffer *rb)
{
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)rb, sizeof(RingBuffer));
	}
	return ((rb->size - 1 + rb->rptr - rb->wptr) % (rb->size));
}

uint32_t RingBuffer_Available(RingBuffer *rb)
{
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)rb, sizeof(RingBuffer));
	}
	return ((rb->size + rb->wptr - rb->rptr) % (rb->size));
}

int32_t RingBuffer_Write(RingBuffer *rb, uint8_t *buffer, uint32_t count)
{
	if (!count || !buffer) {
		RTK_LOGW(LOG_TAG, "try to write from empty buffer.\n");
		return -1;
	}
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)rb, sizeof(RingBuffer));
	}
	if (rb->wptr < rb->rptr) {
		memcpy(rb->wptr, buffer, count);
		if (rb->type == SHARED_RINGBUFF) {
			DCache_Clean((uint32_t)rb->wptr, count);
		}
		rb->wptr += count;
		if (rb->wptr == rb->start + rb->size) {
			rb->wptr = rb->start;
		}
	} else {
		uint32_t size = (uint32_t)(rb->start + rb->size - rb->wptr);
		if (size >= count) {
			size = count;
		}
		memcpy(rb->wptr, buffer, size);
		if (rb->type == SHARED_RINGBUFF) {
			DCache_Clean((uint32_t)rb->wptr, size);
		}
		rb->wptr += size;
		if (rb->wptr == rb->start + rb->size) {
			rb->wptr = rb->start;
		}

		if (size != count) {
			memcpy(rb->wptr, buffer + size, count - size);
			if (rb->type == SHARED_RINGBUFF) {
				DCache_Clean((uint32_t)rb->wptr, count - size);
			}
			rb->wptr += count - size;
			if (rb->wptr == rb->start + rb->size) {
				rb->wptr = rb->start;
			}
		}
	}
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Clean((uint32_t)(&(rb->wptr)), RB_BYTE_ALIGNMENT);
	}
	return 0;
}

int32_t RingBuffer_Read(RingBuffer *rb, uint8_t *buffer, uint32_t count)
{
	if (!count || !buffer) {
		RTK_LOGW(LOG_TAG, "try to read to empty buffer.\n");
		return -1;
	}
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)rb, sizeof(RingBuffer));
	}
	if (rb->wptr > rb->rptr) {
		if (rb->type == SHARED_RINGBUFF) {
			DCache_Invalidate((uint32_t)rb->rptr, count);
		}
		memcpy(buffer, rb->rptr, count);
		rb->rptr += count;
		if (rb->rptr == rb->start + rb->size) {
			rb->rptr = rb->start;
		}
	} else {
		uint32_t size = (uint32_t)(rb->start + rb->size - rb->rptr);
		if (size >= count) {
			size = count;
		}
		if (rb->type == SHARED_RINGBUFF) {
			DCache_Invalidate((uint32_t)rb->rptr, size);
		}
		memcpy(buffer, rb->rptr, size);
		rb->rptr += size;
		if (rb->rptr == rb->start + rb->size) {
			rb->rptr = rb->start;
		}

		if (size != count) {
			if (rb->type == SHARED_RINGBUFF) {
				DCache_Invalidate((uint32_t)rb->rptr, count - size);
			}
			memcpy(buffer + size, rb->rptr, count - size);
			rb->rptr += count - size;
			if (rb->rptr == rb->start + rb->size) {
				rb->rptr = rb->start;
			}
		}
	}
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Clean((uint32_t)(&(rb->rptr)), RB_BYTE_ALIGNMENT);
	}
	return 0;
}

uint8_t *RingBuffer_GetWritePoint(RingBuffer *rb)
{
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)(&(rb->wptr)), RB_BYTE_ALIGNMENT);
	}
	return rb->wptr;
}

uint8_t *RingBuffer_GetReadPoint(RingBuffer *rb)
{
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Invalidate((uint32_t)(&(rb->rptr)), RB_BYTE_ALIGNMENT);
	}
	return rb->rptr;
}

void RingBuffer_Reset(RingBuffer *rb)
{
	rb->wptr = rb->rptr = rb->start;
	if (rb->type == SHARED_RINGBUFF) {
		DCache_Clean((uint32_t)rb, sizeof(RingBuffer));
	}
}
