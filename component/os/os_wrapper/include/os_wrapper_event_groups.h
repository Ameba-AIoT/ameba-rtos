/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_EVENT_GROUPS_H__
#define __OS_WRAPPER_EVENT_GROUPS_H__

/**
 * @brief  event group handle type
 */
typedef void *rtos_event_group_t;

/**
 * @brief  event bits type
 */
typedef uint32_t rtos_event_bits_t;

/**
 * @brief  Create a new event group.
 * Wrapper for FreeRTOS API: xEventGroupCreate
 * @retval If the event group was created then a handle to the event group is
 * returned.  If there was insufficient heap available to create the
 * event group then NULL is returned.
 */
rtos_event_group_t rtos_event_group_create(void);

/**
 * @brief  Delete an event group.
 * Wrapper for FreeRTOS API: vEventGroupDelete
 * @param  xEventGroup:
 */
void rtos_event_group_delete(rtos_event_group_t xEventGroup);

/**
 * @brief  block to wait for one or more bits to be set within a previously created event group.
 * Wrapper for FreeRTOS API: xEventGroupWaitBits
 * @param  xEventGroup: The event group in which the bits are being tested.
 * @param  uxBitsToWaitFor: A bitwise value that indicates the bit or bits to test inside the event group.
 * @param  xClearOnExit: the event group will be cleared or not
 * @param  xWaitForAllBits: all or anyone of the bits in uxBitsToWaitFor
 * @param  MsToWait: maximum amount of time (ms) to wait
 * @retval The value of the event group at the time either the bits being waited
 * for became set, or the block time expired.
 */
rtos_event_bits_t rtos_event_group_wait_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToWaitFor, const long xClearOnExit,
		const long xWaitForAllBits, uint32_t MsToWait);

/**
 * @brief  Clear bits within an event group.
 * Wrapper for FreeRTOS API: xEventGroupWaitBits and xEventGroupClearBitsFromISR
 * @param  xEventGroup: The event group in which the bits are to be cleared.
 * @param  uxBitsToClear: A bitwise value that indicates the bit or bits to clear.
 * @retval The value of the event group before the specified bits were cleared.
 */
rtos_event_bits_t rtos_event_group_clear_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToClear);

/**
 * @brief  Clear bits within an event group from ISR.
 * Wrapper for FreeRTOS API: xEventGroupClearBitsFromISR
 * @param  xEventGroup: The event group in which the bits are to be cleared.
 * @param  uxBitsToClear: A bitwise value that indicates the bit or bits to clear.
 * @retval If the request to execute the function was posted successfully then
 * RTK_SUCCESS is returned, otherwise RTK_FAIL is returned. RTK_FAIL will be returned
 * if the timer service queue was full.
 */
int rtos_event_group_clear_bits_from_isr(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToClear);

/**
 * @brief  Set bits within an event group.
 * Wrapper for FreeRTOS API: xEventGroupSetBits
 * @param  xEventGroup: The event group in which the bits are to be set.
 * @param  uxBitsToSet: A bitwise value that indicates the bit or bits to set.
 * @retval The value of the event group at the time the call to
 * rtos_event_group_set_bits() returns.
 */
rtos_event_bits_t rtos_event_group_set_bits(rtos_event_group_t xEventGroup,
		const rtos_event_bits_t uxBitsToSet);

/**
 * @brief  Set bits within an event group from ISR.
 * Wrapper for FreeRTOS API: xEventGroupSetBitsFromISR
 * @param  xEventGroup: The event group in which the bits are to be set.
 * @param  uxBitsToSet: A bitwise value that indicates the bit or bits to set.
 * @retval If the request to execute the function was posted successfully then
 * RTK_SUCCESS is returned, otherwise RTK_FAIL is returned.  RTK_FAIL will be returned
 * if the timer service queue was full.
 */
int rtos_event_group_set_bits_from_isr(rtos_event_group_t xEventGroup,
									   const rtos_event_bits_t uxBitsToSet);

/**
 * @brief  Returns the current value of the bits in an event group.  This function
 * cannot be used from an interrupt.
 * Wrapper for FreeRTOS API: xEventGroupGetBits
 * @param  xEventGroup: The event group being queried.
 * @retval The event group bits at the time rtos_event_group_get_bits() was called.
 */
rtos_event_bits_t rtos_event_group_get_bits(rtos_event_group_t xEventGroup);

/**
 * @brief  Atomically set bits within an event group, then wait for a combination of
 * bits to be set within the same event group.
 * Wrapper for FreeRTOS API: xEventGroupSync
 * @param  xEventGroup: The event group in which the bits are being tested.
 * @param  uxBitsToSet: The bits to set in the event group before determining uxBitsToWaitFor
 * @param  uxBitsToWaitFor: A bitwise value that indicates the bit or bits to test
 * inside the event group.
 * @param  MsToWait: maximum amount of time (ms) to wait
 * @retval The value of the event group at the time either the all uxBitsToWaitFor bits being waited
 * for became set, or the block time expired.
 */
rtos_event_bits_t rtos_event_group_sync(rtos_event_group_t xEventGroup, const rtos_event_bits_t uxBitsToSet,
										const rtos_event_bits_t uxBitsToWaitFor, uint32_t MsToWait);

#endif
