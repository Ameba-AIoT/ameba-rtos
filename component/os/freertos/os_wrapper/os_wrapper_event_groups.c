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

rtos_event_bits_t rtos_event_group_clear_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToClear)
{
	if (rtos_critical_is_in_interrupt()) {
		return (rtos_event_bits_t) xEventGroupClearBitsFromISR((EventGroupHandle_t) xEventGroup,
				(const EventBits_t) uxBitsToClear);
	} else {
		return (rtos_event_bits_t) xEventGroupClearBits((EventGroupHandle_t) xEventGroup,
				(const EventBits_t) uxBitsToClear);
	}
}

int rtos_event_group_clear_bits_from_isr(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToClear)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	ret = xEventGroupClearBitsFromISR((EventGroupHandle_t) xEventGroup,
									  (const EventBits_t) uxBitsToClear);
	portEND_SWITCHING_ISR(task_woken);

	if (ret == pdTRUE) {
		return RTK_SUCCESS;
	} else {
		return RTK_FAIL;
	}
}

rtos_event_bits_t rtos_event_group_set_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToSet)
{
	return (rtos_event_bits_t) xEventGroupSetBits((EventGroupHandle_t) xEventGroup,
			(const EventBits_t) uxBitsToSet);
}

int rtos_event_group_set_bits_from_isr(rtos_event_group_t xEventGroup,
									   const rtos_event_bits_t uxBitsToSet)
{
	BaseType_t ret;
	BaseType_t task_woken = pdFALSE;

	ret = xEventGroupSetBitsFromISR((EventGroupHandle_t) xEventGroup,
									(const EventBits_t) uxBitsToSet, &task_woken);
	portEND_SWITCHING_ISR(task_woken);

	if (ret == pdTRUE) {
		return RTK_SUCCESS;
	} else {
		return RTK_FAIL;
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
