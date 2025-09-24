/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * A sample implementation of pvPortMalloc() that allows the heap to be defined
 * across multiple non-contigous blocks and combines (coalescences) adjacent
 * memory blocks as they are freed.
 *
 * See heap_1.c, heap_2.c, heap_3.c and heap_4.c for alternative
 * implementations, and the memory management pages of https://www.FreeRTOS.org
 * for more information.
 *
 * Usage notes:
 *
 * vPortDefineHeapRegions() ***must*** be called before pvPortMalloc().
 * pvPortMalloc() will be called if any task objects (tasks, queues, event
 * groups, etc.) are created, therefore vPortDefineHeapRegions() ***must*** be
 * called before any other objects are defined.
 *
 * vPortDefineHeapRegions() takes a single parameter.  The parameter is an array
 * of HeapRegion_t structures.  HeapRegion_t is defined in portable.h as
 *
 * typedef struct HeapRegion
 * {
 *	uint8_t *pucStartAddress; << Start address of a block of memory that will be part of the heap.
 *	size_t xSizeInBytes;	  << Size of the block of memory.
 * } HeapRegion_t;
 *
 * The array is terminated using a NULL zero sized region definition, and the
 * memory regions defined in the array ***must*** appear in address order from
 * low address to high address.  So the following is a valid example of how
 * to use the function.
 *
 * HeapRegion_t xHeapRegions[] =
 * {
 *  { ( uint8_t * ) 0x80000000UL, 0x10000 }, << Defines a block of 0x10000 bytes starting at address 0x80000000
 *  { ( uint8_t * ) 0x90000000UL, 0xa0000 }, << Defines a block of 0xa0000 bytes starting at address of 0x90000000
 *  { NULL, 0 }                << Terminates the array.
 * };
 *
 * vPortDefineHeapRegions( xHeapRegions ); << Pass the array into vPortDefineHeapRegions().
 *
 * Note 0x80000000 is the lower address so appears in the array first.
 *
 */
#include <string.h>
#include <stdlib.h>
#include "ameba.h"

#if defined (CONFIG_HEAP_PROTECTOR) && (CONFIG_HEAP_PROTECTOR== 1)
#include "heap_trace.h"
#endif

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#ifdef CONFIG_HEAP_TRACE
#if defined CONFIG_ARM_CORE_CM4
#include "ameba_v8m_backtrace.h"
#elif defined CONFIG_RSICV_CORE_KR4
#include "ameba_rv32_backtrace.h"
#endif
#endif

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE    ( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE         ( ( size_t ) 8 )

/* Define the linked list structure.  This is used to link free blocks in order
 * of their memory address. */
typedef struct A_BLOCK_LINK {
	struct A_BLOCK_LINK *pxNextFreeBlock;  /*<< The next free block in the list. */
	size_t xBlockSize;                     /*<< The size of the free block. */
#ifdef CONFIG_HEAP_TRACE
	struct tskTaskControlBlock *TaskHandle;/*<< The owner of this block. */
#endif
} BlockLink_t;

/* Check if the subtraction operation ( a - b ) will result in underflow. */
#define heapSUBTRACT_WILL_UNDERFLOW( a, b )    ( ( a ) < ( b ) )


#if ( defined CONFIG_HEAP_PROTECTOR )

/* Canary value for protecting internal heap pointers. */
PRIVILEGED_DATA static portPOINTER_SIZE_TYPE xHeapCanary;

/* Highest and lowest heap addresses used for heap block bounds checking. */
PRIVILEGED_DATA static uint8_t *pucHeapHighAddress = NULL;
PRIVILEGED_DATA static uint8_t *pucHeapLowAddress = NULL;

#endif /* CONFIG_HEAP_PROTECTOR */

/* Setting CONFIG_HEAP_PROTECTOR to 1 enables heap block pointers
 * protection using an application supplied canary value to catch heap
 * corruption should a heap buffer overflow occur.
 */
#if ( defined CONFIG_HEAP_PROTECTOR )

/* Macro to load/store BlockLink_t pointers to memory. By XORing the
 * pointers with a random canary value, heap overflows will result
 * in randomly unpredictable pointer values which will be caught by
 * heapVALIDATE_BLOCK_POINTER assert. */
#define heapPROTECT_BLOCK_POINTER( pxBlock )    ( ( BlockLink_t * ) ( ( ( portPOINTER_SIZE_TYPE ) ( pxBlock ) ) ^ xHeapCanary ) )

/* Assert that a heap block pointer is within the heap bounds. */
#define heapVALIDATE_BLOCK_POINTER( pxBlock )                       \
    configASSERT( ( pucHeapHighAddress != NULL ) &&                     \
                  ( pucHeapLowAddress != NULL ) &&                      \
                  ( ( uint8_t * ) ( pxBlock ) >= pucHeapLowAddress ) && \
                  ( ( uint8_t * ) ( pxBlock ) < pucHeapHighAddress ) )

#else /* if ( defined CONFIG_HEAP_PROTECTOR ) */

#define heapPROTECT_BLOCK_POINTER( pxBlock )    ( pxBlock )

#define heapVALIDATE_BLOCK_POINTER( pxBlock )

#endif /* CONFIG_HEAP_PROTECTOR */

#if ( defined CONFIG_HEAP_PROTECTOR )

/**
 * @brief Application provided function to get a random value to be used as canary.
 *
 * @param pxHeapCanary [out] Output parameter to return the canary value.
 */
extern void vApplicationGetRandomHeapCanary(portPOINTER_SIZE_TYPE *pxHeapCanary);
#endif /* CONFIG_HEAP_PROTECTOR */
/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert);

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
 * block must by correctly byte aligned. */
 const size_t xHeapStructSize = (sizeof(BlockLink_t) + ((size_t)(portBYTE_ALIGNMENT - 1))) & ~((size_t) portBYTE_ALIGNMENT_MASK);

#if ( (defined CONFIG_HEAP_PROTECTOR ) && ( defined CONFIG_HEAP_CORRUPTION_DETECT_LITE ) )
#define xHeadCanaryValue 0xDEADBEEF
#define xTailCanaryValue 0xCAFEBABE

const size_t xHeadCanarySize = portBYTE_ALIGNMENT;
const size_t xTailCanarySize = portBYTE_ALIGNMENT;
const size_t xNumPadding = portBYTE_ALIGNMENT / sizeof(uint32_t);

#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE )
#define xFillAlocated   0xCC
#define xFillFreed      0xDD
#endif /* CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE */

#endif /* CONFIG_HEAP_CORRUPTION_DETECT_LITE */

#ifdef CONFIG_HEAP_TRACE
#include "task_heap_info.h"
task_heap_info_t heap_task_info[ CONFIG_HEAP_TRACE_MAX_TASK_NUMBER ] = {0};
#endif

/*-----------------------------------------------------------*/

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, * pxEnd = NULL;

/* Keeps track of the number of calls to allocate and free memory as well as the
 * number of free bytes remaining, but says nothing about fragmentation. */
static size_t xFreeBytesRemaining = 0U;
static size_t xMinimumEverFreeBytesRemaining = 0U;
static size_t xNumberOfSuccessfulAllocations = 0;
static size_t xNumberOfSuccessfulFrees = 0;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
 * member of an BlockLink_t structure is set then the block belongs to the
 * application.  When the bit is free the block is still part of the free heap
 * space. */
static size_t xBlockAllocatedBit = 0;

#ifdef CONFIG_HEAP_TRACE
static void vPortInsertHeapInfo(BlockLink_t *pxBlock)
{
	UBaseType_t x;
	UBaseType_t xInserted = pdFALSE;
	char *pcTaskName = NULL;
	UBaseType_t xBlockSize = pxBlock->xBlockSize;
	configASSERT((xBlockSize & xBlockAllocatedBit) != 0);
	xBlockSize &= ~xBlockAllocatedBit;
	if (pxBlock->TaskHandle != NULL) {
		for (int i = 0; i < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; i++) {
			if (heap_task_info[i].TaskHandle == NULL) {
				heap_task_info[i].TaskHandle = pxBlock->TaskHandle;
				pcTaskName = pcTaskGetName(pxBlock->TaskHandle);
				heap_task_info[i].heap_size = xBlockSize;
				heap_task_info[i].isExist = pdTRUE;

				for (x = (UBaseType_t) 0; x < (UBaseType_t) configMAX_TASK_NAME_LEN; x++) {
					heap_task_info[i].Task_Name[ x ] = pcTaskName[ x ];

					/* Don't copy all configMAX_TASK_NAME_LEN if the string is shorter than
					* configMAX_TASK_NAME_LEN characters just in case the memory after the
					* string is not accessible (extremely unlikely). */
					if (pcTaskName[ x ] == (char) 0x00) {
						break;
					}
				}
				xInserted = pdTRUE;
				break;
			} else if ((heap_task_info[i].TaskHandle == pxBlock->TaskHandle) && (heap_task_info[i].isExist == pdTRUE)) {
				heap_task_info[i].heap_size += xBlockSize;
				xInserted = pdTRUE;
				break;
			}
		}

		if (xInserted == pdFALSE) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] Warning: CONFIG_HEAP_TRACE_MAX_TASK_NUMBER is not enought, record 0x%08x failed!\n", __func__, pxBlock);
		}
	}
}

static void vPortRemoveHeapInfo( BlockLink_t *pxBlock )
{
    UBaseType_t x;
    UBaseType_t xRemoved = pdFALSE;
    char * pcTaskName = NULL;

    if( pxBlock->TaskHandle != NULL )
    {
        /* The task name calling the free */
        pcTaskName = pcTaskGetName( pxBlock->TaskHandle );
        configASSERT( ( pxBlock->xBlockSize & xBlockAllocatedBit ) == 0 );
        for( x = 0; x < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; x++)
        {
			if ( (heap_task_info[x].TaskHandle == pxBlock->TaskHandle) )
            {
                /* The task name stored in heap_task_info equals to the task name calling the free,
                 * update the log
                 */
				if(strcmp(pcTaskName,heap_task_info[x].Task_Name) == 0)
				{
                    configASSERT( heap_task_info[x].heap_size >= pxBlock->xBlockSize );
                    heap_task_info[x].heap_size -= pxBlock->xBlockSize;
                    xRemoved = pdTRUE;
                    break;
                }
            }
        }
        if ( xRemoved == pdFALSE )
        {
            RTK_LOGS(NOTAG, RTK_LOG_WARN, "[%s] Warning: Free memory which is allocated by deleted task: 0x%08x, size: %u\n", __func__, pxBlock->TaskHandle, pxBlock->xBlockSize);
        }
    }
}
#endif

#if ( (defined CONFIG_HEAP_PROTECTOR ) && ( defined CONFIG_HEAP_CORRUPTION_DETECT_LITE ) )
static size_t xPortCanaryCheck(BlockLink_t *pxBlock)
{
	size_t xCurBlockSize = pxBlock->xBlockSize;
	xCurBlockSize &= ~xBlockAllocatedBit;
	size_t xTailCanaryOffset = xCurBlockSize - xTailCanarySize;
	uint32_t *pvStartHead = (uint32_t *)((uint8_t *) pxBlock + xHeapStructSize);
	uint32_t *pvStartTail = (uint32_t *)((uint8_t *) pxBlock + xTailCanaryOffset);
	size_t result = pdTRUE;

	for (size_t i = 0; i < xNumPadding; i++) {
		if (pvStartHead[i] != xHeadCanaryValue) {
			result = pdFALSE;
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "HEAD CANARY CORRUPT at 0x%08x. Underrun value: 0x%08x\n", &pvStartHead[i], pvStartHead[i]);
			break;
		}

		if (pvStartTail[i] != xTailCanaryValue) {
			result = pdFAIL;
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "TAIL CANARY CORRUPT at 0x%08x. Overrun value: 0x%08x\n", &pvStartTail[i], pvStartTail[i]);
			break;
		}
	}

	return result;
}
#endif
/*-----------------------------------------------------------*/

#if ( ( !defined CONFIG_HEAP_CORRUPTION_DETECT_LITE ) )
void *pvPortMallocBase(size_t xWantedSize, uint32_t startAddr)
{
	BlockLink_t *pxBlock, * pxPreviousBlock, * pxNewBlockLink;
	void *pvReturn = NULL;

	/* The heap must be initialised before the first call to
	 * prvPortMalloc(). */
	configASSERT(pxEnd);

	vTaskSuspendAll();
	{
		/* Check the requested block size is not so large that the top bit is
		 * set.  The top bit of the block size member of the BlockLink_t structure
		 * is used to determine who owns the block - the application or the
		 * kernel, so it must be free. */
		if ((xWantedSize & xBlockAllocatedBit) == 0) {
			/* The wanted size is increased so it can contain a BlockLink_t
			 * structure in addition to the requested amount of bytes. */
			if ((xWantedSize > 0) &&
				((xWantedSize + xHeapStructSize) >  xWantedSize)) {    /* Overflow check */
				xWantedSize += xHeapStructSize;

				/* Ensure that blocks are always aligned */
				if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00) {
					/* Byte alignment required. Check for overflow */
					if ((xWantedSize + (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK))) >
						xWantedSize) {
						xWantedSize += (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK));
					} else {
						xWantedSize = 0;
					}
				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				xWantedSize = 0;
			}

			if ((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining)) {
				/* Traverse the list from the start	(lowest address) block until
				 * one of adequate size is found. */
				pxPreviousBlock = &xStart;
				pxBlock = heapPROTECT_BLOCK_POINTER(xStart.pxNextFreeBlock);
				heapVALIDATE_BLOCK_POINTER(pxBlock);

				while (((pxBlock->xBlockSize < xWantedSize) || ((uint32_t)pxBlock + xHeapStructSize) < startAddr ) && (pxBlock->pxNextFreeBlock != heapPROTECT_BLOCK_POINTER(NULL))) {
					pxPreviousBlock = pxBlock;
					pxBlock = heapPROTECT_BLOCK_POINTER(pxBlock->pxNextFreeBlock);
					heapVALIDATE_BLOCK_POINTER(pxBlock);
				}

				/* If the end marker was reached then a block of adequate size
				 * was not found. */
				if (pxBlock != pxEnd) {
#ifdef CONFIG_HEAP_TRACE
					/* Record the TaskHandle in the block header */
					TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();
					pxBlock->TaskHandle = curTaskHandle;
#endif

					/* Return the memory space pointed to - jumping over the
					 * BlockLink_t structure at its start. */
					pvReturn = (void *)(((uint8_t *) heapPROTECT_BLOCK_POINTER(pxPreviousBlock->pxNextFreeBlock)) + xHeapStructSize);
					heapVALIDATE_BLOCK_POINTER(pvReturn);

					/* This block is being returned for use so must be taken out
					 * of the list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

					/* If the block is larger than required it can be split into
					 * two. */
					configASSERT(heapSUBTRACT_WILL_UNDERFLOW(pxBlock->xBlockSize, xWantedSize) == 0);

					if ((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE) {
						/* This block is to be split into two.  Create a new
						 * block following the number of bytes requested. The void
						 * cast is used to prevent byte alignment warnings from the
						 * compiler. */
						pxNewBlockLink = (void *)(((uint8_t *) pxBlock) + xWantedSize);
						configASSERT((((size_t) pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

						/* Calculate the sizes of two blocks split from the
						 * single block. */
						pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
						pxBlock->xBlockSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						pxNewBlockLink->pxNextFreeBlock = pxPreviousBlock->pxNextFreeBlock;
						pxPreviousBlock->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxNewBlockLink);
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					xFreeBytesRemaining -= pxBlock->xBlockSize;

					if (xFreeBytesRemaining < xMinimumEverFreeBytesRemaining) {
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					/* The block is being returned - it is allocated and owned
					 * by the application and has no "next" block. */
					pxBlock->xBlockSize |= xBlockAllocatedBit;
					pxBlock->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(NULL);
					xNumberOfSuccessfulAllocations++;
#ifdef CONFIG_HEAP_TRACE
					vPortInsertHeapInfo(pxBlock);
#endif
				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		}

		traceMALLOC(pvReturn, xWantedSize);
	}
	(void) xTaskResumeAll();

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if (pvReturn == NULL) {
			extern void vApplicationMallocFailedHook(size_t xWantedSize);
			vApplicationMallocFailedHook(xWantedSize);
		} else {
			mtCOVERAGE_TEST_MARKER();
		}
	}
#endif /* if ( configUSE_MALLOC_FAILED_HOOK == 1 ) */

	configASSERT((((size_t) pvReturn) & (size_t) portBYTE_ALIGNMENT_MASK) == 0);
	return pvReturn;
}

#else /* !defined CONFIG_HEAP_CORRUPTION_DETECT_LITE */
void *pvPortMallocBase(size_t xWantedSize, uint32_t startAddr)
{
	BlockLink_t *pxBlock, * pxPreviousBlock, * pxNewBlockLink;
	void *pvReturn = NULL;
	uint32_t *pvHeadCanary = NULL;
	uint32_t *pvTailCanary = NULL;
#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE )
	void *pvBlockToFill = NULL;
	size_t xFillBlockSize = 0;
	size_t xCheckFreedBlockSize = 0;
#endif

	/* The heap must be initialised before the first call to
	 * prvPortMalloc(). */
	configASSERT(pxEnd);

	size_t xBlockAdditionSize = xHeapStructSize + xHeadCanarySize + xTailCanarySize;

	vTaskSuspendAll();
	{
		/* Check the requested block size is not so large that the top bit is
		 * set.  The top bit of the block size member of the BlockLink_t structure
		 * is used to determine who owns the block - the application or the
		 * kernel, so it must be free. */
		if ((xWantedSize & xBlockAllocatedBit) == 0) {
			/* The wanted size is increased so it can contain a BlockLink_t
			 * structure in addition to the requested amount of bytes. */
			if ((xWantedSize > 0) &&
				((xWantedSize + xBlockAdditionSize) >  xWantedSize)) {    /* Overflow check */
				xWantedSize += xBlockAdditionSize;

				/* Ensure that blocks are always aligned */
				if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00) {
					/* Byte alignment required. Check for overflow */
					if ((xWantedSize + (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK))) >
						xWantedSize) {
						xWantedSize += (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK));
					} else {
						xWantedSize = 0;
					}
				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				xWantedSize = 0;
			}

			if ((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining)) {
				/* Traverse the list from the start	(lowest address) block until
				 * one of adequate size is found. */
				pxPreviousBlock = &xStart;
				pxBlock = heapPROTECT_BLOCK_POINTER(xStart.pxNextFreeBlock);
				heapVALIDATE_BLOCK_POINTER(pxBlock);

				while (((pxBlock->xBlockSize < xWantedSize) || ((uint32_t)pxBlock + xHeapStructSize) < startAddr ) && (pxBlock->pxNextFreeBlock != heapPROTECT_BLOCK_POINTER(NULL))) {
					pxPreviousBlock = pxBlock;
					pxBlock = heapPROTECT_BLOCK_POINTER(pxBlock->pxNextFreeBlock);
					heapVALIDATE_BLOCK_POINTER(pxBlock);
				}

				/* If the end marker was reached then a block of adequate size
				 * was not found. */
				if (pxBlock != pxEnd) {
#ifdef CONFIG_HEAP_TRACE
					/* Record the TaskHandle in the block header */
					TaskHandle_t curTaskHandle = xTaskGetCurrentTaskHandle();
					pxBlock->TaskHandle = curTaskHandle;
#endif

					/* Return the memory space pointed to - jumping over the
					 * BlockLink_t structure at its start. */
					pvReturn = (void *)(((uint8_t *) heapPROTECT_BLOCK_POINTER(pxPreviousBlock->pxNextFreeBlock)) + xHeapStructSize + xHeadCanarySize);
					heapVALIDATE_BLOCK_POINTER(pvReturn);

					pvHeadCanary = (uint32_t *)(((uint8_t *) pxBlock) + xHeapStructSize);
#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
					xCheckFreedBlockSize = xWantedSize - xHeapStructSize;
					uint8_t *pxAddress = (uint8_t *) pvHeadCanary;
					for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
						if (pxAddress[i] != xFillFreed) {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write to a unallocated Block: 0x%08x. Invalidate value: 0x%08x\n", &pxAddress[i], pxAddress[i]);
							configASSERT(pdFALSE);
						}
					}
#endif
					for (size_t i = 0; i < xNumPadding; i++) {
						*(pvHeadCanary + i) = xHeadCanaryValue;
					}
#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
					pvBlockToFill = (void *)(((uint8_t *) pvHeadCanary) + xHeadCanarySize);
					xFillBlockSize = xWantedSize - xHeapStructSize - xTailCanarySize - xHeadCanarySize;
					_memset(pvBlockToFill, xFillAlocated, xFillBlockSize);
#endif
					/* This block is being returned for use so must be taken out
					 * of the list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

					/* If the block is larger than required it can be split into
					 * two. */
					configASSERT(heapSUBTRACT_WILL_UNDERFLOW(pxBlock->xBlockSize, xWantedSize) == 0);

					if ((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE) {
						/* This block is to be split into two.  Create a new
						 * block following the number of bytes requested. The void
						 * cast is used to prevent byte alignment warnings from the
						 * compiler. */
						pxNewBlockLink = (void *)(((uint8_t *) pxBlock) + xWantedSize);
						configASSERT((((size_t) pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

						/* Calculate the sizes of two blocks split from the
						 * single block. */
						pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
						pxBlock->xBlockSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						pxNewBlockLink->pxNextFreeBlock = pxPreviousBlock->pxNextFreeBlock;
						pxPreviousBlock->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxNewBlockLink);
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					pvTailCanary = (uint32_t *)((uint8_t *)pxBlock + pxBlock->xBlockSize - xTailCanarySize);
					for (size_t i = 0; i < xNumPadding; i++) {
						*(pvTailCanary + i) = xTailCanaryValue;
					}

					xFreeBytesRemaining -= pxBlock->xBlockSize;

					if (xFreeBytesRemaining < xMinimumEverFreeBytesRemaining) {
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					/* The block is being returned - it is allocated and owned
					 * by the application and has no "next" block. */
					pxBlock->xBlockSize |= xBlockAllocatedBit;
					pxBlock->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(NULL);
					xNumberOfSuccessfulAllocations++;
#ifdef CONFIG_HEAP_TRACE
					vPortInsertHeapInfo(pxBlock);
#endif

				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		}

		traceMALLOC(pvReturn, xWantedSize);
	}
	(void) xTaskResumeAll();

#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if (pvReturn == NULL) {
			extern void vApplicationMallocFailedHook(size_t xWantedSize);
			vApplicationMallocFailedHook(xWantedSize);
		} else {
			mtCOVERAGE_TEST_MARKER();
		}
	}
#endif /* if ( configUSE_MALLOC_FAILED_HOOK == 1 ) */

	configASSERT((((size_t) pvReturn) & (size_t) portBYTE_ALIGNMENT_MASK) == 0);
	return pvReturn;
}
#endif /* defined CONFIG_HEAP_CORRUPTION_DETECT_LITE */

void *pvPortMalloc(size_t xWantedSize)
{
	return pvPortMallocBase(xWantedSize, 0);
}

/*-----------------------------------------------------------*/

#if (defined CONFIG_HEAP_CORRUPTION_DETECT_LITE)
void vPortFree(void *pv)
{
	uint8_t *puc = (uint8_t *) pv;
	BlockLink_t *pxLink;

#if( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE )
	uint8_t *pucBlockToFree = NULL;
	size_t xBlockToFillSize = 0;
#endif

	if (pv != NULL) {
		/* The memory being freed will have an BlockLink_t structure immediately
		 * before it. */
		puc -= (xHeapStructSize + xHeadCanarySize);

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = (void *) puc;

		heapVALIDATE_BLOCK_POINTER(pxLink);

		/* Check the block is actually allocated. */
		configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
		configASSERT(pxLink->pxNextFreeBlock == heapPROTECT_BLOCK_POINTER(NULL));

		if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
			if (pxLink->pxNextFreeBlock == heapPROTECT_BLOCK_POINTER(NULL)) {
				/* The block is being returned to the heap - it is no longer
				 * allocated. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;

#if( defined CONFIG_HEAP_CORRUPTION_DETECT_LITE)
				configASSERT(xPortCanaryCheck(pxLink));
#endif

#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
				pucBlockToFree = puc + xHeapStructSize;
				xBlockToFillSize = pxLink->xBlockSize - xHeapStructSize;
				_memset(pucBlockToFree, xFillFreed, xBlockToFillSize);
#endif

				vTaskSuspendAll();
				{
					/* Add this block to the list of free blocks. */
					xFreeBytesRemaining += pxLink->xBlockSize;
					traceFREE(pv, pxLink->xBlockSize);
#ifdef CONFIG_HEAP_TRACE
					vPortRemoveHeapInfo(pxLink);
#endif
					prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
					xNumberOfSuccessfulFrees++;
				}
				(void) xTaskResumeAll();
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		}
	}
}
#else /* defined CONFIG_HEAP_CORRUPTION_DETECT_LITE */
void vPortFree(void *pv)
{
	uint8_t *puc = (uint8_t *) pv;
	BlockLink_t *pxLink;

	if (pv != NULL) {
		/* The memory being freed will have an BlockLink_t structure immediately
		 * before it. */
		puc -= xHeapStructSize;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = (void *) puc;

		heapVALIDATE_BLOCK_POINTER(pxLink);
		/* Check the block is actually allocated. */
		configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
		configASSERT(pxLink->pxNextFreeBlock == heapPROTECT_BLOCK_POINTER(NULL));

		if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
			if (pxLink->pxNextFreeBlock == heapPROTECT_BLOCK_POINTER(NULL)) {
				/* The block is being returned to the heap - it is no longer
				 * allocated. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;

				vTaskSuspendAll();
				{
					/* Add this block to the list of free blocks. */
					xFreeBytesRemaining += pxLink->xBlockSize;
					traceFREE(pv, pxLink->xBlockSize);
#ifdef CONFIG_HEAP_TRACE
					vPortRemoveHeapInfo(pxLink);
#endif
					prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
					xNumberOfSuccessfulFrees++;
				}
				(void) xTaskResumeAll();
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		}
	}
}
#endif /* !defined CONFIG_HEAP_CORRUPTION_DETECT_LITE */
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize(void)
{
	return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSize(void)
{
	return xMinimumEverFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

void xPortResetHeapMinimumEverFreeHeapSize(void)
{
    taskENTER_CRITICAL();
    {
        xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
    }
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert)
{
	BlockLink_t *pxIterator;
	uint8_t *puc;

	/* Iterate through the list until a block is found that has a higher address
	 * than the block being inserted. */
	for (pxIterator = &xStart; heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock) < pxBlockToInsert;
		 pxIterator = heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock)) {
		/* Nothing to do here, just iterate to the right position. */
	}

	if (pxIterator != &xStart) {
		heapVALIDATE_BLOCK_POINTER(pxIterator);
	}

	/* Do the block being inserted, and the block it is being inserted after
	 * make a contiguous block of memory? */
	puc = (uint8_t *) pxIterator;

	if ((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert) {
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
#if( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE )
		_memset(pxBlockToInsert, xFillFreed, xHeapStructSize);
#endif
		pxBlockToInsert = pxIterator;
	} else {
		mtCOVERAGE_TEST_MARKER();
	}

	/* Do the block being inserted, and the block it is being inserted before
	 * make a contiguous block of memory? */
	puc = (uint8_t *) pxBlockToInsert;

	if ((puc + pxBlockToInsert->xBlockSize) == (uint8_t *)heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock)) {
		if (heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock) != pxEnd) {
			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize += heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock)->xBlockSize;
#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
			BlockLink_t *pxHeadStuct = heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock);
#endif
			pxBlockToInsert->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxIterator->pxNextFreeBlock)->pxNextFreeBlock;
#if( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE )
			_memset((void *) pxHeadStuct, xFillFreed, xHeapStructSize);
#endif
		} else {
			pxBlockToInsert->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxEnd);
		}
	} else {
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	 * before and the block after, then it's pxNextFreeBlock pointer will have
	 * already been set, and should not be set here as that would make it point
	 * to itself. */
	if (pxIterator != pxBlockToInsert) {
		pxIterator->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxBlockToInsert);
	} else {
		mtCOVERAGE_TEST_MARKER();
	}
}
/*-----------------------------------------------------------*/

void vPortDefineHeapRegions(const HeapRegion_t *const pxHeapRegions)
{
	BlockLink_t *pxFirstFreeBlockInRegion = NULL, * pxPreviousFreeBlock;
	size_t xAlignedHeap;
	size_t xTotalRegionSize, xTotalHeapSize = 0;
	BaseType_t xDefinedRegions = 0;
	size_t xAddress;
	const HeapRegion_t *pxHeapRegion;

	/* Can only call once! */
	configASSERT(pxEnd == NULL);

#if ( defined CONFIG_HEAP_PROTECTOR )
	{
		vApplicationGetRandomHeapCanary(&(xHeapCanary));
	}
#endif

	pxHeapRegion = &(pxHeapRegions[ xDefinedRegions ]);

	while (pxHeapRegion->xSizeInBytes > 0) {
		xTotalRegionSize = pxHeapRegion->xSizeInBytes;

		/* Ensure the heap region starts on a correctly aligned boundary. */
		xAddress = (size_t) pxHeapRegion->pucStartAddress;

		if ((xAddress & portBYTE_ALIGNMENT_MASK) != 0) {
			xAddress += (portBYTE_ALIGNMENT - 1);
			xAddress &= ~portBYTE_ALIGNMENT_MASK;

			/* Adjust the size for the bytes lost to alignment. */
			xTotalRegionSize -= xAddress - (size_t) pxHeapRegion->pucStartAddress;
		}

		xAlignedHeap = xAddress;

		/* Set xStart if it has not already been set. */
		if (xDefinedRegions == 0) {
			/* xStart is used to hold a pointer to the first item in the list of
			 *  free blocks.  The void cast is used to prevent compiler warnings. */
			xStart.pxNextFreeBlock = (BlockLink_t *) heapPROTECT_BLOCK_POINTER(xAlignedHeap);
			xStart.xBlockSize = (size_t) 0;
		} else {
			/* Should only get here if one region has already been added to the
			 * heap. */
			configASSERT(pxEnd != heapPROTECT_BLOCK_POINTER(NULL));

			/* Check blocks are passed in with increasing start addresses. */
			configASSERT(xAddress > (size_t) pxEnd);
		}

#if ( defined CONFIG_HEAP_PROTECTOR )
		{
			if ((pucHeapLowAddress == NULL) ||
				((uint8_t *) xAlignedHeap < pucHeapLowAddress)) {
				pucHeapLowAddress = (uint8_t *) xAlignedHeap;
			}
		}
#endif /* CONFIG_HEAP_PROTECTOR */

		/* Remember the location of the end marker in the previous region, if
		 * any. */
		pxPreviousFreeBlock = pxEnd;

		/* pxEnd is used to mark the end of the list of free blocks and is
		 * inserted at the end of the region space. */
		xAddress = xAlignedHeap + xTotalRegionSize;
		xAddress -= xHeapStructSize;
		xAddress &= ~portBYTE_ALIGNMENT_MASK;
		pxEnd = (BlockLink_t *) xAddress;
		pxEnd->xBlockSize = 0;
		pxEnd->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(NULL);

		/* To start with there is a single free block in this region that is
		 * sized to take up the entire heap region minus the space taken by the
		 * free block structure. */
		pxFirstFreeBlockInRegion = (BlockLink_t *) xAlignedHeap;
		pxFirstFreeBlockInRegion->xBlockSize = xAddress - (size_t) pxFirstFreeBlockInRegion;
		pxFirstFreeBlockInRegion->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxEnd);

#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
		size_t pxBlockTofill = xAlignedHeap + xHeapStructSize;
		size_t xBlockTofileSize = pxFirstFreeBlockInRegion->xBlockSize - xHeapStructSize;
		_memset((void *) pxBlockTofill, xFillFreed, xBlockTofileSize);
#endif
		/* If this is not the first region that makes up the entire heap space
		 * then link the previous region to this region. */
		if (pxPreviousFreeBlock != NULL) {
			pxPreviousFreeBlock->pxNextFreeBlock = heapPROTECT_BLOCK_POINTER(pxFirstFreeBlockInRegion);
		}

		xTotalHeapSize += pxFirstFreeBlockInRegion->xBlockSize;

#if ( defined CONFIG_HEAP_PROTECTOR )
		{
			if ((pucHeapHighAddress == NULL) ||
				((((uint8_t *) pxFirstFreeBlockInRegion) + pxFirstFreeBlockInRegion->xBlockSize) > pucHeapHighAddress)) {
				pucHeapHighAddress = ((uint8_t *) pxFirstFreeBlockInRegion) + pxFirstFreeBlockInRegion->xBlockSize;
			}
		}
#endif

		/* Move onto the next HeapRegion_t structure. */
		xDefinedRegions++;
		pxHeapRegion = &(pxHeapRegions[ xDefinedRegions ]);
	}

	xMinimumEverFreeBytesRemaining = xTotalHeapSize;
	xFreeBytesRemaining = xTotalHeapSize;

	/* Check something was actually defined before it is accessed. */
	configASSERT(xTotalHeapSize);

	/* Work out the position of the top bit in a size_t variable. */
	xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}
/*-----------------------------------------------------------*/

void vPortGetHeapStats(HeapStats_t *pxHeapStats)
{
	BlockLink_t *pxBlock;
	size_t xBlocks = 0, xMaxSize = 0, xMinSize = portMAX_DELAY; /* portMAX_DELAY used as a portable way of getting the maximum value. */

	vTaskSuspendAll();
	{
		pxBlock = heapPROTECT_BLOCK_POINTER(xStart.pxNextFreeBlock);

		/* pxBlock will be NULL if the heap has not been initialised.  The heap
		 * is initialised automatically when the first allocation is made. */
		if (pxBlock != NULL) {
			do {
				/* Increment the number of blocks and record the largest block seen
				 * so far. */
				xBlocks++;

				if (pxBlock->xBlockSize > xMaxSize) {
					xMaxSize = pxBlock->xBlockSize;
				}

				/* Heap five will have a zero sized block at the end of each
				 * each region - the block is only used to link to the next
				 * heap region so it not a real block. */
				if (pxBlock->xBlockSize != 0) {
					if (pxBlock->xBlockSize < xMinSize) {
						xMinSize = pxBlock->xBlockSize;
					}
				}

				/* Move to the next block in the chain until the last block is
				 * reached. */
				pxBlock = heapPROTECT_BLOCK_POINTER(pxBlock->pxNextFreeBlock);
			} while (pxBlock != pxEnd);
		}
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

#if ( defined CONFIG_HEAP_PROTECTOR )
extern HeapRegion_t xHeapRegions[];
uint32_t ulPortCheckHeapIntegrity(int COMPREHENSIVE_CHECK)
{
	BlockLink_t *pxBlockToCheck;
	uint8_t *pucAlignedHeap;
	size_t xAddress, xEndAddress;
	size_t xTotalRegionSize, xCurBlockSize;

	/* Count from the first heap region */
	const HeapRegion_t *pxHeapRegion;
	BaseType_t xDefinedRegions = 0;
	pxHeapRegion = &(xHeapRegions[ xDefinedRegions ]);

	vTaskSuspendAll();

	while (pxHeapRegion->xSizeInBytes > 0) {
		/* Check the last block of the previous region. */
		if (xDefinedRegions != 0) {
			heapVALIDATE_BLOCK_POINTER(pxBlockToCheck);

			if (heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock) == NULL) {
				/* Check the block is actually allocated. */
				configASSERT((pxBlockToCheck->xBlockSize & xBlockAllocatedBit) != 0);
			} else if (heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock) != pxEnd) {
				/* Check the free heap block pointer is within the heap bounds. */
				heapVALIDATE_BLOCK_POINTER(heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock));
				if ((pxBlockToCheck->xBlockSize & xBlockAllocatedBit) != 0) {
					configASSERT(pdFALSE);
				}
			}
		}

		/* Ensure the heap region starts on a correctly aligned boundary. */
		xAddress = (size_t) pxHeapRegion->pucStartAddress;
		xTotalRegionSize = pxHeapRegion->xSizeInBytes;

		if ((xAddress & portBYTE_ALIGNMENT_MASK) != 0) {
			xAddress += (portBYTE_ALIGNMENT - 1);
			xAddress &= ~portBYTE_ALIGNMENT_MASK;

			xTotalRegionSize -= xAddress - (size_t) pxHeapRegion->pucStartAddress;
		}

		pucAlignedHeap = (uint8_t *)xAddress;

		pxBlockToCheck = (BlockLink_t *)pucAlignedHeap;

		/* xEndAdress is used to mark the end of the list of a free block in heap 5.
		 * There is a "pxFakeEnd" at the end of the region space which points to the next
		 * heap region and the size of xBlockSize is zero exception the last region.
		 * The last region holds pxEnd which its pxNextFreeBlock is NULL and the size of
		 * xBlockSize is zero. */
		xEndAddress = xAddress + xTotalRegionSize;
		xEndAddress -= xHeapStructSize;
		xEndAddress &= ~portBYTE_ALIGNMENT_MASK;

		/* Traverse all blocks in a region and check integrity. */
		do {
			heapVALIDATE_BLOCK_POINTER(pxBlockToCheck);

			if (heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock) == NULL) {
				/* Check the block is actually allocated. */
				if ((pxBlockToCheck->xBlockSize & xBlockAllocatedBit) == 0) {
					configASSERT(pdFAIL);
				}
#if( defined CONFIG_HEAP_CORRUPTION_DETECT_LITE)
				if (xPortCanaryCheck(pxBlockToCheck) == pdFALSE) {
					configASSERT(pdFAIL);
				}
#endif
			} else if (heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock) != pxEnd) {
				/* Check the free heap block pointer is within the heap bounds. */
				heapVALIDATE_BLOCK_POINTER(heapPROTECT_BLOCK_POINTER(pxBlockToCheck->pxNextFreeBlock));
				if ((pxBlockToCheck->xBlockSize & xBlockAllocatedBit) != 0) {
					configASSERT(pdFAIL);
				}

#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
				if (COMPREHENSIVE_CHECK) {
					size_t xCheckFreedBlockSize = pxBlockToCheck->xBlockSize - xHeapStructSize;
					uint8_t *pxAddress = (uint8_t *) pxBlockToCheck + xHeapStructSize;
					for (size_t i = 0; i < xCheckFreedBlockSize; i++) {
						if (pxAddress[i] != xFillFreed) {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write to a unallocated Block: 0x%08x. Invalidate value: 0x%08x\n", &pxAddress[i], pxAddress[i]);
							configASSERT(pdFALSE);
						}
					}
				}
#else
				UNUSED(COMPREHENSIVE_CHECK);
#endif
			} else {
					/* This is the last free block before pxEnd */
#if ( defined CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE)
					size_t xCheckFreedBlockSize = pxBlockToCheck->xBlockSize - xHeapStructSize;
					uint8_t * pxAddress = ( uint8_t * ) pxBlockToCheck + xHeapStructSize;
					for( size_t i = 0; i < xCheckFreedBlockSize; i++)
					{
						if ( pxAddress[i] != xFillFreed) {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write to a unallocated Block: 0x%08x. Invalidate value: 0x%08x\n", &pxAddress[i], pxAddress[i]);
							configASSERT( pdFALSE );
						}
					}
#endif
			}

			xCurBlockSize = pxBlockToCheck->xBlockSize;
			xCurBlockSize &= ~xBlockAllocatedBit;
			pxBlockToCheck = (BlockLink_t *)(((uint8_t *) pxBlockToCheck) + xCurBlockSize);
		} while ((size_t) pxBlockToCheck < xEndAddress);

		xDefinedRegions++;
		pxHeapRegion = &(xHeapRegions[ xDefinedRegions ]);
	}

	(void) xTaskResumeAll();

	if (pxBlockToCheck != pxEnd) {
		configASSERT(pdFAIL);
	}

	if ((pxEnd->pxNextFreeBlock != heapPROTECT_BLOCK_POINTER(NULL)) || (pxEnd->xBlockSize != 0)) {
		configASSERT(pdFAIL);
	}

	return pdPASS;
}

#ifdef CONFIG_HEAP_TRACE
void vPortGetTaskHeapInfo(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "********** Each Task Heap Usage **********\n");
	vTaskSuspendAll();

	for (int i = 0; i < CONFIG_HEAP_TRACE_MAX_TASK_NUMBER; i++) {
		if (heap_task_info[i].TaskHandle != NULL) {
			switch (heap_task_info[i].isExist) {
			case pdTRUE:
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Task: %s, task heap usage: 0x%x\n", heap_task_info[i].Task_Name, heap_task_info[i].heap_size);
				break;
			case pdFALSE:
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Deleted Task: %s, task heap usage: 0x%x\n", heap_task_info[i].Task_Name, heap_task_info[i].heap_size);
				break;
			default :
				break;
			}
		}
	}

	(void) xTaskResumeAll();
}
#endif
#endif

void* pvPortReAlloc( void *pv,  size_t xWantedSize )
{
	BlockLink_t *pxLink;
	unsigned char *puc = ( unsigned char * ) pv;

	if( pv )
	{
		if( !xWantedSize )
		{
			vPortFree( pv );
			return NULL;
		}

		void *newArea = pvPortMalloc( xWantedSize );
		if( newArea )
		{
			/* The memory being freed will have an xBlockLink structure immediately
				before it. */
			puc -= xHeapStructSize;

			/* This casting is to keep the compiler from issuing warnings. */
			pxLink = ( void * ) puc;

			size_t oldSize =  (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;
			size_t copySize = ( oldSize < xWantedSize ) ? oldSize : xWantedSize;
			memcpy( newArea, pv, copySize );

			vTaskSuspendAll();
			{
				/* Add this block to the list of free blocks. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;
				xFreeBytesRemaining += pxLink->xBlockSize;
				prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
			}
			xTaskResumeAll();
			return newArea;
		}
	}
	else if( xWantedSize )
		return pvPortMalloc( xWantedSize );
	else
		return NULL;

	return NULL;
}

void* pvPortReAllocBase( void *pv,  size_t xWantedSize, uint32_t startAddr)
{
	BlockLink_t *pxLink;
	unsigned char *puc = ( unsigned char * ) pv;

	if( pv )
	{
		if( !xWantedSize )
		{
			vPortFree( pv );
			return NULL;
		}

		void *newArea = pvPortMallocBase( xWantedSize, startAddr);
		if( newArea )
		{
			/* The memory being freed will have an xBlockLink structure immediately
				before it. */
			puc -= xHeapStructSize;

			/* This casting is to keep the compiler from issuing warnings. */
			pxLink = ( void * ) puc;

			size_t oldSize =  (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;
			size_t copySize = ( oldSize < xWantedSize ) ? oldSize : xWantedSize;
			memcpy( newArea, pv, copySize );

			vTaskSuspendAll();
			{
				/* Add this block to the list of free blocks. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;
				xFreeBytesRemaining += pxLink->xBlockSize;
				prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
			}
			xTaskResumeAll();
			return newArea;
		}
	}
	else if( xWantedSize )
		return pvPortMallocBase( xWantedSize, startAddr);
	else
		return NULL;

	return NULL;
}

void *pvPortCalloc(size_t xWantedCnt, size_t xWantedSize)
{
	void *p;

	/* allocate 'xWantedCnt' objects of size 'xWantedSize' */
	p = pvPortMalloc(xWantedCnt * xWantedSize);
	if (p) {
		/* zero the memory */
		memset(p, 0, xWantedCnt * xWantedSize);
	}
	return p;
}

#if defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAD)
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
#endif

	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		pcCurrentTask = pcTaskGetName(NULL);
	}

	/* amebasmart use portGET_TASK_LOCK in xTaskGetSchedulerState */
	taskENTER_CRITICAL();

	/* 1. Basic info: Task name / Free Heap Size / WantedSize */
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Malloc failed. Core:[%s], Task:[%s], [free heap size: %d] [xWantedSize:%d]\r\n",
			core_name, pcCurrentTask, xPortGetFreeHeapSize(), xWantedSize);

#if defined (CONFIG_HEAP_PROTECTOR) && (CONFIG_HEAP_PROTECTOR== 1)
	rtos_heap_stats stats;
	/* 2. Full heap status */
    heap_get_stats(&stats);

#if defined (CONFIG_HEAP_TRACE) && (CONFIG_HEAP_TRACE == 1)
	/* 3. Task heap useage */
    heap_get_per_task_info();

	/* 4. Back trace */
#if defined (CONFIG_ARM_CORE_CM4) || defined (CONFIG_RSICV_CORE_KR4)
	void *pc_trace[CONFIG_HEAP_TRACE_STACK_DEPTH] = {0};
	get_call_stack(pc_trace, CONFIG_HEAP_TRACE_STACK_DEPTH);
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "********** stack backtrac cmd **********\n");
#if defined (CONFIG_ARM_CORE_CM0) || defined (CONFIG_ARM_CORE_CM4)
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "%s/bin/arm-none-eabi-addr2line -e %s/target_img2.axf -afpiC 0x%08x", SDK_TOOLCHAIN, IMAGE_DIR, pc_trace[0]);
#elif defined (CONFIG_RSICV_CORE_KR4)
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "%s/bin/riscv32-elf-addr2line -e %s/target_img2.axf -afpiC 0x%08x", SDK_TOOLCHAIN, IMAGE_DIR, pc_trace[0]);
#endif
	for (size_t i = 1; i < CONFIG_HEAP_TRACE_STACK_DEPTH; i++) {
		if (pc_trace[i] == NULL) {
			break;
		}
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " 0x%08x", pc_trace[i]);
	}
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n");
#endif /* CONFIG_ARM_CORE_CM4 || CONFIG_RSICV_CORE_KR4 */
#endif /* CONFIG_HEAP_TRACE */
#endif /* CONFIG_HEAP_PROTECTOR */
	for (;;);
}
#endif

/*-----------------------------------------------------------*/
