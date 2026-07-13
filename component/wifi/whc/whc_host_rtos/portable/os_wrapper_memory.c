/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * @file os_wrapper_memory.c
 *
 * @brief Memory allocation wrapper - portable stub layer.
 *
 * This file provides the memory allocation interface used by the WHC host
 * stack.  The functions are intentionally left empty.
 *
 * === Porting Guide ===
 * When porting the WHC host code to a third-party chip / OS:
 *   1. Implement each function below using the target platform's heap API
 *      (e.g. standard malloc/free/realloc, or an RTOS-specific allocator).
 *
 * Typical mapping:
 *   rtos_mem_malloc   -> malloc
 *   rtos_mem_zmalloc  -> malloc + memset(..., 0, ...)
 *   rtos_mem_calloc   -> calloc
 *   rtos_mem_free     -> free
 */

void *rtos_mem_malloc(uint32_t size)
{
	return NULL;
}

void *rtos_mem_zmalloc(uint32_t size)
{
	return NULL;
}

void *rtos_mem_calloc(uint32_t elementNum, uint32_t elementSize)
{
	return NULL;
}

void rtos_mem_free(void *pbuf)
{

}
