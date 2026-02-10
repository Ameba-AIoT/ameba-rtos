/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_MUTEX_H__
#define __OS_WRAPPER_MUTEX_H__

/**
 * @brief  mutex handle type
 */
typedef void *rtos_mutex_t;

#define MUTEX_WAIT_TIMEOUT	0xFFFFFFFFU //will be replaced by common max timeout later

int rtos_mutex_create(rtos_mutex_t *pp_handle);

int rtos_mutex_delete(rtos_mutex_t p_handle);

int rtos_mutex_take(rtos_mutex_t p_handle, uint32_t wait_ms);

int rtos_mutex_give(rtos_mutex_t p_handle);


#endif

