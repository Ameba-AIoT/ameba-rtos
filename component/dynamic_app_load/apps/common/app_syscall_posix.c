/**
 ******************************************************************************
 * @file    app_syscall_posix.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
// #include "basic_types.h"
#include "app_syscall.h"
#include "os_wrapper.h"
extern uint8_t app_sgot[];

uint32_t __wrap_rand(void)
{
	return sys->rand();
}

int __wrap_printf(const char *__restrict fmt, ...)
{
	int ret = 0;
	va_list ap;
	va_start(ap, fmt);
	ret = sys->log_write_with_ap(fmt, ap);
	va_end(ap);

	return ret;
}

uint32_t rtos_time_get_current_system_time_ms(void)
{
	return 0;
}

int rtos_sema_create_binary(rtos_sema_t *pp_handle)
{
	return 0;
}

int rtos_sema_delete(rtos_sema_t p_handle)
{
	return 0;
}

int rtos_sema_take(rtos_sema_t p_handle, uint32_t wait_ms)
{
	return 0;
}

int rtos_sema_give(rtos_sema_t p_handle)
{
	return 0;
}

int rtos_queue_create(rtos_queue_t *pp_handle, uint32_t msg_num, uint32_t msg_size)
{
	return 0;
}

int rtos_queue_delete(rtos_queue_t p_handle)
{
	return 0;
}

int rtos_queue_send(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	return 0;
}

int rtos_queue_receive(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	return 0;
}

uint32_t rtos_queue_massage_waiting(rtos_queue_t p_handle)
{
	return 0;
}

int rtos_mutex_give(rtos_mutex_t p_handle)
{
	return 0;
}

/* clock */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	return sys->nanosleep(rqtp, rmtp);
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
	return sys->clock_gettime(clock_id, tp);
}

/* memory */
void *rtos_mem_malloc(uint32_t size)
{
	return sys->rtos_mem_malloc(size);
}

void *rtos_mem_zmalloc(uint32_t size)
{
	return sys->rtos_mem_zmalloc(size);
}

void *rtos_mem_calloc(uint32_t elementNum, uint32_t elementSize)
{
	return sys->rtos_mem_calloc(elementNum, elementSize);
}
void *rtos_mem_realloc(void *pbuf, uint32_t size)
{
	return sys->rtos_mem_realloc(pbuf, size);
}

void rtos_mem_free(void *pbuf)
{
	sys->rtos_mem_free(pbuf);
}

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

uint32_t rtos_mem_get_free_heap_size(void)
{
	return sys->rtos_mem_get_free_heap_size();
}

/* pthread */
pthread_t pthread_self(void)
{
	return (pthread_t)sys->pthread_self();
}

void pthread_exit(void *value_ptr)
{
	sys->pthread_exit(value_ptr);
}

int pthread_attr_init(pthread_attr_t *attr)
{
	return sys->pthread_attr_init(attr);
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
	return sys->pthread_attr_destroy(attr);
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	return sys->pthread_attr_setschedpolicy(attr, policy);
}

int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy)
{
	return sys->pthread_attr_getschedpolicy(attr, policy);
}
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size)
{
	return sys->pthread_attr_setstacksize(attr, stack_size);
}

int pthread_attr_getstacksize(pthread_attr_t const *attr, size_t *stack_size)
{
	return sys->pthread_attr_getstacksize(attr, stack_size);
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int state)
{
	return sys->pthread_attr_setdetachstate(attr, state);
}

int pthread_attr_getdetachstate(pthread_attr_t const *attr, int *state)
{
	return sys->pthread_attr_getdetachstate(attr, state);
}

int pthread_attr_setschedparam(pthread_attr_t *attr, struct sched_param const *param)
{
	return sys->pthread_attr_setschedparam(attr, param);
}

int pthread_attr_getschedparam(pthread_attr_t const *attr, struct sched_param *param)
{
	return sys->pthread_attr_getschedparam(attr, param);
}

int pthread_attr_setpriority(pthread_attr_t *attr, uint8_t priority)
{
	return sys->pthread_attr_setpriority(attr, priority);
}

int pthread_create(pthread_t *tid, const pthread_attr_t *attr,
				   void *(*start)(void *), void *arg)
{
	return sys->pthread_create(tid, attr, start, arg, (uint32_t)app_sgot);
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	return sys->pthread_mutex_init(mutex, attr);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return sys->pthread_mutex_destroy(mutex);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return sys->pthread_mutex_lock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return sys->pthread_mutex_unlock(mutex);
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
	return sys->pthread_cond_init(cond, attr);
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
	return sys->pthread_cond_destroy(cond);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	return sys->pthread_cond_signal(cond);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	return sys->pthread_cond_wait(cond, mutex);
}

int pthread_cond_timedwait(pthread_cond_t		 *cond,
						   pthread_mutex_t		*mutex,
						   const struct timespec *abstime)
{
	return sys->pthread_cond_timedwait(cond, mutex, abstime);
}

int pthread_key_create(sys_pthread_key_t *key, void (*destructor)(void *))
{
	return sys->pthread_key_create(key, destructor);
}

int pthread_key_delete(sys_pthread_key_t key)
{
	return sys->pthread_key_delete(key);
}

int pthread_setspecific(sys_pthread_key_t key, const void *value)
{
	return sys->pthread_setspecific(key, value);
}

void *pthread_getspecific(sys_pthread_key_t key)
{
	return sys->pthread_getspecific(key);
}

void pthread_clrspecific(sys_pthread_key_t key)
{
	sys->pthread_clrspecific(key);
}

int pthread_setname_np(pthread_attr_t *attr, const char *name)
{
	return sys->pthread_setname_np(attr, name);
}

int pthread_join(pthread_t thread, void **retval)
{
	return sys->pthread_join(thread, retval);
}

int pthread_detach(pthread_t thread)
{
	return sys->pthread_detach(thread);
}