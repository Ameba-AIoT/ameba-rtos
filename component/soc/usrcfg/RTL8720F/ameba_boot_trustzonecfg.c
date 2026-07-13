/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE

#ifdef CONFIG_IMG3_FLASH
/* XIP-flash mode: Secure code runs from Flash XIP [TZ_S_START, TZ_XIP_REGION_END).
 * Secure SRAM holds .data/.bss at [TZ_SRAM_START, TZ_S_END).
 *
 * SAU strategy (4 entries, RTL8720F hardware limit):
 *   entry0: [__km4tz_ns_rom_start__, TZ_S_START-1]   -> NS
 *           (covers NS ROM + NS flash/SRAM below Secure XIP start)
 *   entry1: [TZ_NSC_START, TZ_NSC_END-1]             -> NSC
 *           (NSC gateway sits inside the Secure XIP region)
 *   entry2: [TZ_XIP_REGION_END, TZ_SRAM_START-1]     -> NS
 *           (gap between Secure XIP end and Secure SRAM start;
 *            covers NS XIP code windows 0x02000000, 0x04000000, etc.)
 *   entry3: [TZ_S_END, 0xFFFFFFFF]                   -> NS
 *           (everything after Secure SRAM)
 *
 * Addresses NOT in any NS entry -> Secure by default:
 *   [TZ_S_START, TZ_XIP_REGION_END)  : Secure Flash XIP (code)
 *   [TZ_SRAM_START, TZ_S_END)        : Secure SRAM-TZ (.data/.bss)
 */
#define TZ_S_START			(u32)__image3_entry_func__
#define TZ_SRAM_START		(u32)__image3_ram_start__
#define TZ_XIP_REGION_END	0x01000000U
#elif defined(CONFIG_IMG3_PSRAM)
/* PSRAM mode: Secure code in PSRAM-TZ; .data/.bss in SRAM-TZ (4 KB minimum). */
#define TZ_SRAM_START		(u32)__image3_ram_start__
#define TZ_SRAM_END			(u32)__image3_ram_end__
#define TZ_PSRAM_START		(u32)__image3_psram_ram_start__
#define TZ_PSRAM_END		(u32)__image3_psram_ram_end__
#else /* CONFIG_IMG3_SRAM */
#define TZ_S_START			(u32)__image3_ram_start__
#define TZ_XIP_REGION_END	0U	/* unused in SRAM mode */
#endif

#define TZ_NSC_START		(u32)__ram_image3_nsc_start__
#define TZ_NSC_END			(u32)__ram_image3_nsc_end__
#ifndef CONFIG_IMG3_PSRAM
#define TZ_S_END			(u32)__image3_ram_end__
#endif

const SAU_CFG_TypeDef sau_config[SAU_ENTRY_NUM] = {
//  Start				End						NSC
#ifdef CONFIG_IMG3_PSRAM
	/* Secure = SRAM-TZ [TZ_SRAM_START,TZ_SRAM_END) + PSRAM-TZ [TZ_PSRAM_START,TZ_PSRAM_END) */
	{(u32)__km4tz_ns_rom_start__,	TZ_SRAM_START - 1,		0},	/* entry0: NS ROM + flash/SRAM before Secure SRAM window */
	{TZ_SRAM_END,		TZ_PSRAM_START - 1,		0},	/* entry1: gap between Secure SRAM and Secure PSRAM -> NS */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry2: NSC gateway inside Secure PSRAM */
	{TZ_PSRAM_END,		0xFFFFFFFF,				0},	/* entry3: region after Secure PSRAM -> NS */
#elif defined(CONFIG_IMG3_FLASH)
	/* 4 entries cover all NS ranges; Secure XIP and Secure SRAM are NS-free gaps */
	{(u32)__km4tz_ns_rom_start__,	TZ_S_START - 1,			0},	/* entry0: NS ROM + flash/SRAM before Secure XIP */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry1: NSC gateway inside Secure XIP */
	{TZ_XIP_REGION_END,	TZ_SRAM_START - 1,		0},	/* entry2: gap between Secure XIP end and Secure SRAM -> NS */
	{TZ_S_END,			0xFFFFFFFF,				0},	/* entry3: region after Secure SRAM -> NS */
#else /* CONFIG_IMG3_SRAM */
	{(u32)__km4tz_ns_rom_start__,	TZ_S_START - 1,		0},	/* entry0: NS ROM + flash/SRAM before Secure TZ area */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry1: NSC gateway inside Secure SRAM */
	{TZ_S_END,			0xFFFFFFFF,				0},	/* entry2: region after Secure SRAM -> NS */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry3: padding */
#endif
};

#define BD_RAM_START	(u32)__km4tz_bd_ram_start__
#define BD_RAM_END		(u32)__km4ns_bd_ram_end__

#define BD_PSRAM_START 	(u32)__km4tz_bd_psram_start__
#define BD_PSRAM_END	(u32)__non_secure_psram_end__

const TZ_CFG_TypeDef mpc3_config[MPC_ENTRY_NUM] =						/* Security configuration for PSRAM */
{
//  Start					End						CTRL
	/* Note: __non_secure_psram_end__ maybe not real, update this info by the chipinfo value in bootloader. */
	{BD_PSRAM_START,		BD_PSRAM_END - 1, 		MPC_RW | MPC_NS},	/* entry0: KM4TZ_BD_PSRAM  KM4NS_BD_PSRAM*/
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry3: TODO */
};

//S2 0x2000_0000 ~ 0x2007_FFFF 512K sram0
const TZ_CFG_TypeDef mpc2_config[MPC_ENTRY_NUM] =						/* Security configuration for SRAM */
{
//  Start					End						CTRL
	{(u32)__km4tz_rombss_ram_ns_start__,	(u32)__km4tz_bootloader_ram_s_start__ - 0x10000000 - 1,	MPC_RW | MPC_NS},	/* entry0: KM4TZ_ROMBSS_RAM_NS / KM4TZ_MSP_RAM_NS / KM4NS_MSP_RAM */
	{BD_RAM_START,							BD_RAM_END - 1,						MPC_RW | MPC_NS},	/* entry1: KM4TZ_BD_RAM KM4NS_BD_RAM */
	{0xFFFFFFFF,							0xFFFFFFFF,						MPC_RW | MPC_NS},	/* entry2: TODO*/
	{0xFFFFFFFF,							0xFFFFFFFF,						MPC_RW | MPC_NS},	/* entry3: TODO*/
};

const TZ_CFG_TypeDef mpc1_config[MPC_ENTRY_NUM] =						/* Security configuration for Flash */
{
//  Start					End						CTRL
	{(u32)0x02000000,	0xFFFFFFFF,		MPC_RO | MPC_NS},	/* entry0: Flash can be rw by ns word */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry3: TODO */
};
#endif
