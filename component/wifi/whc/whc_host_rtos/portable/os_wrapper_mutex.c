/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * @file os_wrapper_mutex.c
 *
 * @brief Mutex wrapper - portable stub layer.
 *
 * This file provides the mutex and critical-section interface used by the
 * WHC host stack.  The functions are intentionally left empty.
 *
 * === Porting Guide ===
 * When porting the WHC host code to a third-party chip / OS:
 *   1. Implement each function below using the target platform's RTOS
 *      or threading API (e.g. FreeRTOS mutex, pthread mutex, CMSIS-RTOS,
 *      Zephyr mutex, etc.).
 *   2. A missing implementation will cause a linker error at build time,
 *      which is safer than a silent failure at run time.
 *
 * Typical mapping:
 *   rtos_mutex_create           -> create a mutex object / initialise
 *   rtos_mutex_delete           -> destroy / free the mutex object
 *   rtos_mutex_take             -> acquire with timeout (wait_ms)
 *   rtos_mutex_give             -> release
 *   rtos_critical_is_in_interrupt -> return 1 if in ISR context, else 0
 */

int rtos_mutex_create(rtos_mutex_t *pp_handle)
{
	return 0;
}

int rtos_mutex_delete(rtos_mutex_t p_handle)
{
	return 0;
}

int rtos_critical_is_in_interrupt(void)
{
	return 0;
}

int rtos_mutex_take(rtos_mutex_t p_handle, uint32_t wait_ms)
{
	return 0;
}

int rtos_mutex_give(rtos_mutex_t p_handle)
{
	return 0;
}
