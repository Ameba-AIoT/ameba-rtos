/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "ameba.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

/* FreeRTOS Static Implementation */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
extern StaticSemaphore_t *__reserved_get_sema_from_poll(void);
extern void __reserved_release_sema_to_poll(void *buf);
#endif

int rtos_sema_create_static(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	StaticSemaphore_t *sema;

	sema = __reserved_get_sema_from_poll();

	if (sema == NULL) {
		return rtos_sema_create(pp_handle, init_count, max_count);
	} else {
		*pp_handle = xSemaphoreCreateCountingStatic(max_count, init_count, sema);

		if (*pp_handle != NULL) {
			return SUCCESS;
		} else {
			return FAIL;
		}
	}
#else
	return rtos_sema_create(pp_handle, init_count, max_count);
#endif
}

int rtos_sema_create_binary_static(rtos_sema_t *pp_handle)
{
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	StaticSemaphore_t *sema;

	sema = __reserved_get_sema_from_poll();

	if (sema == NULL) {
		return rtos_sema_create_binary(pp_handle);
	} else {
		*pp_handle = xSemaphoreCreateBinaryStatic(sema);

		if (*pp_handle != NULL) {
			return SUCCESS;
		} else {
			return FAIL;
		}
	}
#else
	return rtos_sema_create_binary(pp_handle);
#endif
}

int rtos_sema_delete_static(rtos_sema_t p_handle)
{
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	rtos_sema_delete(p_handle);
	__reserved_release_sema_to_poll(p_handle);
	return SUCCESS;
#else
	return rtos_sema_delete(p_handle);
#endif
}


int rtos_sema_create(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
	if (pp_handle == NULL) {
		return FAIL;
	}

	*pp_handle = (rtos_sema_t)xSemaphoreCreateCounting(max_count, init_count);

	if (*pp_handle != NULL) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_sema_create_binary(rtos_sema_t *pp_handle)
{
	if (pp_handle == NULL) {
		return FAIL;
	}

	*pp_handle = (rtos_sema_t)xSemaphoreCreateBinary();

	if (*pp_handle != NULL) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_sema_delete(rtos_sema_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	vSemaphoreDelete((QueueHandle_t)p_handle);

	return SUCCESS;
}

int rtos_sema_take(rtos_sema_t p_handle, uint32_t wait_ms)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreTakeFromISR((QueueHandle_t)p_handle, &task_woken);
		if (ret != pdTRUE) {
			return FAIL;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreTake((QueueHandle_t)p_handle, RTOS_CONVERT_MS_TO_TICKS(wait_ms));

		if (ret != pdTRUE) {
			return FAIL;
		}
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_sema_give(rtos_sema_t p_handle)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreGiveFromISR((QueueHandle_t)p_handle, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreGive((QueueHandle_t)p_handle);
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

uint32_t rtos_sema_get_count(rtos_sema_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	return (uint32_t)uxSemaphoreGetCount((QueueHandle_t)p_handle);
}
