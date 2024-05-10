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
