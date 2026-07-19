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

#if defined CONFIG_CPUS_NUM
static_assert(configNUM_CORES == RTOS_NUM_CORES, "Incorrect RTOS_NUM_CORES value config");
#else
static_assert(1 == RTOS_NUM_CORES, "Incorrect RTOS_NUM_CORES value config");
#endif

#if (RTOS_NUM_CORES == 1) && !defined(portGET_CORE_ID)
#define portGET_CORE_ID()		0
#endif

static uint32_t uxSavedInterruptStatus[RTOS_NUM_CORES] = {0};
static uint16_t uxCriticalNestingCnt[RTOS_NUM_CORES] = {0};

#if !defined(RTOS_NUM_CORES) || (RTOS_NUM_CORES == 1)
uint32_t xPortGetCriticalState(void);
#endif

int rtos_critical_is_in_interrupt(void)
{
#ifdef CONFIG_ARM_CORE_CA32
	return (__get_mode() != CPSR_M_USR) && (__get_mode() != CPSR_M_SYS);
#else
#if defined(__ICCARM__)
	return (__get_PSR() & 0x1FF) != 0;
#elif defined(__GNUC__)

#ifdef CONFIG_ARM_CORE_CM4
	return (__get_xPSR() & 0x1FF) != 0;
#elif defined(CONFIG_RSICV_CORE_KR4)
	return CPU_InInterrupt() != 0;
#else
	return __get_IPSR() != 0;
#endif

#endif
#endif
}

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
#include "spinlock.h"
static spinlock_t rtos_critical_spin_lock_list[RTOS_CRITICAL_MAX];
static volatile uint16_t rtos_critical_spin_lock_nesting[RTOS_NUM_CORES][RTOS_CRITICAL_MAX];
#endif

uint32_t GetOSCriticalNesting(uint32_t xCoreID);

uint32_t GetComponentCriticalNesting(uint32_t xCoreID)
{
	return uxCriticalNestingCnt[xCoreID];
}

void rtos_critical_enter(uint32_t component_id)
{
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	UNUSED(uxSavedInterruptStatus);
	if (component_id >= RTOS_CRITICAL_MAX) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] component_id invalid\r\n", __func__);
	}

	/* Context-switch guard is implemented in two places:
	 * 1. portYIELD() macro (portmacro.h): when component nesting > 0, SWI is
	 *    suppressed and xYieldPendings is set instead, covering all SWI paths
	 *    ( vTaskYieldWithinAPI, vTaskExitCritical portYIELD).
	 * 2. prvCheckForRunStateChange() early return: when component nesting > 0
	 *    the IRQ window is not opened, covering all IRQ paths
	 *    ( vTaskEnterCritical, vTaskSuspendAll).
	 *
	 * vTaskSuspendAll() is therefore no longer needed here. */

	portDISABLE_INTERRUPTS();

	/* support the same core multi-times take a particular spin_lock */
	if (rtos_critical_spin_lock_nesting[portGET_CORE_ID()][component_id] == 0) {
		spin_lock(&rtos_critical_spin_lock_list[component_id]);
	}
	rtos_critical_spin_lock_nesting[portGET_CORE_ID()][component_id] ++;

	/* get core id after disable INT and get spin_lock */
	uxCriticalNestingCnt[portGET_CORE_ID()] ++;
#else
	UNUSED(component_id);
	/* Non-SMP env, keep privious actions */
	if (rtos_critical_is_in_interrupt()) {
		portASSERT_IF_INTERRUPT_PRIORITY_INVALID();
		if (uxCriticalNestingCnt[0] == 0U) {
			uxSavedInterruptStatus[0] = portSET_INTERRUPT_MASK_FROM_ISR();
		}
		uxCriticalNestingCnt[0]++;
	} else {
		taskENTER_CRITICAL();
	}
#endif
}

void rtos_critical_exit(uint32_t component_id)
{
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	UNUSED(uxSavedInterruptStatus);
	if (component_id >= RTOS_CRITICAL_MAX) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] component_id invalid\r\n", __func__);
	}

	uxCriticalNestingCnt[portGET_CORE_ID()] --;

	/* support the same core multi-times take a particular spin_lock */
	rtos_critical_spin_lock_nesting[portGET_CORE_ID()][component_id] --;
	if (rtos_critical_spin_lock_nesting[portGET_CORE_ID()][component_id] == 0) {
		spin_unlock(&rtos_critical_spin_lock_list[component_id]);
	}

	/* Before enabling interrupts, OS critical nesting must have returned to 0.
	 * After enabling, call vTaskYieldWithinAPI() to immediately service any
	 * xYieldPendings accumulated during this component critical section.
	 * vTaskYieldWithinAPI() checks OS uxCriticalNesting: if 0 it fires SWI
	 * directly (portYIELD), which will now pass the portYIELD macro guard
	 * since component nesting is 0; if > 0 it sets xYieldPendings for
	 * vTaskExitCritical to handle. Either way the deferred yield is consumed
	 * at the earliest safe opportunity, not delayed until the next IRQ. */
	if (GetComponentCriticalNesting(portGET_CORE_ID()) == 0) {
		extern volatile uint32_t uxPortSchedulerStart[configNUM_CORES];
		if (uxPortSchedulerStart[portPrimaryCoreID] == pdFALSE) {
			/* if Scheduler not start, pxCurrentTCBs is invalid */
			portENABLE_INTERRUPTS();
		} else {
			if (GetOSCriticalNesting(portGET_CORE_ID()) == 0) {
				portENABLE_INTERRUPTS();
			}

			if (!rtos_critical_is_in_interrupt()) {
				vTaskYieldWithinAPI();
			}
		}
	}

#else
	UNUSED(component_id);
	/* Non-SMP env, keep privious actions */
	if (rtos_critical_is_in_interrupt()) {
		uxCriticalNestingCnt[0]--;
		if (uxCriticalNestingCnt[0] == 0U) {
			portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus[0]);
		}
	} else {
		taskEXIT_CRITICAL();
	}
#endif
}

void __rtos_critical_enter_os(void)
{
	if (rtos_critical_is_in_interrupt()) {
		portASSERT_IF_INTERRUPT_PRIORITY_INVALID();
		/* Only mask interrupts on the first nesting level; deeper nestings just
		 * increment the counter. */
		if (uxCriticalNestingCnt[portGET_CORE_ID()] == 0U) {
			uxSavedInterruptStatus[portGET_CORE_ID()] = portSET_INTERRUPT_MASK_FROM_ISR();
		}
		uxCriticalNestingCnt[portGET_CORE_ID()]++;
	} else {
		taskENTER_CRITICAL();
	}
}

void __rtos_critical_exit_os(void)
{
	if (rtos_critical_is_in_interrupt()) {
		uxCriticalNestingCnt[portGET_CORE_ID()]--;
		if (uxCriticalNestingCnt[portGET_CORE_ID()] == 0U) {
			portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus[portGET_CORE_ID()]);
		}
	} else {
		taskEXIT_CRITICAL();
	}
}

uint32_t rtos_get_critical_state(void)
{
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* avoid wrong status when switching core right after getting core id */
	uint32_t flags = portSET_INTERRUPT_MASK_FROM_ISR();
	uint32_t xCoreID = portGET_CORE_ID();
	uint32_t ret = GetComponentCriticalNesting(xCoreID) || GetOSCriticalNesting(xCoreID);
	portCLEAR_INTERRUPT_MASK_FROM_ISR(flags);
	return ret;
#else
	return xPortGetCriticalState();
#endif
}
