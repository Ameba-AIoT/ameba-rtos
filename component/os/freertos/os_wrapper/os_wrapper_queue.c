/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "ameba.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

int rtos_queue_create(rtos_queue_t *pp_handle, uint32_t msg_num, uint32_t msg_size)
{
	if (pp_handle == NULL) {
		return FAIL;
	}

	*pp_handle = (rtos_queue_t)xQueueCreate(msg_num, msg_size);
	if (*pp_handle != NULL) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_queue_delete(rtos_queue_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_queue_massage_waiting(p_handle) != 0) {
		RTK_LOGS(NOTAG, "[%s] %s <<< The queue is not empty, but the queue has been deleted. >>>\n", pcTaskGetTaskName(NULL), __FUNCTION__);
		vQueueDelete((QueueHandle_t)p_handle);
		return FAIL;
	} else {
		vQueueDelete((QueueHandle_t)p_handle);
		return SUCCESS;
	}
}

uint32_t rtos_queue_massage_waiting(rtos_queue_t p_handle)
{
	if (p_handle == NULL) {
		return FAIL;
	}
	if (rtos_critical_is_in_interrupt()) {
		return (uint32_t)uxQueueMessagesWaitingFromISR((QueueHandle_t)p_handle);
	} else {
		return (uint32_t)uxQueueMessagesWaiting((QueueHandle_t)p_handle);
	}
}

int rtos_queue_send(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xQueueSendToBackFromISR((QueueHandle_t)p_handle, p_msg, &task_woken);
		if (ret == pdTRUE) {
			portEND_SWITCHING_ISR(task_woken);
		}
	} else {
		ret = xQueueSendToBack((QueueHandle_t)p_handle, p_msg, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_queue_send_to_front(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xQueueSendToFrontFromISR((QueueHandle_t)p_handle, p_msg, &task_woken);
		if (ret == pdTRUE) {
			portEND_SWITCHING_ISR(task_woken);
		}
	} else {
		ret = xQueueSendToFront((QueueHandle_t)p_handle, p_msg, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_queue_receive(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xQueueReceiveFromISR((QueueHandle_t)p_handle, p_msg, &task_woken);
		if (ret == pdTRUE) {
			portEND_SWITCHING_ISR(task_woken);
		}
	} else {
		ret = xQueueReceive((QueueHandle_t)p_handle, p_msg, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

int rtos_queue_peek(rtos_queue_t p_handle, void *p_msg, uint32_t wait_ms)
{
	BaseType_t ret;

	if (p_handle == NULL) {
		return FAIL;
	}

	if (rtos_critical_is_in_interrupt()) {
		ret = xQueuePeekFromISR((QueueHandle_t)p_handle, p_msg);
	} else {
		ret = xQueuePeek((QueueHandle_t)p_handle, p_msg, RTOS_CONVERT_MS_TO_TICKS(wait_ms));
	}

	if (ret == pdTRUE) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}
