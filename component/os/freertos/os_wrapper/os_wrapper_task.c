/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "FreeRTOS.h"
#include "task.h"
#include "os_wrapper.h"
#include <assert.h>

static_assert(RTOS_TASK_MAX_PRIORITIES == configMAX_PRIORITIES, "Incorrect RTOS_TASK_MAX_PRIORITIES value config");
static_assert(RTOS_MINIMAL_SECURE_STACK_SIZE == configMINIMAL_SECURE_STACK_SIZE, "Incorrect RTOS_MINIMAL_SECURE_STACK_SIZE value config");
static_assert(sizeof(rtos_time_out_t) == sizeof(TimeOut_t), "sizeof(rtos_time_out_t) and sizeof(TimeOut_t) missmatch");

int rtos_sched_start(void)
{
	vTaskStartScheduler();
	return RTK_SUCCESS;
}

int rtos_sched_stop(void)
{
	vTaskEndScheduler();
	return RTK_SUCCESS;
}

int rtos_sched_suspend(void)
{
	vTaskSuspendAll();
	return RTK_SUCCESS;
}

int rtos_sched_resume(void)
{
	xTaskResumeAll();
	return RTK_SUCCESS;
}

int rtos_sched_get_state(void)
{
	int status = RTK_FAIL;

	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
		status = RTOS_SCHED_NOT_STARTED;
	} else if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
		status = RTOS_SCHED_SUSPENDED;
	} else if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
		status = RTOS_SCHED_RUNNING;
	}

	return status;
}

int rtos_task_create(rtos_task_t *pp_handle, const char *p_name, void (*p_routine)(void *),
					 void *p_param, size_t stack_size_in_byte, uint16_t priority)
{
	BaseType_t ret;
#if defined (CONFIG_HEAP_PROTECTOR)
	/* if enable heap trace, we need to increase heap size */
	stack_size_in_byte += 1024;
#endif
	ret = xTaskCreate(p_routine, (const char *)p_name, stack_size_in_byte / sizeof(portSTACK_TYPE),
					  p_param, priority, (TaskHandle_t *)pp_handle);
	if (ret == pdPASS) {
		return RTK_SUCCESS;
	} else {
		return RTK_FAIL;
	}
}

int rtos_task_delete(rtos_task_t p_handle)
{
	vTaskDelete((TaskHandle_t)p_handle);
	return RTK_SUCCESS;
}

int rtos_task_suspend(rtos_task_t p_handle)
{
	vTaskSuspend((TaskHandle_t)p_handle);
	return RTK_SUCCESS;
}

int rtos_task_resume(rtos_task_t p_handle)
{
	vTaskResume((TaskHandle_t)p_handle);
	return RTK_SUCCESS;
}

int rtos_task_yield(void)
{
	taskYIELD();
	return RTK_SUCCESS;
}

rtos_task_t rtos_task_handle_get(void)
{
	return (rtos_task_t)xTaskGetCurrentTaskHandle();
}

char *rtos_task_name_get(rtos_task_t p_handle)
{
	return pcTaskGetName((TaskHandle_t)p_handle);
}

uint32_t rtos_task_priority_get(rtos_task_t p_handle)
{
	return (uint32_t)uxTaskPriorityGet((TaskHandle_t)p_handle);
}

int rtos_task_priority_set(rtos_task_t p_handle, uint16_t priority)
{
	vTaskPrioritySet((TaskHandle_t)p_handle, priority);

	return RTK_SUCCESS;
}

void rtos_task_set_time_out_state(rtos_time_out_t *const p_rtos_time_out)
{
	vTaskSetTimeOutState((TimeOut_t *const)p_rtos_time_out);
}

int rtos_task_check_for_time_out(rtos_time_out_t *const p_rtos_time_out, uint32_t *p_ms_to_wait)
{
	uint32_t ticks_to_wait = RTOS_TIME_SET_MS_TO_SYSTIME(*p_ms_to_wait);
	int ret = xTaskCheckForTimeOut((TimeOut_t *const)p_rtos_time_out, &ticks_to_wait);
	*p_ms_to_wait = RTOS_TIME_SET_SYSTIME_TO_MS(ticks_to_wait);

	if (ret == pdTRUE) {
		/* no block time remains, and a timeout has occurred */
		return TRUE;
	} else {
		/* some block time remains (pass out by p_ms_to_wait), so a timeout has not occurred. */
		return FALSE;
	}
}

/* FreeRTOS version must be greater than 10.2.0 */
void rtos_create_secure_context(uint32_t size)
{
	(void) size;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	/* This task calls secure side functions. So allocate a secure context for it. */
	portALLOCATE_SECURE_CONTEXT(size);
#endif
}

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
// Each task contains an array of pointers that is dimensioned by the configNUM_THREAD_LOCAL_STORAGE_POINTERS setting in FreeRTOSConfig.h.
void rtos_task_set_thread_local_storage_pointer(rtos_task_t p_handle,  uint16_t index, void *p_param)
{
	vTaskSetThreadLocalStoragePointer(p_handle, index, p_param);
}

void *rtos_task_get_thread_local_storage_pointer(rtos_task_t p_handle,  uint16_t index)
{
	return pvTaskGetThreadLocalStoragePointer(p_handle, index);
}
#endif
