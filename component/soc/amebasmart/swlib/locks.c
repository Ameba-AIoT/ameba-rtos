/*
*  Routines to access hardware
*
*  Copyright (c) 2013 Realtek Semiconductor Corp.
*
*  This module is a confidential and proprietary property of RealTek and
*  possession or use of this module requires written permission of RealTek.
*/

#include <sys/lock.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "os_wrapper.h"
#include "log.h"

StaticSemaphore_t __lock___sinit_recursive_mutex;
StaticSemaphore_t __lock___sfp_recursive_mutex;
StaticSemaphore_t __lock___atexit_recursive_mutex;
StaticSemaphore_t __lock___at_quick_exit_mutex;
StaticSemaphore_t __lock___malloc_recursive_mutex;
StaticSemaphore_t __lock___env_recursive_mutex;
StaticSemaphore_t __lock___tz_mutex;
StaticSemaphore_t __lock___dd_hash_mutex;
StaticSemaphore_t __lock___arc4random_mutex;

static const char *const TAG = "LOCKS";


// __attribute__((constructor))
static void init_retarget_locks(void)
{
#ifdef CONFIG_ARM_CORE_CA32
	RTK_LOGI(TAG, "CA32 %s\n", __func__);
#elif defined CONFIG_ARM_CORE_CM4
	RTK_LOGI(TAG, "KM4 %s\n", __func__);
#else
	RTK_LOGI(TAG, "KM0 %s\n", __func__);
#endif
#if(configSUPPORT_STATIC_ALLOCATION == 1)
	xSemaphoreCreateRecursiveMutexStatic(&__lock___sinit_recursive_mutex);
	xSemaphoreCreateRecursiveMutexStatic(&__lock___sfp_recursive_mutex);
	xSemaphoreCreateRecursiveMutexStatic(&__lock___atexit_recursive_mutex);
	xSemaphoreCreateMutexStatic(&__lock___at_quick_exit_mutex);
	//    xSemaphoreCreateRecursiveMutexStatic(&__lock___malloc_recursive_mutex);  // see below
	xSemaphoreCreateRecursiveMutexStatic(&__lock___env_recursive_mutex);
	xSemaphoreCreateMutexStatic(&__lock___tz_mutex);
	xSemaphoreCreateMutexStatic(&__lock___dd_hash_mutex);
	xSemaphoreCreateMutexStatic(&__lock___arc4random_mutex);
#endif
}

// Special case for malloc/free. Without this, the full
// malloc_recursive_mutex would be used, which is much slower.
//
// void __malloc_lock(struct _reent *r)
// {
// 	(void)r;
// 	rtos_critical_enter(RTOS_CRITICAL_SOC);
// }

// void __malloc_unlock(struct _reent *r)
// {
// 	(void)r;
// 	rtos_critical_exit(RTOS_CRITICAL_SOC);
// }

void __retarget_lock_init(_LOCK_T *lock_ptr)
{
	if (*lock_ptr) {
		/* Lock already initialized */
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s, lock_ptr %p is already initialized!!!\n", __func__, *lock_ptr);
		return;
	}

	*lock_ptr = (_LOCK_T)xSemaphoreCreateMutex();

	if (*lock_ptr == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock_ptr create failed!!!\n", __func__);
	}
}

void __retarget_lock_init_recursive(_LOCK_T *lock_ptr)
{
	if (*lock_ptr) {
		/* Lock already initialized */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock_ptr %p is already initialized!!!\n", __func__, *lock_ptr);
		return;
	}

	*lock_ptr = (_LOCK_T)xSemaphoreCreateRecursiveMutex();

	if (*lock_ptr == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock_ptr create failed!!!\n", __func__);
	}
}

void __retarget_lock_close(_LOCK_T lock)
{
	if (lock) {
		vSemaphoreDelete(lock);
	}
}

void __retarget_lock_close_recursive(_LOCK_T lock)
{
	if (lock) {
		if (xSemaphoreGetMutexHolder((QueueHandle_t)lock) == NULL) {
			vSemaphoreDelete(lock);
		}
	}
}

void __retarget_lock_acquire(_LOCK_T lock)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		return;
	}

	if (!lock) {
		__retarget_lock_init(&lock);
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreTakeFromISR(lock, &task_woken);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire from isr failed!!!\n", __func__, lock);
			return;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreTake((QueueHandle_t)lock, portMAX_DELAY);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire failed!!!\n", __func__, lock);
		}
	}
}

void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
	BaseType_t ret;
	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		// DiagPrintf("scheduler not start\n");
		return;
	}

	if (rtos_critical_is_in_interrupt()) {
		return;
	}

	if (!lock) {
		__retarget_lock_init_recursive(&lock);
	}

	ret = xSemaphoreTakeRecursive((QueueHandle_t)lock, portMAX_DELAY);
	if (ret != pdTRUE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire failed!!!\n", __func__, lock);
	}
}

int __retarget_lock_try_acquire(_LOCK_T lock)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		return 0;
	}

	if (!lock) {
		__retarget_lock_init(&lock);
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreTakeFromISR(lock, &task_woken);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire from isr failed!!!\n", __func__, lock);
			return -1;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreTake((QueueHandle_t)lock, 0);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire failed!!!\n", __func__, lock);
		}
	}

	return (ret == pdTRUE) ? 0 : -1;
}

int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
	BaseType_t ret;

	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		return 0;
	}

	if (rtos_critical_is_in_interrupt()) {
		return 0;
	}

	if (!lock) {
		__retarget_lock_init_recursive(&lock);
	}

	ret = xSemaphoreTakeRecursive((QueueHandle_t)lock, 0);
	if (ret != pdTRUE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p acquire failed!!!\n", __func__, lock);
	}
	return (ret == pdTRUE) ? 0 : -1;
}

void __retarget_lock_release(_LOCK_T lock)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		return;
	}

	if (!lock) {
		return;
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xSemaphoreGiveFromISR(lock, &task_woken);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p release from isr failed!!!\n", __func__, lock);
			return;
		}
		portEND_SWITCHING_ISR(task_woken);
	} else {
		ret = xSemaphoreGive(lock);
		if (ret != pdTRUE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p release failed!!!\n", __func__, lock);
		}
	}
}

void __retarget_lock_release_recursive(_LOCK_T lock)
{
	BaseType_t ret;
	if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		return;
	}

	if (rtos_critical_is_in_interrupt()) {
		return;
	}

	if (!lock) {
		return;
	}

	ret = xSemaphoreGiveRecursive((QueueHandle_t)lock);
	if (ret != pdTRUE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s, lock %p release failed!!!\n", __func__, lock);
	}
}

void newlib_locks_init(void)
{
	init_retarget_locks();
}
