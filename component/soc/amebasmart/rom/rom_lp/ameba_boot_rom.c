/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"

HAL_ROM_TEXT_SECTION
void BOOT_ROM_InitDebugFlg(void)
{
	/* to initial ROM code using global variable */
	if ((HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) & SEC_BIT_DIS_BOOT_LOG_EN) == 0) {
		LOG_MASK(LEVEL_ERROR, 0xFFFFFFFF);
	}
}

__weak HAL_ROM_TEXT_SECTION
void BOOT_ROM_ResetVsr(void)
{
	PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;

	/* Set ACTLR.LoopBuf_Dis(0xE000E008, bit 6) to avoid Loop Buffer Bug in KM0/KM4 version 1.8, This register is not banked between Security domains.*/
	SCnSCB->ACTLR |= SCnSCB_ACTLR_LOOPBUF_DIS_Msk;
	__DSB();

	/* Check boot or wake up from power saving */
	if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_WAKE_FROM_PS_LS) {
		pRamStartFun->RamWakeupFun();
	}

	/*enable IWDG low power mode, will remove in MP chip*/
	IWDG_LP_Enable(IWDG_DEV, ENABLE);

	/*wait until KM4 send event to let KM0 go*/
	asm volatile("wfe");
	asm volatile("wfe");

	while (1) {
		u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG);
		if (Temp & LSYS_BIT_BOOT_KM0_RUN) {
			Temp = Temp & (~LSYS_BIT_BOOT_KM0_RUN);
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, Temp);
			break;
		}
	}

	/* Goto Flash code */
	pRamStartFun->FlashStartFun();
}
