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

#if (defined ARM_CORE_CM4) || (defined ARM_CORE_CM0)
#define portNVIC_SYSTICK_LOAD_REG                 ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG        ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#endif

#if defined RSICV_CORE_KR4
extern size_t uxTimerIncrementsForOneTick;
extern volatile uint64_t *pullMachineTimerCompareRegister;
extern volatile uint64_t *pullMachineTimerCounterRegister;
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
	uint32_t time_us = 0;
	rtos_critical_enter();
#if defined ARM_CORE_CM4 || defined ARM_CORE_CM0
	uint32_t timer_cnt_pass = portNVIC_SYSTICK_LOAD_REG - portNVIC_SYSTICK_CURRENT_VALUE_REG;
	uint32_t time_pass_us = timer_cnt_pass / (configCPU_CLOCK_HZ / 1000000UL);
	uint32_t time_ms = rtos_time_get_current_system_time_ms();
	if (__NVIC_GetPendingIRQ(SysTick_IRQn)) {
		time_ms ++;
	}
	time_us = time_ms * 1000 + time_pass_us;
#elif defined RSICV_CORE_KR4
	uint64_t cur_interrupt_cnt = *pullMachineTimerCompareRegister - uxTimerIncrementsForOneTick;
	uint32_t timer_cnt_pass = *pullMachineTimerCounterRegister - cur_interrupt_cnt;
	uint32_t time_pass_us = timer_cnt_pass / (configCPU_CLOCK_HZ / 1000000UL);
	uint32_t time_ms = rtos_time_get_current_system_time_ms();
	time_us = time_ms * 1000 + time_pass_us;
#elif defined ARM_CORE_CA32
	time_us = arm_arch_timer_count() / 50;  // arm_arch_timer_count is 50 Mhz
#else
#error "Undefined core"
#endif
	rtos_critical_exit();
	return time_us;
}
