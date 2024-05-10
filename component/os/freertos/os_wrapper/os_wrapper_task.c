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

int rtos_sched_start(void)
{
	vTaskStartScheduler();
	return SUCCESS;
}

int rtos_sched_stop(void)
{
	vTaskEndScheduler();
	return SUCCESS;
}

int rtos_sched_suspend(void)
{
	vTaskSuspendAll();
	return SUCCESS;
}

int rtos_sched_resume(void)
{
	xTaskResumeAll();
	return SUCCESS;
}

int rtos_sched_get_state(void)
{
	int status = FAIL;

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
					 void *p_param, uint16_t stack_size_in_byte, uint16_t priority)
{
	BaseType_t ret;

	ret = xTaskCreate(p_routine, (const char *)p_name, stack_size_in_byte / sizeof(portSTACK_TYPE),
					  p_param, priority, (TaskHandle_t *)pp_handle);
	if (ret == pdPASS) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_task_delete(rtos_task_t p_handle)
{
	vTaskDelete((TaskHandle_t)p_handle);
	return SUCCESS;
}

int rtos_task_suspend(rtos_task_t p_handle)
{
	vTaskSuspend((TaskHandle_t)p_handle);
	return SUCCESS;
}

int rtos_task_resume(rtos_task_t p_handle)
{
	vTaskResume((TaskHandle_t)p_handle);
	return SUCCESS;
}

int rtos_task_yield(void)
{
	taskYIELD();
	return SUCCESS;
}

rtos_task_t rtos_task_handle_get(void)
{
	return (rtos_task_t)xTaskGetCurrentTaskHandle();
}

uint32_t rtos_task_priority_get(rtos_task_t p_handle)
{
	return (uint32_t)uxTaskPriorityGet((TaskHandle_t)p_handle);
}

int rtos_task_priority_set(rtos_task_t p_handle, uint16_t priority)
{
	vTaskPrioritySet((TaskHandle_t)p_handle, priority);

	return SUCCESS;
}

void rtos_create_secure_context(uint32_t size)
{
	(void) size;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	/* This task calls secure side functions. So allocate a secure context for it. */
	portALLOCATE_SECURE_CONTEXT(size);
#endif
}
