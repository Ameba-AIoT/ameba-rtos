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

/**
 * Disable IRQ Interrupts and Save Previous Status
 *
 * @param  none
 * @return the previous value of IQR status
 *
 * @brief  disables interrupts by setting the primask register,
 * and returns the previous value of that register.
 */
u32 irq_disable_save(void)
{
#ifndef CONFIG_ARM_CORE_CA32
	uint32_t PrevStatus = __get_PRIMASK();
	__disable_irq();
	return PrevStatus;
#else
	return portDISABLE_INTERRUPTS();
#endif
}

/**
 * enable IRQ Interrupts according to Previous Status
 *
 * @param  the previous value of IQR status
 * @return none
 *
 * @brief  This function enables interrupt by restoring the saved interrupt status register.
 * irq_disable_save and irq_enable_restore normally appear in pairs
 */
void irq_enable_restore(u32 PrevStatus)
{
#ifndef CONFIG_ARM_CORE_CA32
	__set_PRIMASK(PrevStatus);
#else
	portRESTORE_INTERRUPTS(PrevStatus);
#endif
}
