/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "os_wrapper.h"

int rtos_mutex_create(rtos_mutex_t *pp_handle)
{
	int status;

	if (*pp_handle == NULL) {
		*pp_handle = k_malloc(sizeof(struct k_mutex));
	}

	if (*pp_handle == NULL) {
		return FAIL;
	}

	status = k_mutex_init(*pp_handle);
	if (status == 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

/**
 * @brief  For FreeRTOS, map to vSemaphoreDelete
 * @note   Do not delete mutex if held by a task
 * @param  p_handle:
 * @retval
 */
int rtos_mutex_delete(rtos_mutex_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	// TBD

	return SUCCESS;
}

/**
 * @brief  For FreeRTOS, map to xSemaphoreTake / xSemaphoreTakeFromISR
 *         The API internally determines whether it is in the interrupt state and calls the corresponding RTOS interface.
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 */
int rtos_mutex_take(rtos_mutex_t p_handle, uint32_t wait_ms)
{
	int status;
	k_timeout_t timeout;

	timeout.ticks = wait_ms;
	status = k_mutex_lock(p_handle, timeout);
	if (status == 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

/**
 * @brief  For FreeRTOS, map to xSemaphoreGive / xSemaphoreGiveFromISR
 *         The API internally determines whether it is in the interrupt state and calls the corresponding RTOS interface.
 * @param  p_handle:
 * @param  wait_ms:
 * @retval
 */
int rtos_mutex_give(rtos_mutex_t p_handle)
{
	int status;

	status = k_mutex_unlock(p_handle);
	if (status == 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_mutex_create_static(rtos_mutex_t *pp_handle)
{
	return rtos_mutex_create(pp_handle);
}

int rtos_mutex_delete_static(rtos_mutex_t p_handle)
{
	return rtos_mutex_delete(p_handle);
}

