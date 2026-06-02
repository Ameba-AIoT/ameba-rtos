/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "basic_types.h"
#include "ameba_soc.h"

#ifndef CONFIG_ARM_CORE_CA32
/* SysTick is a Cortex-M architectural peripheral.  CA32 (Cortex-A) uses
 * the ARM Generic Timer instead and does not own a SysTick block, so this
 * helper is compiled out of the CA32 build.  amebasmart CA32 code does
 * not call Systick_Cmd() (verified across component/soc/amebasmart/).
 */
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
#endif /* CONFIG_ARM_CORE_CA32 */

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
	/* CA32 (Cortex-A32): save CPSR and disable IRQ via CPSID i.
	 * Equivalent to FreeRTOS ulPortInterruptLock() in ARM_CA32/port.c
	 * but avoids pulling in FreeRTOS.h.
	 */
	u32 key;
	__asm volatile("mrs %0, cpsr" : "=r"(key) :: "memory");
	__asm volatile("CPSID i" ::: "memory");
	__asm volatile("DSB sy" ::: "memory");
	__asm volatile("ISB sy" ::: "memory");
	return key;
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
	/* CA32: restore CPSR from saved value.
	 * Equivalent to FreeRTOS ulPortInterruptUnLock() in ARM_CA32/port.c.
	 */
	__asm volatile("msr cpsr_c, %0" :: "r"(PrevStatus) : "memory");
#endif
}
