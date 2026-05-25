/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_MUTEX_H
#define ZEPHYR_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr_time.h>

/**
 * Mutex Structure
 * @ingroup mutex_apis
 */
struct k_mutex {
	void *p_handle;
};


#define Z_MUTEX_INITIALIZER(obj) \
	{ \
	.p_handle = NULL, \
	}

#define K_MUTEX_DEFINE(name) \
	STRUCT_SECTION_ITERABLE(k_mutex, name) = \
		Z_MUTEX_INITIALIZER(name)

int k_mutex_init(struct k_mutex *mutex);
int k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout);
int k_mutex_unlock(struct k_mutex *mutex);
int k_mutex_deinit(struct k_mutex *mutex);


#ifdef __cplusplus
}
#endif

#endif
