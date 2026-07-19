/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE

#ifdef CONFIG_IMG3_FLASH
/* XIP-flash mode: Secure code runs from XIP (0x00C00020–0x00FFFFFF).
 * entry1 must end before XIP start so the Secure XIP range is not marked NS.
 * entry3 must cover flash+SRAM between Secure XIP end and Secure SRAM start
 * so NS XIP code (0x02000000, 0x04000000) and NS stack (0x20001000–0x20005FFF)
 * remain NS-accessible. KM4TZ_IMG3_XIP ends at 0x01000000. */
#define TZ_S_START			(u32)__image3_entry_func__
#define TZ_SRAM_START		(u32)__image3_ram_start__
#define TZ_XIP_REGION_END	0x01000000U
#elif defined(CONFIG_IMG3_PSRAM)
/* PSRAM mode: Secure code (entry + NSC gateway + bulk text) runs from PSRAM
 * ([TZ_PSRAM_START, TZ_PSRAM_END)); .data/.bss and .sram.only code live in the
 * small SRAM-TZ window ([TZ_SRAM_START, TZ_SRAM_END)). Both windows must be
 * Secure, and the NSC gateway sits inside the Secure PSRAM region. */
#define TZ_SRAM_START		(u32)__image3_ram_start__
#define TZ_SRAM_END			(u32)__image3_ram_end__
#define TZ_PSRAM_START		(u32)__image3_psram_ram_start__
#define TZ_PSRAM_END		(u32)__image3_psram_ram_end__
#else
#define TZ_S_START			(u32)__image3_ram_start__
#define TZ_SRAM_START		(u32)__image3_ram_start__
#define TZ_XIP_REGION_END	0U	/* unused in SRAM mode */
#endif
#define TZ_NSC_START		(u32)__ram_image3_nsc_start__
#define TZ_NSC_END			(u32)__ram_image3_nsc_end__
#ifndef CONFIG_IMG3_PSRAM
#define TZ_S_END			(u32)__image3_ram_end__
#endif

const SAU_CFG_TypeDef sau_config[SAU_ENTRY_NUM] = {
//  Start				End						NSC
	/* entry0 merges the Share-ROM NS range [0x1F000,0x200000) with the
	 * contiguous flash/SRAM NS range below the Secure TZ area: both are NS and
	 * adjacent, and [0,0x1F000) (KM4TZ secure ROM) stays Secure below entry0. */
#ifdef CONFIG_IMG3_PSRAM
	/* Secure = [TZ_SRAM_START,TZ_SRAM_END) (SRAM-TZ) + [TZ_PSRAM_START,TZ_PSRAM_END) (PSRAM-TZ) */
	{0x0001F000,		TZ_SRAM_START - 1,		0},	/* entry0: ShareROM + flash/low-SRAM before Secure SRAM window */
	{TZ_SRAM_END,		TZ_PSRAM_START - 1,		0},	/* entry1: between Secure SRAM and Secure PSRAM */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry2: NSC inside Secure PSRAM */
	{TZ_PSRAM_END,		0xFFFFFFFF,				0},	/* entry3: region after Secure PSRAM */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry4: TODO */
#elif defined(CONFIG_IMG3_FLASH)
	{0x0001F000,		TZ_S_START - 1,			0},	/* entry0: ShareROM + flash before Secure XIP */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry1: NSC inside merged TZ area */
	/* entry2: flash+SRAM between Secure XIP end and Secure SRAM (covers NS XIP code + NS stack) */
	{TZ_XIP_REGION_END,	TZ_SRAM_START - 1,		0},
	{TZ_S_END,			0xFFFFFFFF,				0},	/* entry3: region after Secure SRAM TZ */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry4: TODO */
#else
	{0x0001F000,		TZ_S_START - 1,			0},	/* entry0: ShareROM + flash/SRAM before Secure TZ area */
	{TZ_NSC_START,		TZ_NSC_END - 1,			1},	/* entry1: NSC inside merged TZ area */
	{TZ_S_END,			0xFFFFFFFF,				0},	/* entry2: region after Secure SRAM TZ */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry3: TODO */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry4: TODO */
#endif
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,		0xFFFFFFFF,				0},	/* entry7: TODO */
};

#define BD_RAM_START	(u32)__km4tz_bd_ram_start__
/* Non-secure PSRAM starts at the first segment after the TZ segment (KM4NS),
 * not at the AP image (which is now the last segment). */
#define BD_PSRAM_START 	(u32)__non_secure_psram_start__
#define BD_PSRAM_END	(u32)__non_secure_psram_end__

const TZ_CFG_TypeDef mpc3_config[MPC_ENTRY_NUM] =						/* Security configuration for PSRAM */
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
const TZ_CFG_TypeDef mpc2_config[MPC_ENTRY_NUM] =						/* Security configuration for sram S2 */
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

const TZ_CFG_TypeDef mpc1_config[MPC_ENTRY_NUM] =						/* Security configuration for Flash */
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
