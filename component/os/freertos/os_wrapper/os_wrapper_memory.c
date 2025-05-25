/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "ameba.h"
#include "FreeRTOS.h"
#include "os_wrapper.h"


#include <assert.h>
static_assert(RTOS_MEM_BYTE_ALIGNMENT == portBYTE_ALIGNMENT, "Incorrect RTOS_MEM_BYTE_ALIGNMENT value config");

// freertos_heap5_config.c
void os_heap_init(void);

void rtos_mem_init(void)
{
	os_heap_init();
}

void *rtos_mem_malloc(uint32_t size)
{
	return pvPortMalloc(size);
}

void *rtos_mem_zmalloc(uint32_t size)
{
	void *pbuf = NULL;

	pbuf = pvPortMalloc(size);
	if (pbuf != NULL) {
		memset(pbuf, 0, size);
	}

	return pbuf;
}

void *rtos_mem_calloc(uint32_t elementNum, uint32_t elementSize)
{
	uint32_t sz = elementNum * elementSize;
	return rtos_mem_zmalloc(sz);
}

void *rtos_mem_realloc(void *pbuf, uint32_t size)
{
	return pvPortReAlloc(pbuf, size);
}

void rtos_mem_free(void *pbuf)
{
	if (pbuf == NULL) {
		return;
	}
	vPortFree(pbuf);
}

uint32_t rtos_mem_get_free_heap_size(void)
{
	return (uint32_t)xPortGetFreeHeapSize();
}

uint32_t rtos_mem_get_minimum_ever_free_heap_size(void)
{
	return (uint32_t)xPortGetMinimumEverFreeHeapSize();
}

void *rtos_heap_types_malloc(uint32_t size, MALLOC_TYPES type)
{
	switch (type) {
	case TYPE_SRAM:
		return pvPortMallocBase(size, SRAM_BASE);
	case TYPE_DRAM:
		return pvPortMallocBase(size, PSRAM_BASE);
	default:
		return pvPortMallocBase(size, 0);
	}

}

void *rtos_heap_types_zmalloc(uint32_t size, MALLOC_TYPES type)
{
	void *pbuf = NULL;

	pbuf = rtos_heap_types_malloc(size, type);
	if (pbuf != NULL) {
		memset(pbuf, 0, size);
	}

	return pbuf;
}

void *rtos_heap_types_calloc(uint32_t elementNum, uint32_t elementSize, MALLOC_TYPES type)
{
	uint32_t sz = elementNum * elementSize;
	return rtos_heap_types_zmalloc(sz, type);
}

void *rtos_heap_types_realloc(void *pbuf, uint32_t size, MALLOC_TYPES type)
{
	switch (type) {

	case TYPE_SRAM:
		return pvPortReAllocBase(pbuf, size, SRAM_BASE);
	case TYPE_DRAM:
		return pvPortReAllocBase(pbuf, size, PSRAM_BASE);
	default:
		return pvPortReAllocBase(pbuf, size, 0);
	}
}

void rtos_heap_types_free(void *pbuf)
{
	rtos_mem_free(pbuf);
}
