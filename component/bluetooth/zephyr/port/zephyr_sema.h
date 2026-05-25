/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_SEMA_H
#define ZEPHYR_SEMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr_time.h>
#include <zephyr/sys/dlist.h>

struct k_sem{
	void *p_handle;
	uint32_t count;
	uint32_t limit;
	sys_dlist_t poll_events;
};


#define Z_SEM_INITIALIZER(obj, initial_count, count_limit) \
	{ \
	.p_handle = NULL, \
	.count = initial_count, \
	.limit = count_limit, \
	}

#define K_SEM_MAX_LIMIT 0xFFFFFFFF

#define K_SEM_DEFINE(name, initial_count, count_limit) \
	STRUCT_SECTION_ITERABLE(k_sem, name) = \
		Z_SEM_INITIALIZER(name, initial_count, count_limit); \
	BUILD_ASSERT(((count_limit) != 0) && \
		     ((initial_count) <= (count_limit)) && \
			 ((count_limit) <= K_SEM_MAX_LIMIT));


int k_sem_init(struct k_sem *sem, unsigned int initial_count,unsigned int limit);
int k_sem_take(struct k_sem *sem, k_timeout_t timeout);
void k_sem_give(struct k_sem *sem);
uint32_t k_sem_count_get(struct k_sem *sem);
int k_sem_deinit(struct k_sem *sem);


#ifdef __cplusplus
}
#endif

#endif
