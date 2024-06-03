/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"

extern void prvSetupFPU(void);

NON_DRAM_TEXT_SECTION
PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)(void *)__image2_entry_func__;
}

#if( configENABLE_FPU == 1 )
NON_DRAM_TEXT_SECTION
void BOOT_SetupFPU(void)   /* PRIVILEGED_FUNCTION */
{
	asm volatile(
		"csrr t0, misa \n"			/* Get misa */
		"li   t1, 0x10028 \n"		/* 0x10028 = Q,F,D Quad, Single or Double precission floating point */
		"and  t0, t0, t1 \n"
		"beqz t0, 1f \n"			/* check if Q,F or D is present into misa */
		"csrr t0, mstatus \n"		/* Floating point unit is present so need to put it into initial state */
		"lui  t1, 0x2 \n"			/* t1 =  0x1 << 12 */
		"or   t0, t0, t1 \n"
		"csrw mstatus, t0 \n"		/* Set FS to initial state */
		"1: \n"
		:::
	);
}
#else
#define BOOT_SetupFPU() ((void)0)
#endif

/* CPU shall be init at the entrance of the Img */
NON_DRAM_TEXT_SECTION
void BOOT_CPU_init(void)
{
	/* modify memory layout cache attribute */
	Cache_Attribute_MCCA();
	/*no need to flush icache because 0x0000_0000~0x3FFF_FFFF and 0x6000_0000 ~ 0x7FFF_FFFF is cacheable */
	DCache_Enable();

	BOOT_InitGP();
	BOOT_SetupFPU();
	__DSB();

	SOCPS_Kr4_StartTbl_Backup();

	/* Check boot or wake up from power saving */
	if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_WAKE_FROM_PS_LS) {
		BOOT_WakeFromPG();
	} else {
		BOOT_Image1();
	}
}

NON_DRAM_TEXT_SECTION
void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun;

	if (LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1))) {

		if (SYSCFG_OTP_FlashDSleepEn()) {
			FLASH_DeepPowerDown(DISABLE);
			/* to fix hw bug: between flash user read and auto read wrap, need single auto read, otherwise SPIC may return error */
			HAL_READ32(SPI_FLASH_BASE, 0);
		}

		set_psram_resume();
	}

	Image2EntryFun = BOOT_SectionInit();
	Image2EntryFun->RamWakeupFun();
}

/***
* @note It is forbidden to use printf in this document,
* if you need to print information, please use DiagPrintf or RTK_LOGx.
***/
void BOOT_Image1(void)
{
	PRAM_START_FUNCTION Image2EntryFun;

	/* Clear ROM bss*/
	_memset((void *) __rom_bss_start__, 0, (__rom_bss_end__ - __rom_bss_start__));

	RBSS_UDELAY_CLK = CPU_ClkGet() / 1000000;

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();
#ifndef CONFIG_MP_SHRINK
	/* Get flash_init_para address for KR4 */
	_memcpy((void *)&flash_init_para, (const void *)HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_FLASH_PARA_ADDR), sizeof(FLASH_InitTypeDef));
#endif
	/* goto IMG2 */
	Image2EntryFun = (PRAM_START_FUNCTION)(void *)__image2_entry_func__;
	Image2EntryFun->RamStartFun();

	return;
}

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = NULL,
	.RamWakeupFun = BOOT_CPU_init,
	.RamPatchFun0 = NULL,
	.RamPatchFun1 = NULL,
	.RamPatchFun2 = NULL,
	.FlashStartFun = BOOT_CPU_init,
	.Img1ValidCode = NULL,
};
