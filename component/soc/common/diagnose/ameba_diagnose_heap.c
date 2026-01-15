/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_diagnose_types.h"
#include "ameba_diagnose_heap.h"

#include "os_wrapper.h"

// Diag heap structure
struct tagRtkDiagHeapHandler_t {
	u32 magic;
	u8 hdr_ver;
	u8 rsvd[3];
	u32 build_time;
	u32 capacity;
	u32 head_begin;
	u32 head_end;
	u32 tail_begin;
	u32 tail_end;
	u32 used_size;
	rtk_diag_heap_length_getter_t getter;
	rtk_diag_heap_release_notice_t notice;
	u8 heap_pool[0];  // Flexible array member for heap
} ;

RtkDiagHeapHandler_t *rtk_diag_heap_create(u32 capacity, rtk_diag_heap_length_getter_t getter, rtk_diag_heap_release_notice_t notice)
{
	if (capacity < RTK_DIAG_HEAP_SIZE_MIN) {
		RTK_LOGA("DIAG", "Failed init heap: cap(%u) must larger than %u\n", capacity, RTK_DIAG_HEAP_SIZE_MIN);
		return NULL;
	}

	RtkDiagHeapHandler_t *handler = (RtkDiagHeapHandler_t *)rtos_mem_malloc(sizeof(RtkDiagHeapHandler_t) + capacity);
	if (NULL == handler) {
		RTK_LOGA("DIAG", "Malloc failed\n");
		return NULL;
	}

	handler->magic = 0x12345678;
	handler->hdr_ver = 0x01;
	handler->build_time = 0x20250102;
	handler->capacity = capacity;
	handler->head_begin = 0;  //used space's head part's start's offset from begin of heap_pool
	handler->head_end = 0;    //used space's head part's end's offset from begin of heap_pool
	handler->tail_begin = 0;  //used space's tail part's start's offset from begin of heap_pool
	handler->tail_end = 0;    //used space's tail part's end's offset from begin of heap_pool
	handler->used_size = 0;

	handler->getter = getter;
	handler->notice = notice;
	_memset(handler->rsvd, 0, sizeof(handler->rsvd));
	_memset(handler->heap_pool, 0, capacity);

	return handler;
}
int rtk_diag_heap_destroy(RtkDiagHeapHandler_t *handler)
{
	if (handler) {
		rtos_mem_free(handler);
	}
	return RTK_SUCCESS;
}

/**
 * Head at front and tail at rear (used_start < used_end),
 * or initial state with no data (both used_start and used_end are 0)
 */
static void *rtk_diag_heap_malloc_normal(RtkDiagHeapHandler_t *handler, u32 length)
{
	void *result = NULL;
	DIAG_DBG_ASSERT(handler->head_end == handler->tail_end);
	DIAG_DBG_ASSERT(handler->used_size == handler->head_end - handler->head_begin);
	if (handler->capacity - handler->tail_end >= length) {
		/* Sufficient space available at the tail */
		result = handler->heap_pool + handler->tail_end;
		handler->tail_end += length;
		handler->used_size += length;
		if (handler->head_begin == handler->tail_begin) {
			handler->head_end = handler->tail_end;
		}
	} else { /** Allocate space starting from the head */
		u32 free_space = handler->head_begin; /** head_begin may not be 0 (data blocks may have been actively freed) */
		u32 current_head_data_length;
		while (free_space < length) {
			if (handler->head_begin < handler->head_end) {
				/** head_begin points to a freeable data block */

				/** Notify the user of the data block to be freed */
				handler->notice(handler->heap_pool + handler->head_begin);
				/** Get the size of the data block to be freed */
				current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin);
				/** Points to the next data block */
				handler->head_begin += current_head_data_length;
				/** head_end remains unchanged */
				handler->used_size -= current_head_data_length;
				free_space += current_head_data_length;
			} else {
				/** head_begin enters the hole block */
				DIAG_DBG_ASSERT(handler->used_size == 0);
				handler->head_begin = 0;
				handler->head_end = length;
				break;
			}
		}
		handler->tail_begin = 0;
		handler->tail_end = length;
		handler->used_size += length;
		result = handler->heap_pool;
		if (handler->head_begin >= handler->head_end) {
			/** Sufficient memory may be allocated in the if clause of the above while loop,
			 * but head_begin may enter the hole area at this time
			 */
			DIAG_DBG_ASSERT(handler->head_begin == handler->head_end);
			handler->head_begin = 0;
			handler->head_end = handler->tail_end;
		}
	}
	return result;
}

void *rtk_diag_heap_malloc(RtkDiagHeapHandler_t *handler, u32 length)
{
	if (length > handler->capacity) {
		RTK_LOGA("DIAG", "Evt malloc failed, too large: %u > %u\n", length, handler->capacity);
		return NULL;
	}
	void *result = NULL;

	if (handler->head_begin == handler->tail_begin) {
		/*
		 * heap layout like below(* is data, o is free space, │ is block boundary):
		 *
		 *   head_begin         head_end
		 *       │                  │
		 *       ▼                  ▼
		 *       ┌──┬────┬──────────┬────────┐
		 *       │**│****│**********│oooooooo│
		 *       └──┴────┴──────────┴────────┘
		 *       ▲                  ▲
		 *       │                  │
		 *   tail_begin          tail_begin
		 * or
		 *      head_begin      head_end
		 *          │               │
		 *          ▼               ▼
		 *       ┌──┬────┬──────────┬────────┐
		 *       │oo│****│**********│oooooooo│
		 *       └──┴────┴──────────┴────────┘
		 *          ▲               ▲
		 *          │               │
		 *      tail_begin       tail_begin
		 */
		return rtk_diag_heap_malloc_normal(handler, length);
	} else {
		/*
		 * heap layout like below(* is data, o is free space, │ is block boundary):
		 *
		 *               head_begin  head_end
		 *                     │        │
		 *                     ▼        ▼
		 *      ┌─┬─┬──┬───────┬─┬───┬──┬───┐
		 *      │*│*│**│ooooooo│*│***│**│ooo│
		 *      └─┴─┴──┴───────┴─┴───┴──┴───┘
		 *      ▲      ▲
		 *      │      │
		 * tail_begin tail_end
		 */
		// RTK_LOGA("DIAG", "DBG: %u %u %u %u\n", handler->head_begin, handler->head_end, handler->tail_begin, handler->tail_end);
		DIAG_DBG_ASSERT(handler->tail_begin == 0);
		DIAG_DBG_ASSERT(handler->tail_end > handler->tail_begin);
		DIAG_DBG_ASSERT(handler->head_begin >= handler->tail_end);
		DIAG_DBG_ASSERT(handler->head_end > handler->head_begin);
		DIAG_DBG_ASSERT(handler->capacity >= handler->head_end);
		DIAG_DBG_ASSERT(handler->used_size == (handler->tail_end - handler->tail_begin) + (handler->head_end - handler->head_begin));

		if (handler->head_begin - handler->tail_end >= length) {
			/** The area between tail and head is sufficient for this allocation */
			result = handler->heap_pool + handler->tail_end;
			handler->tail_end += length;
			handler->used_size += length;
			//tail_begin, head_begin, head_end keep unchanged
		} else {
			/** The area between tail and head is insufficient for this allocation; need to free space starting from head_begin */
			u32 free_space = handler->head_begin - handler->tail_end;
			u32 current_head_data_length;
			while (free_space < length) {
				if (handler->head_begin < handler->head_end) { /** Free the data block if head_begin points to it */
					/** Notify the user of the data block to be freed */
					handler->notice(handler->heap_pool + handler->head_begin);
					/** Get the size of the data block to be freed */
					current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin);
					/** Points to the next data block */
					handler->head_begin += current_head_data_length;
					handler->used_size -= current_head_data_length;
					free_space += current_head_data_length;
				} else {
					/** Try using the remaining hole space */
					free_space += handler->capacity - handler->head_end;
					handler->head_begin = 0;
					handler->head_end = handler->tail_end;
					if (free_space >= length) { /** Try the remaining hole space */
						break;
					} else {/** Enter the first state if still insufficient */
						return rtk_diag_heap_malloc_normal(handler, length);
					}
				}
			}
			result = handler->heap_pool + handler->tail_end;
			handler->tail_end += length;
			handler->used_size += length;

			if (handler->head_begin == 0) {
				handler->head_end = handler->tail_end;
			} else if (handler->head_begin >= handler->head_end) {
				/** Sufficient memory may be allocated in the if clause of the above while loop,
				 * but head_begin may enter the hole area at this time
				 */
				DIAG_DBG_ASSERT(handler->head_begin == handler->head_end);
				handler->head_begin = 0;
				handler->head_end = handler->tail_end;
			}
		}
	}
	return result;
}

u32 rtk_diag_heap_free(RtkDiagHeapHandler_t *handler)
{
	u32 current_head_data_length = 0;
	if (handler->used_size > 0) {
		u8 is_head_overlap_tail = handler->head_begin == handler->tail_begin;
		// handler->notice(handler->heap_pool + handler->head_begin);
		current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin);
		handler->used_size -= current_head_data_length;
		handler->head_begin += current_head_data_length;
		if (handler->head_begin >= handler->head_end) {
			//when points to hole data block
			DIAG_DBG_ASSERT(handler->head_begin == handler->head_end);
			if (is_head_overlap_tail) {
				handler->tail_begin = 0;
				handler->tail_end = 0;
			}
			handler->head_begin = handler->tail_begin;
			handler->head_end = handler->tail_end;
		} else if (is_head_overlap_tail) { //update tail_begin
			handler->tail_begin = handler->head_begin;
		}

	}
	return current_head_data_length;
}

u32 rtk_diag_heap_get_capacity(const RtkDiagHeapHandler_t *handler)
{
	return handler->capacity;
}