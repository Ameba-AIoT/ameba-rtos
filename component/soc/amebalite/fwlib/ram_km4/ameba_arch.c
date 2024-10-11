/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "basic_types.h"
#include "ameba_soc.h"

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
u32 SystemCoreClock = 150000000;/*!< System Clock Frequency (Core Clock)*/

/**
  * Update SystemCoreClock variable
  *
  * @param  none
  * @return none
  *
  * @brief  Updates the SystemCoreClock with current core Clock
  *         retrieved from cpu registers.
  */
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency      */
{
	SystemCoreClock = CPU_ClkGet();
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void)
{
	// TODO: Hardware initial
#ifdef UNALIGNED_SUPPORT_DISABLE
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
}

void Systick_Cmd(u32 enable)
{
	if (enable) {
		/* Enable */
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	} else {
		/* Disable */
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}

	__DSB();
}

u32 Systick_State(void)
{
	if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
		return ENABLE;
	} else {
		return DISABLE;
	}
}

#ifdef CONFIG_PLATFORM_FREERTOS
/* Notice: Maximum miss 1 tick because of cpu clock changed */
void Systick_update(void)
{
	/* write any value will reset current register */
	SysTick->VAL = 0UL;

	/* Configure SysTick to interrupt at the requested rate. */
	SysTick->LOAD = (SystemCoreClock / RTOS_TICK_RATE_HZ) - 1UL;
}

/**
  * @brief  Set CPU clock Source.
  * @param  Source: This parameter can be one of the following values:
  *		 @arg CLK_CPU_DPLL
  *		 @arg CLK_CPU_MPLL
  *		 @arg CLK_CPU_LBUS
   *		 @arg CLK_CPU_XTAL
  */
void CPU_ClkSet(u32 Source)
{
	u32 CpuClk, PreState;
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);

	/* 1. Select CPU clock source */
	if (Source != LSYS_GET_CKSL_PLFM(Temp)) {
		Temp &= ~(LSYS_MASK_CKSL_PLFM);
		Temp |=  LSYS_CKSL_PLFM(Source);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);

		__DSB();
		__NOP();
	}

	CpuClk = CPU_ClkGet(); /* CPU Clock is changed*/

	/* 2. update for DelayUs */
	RBSS_UDELAY_CLK = CpuClk / MHZ_TICK_CNT;

	/* 3. update for OS */
	SystemCoreClock = CpuClk;

	/* 4. update systick */
	PreState = Systick_State();
	Systick_Cmd(DISABLE);
	Systick_update();
	Systick_Cmd(PreState);
}
#endif

/**
  * @brief  Get CPU interrupt status.
  * @return CPU interrupt status 0/1
  */
u32 CPU_InInterrupt(void)
{
	return __get_IPSR() != 0;
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
	uint32_t PrevStatus = __get_PRIMASK();
	__disable_irq();
	return PrevStatus;
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
	__set_PRIMASK(PrevStatus);
}

