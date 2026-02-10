/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "os_wrapper.h"

int rtos_sema_create(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
	int status;

	/* K_SEM_DEFINE or k_malloc for sema struct */
	if (*pp_handle == NULL) {
		*pp_handle = k_malloc(sizeof(struct k_sem));
	}

	if (*pp_handle == NULL) {
		return FAIL;
	}

	status = k_sem_init(*pp_handle, init_count, max_count);

	if (status == 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_sema_create_binary(rtos_sema_t *pp_handle)
{
	int status;

	if (pp_handle == NULL) {
		return FAIL;
	}

	status = k_sem_init(*pp_handle, 0, 1);
	if (status == 0) {
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

	k_sem_reset(p_handle);

	return SUCCESS;
}

int rtos_sema_take(rtos_sema_t p_handle, uint32_t wait_ms)
{
	int status;
	k_timeout_t timeout;

	timeout.ticks = wait_ms;
	status = k_sem_take(p_handle, timeout);
	if (status == 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_sema_give(rtos_sema_t p_handle)
{
	k_sem_give(p_handle);

	return SUCCESS;
}

uint32_t rtos_sema_get_count(rtos_sema_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	return k_sem_count_get(p_handle);
}

int rtos_sema_create_static(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
	return rtos_sema_create(pp_handle, init_count, max_count);
}

int rtos_sema_delete_static(rtos_sema_t p_handle)
{
	return rtos_sema_delete(p_handle);
}

