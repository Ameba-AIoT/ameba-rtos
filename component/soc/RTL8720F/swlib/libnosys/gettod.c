/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stub version of gettimeofday.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"
#include "ameba_soc.h"
#include "os_wrapper.h"

struct timeval;
static struct {
	uint64_t sec;
	uint32_t usec;
	uint64_t last_tick;
} system_time;

static rtos_mutex_t time_mutex = NULL;

int
_gettimeofday(struct timeval  *ptimeval,
			  void *ptimezone)
{
	UNUSED(ptimezone);

	if (ptimeval == NULL) {
		errno = EFAULT;
		return -1;
	}

	if (time_mutex == NULL) {
		if (rtos_mutex_create(&time_mutex) != RTK_SUCCESS) {
			errno = ENOMEM;
			return -1;
		}
	}

	if (rtos_mutex_take(time_mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		errno = EBUSY;
		return -1;
	}

	uint64_t current_ms = (uint64_t)rtos_time_get_current_system_time_ms();
	uint64_t delta_ticks = current_ms - system_time.last_tick;
	uint64_t delta_us = delta_ticks * 1000;

	uint64_t total_us = system_time.usec + delta_us;
	ptimeval->tv_sec = (time_t)(system_time.sec + total_us / 1000000);
	ptimeval->tv_usec = (suseconds_t)(total_us % 1000000);

	rtos_mutex_give(time_mutex);
	return 0;
}

int
_settimeofday(struct timeval  *ptimeval,
			  void *ptimezone)
{
	if (ptimeval == NULL || ptimezone != NULL || ptimeval->tv_usec >= 1000000) {
		errno = EINVAL;
		return -1;
	}

	if (time_mutex == NULL) {
		if (rtos_mutex_create(&time_mutex) != RTK_SUCCESS) {
			errno = ENOMEM;
			return -1;
		}
	}

	if (rtos_mutex_take(time_mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		errno = EBUSY;
		return -1;
	}

	system_time.last_tick = (uint64_t)rtos_time_get_current_system_time_ms();
	system_time.sec = (uint64_t)ptimeval->tv_sec;
	system_time.usec = ptimeval->tv_usec;

	rtos_mutex_give(time_mutex);
	return 0;
}