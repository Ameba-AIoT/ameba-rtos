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
uint32_t SystemCoreClock = 20000000;/*!< System Clock Frequency (Core Clock)*/

u32
SystemGetCpuClk(void)
{
	return CPU_ClkGet();
}

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency      */
{
	SystemCoreClock = SystemGetCpuClk();
}

/**
  * @brief  Set CPU clock.
  * @param  CpuType: For KM0, this parameter can be one of the following values
  *		 @arg CLK_KM0_XTAL_40M
  *		 @arg CLK_KM0_XTAL_26M
  *		 @arg CLK_KM0_ANA_4M
  * @note NA
  */
void SystemSetCpuClk(u8 CpuClk)
{
	CPU_ClkSet(CpuClk);
	SystemCoreClockUpdate();
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

	SystemCoreClockUpdate();
}


/**
  * @brief  Get CPU interrupt status.
  * @return CPU interrupt status 0/1
  */
u32 CPU_InInterrupt(void)
{
	return __get_IPSR() != 0;
}

