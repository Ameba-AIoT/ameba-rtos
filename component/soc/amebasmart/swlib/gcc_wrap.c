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

#ifdef ARM_CORE_CA32
/* include apcore/spinlock.h for padding a cache line fully*/
#include "spinlock.h"
/**
 * @brief The CA32 has two cores that need to be locked
 * when printing to avoid interrupting each other
 */
static spinlock_t print_lock;
#endif

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

#ifdef ARM_CORE_CA32
	u32 isr_status = spin_lock_irqsave(&print_lock);
#endif

	va_start(ap, fmt);
	if (CPU_InInterrupt() || rtos_sched_get_state() != RTOS_SCHED_RUNNING) {
		ret = DiagVprintf(fmt, ap);
	} else {
		ret = vprintf(fmt, ap);
		fflush(stdout);
	}
	va_end(ap);

#ifdef ARM_CORE_CA32
	spin_unlock_irqrestore(&print_lock, isr_status);
#endif

	return ret;
}

#endif