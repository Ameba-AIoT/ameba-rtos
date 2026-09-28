/**
 * *****************************************************************************
 * @file    threading_alt.c
 * @brief   MBEDTLS_THREADING_ALT implementation over the RTOS wrappers.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_THREADING_C) && defined(MBEDTLS_THREADING_ALT)

#include "mbedtls/threading.h"
#include "psa/crypto.h"

static int threading_mutex_init_alt(mbedtls_platform_mutex_t *mutex)
{
	if (rtos_mutex_create(mutex) != RTK_SUCCESS) {
		return PSA_ERROR_INSUFFICIENT_MEMORY;
	}

	return 0;
}

static void threading_mutex_destroy_alt(mbedtls_platform_mutex_t *mutex)
{
	rtos_mutex_delete(*mutex);
}

static int threading_mutex_lock_alt(mbedtls_platform_mutex_t *mutex)
{
	if (rtos_mutex_take(*mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	return 0;
}

static int threading_mutex_unlock_alt(mbedtls_platform_mutex_t *mutex)
{
	if (rtos_mutex_give(*mutex) != RTK_SUCCESS) {
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	return 0;
}

/* Condition variables are built from a counting semaphore: signal/broadcast
 * release one token per recorded waiter, wait releases the mutex while
 * blocking. Spurious wakeups are permitted by the API. */
static int threading_cond_init_alt(mbedtls_platform_condition_variable_t *cond)
{
	cond->waiters = 0;

	if (rtos_sema_create(&cond->sema, 0, 0xFFFFFFFFU) != RTK_SUCCESS) {
		return PSA_ERROR_INSUFFICIENT_MEMORY;
	}

	return 0;
}

static void threading_cond_destroy_alt(mbedtls_platform_condition_variable_t *cond)
{
	rtos_sema_delete(cond->sema);
}

static int threading_cond_signal_alt(mbedtls_platform_condition_variable_t *cond)
{
	if (cond->waiters > 0) {
		cond->waiters--;
		if (rtos_sema_give(cond->sema) != RTK_SUCCESS) {
			return MBEDTLS_ERR_THREADING_USAGE_ERROR;
		}
	}

	return 0;
}

static int threading_cond_broadcast_alt(mbedtls_platform_condition_variable_t *cond)
{
	while (cond->waiters > 0) {
		cond->waiters--;
		if (rtos_sema_give(cond->sema) != RTK_SUCCESS) {
			return MBEDTLS_ERR_THREADING_USAGE_ERROR;
		}
	}

	return 0;
}

static int threading_cond_wait_alt(mbedtls_platform_condition_variable_t *cond,
								   mbedtls_platform_mutex_t *mutex)
{
	int ret;

	cond->waiters++;

	if (rtos_mutex_give(*mutex) != RTK_SUCCESS) {
		cond->waiters--;
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	ret = rtos_sema_take(cond->sema, MUTEX_WAIT_TIMEOUT);

	if (rtos_mutex_take(*mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return MBEDTLS_ERR_THREADING_USAGE_ERROR;
	}

	return (ret == RTK_SUCCESS) ? 0 : MBEDTLS_ERR_THREADING_USAGE_ERROR;
}

void mbedtls_threading_init(void)
{
	mbedtls_threading_set_alt(threading_mutex_init_alt,
							  threading_mutex_destroy_alt,
							  threading_mutex_lock_alt,
							  threading_mutex_unlock_alt,
							  threading_cond_init_alt,
							  threading_cond_destroy_alt,
							  threading_cond_signal_alt,
							  threading_cond_broadcast_alt,
							  threading_cond_wait_alt);
}

void mbedtls_threading_free(void)
{
	mbedtls_threading_free_alt();
}

#endif /* MBEDTLS_THREADING_C && MBEDTLS_THREADING_ALT */
