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

int rtos_critical_is_in_interrupt(void)
{
#ifdef ARM_CORE_CA32
	return (__get_mode() != CPSR_M_USR) && (__get_mode() != CPSR_M_SYS);
#else
#if defined(__ICCARM__)
	return (__get_PSR() & 0x1FF) != 0;
#elif defined(__GNUC__)

#ifdef ARM_CORE_CM4
	return (__get_xPSR() & 0x1FF) != 0;
#elif defined(RSICV_CORE_KR4)
	return plic_get_active_irq_id() != 0;
#else
	return __get_IPSR() != 0;
#endif

#endif
#endif
}

void rtos_critical_enter(void)
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

void rtos_critical_exit(void)
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
