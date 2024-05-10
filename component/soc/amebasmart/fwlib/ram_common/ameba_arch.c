/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "basic_types.h"
#include "ameba_soc.h"

void Systick_Cmd(u32 enable)
{
	if (enable) {
		/* Enable */
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	} else {
		/* Disable */
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}

	__asm volatile("dsb");
}

