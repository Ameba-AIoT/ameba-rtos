/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

extern void prvSetupFPU(void);
extern u8 BOOT_Load_KR4_APP(void);

NON_DRAM_TEXT_SECTION
PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)(void *)__image2_entry_func__;
}

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
/* CPU shall be init at the entrance of the Img */
NON_DRAM_TEXT_SECTION
void BOOT_CPU_init(void)
{
	/* modify memory layout cache attribute */
	Cache_Attribute_MCCA();
	/*no need to flush icache because 0x0000_0000~0x3FFF_FFFF and 0x6000_0000 ~ 0x7FFF_FFFF is cacheable */
	DCache_Enable();

	BOOT_SetupFPU();
	__DSB();
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
	/* Init BOOT_InitGP() in Image2EntryFun->RamWakeupFun*/
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

	/* Get flash_init_para address for KR4 */
	_memcpy((void *)&flash_init_para, (const void *)HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_FLASH_PARA_ADDR), sizeof(FLASH_InitTypeDef));

	if (TRUE == BOOT_Load_KR4_APP()) {
		/*Release ROM Flag*/
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_KR4_RUN);
		Image2EntryFun = (PRAM_START_FUNCTION)(void *)__image2_entry_func__;
		Image2EntryFun->RamStartFun();
	}

	while (1);
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
