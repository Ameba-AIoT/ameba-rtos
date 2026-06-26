/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define BOOT_XIP_START  	((u32)__km4_boot_text_start__ - IMAGE_HEADER_LEN)
#define BOOT_XIP_END		0x0C000000

#ifdef CONFIG_TRUSTZONE
#define TZ_S_START			(u32)__image3_ram_start__
#define TZ_NSC_START		(u32)__ram_image3_nsc_start__
#define TZ_NSC_END			(u32)__ram_image3_nsc_end__
#define TZ_S_END			(u32)__image3_ram_end__

const SAU_CFG_TypeDef sau_config[SAU_ENTRY_NUM] = {
//  Start					End						NSC
	{0x0001E000,			0x00054000 - 1,			0},	/* entry0: IROM & DROM NS */
	{SPI_FLASH_BASE,		BOOT_XIP_START - 1,		0},	/* entry1: Flash */
	{BOOT_XIP_END,			TZ_S_START - 1,			0},	/* entry2: BOOT_XIP ~ TZ region */
	{TZ_NSC_START,			TZ_NSC_END - 1,			1},	/* entry3: NSC region */
	{TZ_S_END,				0xFFFFFFFF,				0},	/* entry4: after TZ region ~ End */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};
#endif /* CONFIG_TRUSTZONE */

const TZ_CFG_TypeDef mpc1_config[] =					/* Security configuration for DDR/PSRAM */
{
//  Start					End						NSC
#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
	{0x00000000,					0x0015B000 - 1,			0},	/* entry0: TODO */
#else
	{0x00000000,	(u32)__ca32_bl1_dram_start__ - 0x20 - 1, 		0},   /* entry0: TODO */
#endif
	/* FIP = BL2+BL32+BL33, reserved CA32_FIP_MAX_SIZE is safe for secure BL2/BL32 */
	/* Note: __non_secure_psram_end__ maybe not real, update this info by the chipinfo value in bootloader. */
	{(u32)__ca32_fip_dram_start__ + CA32_FIP_MAX_SIZE,	(u32)__non_secure_psram_end__ - 1,				0},	/* entry1: see above */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: locked to [CA32_BL3_DRAM_NS, 0x00600000 - 1] in ATF for security reason */
};

const TZ_CFG_TypeDef mpc2_config[] =					/* Security configuration for HS_SRAM */
{
//  Start					End						NSC
	{0x00014000,			0x0001F000 - 1,			0},	/* entry0: BD_RAM_NS, MSP_NS, ROM_BSS_COMMON, ROM_BSS_NS, STDLIB_HEAP_RAM_NS */
	{0x00020000,			0x00040000 - 1,			0},	/* entry1: AUDIO_BUF */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};
