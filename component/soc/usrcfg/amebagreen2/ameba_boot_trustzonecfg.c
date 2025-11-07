/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE

#if defined (CONFIG_IMG3_SRAM) || defined (CONFIG_IMG3_FLASH)
#define TZ_NSC_START  	(u32)__km4tz_tz_nsc_start__
#define TZ_ENTRY_START	(u32)__km4tz_tz_entry_start__
#define TZ_ENTRY_END	(u32)__km4tz_tz_entry_end__
#else
#define TZ_NSC_START  	(u32)__km4tz_psram_tz_nsc_start__
#define TZ_ENTRY_START	(u32)__km4tz_psram_tz_entry_start__
#define TZ_ENTRY_END	(u32)__km4tz_psram_tz_entry_end__
#endif

#define BD_RAM_START	(u32)__km4tz_bd_ram_start__
#define BD_RAM_EMD

#define BD_PSRAM_START 	(u32)__km4tz_bd_psram_start__
#define BD_PSRAM_END	(u32)__non_secure_psram_end__

const SAU_CFG_TypeDef sau_config[SAU_ENTRYS_NUM] = {
//  Start					End						NSC
	{0x0001F000,			0x00200000 - 1,			0},	/* entry0: Share ROM NS */
#if defined (CONFIG_TRUSTZONE_EN)
	{0x00200000,			TZ_NSC_START - 1,		0},	/* entry1: IMG1/IMG3 S */
	{TZ_NSC_START,			TZ_ENTRY_START - 1,		1},	/* entry2: NSC */
	{TZ_ENTRY_END,			0xFFFFFFFF,				0},	/* entry3: TODO */
#else
	{0x00200000,			0xFFFFFFFF,				0},	/* entry1: Others */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry3: TODO */
#endif
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
};

const TZ_CFG_TypeDef mpc3_config[MPC_ENTRYS_NUM] =						/* Security configuration for PSRAM */
{
//  Start					End						CTRL
	/* Note: __non_secure_psram_end__ maybe not real, update this info by the chipinfo value in bootloader. */
	{BD_PSRAM_START,		BD_PSRAM_END - 1, 		MPC_RW | MPC_NS},	/* entry0: psram can be rw by ns word */
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry5: TODO */
};

//S2 0x2000_0000 ~ 0x2009_FFFF 640K sram0
const TZ_CFG_TypeDef mpc2_config[MPC_ENTRYS_NUM] =						/* Security configuration for sram S2 */
{
//  Start					End						CTRL
	{0x20001000,			0x20005000 - 1,			MPC_RW | MPC_NS},	/* entry0: Non-Secure MSP_NS, ROM_BSS_NS */
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	{0x20005000,			0x200A0000 - 1, 		MPC_RW | MPC_NS},	/* entry1: set sram all ns for fullmac mode */
#else
	{BD_RAM_START,			0x200A0000 - 1,			MPC_RW | MPC_NS},	/* entry1: BD_RAM*/
#endif
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_RW | MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry5: TODO */
};

const TZ_CFG_TypeDef mpc1_config[MPC_ENTRYS_NUM] =						/* Security configuration for Flash */
{
//  Start					End						CTRL
	{0x02000000,			0xFFFFFFFF, 			MPC_RO | MPC_NS},	/* entry0: Flash can be rw by ns word */
	{0xFFFFFFFF,			0xFFFFFFFF, 			MPC_RW | MPC_NS},	/* entry1: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				MPC_RW | MPC_NS},	/* entry5: TODO */
};
#endif
