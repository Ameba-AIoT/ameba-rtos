/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SECTION_CONFIG_H_
#define _SECTION_CONFIG_H_

#include "basic_types.h"
#include "platform_autoconf.h"

#ifdef CONFIG_BUILD_ROM
#include "section_config_rom.h"
#endif

// special purpose
#define CMD_TABLE_DATA_SECTION				SECTION(".cmd.table.data")
#define GIT_VER_TABLE_SECTION				SECTION(".git.ver.data")
#define IPC_TABLE_DATA_SECTION				SECTION(".ipc.table.data")
#define UNITY_TABLE_DATA_SECTION			SECTION(".unity.table.data")
#define ATCMD_TABLE_DATA_SECTION				SECTION(".atcmd.table.rodata")

// Image 1 Entry Data
#define IMAGE1_ENTRY_SECTION				SECTION(".image1.entry.data")
#define IMAGE1_EXPORT_SYMB_SECTION			SECTION(".image1.export.symb")
#define IMAGE1_VALID_PATTEN_SECTION			SECTION(".image1.validate.rodata")

// Image 2 Entry Data
#define IMAGE2_ENTRY_SECTION				SECTION(".image2.entry.data")

// Image 3 Entry Data
#define IMAGE3_ENTRY_SECTION				SECTION(".image3.nsc_entry.text")

// Define Non-Cacheable region
#define SRAM_NOCACHE_DATA_SECTION			SECTION(".nocache.data")

#define TIMESENSITIVE_TEXT_SECTION			SECTION(".timesensitive.text")
#define TIMESENSITIVE_DATA_SECTION			SECTION(".timesensitive.data")

/* non.dram can put in Flash(No DeepPowerDown) or SRAM after psram disabled, such as pmc code */
#define NON_DRAM_TEXT_SECTION				SECTION(".non.dram.text")

/* .sramdram.only means cannot put in Flash, such as flash api */
#define SRAMDRAM_ONLY_TEXT_SECTION			SECTION(".sramdram.only.text")
#define SRAMDRAM_ONLY_DATA_SECTION			SECTION(".sramdram.only.data")  // Only used to replace HAL_ROM_DATA_SECTION in CA32

/* sram only used in pmc flow, such as deepsleep entry when flash deep down or after psram disabled */
#define SRAM_ONLY_TEXT_SECTION				SECTION(".sram.only.text")
#define SRAM_ONLY_DATA_SECTION				SECTION(".sram.only.data")

#define SRAM_WLAN_CRITICAL_CODE_SECTION

// Wlan Section(Not Used)
#define WLAN_ROM_TEXT_SECTION
#define WLAN_ROM_DATA_SECTION
#define WLAN_RAM_MAP_SECTION
#define WLAN_ROM_BSS_SECTION

#define	HAL_ROM_TEXT_SECTION				SRAMDRAM_ONLY_TEXT_SECTION
#define	HAL_ROM_DATA_SECTION				SRAMDRAM_ONLY_DATA_SECTION
#define	HAL_ROM_BSS_SECTION

#define LIBC_ROM_TEXT_SECTION				SRAMDRAM_ONLY_TEXT_SECTION
#define	LIBC_ROM_DATA_SECTION				SRAMDRAM_ONLY_TEXT_SECTION

#define SHELL_ROM_TEXT_SECTION				SRAMDRAM_ONLY_TEXT_SECTION
#define SHELL_ROM_BSS_SECTION
#define SHELL_ROM_DATA_SECTION				SRAMDRAM_ONLY_TEXT_SECTION

#define BOOT_RAM_TEXT_SECTION				SECTION(".boot.ram.text")
#endif //_SECTION_CONFIG_H_
