/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_TIMER_H__
#define __OS_WRAPPER_TIMER_H__

#define RTOS_TIMER_MAX_DELAY    				0xFFFFFFFFUL

/**
 * @brief  timer handle type
 */
typedef void *rtos_timer_t;

/**
 * @brief  Static memory allocation implementation of rtos_timer_create
 * @param  pp_handle:
 * @param  p_timer_name:
 * @param  timer_id:
 * @param  interval_ms:
 * @param  reload:
 * @param  p_timer_callback:
 * @retval
 */
int rtos_timer_create_static(rtos_timer_t *pp_handle, const char *p_timer_name, uint32_t timer_id,
							 uint32_t interval_ms, uint8_t reload, void (*p_timer_callback)(void *));

/**
 * @brief  Static memory allocation implementation of rtos_timer_delete
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 */
int rtos_timer_delete_static(rtos_timer_t p_handle, uint32_t wait_ms);

/**
 * @brief  For FreeRTOS, map to xTimerCreate
 * @note   Usage example:
 * Create:
 *          rtos_timer_t timer_handle;
 *          rtos_timer_create(&timer_handle, "timer_test", timer_id, delay_ms, is_reload, timer_cb_function);
 * Start:
 *          rtos_timer_start(timer_handle, wait_ms);
 * Stop:
 *          rtos_timer_stop(timer_handle, wait_ms);
 * Delete:
 *          rtos_timer_delete(timer_handle, wait_ms);
 * @param  pp_handle: The handle itself is a pointer, and the pp_handle means a pointer to the handle.
 * @param  p_timer_name:
 * @param  timer_id:
 * @param  interval_ms:
 * @param  reload:
 * @param  p_timer_callback:
 * @retval
 */
int rtos_timer_create(rtos_timer_t *pp_handle, const char *p_timer_name, uint32_t timer_id,
					  uint32_t interval_ms, uint8_t reload, void (*p_timer_callback)(void *));

/**
 * @brief
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 * @retval
 */
int rtos_timer_delete(rtos_timer_t p_handle, uint32_t wait_ms);

/**
 * @brief  For FreeRTOS, map to xTimerStart / xTimerStartFromISR
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 * @retval
 */
int rtos_timer_start(rtos_timer_t p_handle, uint32_t wait_ms);

/**
 * @brief
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 * @retval
 */
int rtos_timer_stop(rtos_timer_t p_handle, uint32_t wait_ms);

/**
 * @brief
 * @param  p_handle:
 * @param  interval_ms:
 * @param  wait_ms:
 * @retval
 * @retval
 */
int rtos_timer_change_period(rtos_timer_t p_handle, uint32_t interval_ms, uint32_t wait_ms);

/**
 * @brief  For FreeRTOS, map to xTimerIsTimerActive
 * Queries a timer to see if it is active or dormant. A timer will be dormant if:
 * 1) It has been created but not started, or
 * 2) It is an expired one-shot timer that has not been restarted.
 * @note   non-interrupt safety
 * @param  p_handle:
 * @retval
 */
uint32_t rtos_timer_is_timer_active(rtos_timer_t p_handle);

/**
 * @brief  For FreeRTOS, map to pvTimerGetTimerID
 * @note   non-interrupt safety
 * @param  p_handle:
 * @retval
 */
uint32_t rtos_timer_get_id(rtos_timer_t p_handle);

/**
 * @brief  Pend a function call to the timer daemon task.
 * @note   This function is used to defer the execution of a function to the RTOS
 *         daemon task (the timer service task). It allows a function to be executed
 *         in the context of the timer daemon task rather than in the caller's context.
 * @param  p_func: Pointer to the function to be executed in the timer daemon task.
 *                The function must have the signature: void func(void *pv_parameter1, uint32_t ul_parameter2)
 * @param  pv_parameter1: The first parameter passed to p_func (void pointer)
 * @param  ul_parameter2: The second parameter passed to p_func (uint32_t)
 * @param  wait_ms: The maximum time to wait for the function to be successfully queued (in milliseconds).
 *                 Use RTOS_TIMER_MAX_DELAY to wait indefinitely.
 * @retval The status is SUCCESS or FAIL
 * @note   This function can be called from both task and interrupt context.
 *         When called from ISR context, the wait_ms parameter is ignored.
 */
int rtos_timer_pend_function_call(void (*p_func)(void *, uint32_t),
								  void *pv_parameter1,
								  uint32_t ul_parameter2,
								  uint32_t wait_ms);

#endif
