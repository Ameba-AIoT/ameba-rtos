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

const SAU_CFG_TypeDef sau_config[SAU_ENTRYS_NUM] = {
//  Start				End						NSC
	{0x0001E000,		0x000447FF,				0},	/* entry0: IROM & DROM NS */
	{SPI_FLASH_BASE,	BOOT_XIP_START - 1,		0},	/* entry1: Flash */
	{BOOT_XIP_END,		TZ_NSC_START - 1,		0},	/* entry2: BOOT_XIP ~ NSC */
	{TZ_NSC_START,		TZ_ENTRY_START - 1,		1},	/* entry3: NSC */
	{TZ_ENTRY_END,		0xFFFFFFFF,				0},	/* entry4: TZ Entry ~ End */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry7: TODO */
};

const TZ_CFG_TypeDef mpc2_config[MPC_ENTRYS_NUM] =						/* Security configuration for PSRAM */
{
//  Start							End					CTRL
	/* Note: __non_secure_psram_end__ maybe not real, update this info by the chipinfo value in bootloader. */
	{(u32)__km4_bd_psram_start__,	(u32)__non_secure_psram_end__ - 1,			MPC_RW | MPC_NS},	/* entry0: psram can be rw by ns word */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry1: End Flag */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry2: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry3: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry7: TODO */
};

//S1 0x2000_0000 ~ 0x2007_FFFF 512K sram0
//S2 0x2008_0000 ~ 0x200F_FFFF 512K(120K+40K) share sram
const TZ_CFG_TypeDef mpc1_config[MPC_ENTRYS_NUM] =						/* Security configuration for sram S1/S2 */
{
//  Start							End										CTRL
	{0x20000000,					0x20006FFF,			MPC_RW | MPC_NS},	/* entry0: MSP_NS, ROM_BSS_COMMON, ROM_BSS_NS, STDLIB_HEAP_RAM_NS */
	{0x20008000,					0x20008FFF,			MPC_RW | MPC_NS},	/* entry1: KM0_RTOS_STATIC_0_NS */
	{(u32)__km4_bd_ram_start__,		0x20100000 - 1,		MPC_RW | MPC_NS},	/* entry2: BD_RAM_NS*/
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry3: End Flag */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry4: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry5: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry6: TODO */
	{0xFFFFFFFF,					0xFFFFFFFF,			MPC_RW | MPC_NS},	/* entry7: TODO */
};
#endif

