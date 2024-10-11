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
	u32 Rtemp;

	if (enable) {
		/* enablr mtick clock */
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1);
		Rtemp |= LSYS_BIT_PLIC_TIMER_CKEN;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1, Rtemp);

	} else {
		/* Disable mtick clock */
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1);
		Rtemp &= (~LSYS_BIT_PLIC_TIMER_CKEN);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1, Rtemp);
	}

	__DSB();
}

u32 Systick_State(void)
{
	u32 Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1);

	if (LSYS_GET_PLIC_TIMER_CKEN(Rtemp)) {
		return ENABLE;
	} else {
		return DISABLE;
	}
}

/* Notice: Maximum miss 1 tick because of cpu clock changed */
void Systick_update(void)
{
#ifdef CONFIG_PLATFORM_FREERTOS
	extern void vPortSetupTimerInterrupt(void);
	vPortSetupTimerInterrupt();
#else
#error "Please add implementation methods for other OS"
#endif
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

/**
  * @brief  Get CPU interrupt status.
  * @return CPU interrupt status 0/1
  */
u32 CPU_InInterrupt(void)
{
	return plic_get_active_irq_id() != 0;

}

/**
 * Disable IRQ Interrupts and Save Previous Status
 *
 * @param  none
 * @return the previous value of IQR status
 *
 * @brief  disables interrupts by clearing the MIE-bit in the
 * mstatus register, and returns the previous value of that register.
 * irq_disable_save and irq_enable_restore normally appear in pairs
 */
u32 irq_disable_save(void)
{
	return __disable_irq_save();
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
	__enable_irq_restore(PrevStatus);
}

NON_DRAM_TEXT_SECTION
void BOOT_InitGP(void)
{
//# Initialize global pointer
	asm volatile(".option push								\n"
				 ".option norelax							\n"
				 "1:										\n"
				 "auipc gp, % pcrel_hi(__global_pointer$)	\n"
				 "addi  gp, gp, % pcrel_lo(1b)				\n"
				 ".option pop								\n"
				);
}

