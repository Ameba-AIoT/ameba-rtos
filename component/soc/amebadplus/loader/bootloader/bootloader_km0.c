/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}
/**
 * @brief It is forbidden to use any print function in BOOT_WakeFromPG,
 * because print function (including rodata section) is in flash at this time, but flash has
 * not been woken up.
 */
SRAM_ONLY_TEXT_SECTION
void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

	RSIP_OTF_Load_Key(); //Wait RSIP AES Ready

	/* we should Cache_Flush when we wake */
	Cache_Enable(ENABLE);

	SCB->VTOR = Image2EntryFun->VectorNS;

	Image2EntryFun->RamWakeupFun();
}

void BOOT_CleanupSection(void)
{
	/* Important: Cleaning up the BSS section must be done before Heap init. */
	/* Clear ROM bss*/
	_memset((void *) __rom_bss_start__, 0, (__rom_bss_end__ - __rom_bss_start__));
	/* Clear application bss */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
}
//3 Image 1
/***
* @note It is forbidden to use RTK_LOGx or printf in this function,
* if you need to print information, please use DiagPrintf.
***/
void BOOT_Image1(void)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_SectionInit();
	u32 CpuClk;

	/* enable cache */
	Cache_Enable(ENABLE);

	BOOT_CleanupSection();

	CpuClk = CPU_ClkGet();
	DelayClkUpdate(CpuClk);

	/* Relocate IRQ table */
	irq_table_init(MSP_RAM_LP);

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();

#ifndef CONFIG_MP_SHRINK
	/* Get flash_init_para address for KM0 */
	_memcpy((void *)&flash_init_para, (const void *)HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_FLASH_PARA_ADDR), sizeof(FLASH_InitTypeDef));
#endif

	/* goto IMG2 */
	Image2EntryFun->RamStartFun();

	return;
}

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = NULL,
	.RamWakeupFun = BOOT_WakeFromPG,
	.RamPatchFun0 = NULL,
	.RamPatchFun1 = NULL,
	.RamPatchFun2 = NULL,
	.FlashStartFun = BOOT_Image1,
	.Img1ValidCode = NULL,
};
