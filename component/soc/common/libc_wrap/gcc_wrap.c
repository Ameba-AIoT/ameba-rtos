/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**************************************************
 * malloc/free/realloc wrap for gcc compiler
 *
 **************************************************/
#if defined(__GNUC__)
#include <stdint.h>
#include <stddef.h> /* for size_t */
#include <stdarg.h>
#include "diag.h"
#include "os_wrapper.h"

void *__wrap_malloc(size_t size)
{
	return rtos_heap_types_malloc(size, 0);
}

void *__wrap_realloc(void *p, size_t size)
{
	return rtos_heap_types_realloc(p, size, 0);
}

void __wrap_free(void *p)
{
	rtos_mem_free(p);
}

void *__wrap_calloc(size_t xWantedCnt, size_t xWantedSize)
{
	return rtos_heap_types_calloc(xWantedCnt, xWantedSize, 0);
}

/* For GCC stdlib */
void *__wrap__malloc_r(void *reent, size_t size)
{
	(void) reent;
	return __wrap_malloc(size);
}

void *__wrap__realloc_r(void *reent, void *p, size_t size)
{
	(void) reent;
	return __wrap_realloc(p, size);
}

void __wrap__free_r(void *reent, void *p)
{
	(void) reent;
	__wrap_free(p);
}

void *__wrap__calloc_r(void *reent, size_t xWantedCnt, size_t xWantedSize)
{
	(void) reent;
	return __wrap_calloc(xWantedCnt, xWantedSize);
}

extern u32 CPU_InInterrupt(void);
extern int vprintf(const char *fmt, va_list ap);

__weak int rtk_printf_hook(const char *fmt, va_list ap)
{
	(void)fmt;
	(void)ap;
	return -1;
}

int __wrap_printf(const char *__restrict fmt, ...)
{
	int ret;
	va_list ap;

#ifdef CONFIG_ARM_CORE_CA32
	extern rtos_mutex_t log_mutex;
	u32 in_isr = CPU_InInterrupt();
	if ((!in_isr) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (log_mutex != NULL)) {
		rtos_mutex_take(log_mutex, RTOS_MAX_DELAY);
	}
#endif

	va_start(ap, fmt);
	ret = rtk_printf_hook(fmt, ap);
	if (ret < 0) {
		/* hook not installed or declined; fall back to UART / libc */
#ifdef CONFIG_ARM_CORE_CA32
		if (in_isr) {
			ret = DiagVprintf(fmt, ap);
		}
#else
		if (CPU_InInterrupt() || rtos_sched_get_state() != RTOS_SCHED_RUNNING || rtos_get_critical_state() > 0) {
			ret = DiagVprintf(fmt, ap);
		}
#endif
		else {
			ret = vprintf(fmt, ap);
			fflush(stdout);
		}
	}
	va_end(ap);

#ifdef CONFIG_ARM_CORE_CA32
	if ((!in_isr) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (log_mutex != NULL)) {
		rtos_mutex_give(log_mutex);
	}
#endif

	return ret;
}
#endif
