/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "ameba_pmu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"
#include <assert.h>

static_assert(RTOS_TICK_RATE_HZ == configTICK_RATE_HZ, "Incorrect RTOS_TICK_RATE_HZ value config");

/* defines for systick timer */
#if (defined ARM_CORE_CM4) || (defined ARM_CORE_CM0)
#define portNVIC_SYSTICK_LOAD_REG                 ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG        ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#endif

#if defined RSICV_CORE_KR4
extern size_t uxTimerIncrementsForOneTick;
extern volatile uint64_t *pullMachineTimerCompareRegister;
extern volatile uint64_t *pullMachineTimerCounterRegister;
#endif

#if defined ARM_CORE_CA32
extern uint64_t GenericTimerFreq;
#define pdTICKS_TO_CNT	(GenericTimerFreq/RTOS_TICK_RATE_HZ)
#endif

void rtos_time_delay_ms(uint32_t ms)
{
	if (pmu_yield_os_check() && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) && (!rtos_critical_is_in_interrupt())) {
		vTaskDelay(RTOS_CONVERT_MS_TO_TICKS(ms));
	} else {
		DelayMs(ms);
	}
}

void rtos_time_delay_us(uint32_t us)
{
	DelayUs(us);
}

uint32_t rtos_time_get_current_system_time_ms(void)
{
	if (rtos_critical_is_in_interrupt()
#if defined CONFIG_AMEBALITE
		|| (!pmu_yield_os_check())	// for Kr4, which use cpsid directly
#endif
	   ) {
		return (uint32_t)(xTaskGetTickCountFromISR() * portTICK_PERIOD_MS);
	} else {
		return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
	}
}

uint32_t rtos_time_get_current_system_time_us(void)
{
	return (rtos_time_get_current_system_time_ns() / 1000);
}

uint64_t rtos_time_get_current_system_time_ns(void)
{
	uint64_t time_ns = 0;
	rtos_critical_enter();
	uint32_t time_ms = rtos_time_get_current_system_time_ms();
#if defined ARM_CORE_CM4 || defined ARM_CORE_CM0
	uint64_t timer_cnt_pass = portNVIC_SYSTICK_LOAD_REG - portNVIC_SYSTICK_CURRENT_VALUE_REG;
	uint32_t time_pass_ns = (timer_cnt_pass * 1000) / (configCPU_CLOCK_HZ / 1000000UL);
	if (__NVIC_GetPendingIRQ(SysTick_IRQn)) {
		time_ms ++;
	}
	time_ns = time_ms * 1000000ULL + time_pass_ns;
#elif defined RSICV_CORE_KR4
	uint64_t pass_value = 0;
	uint64_t compare_value = *pullMachineTimerCompareRegister;
	uint64_t current_value = *pullMachineTimerCounterRegister;
	if (compare_value >= current_value) {
		pass_value = uxTimerIncrementsForOneTick - (compare_value - current_value);
	} else {
		pass_value = uxTimerIncrementsForOneTick + (current_value - compare_value);
	}
	time_ns = time_ms * 1000000ULL + (pass_value * 1000) / (configCPU_CLOCK_HZ / 1000000UL);
#elif defined ARM_CORE_CA32
	uint64_t pass_value = 0;
	uint64_t compare_value = arm_arch_timer_compare();
	uint64_t current_value = arm_arch_timer_count();
	if (compare_value >= current_value) {
		pass_value = pdTICKS_TO_CNT - (compare_value - current_value);
	} else {
		pass_value = pdTICKS_TO_CNT + (current_value - compare_value);
	}
	time_ns = time_ms * 1000000ULL + (pass_value * 1000) / 50;  // arm_arch_timer_count is 50 Mhz
#else
#error "Undefined core"
#endif
	rtos_critical_exit();
	return time_ns;
}
