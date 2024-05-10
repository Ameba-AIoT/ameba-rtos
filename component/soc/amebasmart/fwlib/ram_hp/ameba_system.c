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
uint32_t SystemCoreClock = 200000000;/*!< System Clock Frequency (Core Clock)*/

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
  * @param  CpuClk: This parameter can be one of the following values
  *		 @arg CLK_KM4_200M
  *		 @arg CLK_KM4_100M
  *		 @arg CLK_KM4_50M
  *		 @arg CLK_KM4_25M
  *		 @arg CLK_KM4_XTAL
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
  * @brief  get the state of rdp, enable or disable
  * @retval : This parameter can be one of the following values:
  *            @arg TRUE: rdp is enable
  *            @arg FALSE: rdp is disable
  */
u32 IsRDPenabled(void)
{
	return FALSE;
}


/**
  * @brief  Get CPU interrupt status.
  * @return CPU interrupt status 0/1
  */
u32 CPU_InInterrupt(void)
{
#ifdef ARM_CORE_CA32
	return (__get_mode() != CPSR_M_USR) && (__get_mode() != CPSR_M_SYS);
#else
	return __get_IPSR() != 0;

#endif
}

