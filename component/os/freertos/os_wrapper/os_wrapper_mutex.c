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
#if(configSUPPORT_STATIC_ALLOCATION == 1)
extern StaticSemaphore_t *__reserved_get_mutex_from_poll(void);
extern void __reserved_release_mutex_to_poll(void *buf);
#endif

int rtos_mutex_create_static(rtos_mutex_t *pp_handle)
{
#if(configSUPPORT_STATIC_ALLOCATION == 1)
	StaticSemaphore_t *mutex;

	mutex = __reserved_get_mutex_from_poll();

	if (mutex == NULL) {
		return rtos_mutex_create(pp_handle);
	} else {
		*pp_handle = xSemaphoreCreateMutexStatic(mutex);

		if (*pp_handle != NULL) {
			return SUCCESS;
		} else {
			return FAIL;
		}
	}
#else
	return rtos_mutex_create(pp_handle);
#endif
}

int rtos_mutex_delete_static(rtos_mutex_t p_handle)
{
#if(configSUPPORT_STATIC_ALLOCATION == 1)
	int ret = rtos_mutex_delete(p_handle);
	__reserved_release_mutex_to_poll(p_handle);
	return ret;
#else
	return rtos_mutex_delete(p_handle);
#endif
}

int rtos_mutex_recursive_create_static(rtos_mutex_t *pp_handle)
{
#if(configSUPPORT_STATIC_ALLOCATION == 1)
	StaticSemaphore_t *mutex;

	mutex = __reserved_get_mutex_from_poll();

	if (mutex == NULL) {
		return rtos_mutex_create(pp_handle);
	} else {
		*pp_handle = xSemaphoreCreateRecursiveMutexStatic(mutex);

		if (*pp_handle != NULL) {
			return SUCCESS;
		} else {
			return FAIL;
		}
	}
#else
	return rtos_mutex_recursive_create(pp_handle);
#endif
}

int rtos_mutex_recursive_delete_static(rtos_mutex_t p_handle)
{
	return rtos_mutex_delete_static(p_handle);
}

int rtos_mutex_create(rtos_mutex_t *pp_handle)
{
	if (pp_handle == NULL) {
		return FAIL;
	}

	*pp_handle = (rtos_mutex_t)xSemaphoreCreateMutex();
	if (*pp_handle != NULL) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_mutex_delete(rtos_mutex_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	if (xSemaphoreGetMutexHolder((QueueHandle_t)p_handle) == NULL) {
		vSemaphoreDelete((QueueHandle_t)p_handle);
		return SUCCESS;
	} else {
		vSemaphoreDelete((QueueHandle_t)p_handle);
		RTK_LOGS(NOTAG, "[%s] %s <<< The mutex has not been released, but the mutex has been deleted. >>>\n", pcTaskGetTaskName(NULL), __FUNCTION__);
		return FAIL;
	}
}

int rtos_mutex_take(rtos_mutex_t p_handle, uint32_t wait_ms)
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

	return SUCCESS;
}

int rtos_mutex_give(rtos_mutex_t p_handle)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreGiveFromISR(p_handle, &task_woken);
		if (ret != pdTRUE) {
			return FAIL;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreGive(p_handle);
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

int rtos_mutex_recursive_create(rtos_mutex_t *pp_handle)
{
	if (pp_handle == NULL) {
		return FAIL;
	}

	*pp_handle = (rtos_mutex_t)xSemaphoreCreateRecursiveMutex();
	if (*pp_handle != NULL) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_mutex_recursive_delete(rtos_mutex_t p_handle)
{
	return rtos_mutex_delete(p_handle);
}

int rtos_mutex_recursive_take(rtos_mutex_t p_handle, uint32_t wait_ms)
{
	BaseType_t ret;

	if (rtos_critical_is_in_interrupt()) {
		return FAIL;
	}

	ret = xSemaphoreTakeRecursive((QueueHandle_t)p_handle, RTOS_CONVERT_MS_TO_TICKS(wait_ms));

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_mutex_recursive_give(rtos_mutex_t p_handle)
{
	BaseType_t ret;

	if (rtos_critical_is_in_interrupt()) {
		return FAIL;
	}

	ret = xSemaphoreGiveRecursive((QueueHandle_t)p_handle);
	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}
