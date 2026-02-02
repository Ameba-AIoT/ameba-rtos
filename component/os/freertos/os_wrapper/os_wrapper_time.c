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

#ifndef CONFIG_PLATFORM_FREERTOS_ROM
static_assert(RTOS_TICK_RATE_HZ == configTICK_RATE_HZ, "Incorrect RTOS_TICK_RATE_HZ value config");
#endif

/* defines for systick timer */
#if (defined CONFIG_ARM_CORE_CM4) || (defined CONFIG_ARM_CORE_CM0)
#define portNVIC_SYSTICK_LOAD_REG                 ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG        ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#endif

#if defined CONFIG_RSICV_CORE_KR4
extern size_t uxTimerIncrementsForOneTick;
extern volatile uint64_t *pullMachineTimerCompareRegister;
extern volatile uint64_t *pullMachineTimerCounterRegister;
#endif

#if defined CONFIG_ARM_CORE_CA32
extern uint64_t GenericTimerFreq;
#define pdTICKS_TO_CNT	(GenericTimerFreq/RTOS_TICK_RATE_HZ)
#endif

TickType_t xTaskGetPendedTicks(void);
TickType_t xTaskGetPendedTicksFromISR(void);

void rtos_time_delay_ms(uint32_t ms)
{
	if (pmu_yield_os_check() &&
		(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) &&
		(!rtos_critical_is_in_interrupt()) &&
		// if INT is disabled, vTaskDelay will not work, and ca32 will exec swi cause task switch
		(rtos_get_critical_state() == 0)) {
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

uint64_t rtos_time_get_current_system_time_ms_64bit(void)
{
	static uint32_t last_ms = 0;
	static uint64_t overflow_count = 0;
	uint32_t current_ms;
	uint64_t total_ms;

	current_ms = rtos_time_get_current_system_time_ms();

	__rtos_critical_enter_os();

	if (current_ms < last_ms) {
		overflow_count++;
	}
	last_ms = current_ms;
	total_ms = ((uint64_t)overflow_count << 32) + current_ms;

	__rtos_critical_exit_os();

	return total_ms;
}

uint32_t rtos_time_get_current_pended_time_ms(void)
{
	if (rtos_critical_is_in_interrupt()
#if defined CONFIG_AMEBALITE
		|| (!pmu_yield_os_check())	// for Kr4, which use cpsid directly
#endif
	   ) {
		return (uint32_t)(xTaskGetPendedTicksFromISR() * portTICK_PERIOD_MS);
	} else {
		return (uint32_t)(xTaskGetPendedTicks() * portTICK_PERIOD_MS);
	}
}

uint64_t rtos_time_get_current_system_time_us(void)
{
	return (rtos_time_get_current_system_time_ns() / 1000);
}

uint64_t rtos_time_get_current_system_time_ns(void)
{
	uint64_t time_ns = 0;
	__rtos_critical_enter_os();
	uint64_t time_ms = rtos_time_get_current_system_time_ms_64bit();
#if defined CONFIG_ARM_CORE_CM4 || defined CONFIG_ARM_CORE_CM0
	uint64_t timer_cnt_pass = portNVIC_SYSTICK_LOAD_REG - portNVIC_SYSTICK_CURRENT_VALUE_REG;
	uint32_t time_pass_ns = (timer_cnt_pass * 1000) / (configCPU_CLOCK_HZ / 1000000UL);
	if (__NVIC_GetPendingIRQ(SysTick_IRQn)) {
		time_ms ++;
	}
	time_ns = time_ms * 1000000ULL + time_pass_ns;
#elif defined CONFIG_RSICV_CORE_KR4
	uint64_t pass_value = 0;
	uint64_t compare_value = *pullMachineTimerCompareRegister;
	uint64_t current_value = *pullMachineTimerCounterRegister;
	if (compare_value >= current_value) {
		pass_value = uxTimerIncrementsForOneTick - (compare_value - current_value);
	} else {
		pass_value = uxTimerIncrementsForOneTick + (current_value - compare_value);
	}
	time_ns = time_ms * 1000000ULL + (pass_value * 1000) / (configCPU_CLOCK_HZ / 1000000UL);
#elif defined CONFIG_ARM_CORE_CA32
	uint64_t pass_value = 0;
	uint64_t compare_value = arm_arch_timer_compare();
	uint64_t current_value = arm_arch_timer_count();
	if (compare_value >= current_value) {
		pass_value = pdTICKS_TO_CNT - (compare_value - current_value);
	} else {
		pass_value = pdTICKS_TO_CNT + (current_value - compare_value);
	}
	time_ns = time_ms * 1000000ULL + (pass_value * 1000000000 / GenericTimerFreq) ;
#else
#error "Undefined core"
#endif
	__rtos_critical_exit_os();
	return time_ns;
}
