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
	return rtos_mem_malloc(size);
}

void *__wrap_realloc(void *p, size_t size)
{
	return rtos_mem_realloc(p, size);
}

void __wrap_free(void *p)
{
	rtos_mem_free(p);
}

void *__wrap_calloc(size_t xWantedCnt, size_t xWantedSize)
{
	return rtos_mem_calloc(xWantedCnt, xWantedSize);
}

/* For GCC stdlib */
void *__wrap__malloc_r(void *reent, size_t size)
{
	(void) reent;
	return rtos_mem_malloc(size);
}

void *__wrap__realloc_r(void *reent, void *p, size_t size)
{
	(void) reent;
	return rtos_mem_realloc(p, size);
}

void __wrap__free_r(void *reent, void *p)
{
	(void) reent;
	rtos_mem_free(p);
}

void *__wrap__calloc_r(void *reent, size_t xWantedCnt, size_t xWantedSize)
{
	(void) reent;
	return rtos_mem_calloc(xWantedCnt, xWantedSize);
}

extern u32 CPU_InInterrupt(void);
extern int vprintf(const char *fmt, va_list ap);

int __wrap_printf(const char *__restrict fmt, ...)
{
	int ret;
	va_list ap;

#ifdef CONFIG_ARM_CORE_CA32
	/* The CA32 has two cores that need to be locked when printing to avoid interrupting each other */
	rtos_critical_enter(RTOS_CRITICAL_LOG);
#endif

	va_start(ap, fmt);
#ifdef CONFIG_ARM_CORE_CA32
	if (CPU_InInterrupt()) {
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
	va_end(ap);

#ifdef CONFIG_ARM_CORE_CA32
	rtos_critical_exit(RTOS_CRITICAL_LOG);
#endif

	return ret;
}

#endif