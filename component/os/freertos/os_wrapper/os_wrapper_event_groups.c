/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <assert.h>
#include "ameba.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

static_assert(sizeof(rtos_event_bits_t) == sizeof(EventBits_t), "sizeof(rtos_event_bits_t) and sizeof(EventBits_t) missmatch");


rtos_event_group_t rtos_event_group_create(void)
{
	return (rtos_event_group_t) xEventGroupCreate();
}

void rtos_event_group_delete(rtos_event_group_t xEventGroup)
{
	vEventGroupDelete((EventGroupHandle_t) xEventGroup);
}

rtos_event_bits_t rtos_event_group_wait_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToWaitFor, const long xClearOnExit,
		const long xWaitForAllBits, uint32_t MsToWait)
{
	return (rtos_event_bits_t) xEventGroupWaitBits((EventGroupHandle_t) xEventGroup,
			(const EventBits_t) uxBitsToWaitFor,
			(const BaseType_t) xClearOnExit,
			(const BaseType_t) xWaitForAllBits,
			(TickType_t) RTOS_CONVERT_MS_TO_TICKS(MsToWait));
}

int rtos_event_group_clear_bits(rtos_event_group_t xEventGroup,
								const rtos_event_bits_t uxBitsToClear)
{
	BaseType_t ret = RTK_FAIL;
	if (rtos_critical_is_in_interrupt()) {
		ret = xEventGroupClearBitsFromISR((EventGroupHandle_t) xEventGroup,
										  (const EventBits_t) uxBitsToClear);
		/* xEventGroupClearBitsFromISR returns pdPASS on success, pdFAIL if timer queue is full */
		return (ret == pdPASS) ? RTK_SUCCESS : RTK_FAIL;
	} else {
		/* xEventGroupClearBits returns the previous value of event bits, not a status code.
		 * The operation itself cannot fail in task context, so we always return success. */
		(void) xEventGroupClearBits((EventGroupHandle_t) xEventGroup,
									(const EventBits_t) uxBitsToClear);
		return RTK_SUCCESS;
	}
}

int rtos_event_group_set_bits(rtos_event_group_t xEventGroup,
							  const rtos_event_bits_t uxBitsToSet)
{
	BaseType_t ret = RTK_FAIL;
	BaseType_t task_woken = pdFALSE;

	if (rtos_critical_is_in_interrupt()) {
		ret = xEventGroupSetBitsFromISR((EventGroupHandle_t) xEventGroup,
										(const EventBits_t) uxBitsToSet, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
		/* xEventGroupSetBitsFromISR returns pdPASS on success, pdFAIL if timer queue is full */
		return (ret == pdPASS) ? RTK_SUCCESS : RTK_FAIL;
	} else {
		/* xEventGroupSetBits returns the new value of event bits, not a status code.
		 * The operation itself cannot fail in task context, so we always return success. */
		(void) xEventGroupSetBits((EventGroupHandle_t) xEventGroup,
								  (const EventBits_t) uxBitsToSet);
		return RTK_SUCCESS;
	}
}

rtos_event_bits_t rtos_event_group_get_bits(rtos_event_group_t xEventGroup)
{
	if (rtos_critical_is_in_interrupt()) {
		return (rtos_event_bits_t) xEventGroupGetBitsFromISR((EventGroupHandle_t) xEventGroup);
	} else {
		return (rtos_event_bits_t) xEventGroupGetBits((EventGroupHandle_t) xEventGroup);
	}
}

rtos_event_bits_t rtos_event_group_sync(rtos_event_group_t xEventGroup, const rtos_event_bits_t uxBitsToSet,
										const rtos_event_bits_t uxBitsToWaitFor, uint32_t MsToWait)
{
	return (rtos_event_bits_t) xEventGroupSync((EventGroupHandle_t) xEventGroup, (const EventBits_t) uxBitsToSet,
			(const EventBits_t) uxBitsToWaitFor, (TickType_t) RTOS_CONVERT_MS_TO_TICKS(MsToWait));
}
