/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rom/ameba_dump_stack.h"
#include "rom/ameba_v8m_crashdump.h"


#ifdef CONFIG_SOLO
/* SOLO: KM4NS is not the SPIC owner and never calibrates flash. Before it can
 * drive the SPIC itself (iot OTA), it must adopt KM4TZ's ROM-calibrated
 * flash_init_para, which is fix at 0x20001000*/
void Solo_FlashParaAdopt(void)
{
	extern FLASH_InitTypeDef flash_init_para;
	_memcpy(&flash_init_para, (void *)0x20001000, sizeof(FLASH_InitTypeDef));
}
#endif

//3 Image 1
void BOOT_Start(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

#ifdef CONFIG_SOLO
	Solo_FlashParaAdopt();
#endif

	/* Enable divide-by-zero fault */
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;

	/* goto IMG2 */
	Image2EntryFun->RamStartFun();

	return;
}
