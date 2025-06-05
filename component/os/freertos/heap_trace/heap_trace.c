/*
 * SPDX-License-Identifier: LicenseRef-Realtek-Proprietary
 *
 * Copyright (c) 2023, Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

/**
 * @file	heap_trace.c
 * @brief	A module for tracing heap status for FreeRTOS.
 */

#include "ameba_soc.h"
#include "stdio.h"
#include "stdint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "heap_trace.h"

static_assert(sizeof(rtos_heap_stats) == sizeof(HeapStats_t), "sizeof(rtos_heap_stats) and sizeof(HeapStats_t) missmatch");

extern void vPortGetTaskHeapInfo(void);

#if defined (CONFIG_HEAP_PROTECTOR)
extern uint32_t ulPortCheckHeapIntegrity(int COMPREHENSIVE_CHECK);
/**
 * @brief	Check the integrity of the heap.
 *
 * @return	Returns pdTRUE if the heap is intact, or assert if corruption is detected.
 */
uint32_t heap_check_integrity(void)
{
	uint32_t ret;

	ret = ulPortCheckHeapIntegrity(TRUE);

	return ret;
}
#endif

/**
 * @brief	Retrieve heap statistics.
 *
 * @param	pxHeapStats: A pointer to a HeapStats_t structure where heap statistics will be stored.
 */
void heap_get_stats(rtos_heap_stats *pxHeapStats)
{
	vPortGetHeapStats((HeapStats_t *)pxHeapStats);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "********** Heap Usage Status **********\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Total Number Of Successful Allocations:    %u\n", pxHeapStats->xNumberOfSuccessfulAllocations);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Total Number Of Successful Frees:          %u\n", pxHeapStats->xNumberOfSuccessfulFrees);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Current Available Heap Space:              %u bytes\n", pxHeapStats->xAvailableHeapSpaceInBytes);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Minimum Ever Free Space Remaining:         %u bytes\n", pxHeapStats->xMinimumEverFreeBytesRemaining);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Number Of Free Blocks:                     %u\n", pxHeapStats->xNumberOfFreeBlocks);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Size Of Largest Free Block:                %u bytes\n", pxHeapStats->xSizeOfLargestFreeBlockInBytes);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Size Of Smallest Free Block:               %u bytes\n", pxHeapStats->xSizeOfSmallestFreeBlockInBytes);
}

#ifdef CONFIG_HEAP_TRACE

#if defined CONFIG_ARM_CORE_CM4
#include "ameba_v8m_backtrace.h"
#elif defined CONFIG_RSICV_CORE_KR4
#include "ameba_rv32_backtrace.h"
#endif

static_assert(CONFIG_HEAP_TRACE_STACK_DEPTH <= CONFIG_BACK_TRACE_DEPTH_LIMIT,
			  "CONFIG_HEAP_TRACE_STACK_DEPTH needs to be less than or equal to CONFIG_BACK_TRACE_DEPTH_LIMIT");

/**
 * @var     static tracing_state_t tracing
 * @brief   Stores the current state of heap tracing.
 */
static tracing_state_t tracing = TRACING_UNKNOWN;
static heap_trace_mode_t mode = HEAP_TRACE_LEAKS;

/* Define struct: linked list of records */
TAILQ_HEAD(heap_trace_record_list_struct_t, heap_trace_record_t);
typedef struct heap_trace_record_list_struct_t heap_trace_record_list_t;

/* Linked List of Records */
typedef struct {

	/* Buffer used for records. */
	heap_trace_record_t *buffer;

	/* Linked list of recorded allocations */
	heap_trace_record_list_t list;

	/* Linked list of available record objects */
	heap_trace_record_list_t unused;

	/* capacity of 'buffer' */
	uint32_t capacity;

	/* Count of entries in 'list' */
	uint32_t count;

	/* During execution, we remember the maximum
	   value of 'count'. This can help you
	   choose the right size for your buffer capacity.*/
	size_t high_water_mark;

	/* Has the buffer overflowed and lost trace entries? */
	bool has_overflowed;
} records_t;

/* The actual records. */
static records_t records;

// Forward Defines
static void record_deep_copy(heap_trace_record_t *r_dest, const heap_trace_record_t *r_src);
static void list_setup(void);
static void list_remove(heap_trace_record_t *r_remove);
static heap_trace_record_t *list_add(const heap_trace_record_t *r_append);
static heap_trace_record_t *list_pop_unused(void);
static heap_trace_record_t *list_find(void *p);
static void list_find_and_remove(void *p);

/*-----------------------------------------------------------
              records operation functions
 -----------------------------------------------------------*/
uint32_t heap_trace_init(heap_trace_record_t *record_buffer, size_t num_records)
{
	if ((tracing == TRACING_STARTED) || (tracing == TRACING_ALLOC_PAUSED)) {
		return pdFALSE;
	}

	if (record_buffer == NULL || num_records == 0) {
		return pdFALSE;
	}

	records.buffer = record_buffer;
	records.capacity = num_records;

	return pdTRUE;
}

void heap_trace_start()
{
	portENTER_CRITICAL();

	// clear buffers
	_memset(records.buffer, 0, sizeof(heap_trace_record_t) * records.capacity);

	records.count = 0;
	records.has_overflowed = false;
	list_setup();

	tracing = TRACING_STARTED;

	portEXIT_CRITICAL();
}

void heap_trace_stop()
{
	portENTER_CRITICAL();

	tracing = TRACING_STOPPED;

	portEXIT_CRITICAL();
}

/* Add a new allocation to the heap trace records */
static void record_allocation(const heap_trace_record_t *r_allocation)
{
	if ((tracing != TRACING_STARTED) || (r_allocation->address == NULL)) {
		return;
	}
	portENTER_CRITICAL();

	if (tracing == TRACING_STARTED) {
		// If buffer is full, pop off the oldest
		// record to make more space
		if (records.count == records.capacity) {

			records.has_overflowed = true;

			heap_trace_record_t *r_first = TAILQ_FIRST(&records.list);

			// always remove from hashmap first since list_remove is setting address field
			// of the record to 0x00
			list_remove(r_first);
		}
		// push onto end of list
		list_add(r_allocation);
	}

	portEXIT_CRITICAL();
}

/* record a free event in the heap trace log

   For HEAP_TRACE_ALL, this means filling in the freed_by pointer.
   For HEAP_TRACE_LEAKS, this means removing the record from the log.

   callers is an array of  CONFIG_HEAP_TRACE_STACK_DEPTH function pointer from the call stack
   leading to the call of record_free.
*/
static void record_free(void *p, void **callers)
{
	if ((tracing == TRACING_STOPPED) || (p == NULL)) {
		return;
	}

	portENTER_CRITICAL();
	// return directly if records.count == 0. In case of hashmap being used
	// this prevents the hashmap to return an item that is no longer in the
	// records list.
	if (records.count == 0) {
		portEXIT_CRITICAL();
		return;
	}

	if (tracing != TRACING_STOPPED) {

		if (mode == HEAP_TRACE_ALL) {
			heap_trace_record_t *r_found = list_find(p);
			if (r_found != NULL) {
				// add 'freed_by' info to the record
				_memcpy(r_found->freed_by, callers, sizeof(void *) * CONFIG_HEAP_TRACE_STACK_DEPTH);
			}
		} else { // HEAP_TRACE_LEAKS
			// Leak trace mode, once an allocation is freed
			// we remove it from the list & hashmap
			list_find_and_remove(p);
		}
	}

	portEXIT_CRITICAL();
}

void heap_trace_record_dump()
{
	portENTER_CRITICAL();

	uint32_t delta_size = 0;
	uint32_t delta_allocs = 0;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "********** Heap Trace Log Count: %u records (log buffer capacity: %u) **********\n", records.count, records.capacity);

	heap_trace_record_t *r_cur = TAILQ_FIRST(&records.list);

	for (uint32_t i = 0; i < records.count; i++) {
		// check corruption
		if (r_cur == NULL) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nError: heap trace linked list is corrupt. expected more records.\n");
			break;
		}

		if (r_cur->address != NULL) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%u bytes (@ 0x%08x) allocated, caller bakctrace: ", r_cur->size, r_cur->address);

			// start print backtrace pc from third value, to avoid common pvPortMalloc functions.
			for (int j = 2; j < CONFIG_HEAP_TRACE_STACK_DEPTH; j++) {
				if (r_cur->alloced_by[j] == 0) {
					break;
				}
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "0x%08x ", r_cur->alloced_by[j]);
			}

			if (mode != HEAP_TRACE_ALL || CONFIG_HEAP_TRACE_STACK_DEPTH == 0 || r_cur->freed_by[0] == NULL) {
				delta_size += r_cur->size;
				delta_allocs++;
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
			}
		}
		r_cur = TAILQ_NEXT(r_cur, tailq_list);

	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "========================= Heap Trace Summary =========================\n");

	if (mode == HEAP_TRACE_ALL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Mode: Heap Trace All\n");
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%u bytes alive in trace (%u allocations)\n",
				 delta_size, delta_allocs);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Mode: Heap Trace Leaks\n");
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%u bytes 'leaked' in trace (%u allocations)\n", delta_size, delta_allocs);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "records: %u (%u capacity, %u high water mark)\n",
			 records.count, records.capacity, records.high_water_mark);

	portEXIT_CRITICAL();
}

/*-----------------------------------------------------------
              list operation functions
 -----------------------------------------------------------*/

// connect all records into a linked list of 'unused' records
static void list_setup(void)
{
	TAILQ_INIT(&records.list);
	TAILQ_INIT(&records.unused);

	for (uint32_t i = 0; i < records.capacity; i++) {
		heap_trace_record_t *r_cur = &records.buffer[i];
		TAILQ_INSERT_TAIL(&records.unused, r_cur, tailq_list);
	}
}

/* 1. removes record r_remove from records.list,
   2. places it into records.unused */
static void list_remove(heap_trace_record_t *r_remove)
{
	configASSERT(records.count > 0);

	// remove from records.list
	TAILQ_REMOVE(&records.list, r_remove, tailq_list);

	// set as unused
	r_remove->address = 0;
	r_remove->size = 0;

	// add to records.unused
	TAILQ_INSERT_HEAD(&records.unused, r_remove, tailq_list);

	// decrement
	records.count--;
}

// Append a record to records.list
// Note: This deep copies r_append
static heap_trace_record_t *list_add(const heap_trace_record_t *r_append)
{
	if (records.count < records.capacity) {

		// get unused record
		heap_trace_record_t *r_dest = list_pop_unused();

		// we checked that there is capacity, so this
		// should never be null.
		configASSERT(r_dest != NULL);

		// copy allocation data
		record_deep_copy(r_dest, r_append);

		// append to records.list
		TAILQ_INSERT_TAIL(&records.list, r_dest, tailq_list);

		// increment
		records.count++;

		// high water mark
		if (records.count > records.high_water_mark) {
			records.high_water_mark = records.count;
		}

		return r_dest;

	} else {
		records.has_overflowed = true;
		return NULL;
	}
}

// deep copy a record.
// Note: only copies the *allocation data*, not the next & prev ptrs
static void record_deep_copy(heap_trace_record_t *r_dest, const heap_trace_record_t *r_src)
{
	r_dest->address = r_src->address;
	r_dest->size    = r_src->size;
	_memcpy(r_dest->freed_by,   r_src->freed_by,   sizeof(void *) * CONFIG_HEAP_TRACE_STACK_DEPTH);
	_memcpy(r_dest->alloced_by, r_src->alloced_by, sizeof(void *) * CONFIG_HEAP_TRACE_STACK_DEPTH);
}

// pop record from unused list
static heap_trace_record_t *list_pop_unused(void)
{
	// no records left?
	if (records.count >= records.capacity) {
		return NULL;
	}

	// get from records.unused
	heap_trace_record_t *r_unused = TAILQ_FIRST(&records.unused);
	configASSERT(r_unused->address == NULL);
	configASSERT(r_unused->size == 0);

	// remove from records.unused
	TAILQ_REMOVE(&records.unused, r_unused, tailq_list);

	return r_unused;
}

// search records.list for the allocation record matching this address
static heap_trace_record_t *list_find(void *p)
{
	heap_trace_record_t *r_found = NULL;

	// to the end of the list and most allocations are short lived.
	heap_trace_record_t *r_cur = NULL;
	TAILQ_FOREACH(r_cur, &records.list, tailq_list) {
		if (r_cur->address == p) {
			r_found = r_cur;
			break;
		}
	}

	return r_found;
}

static void list_find_and_remove(void *p)
{
	heap_trace_record_t *r_cur = NULL;
	TAILQ_FOREACH(r_cur, &records.list, tailq_list) {
		if (r_cur->address == p) {
			list_remove(r_cur);
			break;
		}
	}
}

/**
 * @brief	Record a malloc operation for tracing.
 *
 * @param	pvAddress: The address of the allocated memory.
 * @param	uiSize: The size of the allocated memory.
 */
void __attribute__((optimize("O0"))) trace_malloc(void *pvAddress, uint32_t uiSize)
{
#if defined (CONFIG_HEAP_TRACE_MALLOC_FREE_LOG)
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] pvAddress:0x%08x, uiSize:0x%08x \n", __func__, pvAddress, uiSize);
#endif
	if (tracing == TRACING_STARTED) {
		heap_trace_record_t rec = {
			.address = pvAddress,
			.size = uiSize,
		};
#if defined (CONFIG_ARM_CORE_CM4) || defined (CONFIG_RSICV_CORE_KR4)
		get_call_stack(rec.alloced_by, CONFIG_HEAP_TRACE_STACK_DEPTH);
#endif /* CONFIG_ARM_CORE_CM4 || CONFIG_RSICV_CORE_KR4 */
		record_allocation(&rec);
	}
}

/**
 * @brief	Record a free operation for tracing.
 *
 * @param	pvAddress: The address of the freed memory.
 * @param	uiSize: The size of the freed memory.
 */
void __attribute__((optimize("O0"))) trace_free(void *pvAddress, uint32_t uiSize)
{
#if defined (CONFIG_HEAP_TRACE_MALLOC_FREE_LOG)
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] pvAddress:0x%08x, uiSize:0x%08x \n", __func__, pvAddress, uiSize);
#else
	UNUSED(uiSize);
#endif
	if (tracing == TRACING_STARTED) {
		void *callers[CONFIG_HEAP_TRACE_STACK_DEPTH];
		record_free(pvAddress, callers);
	}
}

/**
 * @brief	Retrieve per-task heap usage information.
 *
 * This function will provide detailed information about heap usage for each task.
 */
void heap_get_per_task_info(void)
{
	vPortGetTaskHeapInfo();
}

#endif /* CONFIG_HEAP_TRACE */

#if defined (CONFIG_HEAP_PROTECTOR)
void vApplicationGetRandomHeapCanary(uint32_t *pxHeapCanary)
{
	*pxHeapCanary = 0xdeaf25f3;
}
#endif
