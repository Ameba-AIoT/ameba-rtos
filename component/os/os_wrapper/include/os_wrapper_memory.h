/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_MEMORY_H__
#define __OS_WRAPPER_MEMORY_H__


/**
 * @brief  Define memory alignment settings. This macro will be checked during compilation to ensure
 * that it is consistent with the configuration of the operating system.
 */
#if (defined CONFIG_AMEBASMART)
#if (defined CONFIG_ARM_CORE_CM4) || (defined CONFIG_ARM_CORE_CA32)
#define RTOS_MEM_BYTE_ALIGNMENT 64
#else
#define RTOS_MEM_BYTE_ALIGNMENT 32
#endif
#else
#ifndef RTOS_MEM_BYTE_ALIGNMENT
#define RTOS_MEM_BYTE_ALIGNMENT 32
#endif
#endif

/**
 * @brief  Initialize dynamic memory pool
 */
void rtos_mem_init(void);

/**
 * @brief  Allocate memory from the heap. The buffer value is random
 * @note   The return buffer size/address is cacheline size aligned
 * @param  size: buffer size in byte
 * @retval Pointer to memory the caller can now use
 */
void *rtos_mem_malloc(uint32_t size);

/**
 * @brief  Allocate memory from the heap. The buffer value is zero
 * @note   The return buffer size/address is cacheline size aligned
 * @param  size: buffer size in byte
 * @retval Pointer to memory the caller can now use
 */
void *rtos_mem_zmalloc(uint32_t size);

/**
 * @brief  Allocate memory from the heap. The buffer value is zero
 * @note   The return buffer size/address is cacheline size aligned
 * @param  elementNum:  Number of elements, memory size is elementNum*elementSize
 * @param  elementSize: Size of each array element (in bytes).
 * @retval Pointer to memory the caller can now use
 */
void *rtos_mem_calloc(uint32_t elementNum, uint32_t elementSize);

/**
 * @brief  Reuse or extend memory previously allocated by the malloc(), calloc(), and realloc() functions
 * @note   The return buffer size/address is cacheline size aligned
 * @param  pbuf: Pointer containing the address
 * @param  size: The number of bytes of memory to be newly allocated.
 * @retval Pointer to memory the caller can now use
 */
void *rtos_mem_realloc(void *pbuf, uint32_t size);

/**
 * @brief  Deallocate memory from the heap.
 * @param  pbuf: a pointer to memory previously allocated
 */
void rtos_mem_free(void *pbuf);

/**
 * @brief  Get free heap size.
 * @retval Free heap size in byte
 */
uint32_t rtos_mem_get_free_heap_size(void);

/**
 * @brief  Get minimum ever free heap size.
 * @retval Minimum ever free heap size in byte
 */
uint32_t rtos_mem_get_minimum_ever_free_heap_size(void);

/*
 TYPE_TCM, reserved for tcm, can malloc heap after 0x00;
 TYPE_SRAM, only can malloc heap after 0x20000000;
 TYPE_DRAM, include PSRAM and DDR, only can malloc heap after 0x60000000
 TYPE_ALL, can malloc heap after 0x00;
*/
typedef enum {
	TYPE_TCM = 0,
	TYPE_SRAM,
	TYPE_DRAM,
	TYPE_ALL
} MALLOC_TYPES;

void *rtos_heap_types_malloc(uint32_t size, MALLOC_TYPES type);

void *rtos_heap_types_zmalloc(uint32_t size, MALLOC_TYPES type);

void *rtos_heap_types_calloc(uint32_t elementNum, uint32_t elementSize, MALLOC_TYPES type);

void *rtos_heap_types_realloc(void *pbuf, uint32_t size, MALLOC_TYPES type);

void rtos_heap_types_free(void *pbuf);

#endif
