/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE

#define TZ_S_START		(u32)__image3_ram_start__
#define TZ_NSC_START	(u32)__ram_image3_nsc_start__
#define TZ_NSC_END		(u32)__ram_image3_nsc_end__
#define TZ_S_END		(u32)__image3_ram_end__

#define BD_RAM_START	(u32)__km4tz_bd_ram_start__
#define BD_RAM_END		(u32)__km4ns_bd_ram_end__

#define BD_PSRAM_START 	(u32)__km4tz_bd_psram_start__
#define BD_PSRAM_END	(u32)__non_secure_psram_end__

const SAU_CFG_TypeDef sau_config[SAU_ENTRY_NUM] = {
	// Start							End									IS_NSC
	{(u32)__km4tz_ns_rom_start__,	TZ_S_START - 1,					0},	/* entry0: NS ROM + flash XIP + SRAM below TZ */
	{TZ_NSC_START,					TZ_NSC_END - 1,					1},	/* entry1: NSC (.gnu.sgstubs) */
	{TZ_S_END,						0xFFFFFFFF,						0},	/* entry2: all addresses after TZ region → NS.
	                                                                           0x30000000+ (Secure BIT28 alias / Secure MSP) is still Secure
	                                                                           because RTL8720F IDAU overrides SAU for that range. */
	{0xFFFFFFFF,					0xFFFFFFFF,						0},	/* entry3: padding */
};

const TZ_CFG_TypeDef mpc3_config[MPC_ENTRY_NUM] =						/* Security configuration for PSRAM */
{
	// Start							End					CTRL
	/* Note: __non_secure_psram_end__ maybe not real, update this info by the chipinfo value in bootloader. */
	{BD_PSRAM_START,					BD_PSRAM_END - 1,					MPC_RW | MPC_NS},	/* entry0: KM4TZ_BD_PSRAM  KM4NS_BD_PSRAM*/
	{0xFFFFFFFF,						0xFFFFFFFF,							MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,						0xFFFFFFFF,							MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,						0xFFFFFFFF,							MPC_RW | MPC_NS},	/* entry3: TODO */
};

//S2 0x2000_0000 ~ 0x2007_FFFF 512K sram0
const TZ_CFG_TypeDef mpc2_config[MPC_ENTRY_NUM] =						/* Security configuration for SRAM */
{
	// Start					End						CTRL
	{(u32)__km4tz_rombss_ram_ns_start__,	(u32)__km4tz_bootloader_ram_s_start__ - 0x10000000 - 1,	MPC_RW | MPC_NS},	/* entry0: KM4TZ_ROMBSS_RAM_NS / KM4TZ_MSP_RAM_NS NS / KM4NS_MSP_RAM */
	{BD_RAM_START,							BD_RAM_END - 1,						MPC_RW | MPC_NS},	/* entry1: KM4TZ_BD_RAM KM4NS_BD_RAM */
	{0xFFFFFFFF,							0xFFFFFFFF,						MPC_RW | MPC_NS},	/* entry2: TODO*/
	{0xFFFFFFFF,							0xFFFFFFFF,						MPC_RW | MPC_NS},	/* entry3: TODO*/
};

const TZ_CFG_TypeDef mpc1_config[MPC_ENTRY_NUM] =						/* Security configuration for Flash */
{
	// Start					End						CTRL
	{(u32)0x02000000,	0xFFFFFFFF,		MPC_RO | MPC_NS},	/* entry0: Flash can be rw by ns word */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,		MPC_RW | MPC_NS},	/* entry3: TODO */
};
#endif
