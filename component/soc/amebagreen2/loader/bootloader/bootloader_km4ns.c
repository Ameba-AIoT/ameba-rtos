/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "rom/ameba_dump_stack.h"

SRAM_ONLY_TEXT_SECTION
void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

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
void BOOT_Image1(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	u32 CpuClk;

	/* enable cache */
	Cache_Enable(ENABLE);

	BOOT_CleanupSection();

	/* update delay div for cacheable rom_ns */
	RBSS_UDELAY_DIV = 5;

	CpuClk = CPU_ClkGet();
	DelayClkUpdate(CpuClk);

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();

	/* Set the crash dump stack depth for debugging */
	crash_SetExStackDepth(MIN_DUMP_DEPTH);

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
