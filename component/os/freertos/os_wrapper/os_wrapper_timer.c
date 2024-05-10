/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

/* FreeRTOS Static Implementation */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
extern StaticTimer_t *__reserved_get_timer_from_poll(void);
extern void __reserved_release_timer_to_poll(void *buf);
#endif

int rtos_timer_create_static(rtos_timer_t *pp_handle, const char *p_timer_name, uint32_t timer_id,
							 uint32_t interval_ms, uint8_t reload, void (*p_timer_callback)(void *))
{
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	StaticTimer_t *timer;
	TickType_t timer_ticks;

	timer = __reserved_get_timer_from_poll();

	if (timer == NULL) {
		return rtos_timer_create(pp_handle, p_timer_name, timer_id, interval_ms, reload, p_timer_callback);
	} else {
		timer_ticks = (TickType_t)((interval_ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS);

		*pp_handle = xTimerCreateStatic(p_timer_name, timer_ticks, (BaseType_t)reload,
										(void *)timer_id, (TimerCallbackFunction_t)p_timer_callback, timer);

		if (*pp_handle == NULL) {
			return FAIL;
		}
	}
	return SUCCESS;
#else
	return rtos_timer_create(pp_handle, p_timer_name, timer_id, interval_ms, reload, p_timer_callback);
#endif
}

int rtos_timer_delete_static(rtos_timer_t p_handle, uint32_t wait_ms)
{
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	rtos_timer_t handle_for_delete = p_handle;
	while (rtos_timer_delete(handle_for_delete, wait_ms) != SUCCESS) {};
	// wait timer until inactive in __reserved_release_timer_to_poll
	__reserved_release_timer_to_poll(p_handle);
	return SUCCESS;
#else
	return rtos_timer_delete(p_handle, wait_ms);
#endif
}


int rtos_timer_create(rtos_timer_t *pp_handle, const char *p_timer_name, uint32_t timer_id,
					  uint32_t interval_ms, uint8_t reload, void (*p_timer_callback)(void *))
{
	TickType_t timer_ticks;

	if (pp_handle == NULL || p_timer_callback == NULL) {
		return FAIL;
	}

	timer_ticks = (TickType_t)((interval_ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS);

	*pp_handle = xTimerCreate(p_timer_name, timer_ticks, (BaseType_t)reload,
							  (void *)timer_id, (TimerCallbackFunction_t)p_timer_callback);
	if (*pp_handle == NULL) {
		return FAIL;
	}

	return SUCCESS;
}

int rtos_timer_delete(rtos_timer_t p_handle, uint32_t wait_ms)
{
	BaseType_t ret;

	if (p_handle == NULL) {
		return FAIL;
	}

	ret = xTimerDelete((TimerHandle_t) p_handle, RTOS_CONVERT_MS_TO_TICKS(wait_ms));

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_timer_start(rtos_timer_t p_handle, uint32_t wait_ms)
{
	BaseType_t ret, task_woken;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		task_woken = pdFALSE;
		ret = xTimerStartFromISR((TimerHandle_t) p_handle, &task_woken);
		if (ret != pdTRUE) {
			return FAIL;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xTimerStart((TimerHandle_t) p_handle, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_timer_stop(rtos_timer_t p_handle, uint32_t wait_ms)
{
	BaseType_t ret, task_woken;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		task_woken = pdFALSE;
		ret = xTimerStopFromISR((TimerHandle_t) p_handle, &task_woken);
		if (ret != pdTRUE) {
			return FAIL;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xTimerStop((TimerHandle_t) p_handle, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_timer_change_period(rtos_timer_t p_handle, uint32_t interval_ms, uint32_t wait_ms)
{
	BaseType_t ret, task_woken;
	TickType_t timer_ticks;

	if (p_handle == NULL) {
		return FAIL;
	}

	timer_ticks = RTOS_CONVERT_MS_TO_TICKS(interval_ms);

	if (rtos_critical_is_in_interrupt()) {
		task_woken = pdFALSE;
		ret = xTimerChangePeriodFromISR((TimerHandle_t) p_handle, timer_ticks, &task_woken);
		if (ret != pdTRUE) {
			return FAIL;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xTimerChangePeriod((TimerHandle_t) p_handle, timer_ticks, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

uint32_t rtos_timer_is_timer_active(rtos_timer_t p_handle)
{
	return (uint32_t) xTimerIsTimerActive((TimerHandle_t) p_handle);
}

uint32_t rtos_timer_get_id(rtos_timer_t p_handle)
{
	return (uint32_t) pvTimerGetTimerID((TimerHandle_t) p_handle);
}

