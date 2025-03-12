/**
 ******************************************************************************
 * @file    syscall.h
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
*/

#ifndef _SYSCALL_POSIX_H_
#define _SYSCALL_POSIX_H_

typedef void *sys_pthread_t;
typedef unsigned int sys_pthread_key_t;

int syscall_clock_gettime(int clock_id, void *tp);
int syscall_nanosleep(const void *rqtp, void *rmtp);

sys_pthread_t syscall_pthread_self(void);
void syscall_pthread_exit(void *value_ptr);

int syscall_pthread_attr_init(void *attr);
int syscall_pthread_attr_destroy(void *attr);
int syscall_pthread_attr_setschedpolicy(void *attr, int policy);
int syscall_pthread_attr_getschedpolicy(void const *attr, int *policy);
int syscall_pthread_attr_setstacksize(void *attr, size_t stack_size);
int syscall_pthread_attr_getstacksize(void const *attr, size_t *stack_size);
int syscall_pthread_attr_setdetachstate(void *attr, int state);
int syscall_pthread_attr_getdetachstate(void const *attr, int *state);
int syscall_pthread_attr_setschedparam(void *attr, void const *param);
int syscall_pthread_attr_getschedparam(void const *attr, void *param);
int syscall_pthread_attr_setpriority(void *attr, uint8_t priority);
int syscall_pthread_create(sys_pthread_t *tid, const void *attr, void *(*start)(void *), void *arg, uint32_t got_base_addr);
int syscall_pthread_mutex_init(void *mutex, const void *attr);
int syscall_pthread_mutex_destroy(void *mutex);
int syscall_pthread_mutex_lock(void *mutex);
int syscall_pthread_mutex_unlock(void *mutex);

int syscall_pthread_cond_init(void *cond, void *attr);
int syscall_pthread_cond_destroy(void *cond);
int syscall_pthread_cond_signal(void *cond);
int syscall_pthread_cond_wait(void *cond, void *mutex);
int syscall_pthread_cond_timedwait(void *cond, void *mutex, void *abstime);

int syscall_pthread_key_create(sys_pthread_key_t *key, void (*destructor)(void *));
int syscall_pthread_key_delete(sys_pthread_key_t key);
int syscall_pthread_setspecific(sys_pthread_key_t key, const void *value);
void *syscall_pthread_getspecific(sys_pthread_key_t key);
void syscall_pthread_clrspecific(sys_pthread_key_t key);

int syscall_pthread_setname_np(sys_pthread_t thread, const char *name);
int syscall_pthread_join(sys_pthread_t thread, void **retval);
int syscall_pthread_detach(sys_pthread_t thread);
uint32_t syscall_rand(void);
#endif