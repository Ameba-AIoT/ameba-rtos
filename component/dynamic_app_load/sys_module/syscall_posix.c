/**
 ******************************************************************************
 * @file    syscall_posix.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/utils.h"
#include "syscall_posix.h"
#include "os_wrapper.h"

#ifdef KERNEL

TaskHandle_t pthread_gettaskhandle(pthread_t *tid);
extern uint32_t _rand(void);
int pthread_create_with_pic(pthread_t *thread,
							const pthread_attr_t *attr,
							void *(*startroutine)(void *),
							void *arg,
							size_t got_base_addr);

int syscall_clock_gettime(int clock_id, void *tp)
{
	return clock_gettime((clockid_t)clock_id, (struct timespec *)tp);
}

int syscall_nanosleep(const void *rqtp, void *rmtp)
{
	return nanosleep((const struct timespec *)rqtp, (struct timespec *)rmtp);
}

sys_pthread_t syscall_pthread_self(void)
{
	return pthread_self();
}

void syscall_pthread_exit(void *value_ptr)
{
	pthread_exit(value_ptr);
}

int syscall_pthread_attr_init(void *attr)
{
	return pthread_attr_init((pthread_attr_t *)attr);
}

int syscall_pthread_attr_destroy(void *attr)
{
	return pthread_attr_destroy((pthread_attr_t *)attr);
}

int syscall_pthread_attr_setschedpolicy(void *attr, int policy)
{
	return pthread_attr_setschedpolicy((pthread_attr_t *)attr, policy);
}

int syscall_pthread_attr_getschedpolicy(void const *attr, int *policy)
{
	(void)attr;
	(void)policy;
	// return pthread_attr_getschedpolicy((pthread_attr_t *)attr, policy);
	return 0;
}

int syscall_pthread_attr_setstacksize(void *attr, size_t stack_size)
{
	return pthread_attr_setstacksize((pthread_attr_t *)attr, stack_size);
}

int syscall_pthread_attr_getstacksize(void const *attr, size_t *stack_size)
{
	return pthread_attr_getstacksize((pthread_attr_t *)attr, stack_size);
}

int syscall_pthread_attr_setdetachstate(void *attr, int state)
{
	return pthread_attr_setdetachstate((pthread_attr_t *)attr, state);
}

int syscall_pthread_attr_getdetachstate(void const *attr, int *state)
{
	return pthread_attr_getdetachstate((pthread_attr_t *)attr, state);
}

int syscall_pthread_attr_setschedparam(void *attr, void const *param)
{
	return pthread_attr_setschedparam((pthread_attr_t *)attr, (struct sched_param *)param);
}

int syscall_pthread_attr_getschedparam(void const *attr, void *param)
{
	return pthread_attr_getschedparam((pthread_attr_t *)attr, (struct sched_param *)param);
}

int syscall_pthread_attr_setpriority(void *attr, uint8_t priority)
{
	(void)attr;
	(void)priority;
	return 0;
}

int syscall_pthread_create(sys_pthread_t *tid, const void *attr,
						   void *(*start)(void *), void *arg, uint32_t got_base_addr)
{
	return pthread_create_with_pic((pthread_t *)tid, (pthread_attr_t *)attr, start, arg, got_base_addr);
}

int syscall_pthread_mutex_init(void *mutex, const void *attr)
{
	return pthread_mutex_init((pthread_mutex_t *)mutex, (pthread_mutexattr_t *)attr);
}

int syscall_pthread_mutex_destroy(void *mutex)
{
	return pthread_mutex_destroy((pthread_mutex_t *)mutex);
}

int syscall_pthread_mutex_lock(void *mutex)
{
	return pthread_mutex_lock((pthread_mutex_t *)mutex);
}

int syscall_pthread_mutex_unlock(void *mutex)
{
	return pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

int syscall_pthread_cond_init(void *cond, void *attr)
{
	return pthread_cond_init((pthread_cond_t *)cond, (pthread_condattr_t *)attr);
}
int syscall_pthread_cond_destroy(void *cond)
{
	return pthread_cond_destroy((pthread_cond_t *)cond);
}
int syscall_pthread_cond_signal(void *cond)
{
	return pthread_cond_signal((pthread_cond_t *)cond);
}

int syscall_pthread_cond_wait(void *cond, void *mutex)
{
	return pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex);
}
int syscall_pthread_cond_timedwait(void *cond, void *mutex, void *abstime)
{
	return pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex, (const struct timespec *)abstime);
}

#define PTHREAD_KEYS_MAX 128
typedef struct key_data {
	void *data;
	void (*dtor)(void *);
	int used;
} key_data;

static sys_pthread_key_t key_list_sz = PTHREAD_KEYS_MAX;
static key_data *key_list = NULL;

int syscall_pthread_key_create(sys_pthread_key_t *key, void (*destructor)(void *))
{
	if (!key_list) {
		key_list = (key_data *)rtos_mem_calloc(key_list_sz, sizeof(key_data));
		if (!key_list) {
			return EAGAIN;
		}
	}

	sys_pthread_key_t k = 0;

	// Find an empty slot
	for (; k < key_list_sz; ++k) {
		if (!key_list[k].used) {
			goto set_key;
		}
	}

	if (k >= key_list_sz) {
		return ENOMEM;
	}

set_key:
	*key = k;
	key_list[k].used = 1;
	key_list[k].dtor = destructor;
	return 0;
}

int syscall_pthread_key_delete(sys_pthread_key_t key)
{
	if (key >= key_list_sz) {
		return EINVAL;
	}

	if (key_list[key].dtor && key_list[key].data) {
		key_list[key].dtor(key_list[key].data);
	}
	key_list[key].data = NULL;
	key_list[key].dtor = NULL;
	key_list[key].used = 0;

	return 0;
}

int syscall_pthread_setspecific(sys_pthread_key_t key, const void *value)
{
	if (key >= key_list_sz || !key_list[key].used) {
		return EINVAL;
	}
	key_list[key].data = (void *) value;
	return 0;
}
void *syscall_pthread_getspecific(sys_pthread_key_t key)
{
	if (key >= key_list_sz || !key_list[key].used) {
		return NULL;
	}
	return key_list[key].data;
}
void syscall_pthread_clrspecific(sys_pthread_key_t key)
{
	if (key >= key_list_sz || !key_list[key].used) {
		return;
	}
	key_list[key].data = NULL;
	key_list[key].dtor = NULL;
}

int syscall_pthread_setname_np(void *attr, const char *name)
{
	return pthread_attr_setname_np((pthread_attr_t *)attr, name);
}

int syscall_pthread_join(sys_pthread_t thread, void **retval)
{
	return pthread_join((pthread_t)thread, retval);
}
int syscall_pthread_detach(sys_pthread_t thread)
{
	return pthread_detach((pthread_t)thread);
}

uint32_t syscall_rand(void)
{
	return (uint32_t)_rand();
}

#endif