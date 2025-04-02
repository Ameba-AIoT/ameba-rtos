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
 * @file	heap_trace.h
 * @brief	A module for tracing heap status for FreeRTOS.
 */
#ifndef HEAP_TRACE_H
#define HEAP_TRACE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "sys_queue.h"
#include "platform_autoconf.h"

typedef struct {
	size_t xAvailableHeapSpaceInBytes;          /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
	size_t xSizeOfLargestFreeBlockInBytes;      /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xSizeOfSmallestFreeBlockInBytes;     /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xNumberOfFreeBlocks;                 /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xMinimumEverFreeBytesRemaining;      /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
	size_t xNumberOfSuccessfulAllocations;      /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
	size_t xNumberOfSuccessfulFrees;            /* The number of calls to vPortFree() that has successfully freed a block of memory. */
} rtos_heap_stats;

/**
 * @brief	Retrieve heap statistics.
 *
 * @param	pxHeapStats: A pointer to a rtos_heap_stats structure where heap statistics will be stored.
 */
void heap_get_stats(rtos_heap_stats *pxHeapStats);

#if defined (CONFIG_HEAP_PROTECTOR)
/**
 * @brief	Check the integrity of the heap.
 *
 * @return	Returns pdTRUE if the heap is intact, or assert if corruption is detected.
 */
uint32_t heap_check_integrity(void);
#endif

#ifdef CONFIG_HEAP_TRACE

/**
 * @enum	tracing_state_t
 * @brief	Defines the state of memory operation tracing.
 */
typedef enum {
	TRACING_STARTED,      // start recording allocs and free
	TRACING_STOPPED,      // stop recording allocs and free
	TRACING_ALLOC_PAUSED, // stop recording allocs but keep recording free
	TRACING_UNKNOWN       // default value
} tracing_state_t;

typedef enum {
	HEAP_TRACE_ALL,
	HEAP_TRACE_LEAKS,
} heap_trace_mode_t;

/**
 * @brief Trace record data type. Stores information about an allocated region of memory.
 */
typedef struct heap_trace_record_t {
	void *address;   ///< Address which was allocated. If NULL, then this record is empty.
	size_t size;     ///< Size of the allocation
	void *alloced_by[CONFIG_HEAP_TRACE_STACK_DEPTH]; ///< Call stack of the caller which allocated the memory.
	void *freed_by[CONFIG_HEAP_TRACE_STACK_DEPTH];   ///< Call stack of the caller which freed the memory (all zero if not freed.)
	TAILQ_ENTRY(heap_trace_record_t) tailq_list;       ///< Linked list: prev & next records
} heap_trace_record_t;

/**
 * @brief Initialise malloc/free tracing.
 *
 *        This function must be called before any other heap tracing functions.
 *        To disable heap tracing and allow the buffer to be freed, stop tracing and then call
 *        heap_trace_init_standalone(NULL, 0);
 *
 * @param record_buffer Provide a buffer to use for heap trace data.
 *        num_records Size of the heap trace buffer
 *
 */
uint32_t heap_trace_init(heap_trace_record_t *record_buffer, size_t num_records);

/**
 * @brief Dump heap trace record data
 */
void heap_trace_record_dump(void);

/*
 * @brief Start heap tracing.
 */
void heap_trace_start(void);

/*
 * @brief Start heap tracing.
 */
void heap_trace_stop(void);

/**
 * @brief	Record a malloc operation for tracing.
 *
 * @param	pvAddress: The address of the allocated memory.
 * @param	uiSize: The size of the allocated memory.
 */
void trace_malloc(void *pvAddress, uint32_t uiSize);

/**
 * @brief	Record a free operation for tracing.
 *
 * @param	pvAddress: The address of the freed memory.
 * @param	uiSize: The size of the freed memory.
 */
void trace_free(void *pvAddress, uint32_t uiSize);

/**
 * @brief	Retrieve per-task heap usage information.
 *
 * This function will provide detailed information about heap usage for each task.
 */
void heap_get_per_task_info(void);

#endif
#endif /* CONFIG_HEAP_TRACE */
