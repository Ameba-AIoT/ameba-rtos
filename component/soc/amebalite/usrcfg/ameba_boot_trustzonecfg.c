/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE
#if defined(CONFIG_IMG1_FLASH)
#warning It is not recommended to enable CONFIG_IMG1_FLASH and trustzone configuration at the same time, which is vulnerable to TOC/TOU attacks.
#endif

#define BOOT_XIP_START  	((u32)__km4_boot_text_start__ - IMAGE_HEADER_LEN)
#define BOOT_XIP_END		0x10000000

#if defined (CONFIG_IMG3_SRAM)
#define TZ_NSC_START  	(u32)__km4_tz_nsc_start__
#define TZ_ENTRY_START	(u32)__km4_tz_entry_start__
#define TZ_ENTRY_END	(u32)__km4_tz_entry_end__
#else
#define TZ_NSC_START  	(u32)__km4_psram_tz_nsc_start__
#define TZ_ENTRY_START	(u32)__km4_psram_tz_entry_start__
#define TZ_ENTRY_END	(u32)__km4_psram_tz_entry_end__
#endif

const TZ_CFG_TypeDef sau_config[] = {
//  Start					End						NSC
	{0x0001E000,			0x00045000 - 1,			0},	/* entry0: IROM & DROM NS */
	{SPI_FLASH_BASE,		BOOT_XIP_START - 1,		0},	/* entry1: Flash */
	{BOOT_XIP_END,			TZ_NSC_START - 1,		0},	/* entry2: BOOT_XIP ~ NSC */
	{TZ_NSC_START,			TZ_ENTRY_START - 1,		1},	/* entry3: NSC */
	{TZ_ENTRY_END,		    0xFFFFFFFF,				0},	/* entry4: TZ Entry ~ End */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};

const TZ_CFG_TypeDef mpc3_config[] =					/* Security configuration for PSRAM */
{
//  Start					End						NSC
	{(u32)__km4_bd_psram_start__,	0xFFFFFFFF,		0},	/* entry0: psram can be rw by ns word */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry1: End Flag */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};

const TZ_CFG_TypeDef mpc2_config[] =					/* Security configuration for sram1 S3 */
{
//  Start					End						NSC
	{(u32)__km4_bd_ram_start__,	0x22000000 - 1,		0},	/* entry0: BD_RAM_NS*/
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};

const TZ_CFG_TypeDef mpc1_config[] =					/* Security configuration for sram0 S2 */
{
//  Start					End						NSC
	{0x20000000,			0x20007000 - 1,			0},	/* entry0: MSP_NS, ROM_BSS_COMMON, ROM_BSS_NS, STDLIB_HEAP_RAM_NS */
	{(u32)__km4_bd_ram_start__,	0x22000000 - 1,		0},	/* entry1: BD_RAM_NS*/
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};
#endif

