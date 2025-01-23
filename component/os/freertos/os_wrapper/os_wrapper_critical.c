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

#if (RTOS_NUM_CORES == 1)
#define portGET_CORE_ID()		0
#endif

static uint32_t uxSavedInterruptStatus[RTOS_NUM_CORES] = {0};
static uint16_t uxCriticalNestingCnt[RTOS_NUM_CORES] = {0};

#ifndef CONFIG_ARM_CORE_CA32
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
	return plic_get_active_irq_id() != 0;
#else
	return __get_IPSR() != 0;
#endif

#endif
#endif
}

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
#include "spinlock.h"
static spinlock_t rtos_critical_spin_lock_list[RTOS_CRITICAL_MAX];
#endif

uint32_t GetOSCriticalNesting(uint32_t xCoreID);

uint32_t GetComponentCriticalNesting(uint32_t xCoreID)
{
	return uxCriticalNestingCnt[xCoreID];
}

void rtos_critical_enter(uint32_t component_id)
{
	assert_param(component_id < RTOS_CRITICAL_MAX);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	uint32_t core_id = portGET_CORE_ID();
	portDISABLE_INTERRUPTS();
	spin_lock(&rtos_critical_spin_lock_list[component_id]);
	uxCriticalNestingCnt[core_id] ++;
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
	assert_param(component_id < RTOS_CRITICAL_MAX);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	uint32_t core_id = portGET_CORE_ID();
	uxCriticalNestingCnt[core_id] --;
	spin_unlock(&rtos_critical_spin_lock_list[component_id]);

	/* before enable interrupt, OS critical nesting must return to 0 */
	if (GetComponentCriticalNesting(core_id) == 0 && GetOSCriticalNesting(core_id) == 0) {
		portENABLE_INTERRUPTS();
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

void rtos_critical_enter_old(void)
{
	if (rtos_critical_is_in_interrupt()) {
		portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

		if (uxCriticalNestingCnt[portGET_CORE_ID()] == 0U) {
			uxSavedInterruptStatus[portGET_CORE_ID()] = portSET_INTERRUPT_MASK_FROM_ISR();
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
			portGET_TASK_LOCK();
#endif
		}
		uxCriticalNestingCnt[portGET_CORE_ID()]++;
	} else {
		taskENTER_CRITICAL();
	}
}

void rtos_critical_exit_old(void)
{
	if (rtos_critical_is_in_interrupt()) {
		uxCriticalNestingCnt[portGET_CORE_ID()]--;
		if (uxCriticalNestingCnt[portGET_CORE_ID()] == 0U) {
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
			portRELEASE_TASK_LOCK();
#endif
			portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus[portGET_CORE_ID()]);
		}
	} else {
		taskEXIT_CRITICAL();
	}
}

uint32_t rtos_get_critical_state(void)
{
#ifndef CONFIG_ARM_CORE_CA32
	return xPortGetCriticalState();
#endif
	return 0;
}
