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

/*
 * If FreeRTOS is selected with TLSF, RTOS pvPortMalloc and vPortFree will be
 * implemented with tlsf_malloc() and tlsf_free()
 */
#include "os_wrapper_specific.h"
#include "task.h"
#include "port_tlsf.h"
#include "tlsf.h"

#include "assert.h"
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_DEBUG_HEAP_TRACE
#include "heap_trace.h"
#endif

typedef struct block_header_t {
	/* Points to the previous physical block. */
	struct block_header_t *prev_phys_block;

	/* The size of this block, excluding the block header. */
	size_t size;

	/* Next and previous free blocks. */
	struct block_header_t *next_free;
	struct block_header_t *prev_free;
} block_header_t;

#define heapBLOCK_CURR_FREE_BITMASK    (1 << 0)
#define heapBLOCK_PREV_FREE_BITMASK    (1 << 1)

/* Allocate the memory for the heap. */
#if ( configAPPLICATION_ALLOCATED_HEAP == 1 )

/* The application writer has already defined the array used for the RTOS
* heap - probably so it can be placed in a special segment or address. */
extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#else
PRIVILEGED_DATA uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif /* configAPPLICATION_ALLOCATED_HEAP */

/* Keeps track of the number of calls to allocate and free memory as well as the
 * number of free bytes remaining, but says nothing about fragmentation. */
PRIVILEGED_DATA static size_t xFreeBytesRemaining = (size_t) 0U;
PRIVILEGED_DATA static size_t xMinimumEverFreeBytesRemaining = (size_t) 0U;
PRIVILEGED_DATA static size_t xNumberOfSuccessfulAllocations = (size_t) 0U;
PRIVILEGED_DATA static size_t xNumberOfSuccessfulFrees = (size_t) 0U;

/*
 * These three static variables are only used during a single heap statistics
 * collection:
 *  - xBlocks  : Counts the number of free memory blocks found while walking
 *               through the TLSF memory pool.
 *  - xMaxSize : Records the size (in bytes) of the largest free block
 *               encountered during the walk.
 *  - xMinSize : Records the size (in bytes) of the smallest free block
 *               encountered during the walk.
 * After the walk completes, these values are copied into the HeapStats_t
 * structure to expose the heap status to external callers.
 */
static size_t xBlocks = 0;
static size_t xMaxSize = 0;
static size_t xMinSize = portMAX_DELAY;

/* Heap Trace releated symbols */
#if ( CONFIG_DEBUG_HEAP_TRACE == 1 )
task_heap_info_t heap_task_info[ MAX_TRACE_TASK_HEAP ] = {0};
#endif

#if ( configENABLE_HEAP_PROTECTOR == 1 )
/* Canary value for protecting internal heap pointers. */
PRIVILEGED_DATA portPOINTER_SIZE_TYPE xHeapCanary;

/**
 * @brief Application provided function to get a random value to be used as canary.
 *
 * @param pxHeapCanary [out] Output parameter to return the canary value.
 */
extern void vApplicationGetRandomHeapCanary(portPOINTER_SIZE_TYPE *pxHeapCanary);
#endif /* configENABLE_HEAP_PROTECTOR */

#if ( configLIGHT_IMPACT == 1 )
#define xHeadCanaryValue      0xABBA1234
#define xTailCanaryValue      0xBAAD5678
#define CANARY_SIZE_BYTES     4U
#define HEAD_CANARY_SIZE       CANARY_SIZE_BYTES
#define TAIL_CANARY_SIZE       CANARY_SIZE_BYTES
#define CANARY_PADDING_WORDS  (CANARY_SIZE_BYTES / sizeof(uint32_t))

#if ( configHEAP_COMPREHENSIVE == 1 )
#define xFillAlocated   0xCE
#define xFillFreed      0xFE
#endif /* configHEAP_COMPREHENSIVE */

#endif /* configLIGHT_IMPACT */

uint32_t tlsf_pool_init = 0;

#ifdef CONFIG_DEBUG_HEAP_TRACE

static void vPortInsertHeapInfo(size_t xSize)
{
	char *pcTaskName = NULL;
	UBaseType_t x;
	UBaseType_t xInserted = pdFALSE;
	TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();
	if (curTaskHandle != NULL) {
		for (int i = 0; i < MAX_TRACE_TASK_HEAP; i++) {
			if (heap_task_info[ i ].TaskHandle == NULL) {
				heap_task_info[ i ].TaskHandle = curTaskHandle;
				pcTaskName = pcTaskGetName(curTaskHandle);
				heap_task_info[ i ].heap_size = xSize;
				heap_task_info[ i ].isExist = pdTRUE;

				for (x = (UBaseType_t) 0; x < (UBaseType_t) configMAX_TASK_NAME_LEN; x++) {
					heap_task_info[ i ].Task_Name[ x ] = pcTaskName[ x ];

					/* Don't copy all configMAX_TASK_NAME_LEN if the string is shorter than
					* configMAX_TASK_NAME_LEN characters just in case the memory after the
					* string is not accessible (extremely unlikely). */
					if (pcTaskName[ x ] == (char) 0x00) {
						break;
					}
				}
				xInserted = pdTRUE;
				break;
			} else if ((heap_task_info[ i ].TaskHandle == curTaskHandle) && (heap_task_info[ i ].isExist == pdTRUE)) {
				heap_task_info[ i ].heap_size += xSize;
				xInserted = pdTRUE;
				break;
			}
		}

		if (xInserted == pdFALSE) {
			printf("Warning: MAX_TRACE_TASK_HEAP is not enough, record failed!\n");
		}
	}
}
/*-----------------------------------------------------------*/

static void vPortRemoveHeapInfo(size_t xSize)
{
	UBaseType_t x;
	UBaseType_t xRemoved = pdFALSE;
	TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();

	if (curTaskHandle != NULL) {
		for (x = 0; x < MAX_TRACE_TASK_HEAP; x++) {
			if (heap_task_info[ x ].TaskHandle == curTaskHandle) {
				if (heap_task_info[ x ].isExist == pdTRUE) {
					configASSERT(heap_task_info[ x ].heap_size >= xSize);
					heap_task_info[ x ].heap_size -= xSize;
					xRemoved = pdTRUE;
					break;
				} else {
					printf("Warning: task %p deleted with memory un-freed. It is recommended to free memory before task detelte\n"
						   "         to avoid task heap usage statistics error\n"
						   , curTaskHandle);
				}
			}
		}
		if (xRemoved == pdFALSE) {
			printf("Warning: record failed! MAX_TRACE_TASK_HEAP is not enough\n");
		}
	}
}
/*-----------------------------------------------------------*/
#endif

#if ( configLIGHT_IMPACT == 1 )
static size_t xPortCanaryCheck(void *pv)
{
	size_t xCurBlockSize = tlsf_block_size(pv);
	xCurBlockSize &= ~(heapBLOCK_CURR_FREE_BITMASK | heapBLOCK_PREV_FREE_BITMASK);
	size_t xTailCanaryOffset = xCurBlockSize - TAIL_CANARY_SIZE;
	uint32_t *pvStartHead = (uint32_t *)(pv);
	uint32_t *pvStartTail = (uint32_t *)((uint8_t *) pv + xTailCanaryOffset);
	size_t result = pdTRUE;

	for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
		if (pvStartHead[ i ] != xHeadCanaryValue) {
			result = pdFALSE;
			printf("HEAD CANARY CORRUPT at %p. Underrun value: 0x%08lx\n", &pvStartHead[ i ], pvStartHead[ i ]);
			break;
		}

		if (pvStartTail[ i ] != xTailCanaryValue) {
			result = pdFAIL;
			printf("TAIL CANARY CORRUPT at %p. Overrun value: 0x%08lx\n", &pvStartTail[ i ], pvStartTail[ i ]);
			break;
		}
	}

	return result;
}
#endif /* configLIGHT_IMPACT */

static size_t align_up(size_t x, size_t align)
{
	configASSERT(0 == (align & (align - 1)) && "must align to a power of two");
	return (x + (align - 1)) & ~(align - 1);
}

static void free_block_stats_walker(void *ptr, size_t size, int used, void *user)
{
	/* Unused param ptr: block pointer not needed for size statistics */
	/* Unused param user: no context data required, using global variables */
	(void)ptr;
	(void)user;

	if (!used) {
		xBlocks++;

		if (size > xMaxSize) {
			xMaxSize = size;
		}
		if (xMinSize == 0 || size < xMinSize) {
			xMinSize = size;
		}

		if (tlsf_pool_init == 0) {
			xFreeBytesRemaining += size;
		}
	}
}

void *pvPortMalloc(size_t xWantedSize)
{
	void *pvReturn = NULL;
	size_t xWantedSizeAlign = 0;
	size_t xBlockSize = 0;

#if ( configLIGHT_IMPACT == 1 )
	uint32_t *pvHeadCanary = NULL;
	uint32_t *pvTailCanary = NULL;

	/* Reserve space for Head and Tail Canary */
	xWantedSize = xWantedSize + HEAD_CANARY_SIZE + TAIL_CANARY_SIZE;
#endif /* configLIGHT_IMPACT */

#if ( configHEAP_COMPREHENSIVE == 1 )
	void *pvBlockToFill = NULL;
	size_t xFillBlockSize = 0;
	size_t xCheckFreedBlockSize = 0;
#endif /* configHEAP_COMPREHENSIVE */

	vTaskSuspendAll();
	{
		if (tlsf_pool_init == 0) {
#if ( configENABLE_HEAP_PROTECTOR == 1 )
			vApplicationGetRandomHeapCanary(&(xHeapCanary));
#endif /* configENABLE_HEAP_PROTECTOR */

			tlsf = tlsf_create_with_pool(ucHeap, configTOTAL_HEAP_SIZE);
			pool_t pool = tlsf_get_pool(tlsf);
			tlsf_walk_pool(pool, free_block_stats_walker, NULL);
			xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
			tlsf_pool_init = 1;
#if ( configHEAP_COMPREHENSIVE == 1 )
			pvBlockToFill = (void *)(((uint8_t *) pool) + 3 * sizeof(block_header_t *));
			xFillBlockSize = xFreeBytesRemaining - 3 * sizeof(block_header_t *);
			_memset(pvBlockToFill, xFillFreed, xFillBlockSize);
#endif /* configHEAP_COMPREHENSIVE */
		}
	}
	(void) xTaskResumeAll();

	xWantedSizeAlign = align_up(xWantedSize, portBYTE_ALIGNMENT);

	if (xWantedSizeAlign <= xFreeBytesRemaining) {
		vTaskSuspendAll();
		{
			/* Init the tlsf controller on the first time when malloc is called */

			pvReturn =  tlsf_malloc(tlsf, xWantedSizeAlign);
			xBlockSize = tlsf_block_size(pvReturn);

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
			{
				if (pvReturn == NULL) {
					vApplicationMallocFailedHook();
				}
			}
#endif /* if ( configUSE_MALLOC_FAILED_HOOK == 1 ) */

			xFreeBytesRemaining -= xBlockSize;

			if (xFreeBytesRemaining < xMinimumEverFreeBytesRemaining) {
				xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
			}

			xNumberOfSuccessfulAllocations++;

#ifdef CONFIG_DEBUG_HEAP_TRACE
			vPortInsertHeapInfo(xBlockSize);
#endif

#if ( configLIGHT_IMPACT == 1 )
			pvHeadCanary = (uint32_t *)pvReturn;

#if ( configHEAP_COMPREHENSIVE == 1 )
			/* Check if the free blocks content are un-touched */
			xCheckFreedBlockSize = xBlockSize - 3 * sizeof(block_header_t *);
			uint8_t *pxAddress = (uint8_t *) pvHeadCanary + 2 * sizeof(block_header_t *);
			for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
				if (pxAddress[ i ] != xFillFreed) {
					printf("Write to a unallocated Block: %p. Invalidate value: 0x%x\n", &pxAddress[ i ], pxAddress[ i ]);
					configASSERT(pdFALSE);
				}
			}
#endif /* configHEAP_COMPREHENSIVE */

			/* Set the Head Canary Value */
			for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
				*(pvHeadCanary + i) = xHeadCanaryValue;
			}
#if ( configHEAP_COMPREHENSIVE == 1 )
			/* Fill the allocated address with magic word */
			pvBlockToFill = (void *)(((uint8_t *) pvHeadCanary) + HEAD_CANARY_SIZE);
			xFillBlockSize = xBlockSize - TAIL_CANARY_SIZE - HEAD_CANARY_SIZE;
			_memset(pvBlockToFill, xFillAlocated, xFillBlockSize);
#endif /* configHEAP_COMPREHENSIVE */

			pvTailCanary = (uint32_t *)((uint8_t *)pvHeadCanary + xBlockSize - TAIL_CANARY_SIZE);
			for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
				*(pvTailCanary + i) = xTailCanaryValue;
			}

			pvReturn += HEAD_CANARY_SIZE;
#endif /* configLIGHT_IMPACT */
		}
		(void) xTaskResumeAll();
	}

	return pvReturn;
}

void vPortFree(void *pv)
{
#if ( configLIGHT_IMPACT == 1 )
	pv -= HEAD_CANARY_SIZE;
	void *pxLink = pv;
#endif /* configLIGHT_IMPACT */

#if( configHEAP_COMPREHENSIVE == 1 )
	uint8_t *puc = (uint8_t *) pv;
	uint8_t *pucBlockToFree = NULL;
	size_t xBlockToFillSize = 0;
#endif /* configHEAP_COMPREHENSIVE */

	if (pv != NULL) {
#if( configLIGHT_IMPACT == 1)
		configASSERT(xPortCanaryCheck(pxLink));
#endif /* configLIGHT_IMPACT */

		vTaskSuspendAll();
		{
			xFreeBytesRemaining += (tlsf_block_size(pv));
			xNumberOfSuccessfulFrees++;

#ifdef CONFIG_DEBUG_HEAP_TRACE
			vPortRemoveHeapInfo(tlsf_block_size(pv));
#endif
			tlsf_free(tlsf, pv);

#if ( configHEAP_COMPREHENSIVE == 1 )
			/* Fill the freed address with magic word
			* this will overwrotten configHEAP_CLEAR_MEMORY_ON_FREE. */

			/* Check if previous block is free, if ture, fill current block header. */
			size_t xCurBlockSize = tlsf_block_size(pv);
			if ((*(puc - sizeof(size_t))  & heapBLOCK_PREV_FREE_BITMASK) != 0) {
				pucBlockToFree = puc - sizeof(size_t) - sizeof(block_header_t *);
				xBlockToFillSize = xCurBlockSize + sizeof(size_t);
			} else {
				pucBlockToFree = puc + 2 * sizeof(block_header_t *);
				xBlockToFillSize = xCurBlockSize - 3 * sizeof(block_header_t *);
			}

			_memset(pucBlockToFree, xFillFreed, xBlockToFillSize);

			/* Check if next block is free, if ture, fill next block header. */
			if ((*(puc + xCurBlockSize)  & heapBLOCK_CURR_FREE_BITMASK) != 0) {
				pucBlockToFree = puc + xCurBlockSize - sizeof(block_header_t *);
				xBlockToFillSize = sizeof(block_header_t);
				_memset(pucBlockToFree, xFillFreed, xBlockToFillSize);
			}

#endif /* configHEAP_COMPREHENSIVE */
		}
		(void) xTaskResumeAll();
	}
}

size_t xPortGetFreeHeapSize(void)
{
	return xFreeBytesRemaining;
}

size_t xPortGetMinimumEverFreeHeapSize(void)
{
	return xMinimumEverFreeBytesRemaining;
}

void vPortGetHeapStats(HeapStats_t *pxHeapStats)
{
	xBlocks = 0;
	xMaxSize = 0;
	xMinSize = portMAX_DELAY;

	vTaskSuspendAll();
	{
		pool_t pool = tlsf_get_pool(tlsf);
		tlsf_walk_pool(pool, free_block_stats_walker, NULL);
	}
	(void) xTaskResumeAll();

	pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;
	pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
	pxHeapStats->xNumberOfFreeBlocks = xBlocks;

	taskENTER_CRITICAL();
	{
		pxHeapStats->xAvailableHeapSpaceInBytes = xFreeBytesRemaining;
		pxHeapStats->xNumberOfSuccessfulAllocations = xNumberOfSuccessfulAllocations;
		pxHeapStats->xNumberOfSuccessfulFrees = xNumberOfSuccessfulFrees;
		pxHeapStats->xMinimumEverFreeBytesRemaining = xMinimumEverFreeBytesRemaining;
	}
	taskEXIT_CRITICAL();
}

#ifdef CONFIG_DEBUG_HEAP_TRACE

void vPortGetTaskHeapInfo()
{
	vTaskSuspendAll();

	for (int i = 0; i < MAX_TRACE_TASK_HEAP; i++) {
		if (heap_task_info[ i ].TaskHandle != NULL) {
			switch (heap_task_info[ i ].isExist) {
			case pdTRUE:
				printf("Task: %s, task heap usage: 0x%lx\n", heap_task_info[ i ].Task_Name, heap_task_info[ i ].heap_size);
				break;
			case pdFALSE:
				printf("Deleted Task: %s, task heap usage: 0x%lx\n", heap_task_info[ i ].Task_Name, heap_task_info[ i ].heap_size);
				break;
			default :
				break;
			}
		}
	}

	(void) xTaskResumeAll();
}
/*-----------------------------------------------------------*/
#if ( configLIGHT_IMPACT == 1 )
static void corrupt_detection_walker(void *ptr, size_t size, int used, void *user)
{
	(void)user;
#if ( configHEAP_COMPREHENSIVE == 1 )
	size_t xCheckFreedBlockSize = 0;
	uint8_t *pxAddress = NULL;
#endif /* configHEAP_COMPREHENSIVE */

	void *pxLink = ptr;
	if (!used) {
#if ( configHEAP_COMPREHENSIVE == 1 )
		/* Check if the free blocks content are un-touched */
		xCheckFreedBlockSize = size - 3 * sizeof(block_header_t *);
		pxAddress = (uint8_t *) ptr + 2 * sizeof(block_header_t *);
		for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
			if (pxAddress[ i ] != xFillFreed) {
				printf("Freed block has been modified after freed! Block address: %p. Invalidate value: 0x%x\n", &pxAddress[ i ], pxAddress[ i ]);
				configASSERT(pdFALSE);
			}
		}
#endif /* configHEAP_COMPREHENSIVE */
	} else {
		(void)size;
		configASSERT(xPortCanaryCheck(pxLink));
	}
}
#endif /* configLIGHT_IMPACT */

uint32_t ulPortCheckHeapIntegrity()
{
	int ret;

	vTaskSuspendAll();

	ret =  tlsf_check(tlsf);

	if (ret != 0) {
		(void)xTaskResumeAll();
		configASSERT(pdFALSE);
		return pdFALSE;
	}

#if( configLIGHT_IMPACT == 1 )
	pool_t pool = tlsf_get_pool(tlsf);
	tlsf_walk_pool(pool, corrupt_detection_walker, NULL);
#endif /* configLIGHT_IMPACT */

	(void) xTaskResumeAll();

	return pdTRUE;
}
#endif
