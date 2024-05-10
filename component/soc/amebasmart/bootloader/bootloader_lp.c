/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

#if defined ( __ICCARM__ )
#pragma section=".ram.bss"
#pragma section=".rom.bss"
#pragma section=".ram.start.table"
#pragma section=".ram_image1.bss"
#pragma section=".ram_image2.entry"

BOOT_RAM_RODATA_SECTION u8 *__image2_entry_func__ = 0;
//BOOT_RAM_RODATA_SECTION u8* __image1_bss_start__ = 0;
//BOOT_RAM_RODATA_SECTION u8* __image1_bss_end__ = 0;
#endif

BOOT_RAM_TEXT_SECTION
PRAM_START_FUNCTION BOOT_SectionInit(void)
{
#if defined ( __ICCARM__ )
	// only need __bss_start__, __bss_end__
	__image2_entry_func__		= (u8 *)__section_begin(".ram_image2.entry");
	//__image1_bss_start__		= (u8*)__section_begin(".ram_image1.bss");
	//__image1_bss_end__			= (u8*)__section_end(".ram_image1.bss");
#endif
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}

BOOT_RAM_TEXT_SECTION
void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_SectionInit();

	/* we should Cache_Flush when we wake */
	Cache_Enable(ENABLE);

	/* Need Modify */
	__set_MSP(MSP_RAM_LP);
	irq_table_init(MSP_RAM_LP);
	VectorTableAdd();
	//move to sleep pg in pmc lib, about 168us
	/* soc deepstadby or deepsleep need it, or wakeup will fail */
	//if (SYSCFG_OTP_FlashDSleepEn()) {
	//FLASH_DeepPowerDown(DISABLE);
	//}

	if (!SWR_In_BST_MODE()) {
		/* sys req pfm mode when only km0 */
		SWR_PFM_MODE_Set(ENABLE);
	}

	Image2EntryFun->RamWakeupFun();

	/* avoid compiler to pop stack when call Image2EntryFun->RamStartFun for MSP is changed */
	while (1);
}

void BOOT_CleanupSection(void)
{
	/* Clear ROM bss*/
	_memset((void *) __rom_bss_start__, 0, (__rom_bss_end__ - __rom_bss_start__));
	/* Clear application bss */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
}
//3 Image 1
/***
* @note It is forbidden to use RTK_LOGx or printf in this document,
* if you need to print information, please use DiagPrintf.
***/
void BOOT_Image1(void)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_SectionInit();

	Cache_Enable(ENABLE);

	BOOT_CleanupSection();
	/* Relocate IRQ table */
	__set_MSP(MSP_RAM_LP);

	irq_table_init(MSP_RAM_LP);
	VectorTableAdd();

	RBSS_UDELAY_CLK = CPU_ClkGet() / 1000000;

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();

	/* Get flash_init_para address for KM0 */
	_memcpy((void *)&flash_init_para, (const void *)HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_FLASH_PARA_ADDR), sizeof(FLASH_InitTypeDef));

	/* goto IMG2 */
	Image2EntryFun->RamStartFun();

	/* avoid compiler to pop stack when call Image2EntryFun->RamStartFun for MSP is changed */
	while (1);
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
