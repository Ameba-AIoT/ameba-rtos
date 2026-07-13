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
#include "ameba_soc.h"
#include "os_wrapper_specific.h"
#include "os_wrapper.h"
#include "task.h"
#include "port_tlsf.h"

#include <string.h>
#include "log.h"

#if (CONFIG_HEAP_TRACE == 1)
#include "task_heap_info.h"
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

/* Per-type TLSF instance */
#define MAX_POOLS_PER_TYPE    3

/* Per-type maximum allocation size */
#define TLSF_MAX_SIZE_TCM     ((size_t)64  * 1024)        /* 64KB  */
#define TLSF_MAX_SIZE_SRAM    ((size_t)512 * 1024)        /* 512KB */
#define TLSF_MAX_SIZE_DRAM    ((size_t)64  * 1024 * 1024) /* 64MB, largest PSRAM variant */

#ifdef CONFIG_CA32_FREERTOS_V10_2_1_SMP
#error "CA32 SMP is not supported in FreeRTOS v10.2.1, vTaskSuspendAll() shall replace by taskENTER_CRITICAL()"
#endif

typedef struct {
	tlsf_t handle;
	pool_t pools[MAX_POOLS_PER_TYPE];
	int pool_count;
	size_t xFreeBytesRemaining;
	size_t xMinimumEverFreeBytesRemaining;
} tlsf_inst_t;

static tlsf_inst_t g_tlsf[TYPE_ALL];

static size_t tlsf_max_size_for_type(MALLOC_TYPES type)
{
	switch (type) {
	case TYPE_TCM:
		return TLSF_MAX_SIZE_TCM;
	case TYPE_SRAM:
		return TLSF_MAX_SIZE_SRAM;
	case TYPE_DRAM:
		return TLSF_MAX_SIZE_DRAM;
	default:
		configASSERT(0 && "unknown MALLOC_TYPES");
		return TLSF_MAX_SIZE_SRAM;
	}
}

/* Global allocation/free counters */
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
#if ( CONFIG_HEAP_TRACE == 1 )
task_heap_info_t heap_task_info[ CONFIG_HEAP_TRACE_MAX_TASK_NUMBER ] = {0};
#endif

#if ( CONFIG_HEAP_PROTECTOR == 1 )
/* Canary value for protecting internal heap pointers. */
PRIVILEGED_DATA portPOINTER_SIZE_TYPE xHeapCanary;

/* Highest and lowest heap addresses across all regions. */
PRIVILEGED_DATA uint8_t *pucHeapHighAddress = NULL;
PRIVILEGED_DATA uint8_t *pucHeapLowAddress = NULL;

/**
 * @brief Application provided function to get a random value to be used as canary.
 *
 * @param pxHeapCanary [out] Output parameter to return the canary value.
 */
extern void vApplicationGetRandomHeapCanary(portPOINTER_SIZE_TYPE *pxHeapCanary);
#endif /* CONFIG_HEAP_PROTECTOR */

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
#define xHeadCanaryValue      0xABBA1234
#define xTailCanaryValue      0xBAAD5678
/*
 * The head canary is prepended to every allocation and the user pointer is
 * returned at (block + HEAD_CANARY_SIZE). To avoid breaking the alignment that
 * the underlying tlsf_malloc()/tlsf_memalign() guarantees, HEAD_CANARY_SIZE
 * MUST be a multiple of the strongest alignment we hand out, i.e.
 * portBYTE_CACHE_ALIGNMENT. Using 4 here would turn a 32-byte cache-aligned
 * block (pvPortMallocCacheAligned, used by rtos_mem_zmalloc for WiFi SKB/DMA
 * buffers) into a 32n+4 address -> "skb not align" -> DMA bus fault, and would
 * also break the 8-byte portBYTE_ALIGNMENT contract of pvPortMalloc.
 * Keeping head == tail == portBYTE_CACHE_ALIGNMENT lets vPortFree() recover the
 * block with a single, uniform (pv - HEAD_CANARY_SIZE) for both paths.
 */
#define CANARY_SIZE_BYTES     ((size_t)portBYTE_CACHE_ALIGNMENT)
#define HEAD_CANARY_SIZE       CANARY_SIZE_BYTES
#define TAIL_CANARY_SIZE       CANARY_SIZE_BYTES
#define CANARY_PADDING_WORDS  (CANARY_SIZE_BYTES / sizeof(uint32_t))

#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
#define xFillAlocated   0xCE
#define xFillFreed      0xFE
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

static MALLOC_TYPES classify_addr(uint32_t addr)
{
	if (addr < (uint32_t)SRAM_BASE) {
		return TYPE_TCM;
	}
	if (addr < (uint32_t)PSRAM_BASE) {
		return TYPE_SRAM;
	}
	return TYPE_DRAM;
}

#if (CONFIG_HEAP_TRACE == 1)

static void vPortInsertHeapInfo(size_t xSize)
{
	char *pcTaskName = NULL;
	UBaseType_t x;
	UBaseType_t xInserted = pdFALSE;
	TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();
	if (curTaskHandle != NULL) {
		for (int i = 0; i < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; i++) {
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
			/* Wording/level kept identical to heap_5.c (incl. its "enought" typo). */
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] Warning: CONFIG_HEAP_TRACE_MAX_TASK_NUMBER is not enought, record 0x%08x failed!\n", __func__,
					 (unsigned)(uint32_t)curTaskHandle);
		}
	}
}

static void vPortRemoveHeapInfo(size_t xSize)
{
	UBaseType_t x;
	UBaseType_t xRemoved = pdFALSE;
	TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();

	if (curTaskHandle != NULL) {
		for (x = 0; x < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; x++) {
			if (heap_task_info[ x ].TaskHandle == curTaskHandle) {
				if (heap_task_info[ x ].isExist == pdTRUE) {
					/* A buffer may be allocated by one task and freed by another
					 * (very common: worker frees what a producer allocated), so
					 * this per-task counter can legitimately go negative. Clamp
					 * instead of configASSERT() to keep heap-trace best-effort
					 * and non-fatal (the assert here otherwise halts the system
					 * during normal WiFi bring-up). */
					if (heap_task_info[ x ].heap_size >= xSize) {
						heap_task_info[ x ].heap_size -= xSize;
					} else {
						heap_task_info[ x ].heap_size = 0;
					}
					xRemoved = pdTRUE;
					break;
				} else {
					/* Slot exists but task was marked deleted: freeing memory of a
					 * deleted task. Message aligned to heap_5.c. */
					RTK_LOGS(NOTAG, RTK_LOG_WARN, "[%s] Warning: Free memory which is allocated by deleted task: 0x%08x, size: %u\n", __func__, (unsigned)(uint32_t)curTaskHandle,
							 (unsigned)xSize);
				}
			}
		}
		if (xRemoved == pdFALSE) {
			/* Current task not tracked (table full, or cross-task free where the
			 * freeing task never allocated). TLSF accounts by current task; heap_5.c
			 * accounts by the owner handle stored in each block (see report). */
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "[%s] Warning: Free memory which is allocated by deleted task: 0x%08x, size: %u\n", __func__, (unsigned)(uint32_t)curTaskHandle,
					 (unsigned)xSize);
		}
	}
}
#endif

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
static size_t __attribute__((unused)) xPortCanaryCheck(void *pv)
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
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "HEAD CANARY CORRUPT at 0x%08x. Underrun value: 0x%08x\n", (unsigned)(uint32_t)&pvStartHead[ i ], (unsigned)pvStartHead[ i ]);
			break;
		}

		if (pvStartTail[ i ] != xTailCanaryValue) {
			result = pdFAIL;
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "TAIL CANARY CORRUPT at 0x%08x. Overrun value: 0x%08x\n", (unsigned)(uint32_t)&pvStartTail[ i ], (unsigned)pvStartTail[ i ]);
			break;
		}
	}

	return result;
}
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

static size_t align_up(size_t x, size_t align)
{
	configASSERT(0 == (align & (align - 1)) && "must align to a power of two");
	return (x + (align - 1)) & ~(align - 1);
}

static void free_block_stats_walker(void *ptr, size_t size, int used, void *user)
{
	(void)ptr;

	if (!used) {
		if (user != NULL) {
			tlsf_inst_t *inst = (tlsf_inst_t *)user;
			inst->xFreeBytesRemaining += size;
		}
		xBlocks++;

		if (size > xMaxSize) {
			xMaxSize = size;
		}
		if (xMinSize == 0 || size < xMinSize) {
			xMinSize = size;
		}
	}
}

/*
 * Internal: allocate from a specific type's TLSF instance.
 * Called within vTaskSuspendAll block. Returns user-visible pointer (after canary offset).
 */
static void *pvPortMallocBaseCore(MALLOC_TYPES type, size_t xAlignedSize)
{
	tlsf_inst_t *inst = &g_tlsf[type];

	if (inst->handle == NULL) {
		return NULL;
	}

	if (xAlignedSize > inst->xFreeBytesRemaining) {
		return NULL;
	}

	void *pvReturn = tlsf_malloc(inst->handle, xAlignedSize);
	if (pvReturn == NULL) {
		return NULL;
	}

	size_t xBlockSize = tlsf_block_size(pvReturn);

	inst->xFreeBytesRemaining -= xBlockSize;
	if (inst->xFreeBytesRemaining < inst->xMinimumEverFreeBytesRemaining) {
		inst->xMinimumEverFreeBytesRemaining = inst->xFreeBytesRemaining;
	}

	xNumberOfSuccessfulAllocations++;

#if (CONFIG_HEAP_TRACE == 1)
	vPortInsertHeapInfo(xBlockSize);
#endif

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
	uint32_t *pvHeadCanary = (uint32_t *)pvReturn;

#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
	/* Check if the free blocks content are un-touched */
	size_t xCheckFreedBlockSize = xBlockSize - 3 * sizeof(block_header_t *);
	uint8_t *pxAddress = (uint8_t *) pvHeadCanary + 2 * sizeof(block_header_t *);
	for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
		if (pxAddress[ i ] != xFillFreed) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write to a unallocated Block: 0x%08x. Invalidate value: 0x%08x\n", (unsigned)(uint32_t)&pxAddress[ i ],
					 (unsigned)pxAddress[ i ]);
			configASSERT(pdFALSE);
		}
	}
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

	/* Set the Head Canary Value */
	for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
		*(pvHeadCanary + i) = xHeadCanaryValue;
	}
#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
	/* Fill the allocated address with magic word */
	void *pvBlockToFill = (void *)(((uint8_t *) pvHeadCanary) + HEAD_CANARY_SIZE);
	size_t xFillBlockSize = xBlockSize - TAIL_CANARY_SIZE - HEAD_CANARY_SIZE;
	_memset(pvBlockToFill, xFillAlocated, xFillBlockSize);
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

	uint32_t *pvTailCanary = (uint32_t *)((uint8_t *)pvHeadCanary + xBlockSize - TAIL_CANARY_SIZE);
	for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
		*(pvTailCanary + i) = xTailCanaryValue;
	}

	pvReturn = (void *)((uint8_t *)pvReturn + HEAD_CANARY_SIZE);
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

	return pvReturn;
}

void *pvPortMallocBase(size_t xWantedSize, uint32_t startAddr)
{
#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
	/* Reserve space for Head and Tail Canary */
	xWantedSize = xWantedSize + HEAD_CANARY_SIZE + TAIL_CANARY_SIZE;
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

	size_t xAlignedSize = align_up(xWantedSize, portBYTE_ALIGNMENT);
	MALLOC_TYPES type = (startAddr == 0) ? TYPE_ALL : classify_addr(startAddr);
	void *pvReturn = NULL;

	vTaskSuspendAll();
	{
		if (type == TYPE_ALL) {
			/* Try TYPE_TCM → TYPE_SRAM → TYPE_DRAM */
			for (int t = 0; t < TYPE_ALL; t++) {
				pvReturn = pvPortMallocBaseCore((MALLOC_TYPES)t, xAlignedSize);
				if (pvReturn != NULL) {
					break;
				}
			}
		} else {
			pvReturn = pvPortMallocBaseCore(type, xAlignedSize);
		}
	}
	(void) xTaskResumeAll();

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if (pvReturn == NULL) {
			extern void vApplicationMallocFailedHook(size_t xWantedSize);
			vApplicationMallocFailedHook(xWantedSize);
		}
	}
#endif

	return pvReturn;
}


void *pvPortMalloc(size_t xWantedSize)
{
	return pvPortMallocBase(xWantedSize, 0);
}

void *pvPortCalloc(size_t xWantedCnt, size_t xWantedSize)
{
	void *pvReturn;

	/* Reject overflow: if xWantedCnt * xWantedSize would wrap, bail out. */
	if (xWantedSize != 0 && xWantedCnt > (((size_t) - 1) / xWantedSize)) {
		return NULL;
	}

	pvReturn = pvPortMalloc(xWantedCnt * xWantedSize);
	if (pvReturn != NULL) {
		_memset(pvReturn, 0, xWantedCnt * xWantedSize);
	}

	return pvReturn;
}

void vPortFree(void *pv)
{
#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
	pv = (void *)((uint8_t *)pv - HEAD_CANARY_SIZE);
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

#if( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
	uint8_t *puc = (uint8_t *) pv;
	uint8_t *pucBlockToFree = NULL;
	size_t xBlockToFillSize = 0;
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

	if (pv != NULL) {
#if( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1)
		configASSERT(xPortCanaryCheck(pv));
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

		MALLOC_TYPES type = classify_addr((uint32_t)pv);
		tlsf_inst_t *inst = &g_tlsf[type];

		vTaskSuspendAll();
		{
			size_t xBlockSize = tlsf_block_size(pv);
			inst->xFreeBytesRemaining += xBlockSize;
			xNumberOfSuccessfulFrees++;

#if (CONFIG_HEAP_TRACE == 1)
			vPortRemoveHeapInfo(xBlockSize);
#endif
			tlsf_free(inst->handle, pv);

#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
			/* Fill the freed address with magic word
			* this will overwrotten configHEAP_CLEAR_MEMORY_ON_FREE. */

			/* Check if previous block is free, if ture, fill current block header. */
			size_t xCurBlockSize = xBlockSize;
			if ((*(puc - sizeof(size_t))  & heapBLOCK_PREV_FREE_BITMASK) != 0) {
				pucBlockToFree = puc - sizeof(size_t) - sizeof(block_header_t *);
				xBlockToFillSize = xCurBlockSize + sizeof(size_t);
			} else {
				pucBlockToFree = puc + 2 * sizeof(block_header_t *);
				xBlockToFillSize = xCurBlockSize - 3 * sizeof(block_header_t *);
			}

			_memset(pucBlockToFree, xFillFreed, xBlockToFillSize);

			/* Check if next block is free, if ture, fill next block header. */
			if ((*(puc + xBlockSize)  & heapBLOCK_CURR_FREE_BITMASK) != 0) {
				pucBlockToFree = puc + xCurBlockSize - sizeof(block_header_t *);
				xBlockToFillSize = sizeof(block_header_t);
				_memset(pucBlockToFree, xFillFreed, xBlockToFillSize);
			}

#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */
		}
		(void) xTaskResumeAll();
	}
}

void *pvPortReAllocBase(void *pv, size_t xWantedSize, uint32_t startAddr)
{
	void *pvReturn = NULL;

	if (pv == NULL) {
		return pvPortMallocBase(xWantedSize, startAddr);
	}

	if (xWantedSize == 0) {
		vPortFree(pv);
		return NULL;
	}

	vTaskSuspendAll();
	{
		/* Get old block info before free */
#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
		void *pvBlock = (uint8_t *)pv - HEAD_CANARY_SIZE;
		configASSERT(xPortCanaryCheck(pvBlock));
		size_t xOldBlockSize = tlsf_block_size(pvBlock) - HEAD_CANARY_SIZE - TAIL_CANARY_SIZE;
#else
		void *pvBlock = pv;
		size_t xOldBlockSize = tlsf_block_size(pvBlock);
#endif

		/* Allocate new block, then copy and free old */
		pvReturn = pvPortMallocBase(xWantedSize, startAddr);
		if (pvReturn != NULL) {
			size_t xCopySize = xOldBlockSize < xWantedSize ? xOldBlockSize : xWantedSize;
			_memcpy(pvReturn, pv, xCopySize);
		}
	}
	(void) xTaskResumeAll();

	/* Free old block outside suspend to avoid nesting */
	if (pvReturn != NULL) {
		vPortFree(pv);
	}

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if (pvReturn == NULL) {
			extern void vApplicationMallocFailedHook(size_t xWantedSize);
			vApplicationMallocFailedHook(xWantedSize);
		}
	}
#endif

	return pvReturn;
}

void *pvPortReAlloc(void *pv, size_t xWantedSize)
{
	return pvPortReAllocBase(pv, xWantedSize, 0);
}

void *pvPortMallocCacheAlignedCore(size_t xWantedSize)
{
	void *pvReturn = NULL;

	if (xWantedSize == 0) {
		return NULL;
	}

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
	xWantedSize = xWantedSize + HEAD_CANARY_SIZE + TAIL_CANARY_SIZE;
#endif

	vTaskSuspendAll();
	{
		for (int t = 0; t < TYPE_ALL; t++) {
			tlsf_inst_t *inst = &g_tlsf[t];
			if (inst->handle == NULL) {
				continue;
			}
			pvReturn = tlsf_memalign(inst->handle, portBYTE_CACHE_ALIGNMENT, xWantedSize);
			if (pvReturn != NULL) {
				size_t xBlockSize = tlsf_block_size(pvReturn);
				inst->xFreeBytesRemaining -= xBlockSize;
				if (inst->xFreeBytesRemaining < inst->xMinimumEverFreeBytesRemaining) {
					inst->xMinimumEverFreeBytesRemaining = inst->xFreeBytesRemaining;
				}
				xNumberOfSuccessfulAllocations++;

#if (CONFIG_HEAP_TRACE == 1)
				vPortInsertHeapInfo(xBlockSize);
#endif

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
				uint32_t *pvHeadCanary = (uint32_t *)pvReturn;
				for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
					*(pvHeadCanary + i) = xHeadCanaryValue;
				}
				uint32_t *pvTailCanary = (uint32_t *)((uint8_t *)pvHeadCanary + xBlockSize - TAIL_CANARY_SIZE);
				for (size_t i = 0; i < CANARY_PADDING_WORDS; i++) {
					*(pvTailCanary + i) = xTailCanaryValue;
				}
				pvReturn = (void *)((uint8_t *)pvReturn + HEAD_CANARY_SIZE);
#endif
				break;
			}
		}
	}
	(void) xTaskResumeAll();

	return pvReturn;
}

void *pvPortMallocCacheAligned(size_t xWantedSize)
{
	void *pvReturn = pvPortMallocCacheAlignedCore(xWantedSize);

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if (pvReturn == NULL) {
			extern void vApplicationMallocFailedHook(size_t xWantedSize);
			vApplicationMallocFailedHook(xWantedSize);
		}
	}
#endif /* if ( configUSE_MALLOC_FAILED_HOOK == 1 ) */

	return pvReturn;
}

/*
 * Cache-aligned realloc, mirrors pvPortReAllocBase but allocates the new block
 * with pvPortMallocCacheAligned so the returned pointer stays cache-line
 * aligned. Required by rtos_mem_realloc(). The malloc-failed hook is already
 * invoked inside pvPortMallocCacheAligned on failure.
 */
void *pvPortReAllocCacheAligned(void *pv, size_t xWantedSize)
{
	void *pvReturn = NULL;

	if (pv == NULL) {
		return pvPortMallocCacheAligned(xWantedSize);
	}

	if (xWantedSize == 0) {
		vPortFree(pv);
		return NULL;
	}

	vTaskSuspendAll();
	{
		/* Old usable size (same header/canary offset scheme as regular blocks). */
#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
		void *pvBlock = (uint8_t *)pv - HEAD_CANARY_SIZE;
		configASSERT(xPortCanaryCheck(pvBlock));
		size_t xOldBlockSize = tlsf_block_size(pvBlock) - HEAD_CANARY_SIZE - TAIL_CANARY_SIZE;
#else
		void *pvBlock = pv;
		size_t xOldBlockSize = tlsf_block_size(pvBlock);
#endif

		pvReturn = pvPortMallocCacheAligned(xWantedSize);
		if (pvReturn != NULL) {
			size_t xCopySize = xOldBlockSize < xWantedSize ? xOldBlockSize : xWantedSize;
			_memcpy(pvReturn, pv, xCopySize);
		}
	}
	(void) xTaskResumeAll();

	/* Free old block outside suspend to avoid nesting */
	if (pvReturn != NULL) {
		vPortFree(pv);
	}

	return pvReturn;
}

size_t xPortGetFreeHeapSize(void)
{
	size_t total = 0;
	vTaskSuspendAll();
	{
		for (int t = 0; t < TYPE_ALL; t++) {
			total += g_tlsf[t].xFreeBytesRemaining;
		}
	}
	(void) xTaskResumeAll();
	return total;
}

size_t xPortGetMinimumEverFreeHeapSize(void)
{
	size_t total = 0;
	vTaskSuspendAll();
	{
		for (int t = 0; t < TYPE_ALL; t++) {
			total += g_tlsf[t].xMinimumEverFreeBytesRemaining;
		}
	}
	(void) xTaskResumeAll();
	return total;
}

void xPortResetHeapMinimumEverFreeHeapSize(void)
{
	vTaskSuspendAll();
	{
		for (int t = 0; t < TYPE_ALL; t++) {
			g_tlsf[t].xMinimumEverFreeBytesRemaining = g_tlsf[t].xFreeBytesRemaining;
		}
	}
	(void) xTaskResumeAll();
}

void vPortGetHeapStats(HeapStats_t *pxHeapStats)
{
	int i, t;
	xBlocks = 0;
	xMaxSize = 0;
	xMinSize = portMAX_DELAY;

	vTaskSuspendAll();
	{
		size_t xFreeBytesRemaining = 0;
		size_t xMinimumEverFreeBytesRemaining = 0;
		for (t = 0; t < TYPE_ALL; t++) {
			tlsf_inst_t *inst = &g_tlsf[t];
			for (i = 0; i < inst->pool_count; i++) {
				tlsf_walk_pool(inst->pools[ i ], free_block_stats_walker, NULL);
			}
			xFreeBytesRemaining += inst->xFreeBytesRemaining;
			xMinimumEverFreeBytesRemaining += inst->xMinimumEverFreeBytesRemaining;
		}

		pxHeapStats->xSizeOfLargestFreeBlockInBytes = xMaxSize;
		pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
		pxHeapStats->xNumberOfFreeBlocks = xBlocks;
		pxHeapStats->xAvailableHeapSpaceInBytes = xFreeBytesRemaining;
		pxHeapStats->xNumberOfSuccessfulAllocations = xNumberOfSuccessfulAllocations;
		pxHeapStats->xNumberOfSuccessfulFrees = xNumberOfSuccessfulFrees;
		pxHeapStats->xMinimumEverFreeBytesRemaining = xMinimumEverFreeBytesRemaining;
	}
	(void) xTaskResumeAll();
}

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
/*
 * Malloc-failed hook. When TLSF replaces heap_5.c this definition must live
 * here, because heap_5.c (which used to provide it) is excluded from the build
 * once CONFIG_TLSF_HEAP is selected. Signature matches heap_5.c
 * (size_t xWantedSize) so the failing request size is reported. Uses a global
 * stats buffer to keep the hook's own stack usage small.
 */
static HeapStats_t FailHookHeapStats;
__attribute__((optimize("O0")))
void vApplicationMallocFailedHook(size_t xWantedSize)
{
	char *pcCurrentTask = "NoTsk";
#if defined (CONFIG_ARM_CORE_CM0)
	const char *core_name = "KM0";
#elif defined (CONFIG_ARM_CORE_CM4)
	const char *core_name = "KM4";
#elif defined (CONFIG_RSICV_CORE_KR4)
	const char *core_name = "KR4";
#elif defined (CONFIG_ARM_CORE_CA32)
	const char *core_name = "CA32";
#else
	const char *core_name = "CPU";
#endif

	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		pcCurrentTask = pcTaskGetName(NULL);
	}

	taskENTER_CRITICAL();

	/* 1. Basic info: core / task / requested size. */
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Malloc failed. Core:[%s], Task:[%s], ", core_name, pcCurrentTask);
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[xWantedSize:%u]\r\n", xWantedSize);

	/* 2. Full heap status. Avoid heap_get_stats to reduce stack usage. */
	vPortGetHeapStats(&FailHookHeapStats);
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "AvailHeap: %u, MaxFreeBlock: %u, ",
			 FailHookHeapStats.xAvailableHeapSpaceInBytes,
			 FailHookHeapStats.xSizeOfLargestFreeBlockInBytes);
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Allocs: %u, Frees: %u\r\n",
			 FailHookHeapStats.xNumberOfSuccessfulAllocations,
			 FailHookHeapStats.xNumberOfSuccessfulFrees);
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "MinEverFree: %u\r\n",
			 FailHookHeapStats.xMinimumEverFreeBytesRemaining);

#if ( CONFIG_HEAP_TRACE == 1 )
	/* 3. Task heap useage */
	extern void vPortGetTaskHeapInfo(void);
	vPortGetTaskHeapInfo();

	/* 4. Back trace */
#if defined (CONFIG_ARM_CORE_CM4) || defined (CONFIG_RSICV_CORE_KR4)
	{
		extern void get_call_stack(void **caller, uint32_t max_level);
		void *pc_trace[ CONFIG_HEAP_TRACE_STACK_DEPTH ] = {0};
		get_call_stack(pc_trace, CONFIG_HEAP_TRACE_STACK_DEPTH);
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "********** stack backtrac cmd **********\n");
#if defined (CONFIG_ARM_CORE_CM0) || defined (CONFIG_ARM_CORE_CM4)
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "%s/bin/arm-none-eabi-addr2line -e %s/target_img2.axf -afpiC 0x%08x", SDK_TOOLCHAIN, IMAGE_DIR,
				 (unsigned)(uint32_t)pc_trace[ 0 ]);
#elif defined (CONFIG_RSICV_CORE_KR4)
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "%s/bin/riscv32-elf-addr2line -e %s/target_img2.axf -afpiC 0x%08x", SDK_TOOLCHAIN, IMAGE_DIR, (unsigned)(uint32_t)pc_trace[ 0 ]);
#endif
		for (size_t i = 1; i < CONFIG_HEAP_TRACE_STACK_DEPTH; i++) {
			if (pc_trace[ i ] == NULL) {
				break;
			}
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, " 0x%08x", (unsigned)(uint32_t)pc_trace[ i ]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n");
	}
#endif /* CONFIG_ARM_CORE_CM4 || CONFIG_RSICV_CORE_KR4 */
#endif /* CONFIG_HEAP_TRACE */

	for (;;);
}
#endif /* configUSE_MALLOC_FAILED_HOOK == 1 */

#if (CONFIG_HEAP_TRACE == 1)

void vPortGetTaskHeapInfo(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "********** Each Task Heap Usage **********\n");
	vTaskSuspendAll();

	for (int i = 0; i < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; i++) {
		if (heap_task_info[ i ].TaskHandle != NULL) {
			switch (heap_task_info[ i ].isExist) {
			case pdTRUE:
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Task: %s, task heap usage: 0x%x\n", heap_task_info[ i ].Task_Name, (unsigned)heap_task_info[ i ].heap_size);
				break;
			case pdFALSE:
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Deleted Task: %s, task heap usage: 0x%x\n", heap_task_info[ i ].Task_Name, (unsigned)heap_task_info[ i ].heap_size);
				break;
			default :
				break;
			}
		}
	}

	(void) xTaskResumeAll();
}

#if ( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
static void corrupt_detection_walker(void *ptr, size_t size, int used, void *user)
{
	(void)user;
	(void)ptr;
#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
	size_t xCheckFreedBlockSize = 0;
	uint8_t *pxAddress = NULL;
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

	if (!used) {
#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
		/* Check if the free blocks content are un-touched */
		xCheckFreedBlockSize = size - 3 * sizeof(block_header_t *);
		pxAddress = (uint8_t *) ptr + 2 * sizeof(block_header_t *);
		for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
			if (pxAddress[ i ] != xFillFreed) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write to a unallocated Block: 0x%08x. Invalidate value: 0x%08x\n", (unsigned)(uint32_t)&pxAddress[ i ],
						 (unsigned)pxAddress[ i ]);
				configASSERT(pdFALSE);
			}
		}
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */
	} else {
		(void)size;
		configASSERT(xPortCanaryCheck(ptr));
	}
}
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

/*
 * Signature matches heap_5.c: COMPREHENSIVE_CHECK gates the deep per-block
 * canary / content walk. The structural tlsf_check() always runs (cheap);
 * the boundary/content walk only runs when COMPREHENSIVE_CHECK is set.
 */
uint32_t ulPortCheckHeapIntegrity(int COMPREHENSIVE_CHECK)
{
	int ret;
	int t;

	(void)COMPREHENSIVE_CHECK; /* only consumed when CORRUPTION_DETECT_LITE is on */

	vTaskSuspendAll();

	for (t = 0; t < TYPE_ALL; t++) {
		tlsf_inst_t *inst = &g_tlsf[t];
		if (inst->handle == NULL) {
			continue;
		}

		ret = tlsf_check(inst->handle);
		if (ret != 0) {
			(void)xTaskResumeAll();
			configASSERT(pdFALSE);
			return pdFALSE;
		}

#if( CONFIG_HEAP_CORRUPTION_DETECT_LITE == 1 )
		if (COMPREHENSIVE_CHECK) {
			int i;
			for (i = 0; i < inst->pool_count; i++) {
				tlsf_walk_pool(inst->pools[ i ], corrupt_detection_walker, NULL);
			}
		}
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */
	}

	(void) xTaskResumeAll();

	return pdTRUE;
}
#endif

void vPortDefineHeapRegions(const HeapRegion_t *pxHeapRegions)
{
	int i, t;

#if ( CONFIG_HEAP_PROTECTOR == 1 )
	vApplicationGetRandomHeapCanary(&(xHeapCanary));
#endif

	for (i = 0; pxHeapRegions[ i ].xSizeInBytes > 0; i++) {
		uint32_t addr = (uint32_t)pxHeapRegions[ i ].pucStartAddress;
		MALLOC_TYPES type = classify_addr(addr);
		tlsf_inst_t *inst = &g_tlsf[type];
		size_t max_size = tlsf_max_size_for_type(type);

#if ( CONFIG_HEAP_PROTECTOR == 1 )
		{
			size_t region_size = pxHeapRegions[ i ].xSizeInBytes;
			uint8_t *region_end = (uint8_t *)(addr + region_size);
			if ((pucHeapLowAddress == NULL) || ((uint8_t *)addr < pucHeapLowAddress)) {
				pucHeapLowAddress = (uint8_t *)addr;
			}
			if ((pucHeapHighAddress == NULL) || (region_end > pucHeapHighAddress)) {
				pucHeapHighAddress = region_end;
			}
		}
#endif

		if (inst->pool_count >= MAX_POOLS_PER_TYPE) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "pool overflow for type %d, max %d pools\n", type, MAX_POOLS_PER_TYPE);
			configASSERT(pdFALSE);
			continue;
		}

#if ( CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE == 1 )
		_memset(pxHeapRegions[ i ].pucStartAddress, xFillFreed, pxHeapRegions[ i ].xSizeInBytes);
#endif

		if (inst->handle == NULL) {
			inst->handle = tlsf_create_with_pool_ex(pxHeapRegions[ i ].pucStartAddress, pxHeapRegions[ i ].xSizeInBytes, max_size);
			if (inst->handle != NULL) {
				inst->pools[ inst->pool_count++ ] = tlsf_get_pool_ex(inst->handle, max_size);
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "tlsf_create_with_pool_ex failed for type %d, addr 0x%x, size %d\n", type, addr, pxHeapRegions[ i ].xSizeInBytes);
			}
		} else {
			pool_t pool = tlsf_add_pool(inst->handle, pxHeapRegions[ i ].pucStartAddress, pxHeapRegions[ i ].xSizeInBytes);
			if (pool != NULL) {
				inst->pools[ inst->pool_count++ ] = pool;
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "tlsf_add_pool failed for type %d, addr 0x%x, size %d\n", type, addr, pxHeapRegions[ i ].xSizeInBytes);
			}
		}
	}

	/* Walk each type's pools to count initial free bytes */
	for (t = 0; t < TYPE_ALL; t++) {
		tlsf_inst_t *inst = &g_tlsf[t];
		if (inst->handle == NULL) {
			continue;
		}
		inst->xFreeBytesRemaining = 0;
		for (i = 0; i < inst->pool_count; i++) {
			tlsf_walk_pool(inst->pools[ i ], free_block_stats_walker, inst);
		}
		inst->xMinimumEverFreeBytesRemaining = inst->xFreeBytesRemaining;
	}
}
