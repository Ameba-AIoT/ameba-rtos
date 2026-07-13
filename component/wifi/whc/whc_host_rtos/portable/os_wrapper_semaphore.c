/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * @file os_wrapper_semaphore.c
 *
 * @brief Semaphore wrapper - portable stub layer.
 *
 * This file provides the counting-semaphore interface used by the WHC host
 * stack.  The functions are intentionally left empty.
 *
 * === Porting Guide ===
 * When porting the WHC host code to a third-party chip / OS:
 *   1. Implement each function below using the target platform's RTOS
 *      or threading API (e.g. FreeRTOS semaphore, POSIX semaphore,
 *      CMSIS-RTOS, Zephyr semaphore, etc.).
 *   2. A missing implementation will cause a linker error at build time,
 *      which is safer than a silent failure at run time.
 *
 * Typical mapping:
 *   rtos_sema_create   -> create a counting semaphore with initial/max count
 *   rtos_sema_delete   -> destroy / free the semaphore object
 *   rtos_sema_take     -> decrement (wait) with timeout (wait_ms)
 *   rtos_sema_give     -> increment (signal)
 */

int rtos_sema_create(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
	return 0;
}

int rtos_sema_delete(rtos_sema_t p_handle)
{
	return 0;
}

int rtos_sema_take(rtos_sema_t p_handle, uint32_t wait_ms)
{
	return 0;
}

int rtos_sema_give(rtos_sema_t p_handle)
{
	return 0;
}
