/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"

#if defined ( __ICCARM__ )
#define DelayCheck()						\
	do {							\
		__asm volatile("nop;");	\
	} while (0)
#else
#define DelayCheck()						\
	do {							\
		__asm__ __volatile__("nop;");	\
	} while (0)
#endif

/**
  * @brief  Delay x us.
  * @param  us: delay time, unit is us
  */
void DelayUs(u32 us)
{
	uint64_t cntvct;

	/*check if arch timer is still close, open it */
	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_RTLSIM) || !(arm_arch_timer_check_enable())) {
		arm_arch_timer_enable(pdTRUE);
	}

	cntvct = (uint64_t)(arm_arch_timer_count() + vGetGenericTimerFreq() * us / MHZ_TICK_CNT);

	while (arm_arch_timer_count() < cntvct) {
		DelayCheck();
	}
}

/**
  * @brief  Delay x ms.
  * @param  ms: delay time, unit is ms
  */
void DelayMs(u32 ms)
{
	DelayUs(ms * 1000);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
