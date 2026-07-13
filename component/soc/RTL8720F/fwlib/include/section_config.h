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

#ifdef __cplusplus
extern "C" {
#endif

// special purpose
#define CMD_TABLE_DATA_SECTION				SECTION(".cmd.table.data")
#define GIT_VER_TABLE_SECTION				SECTION(".git.ver.data")
#define IPC_TABLE_DATA_SECTION				SECTION(".ipc.table.data")
#define UNITY_TABLE_DATA_SECTION			SECTION(".unity.table.rodata")

/* ATCMD section type 1: used by all cores */
#ifdef CONFIG_SUPPORT_ATCMD
#define ATCMD_TABLE_DATA_SECTION			CMD_TABLE_DATA_SECTION
#else
#define ATCMD_TABLE_DATA_SECTION
#endif

/* ATCMD section type 2: Used in AP && !MP_SHRINK mode */
#if (defined(CONFIG_SUPPORT_ATCMD) && (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE))) && !defined(CONFIG_MP_SHRINK)
#define ATCMD_APONLY_TABLE_DATA_SECTION		CMD_TABLE_DATA_SECTION
#else
#define ATCMD_APONLY_TABLE_DATA_SECTION
#endif

// Image 1 Entry Data
#define IMAGE1_ENTRY_SECTION				SECTION(".image1.entry.data")
#define IMAGE1_EXPORT_SYMB_SECTION			SECTION(".image1.export.symb")
#define IMAGE1_VALID_PATTEN_SECTION			SECTION(".image1.validate.rodata")

// Image 2 Entry Data
#define IMAGE2_ENTRY_SECTION				SECTION(".image2.entry.data")

// Image 3 Entry Data
#define IMAGE3_ENTRY_SECTION				SECTION(".image3.nsc_entry.text")
#define IMAGE3_BOOT_SECTION					SECTION(".image3.entry.data")

#define _SEC_STR2(x)						#x
#define _SEC_STR(x)							_SEC_STR2(x)

// Define Non-Cacheable region
#define SRAM_NOCACHE_DATA_SECTION			SECTION(".nocache.data." _SEC_STR(__LINE__))

/* non.dram can put in Flash(No DeepPowerDown) or SRAM after psram disabled, such as pmc code */
#define NON_DRAM_TEXT_SECTION				SECTION(".non.dram.text." _SEC_STR(__LINE__))

/* .sramdram.only means cannot put in Flash, such as flash api */
#define SRAMDRAM_ONLY_TEXT_SECTION			SECTION(".sramdram.only.text." _SEC_STR(__LINE__))

/* sram only used in pmc flow, such as deepsleep entry when flash deep down or after psram disabled */
#define SRAM_ONLY_TEXT_SECTION				SECTION(".sram.only.text." _SEC_STR(__LINE__))
#define SRAM_ONLY_DATA_SECTION				SECTION(".sram.only.data." _SEC_STR(__LINE__))

/* BT SDN interrupter handler is time critical, put it to SRAM to speed up */
#define SDN_SRAM_TEXT_SECTION				SRAM_ONLY_TEXT_SECTION
#define SDN_SRAM_DATA_SECTION				SRAM_ONLY_DATA_SECTION

#define SRAM_WLAN_CRITICAL_CODE_SECTION     //SECTION(".wlan.critical.text")
#define SRAM_IPERF_CRITICAL_CODE_SECTION    SECTION(".wlan.critical.text")
#define SRAM_LWIP_CRITICAL_CODE_SECTION_L1  //SECTION(".wlan.critical.text")
#define SRAM_LWIP_CRITICAL_CODE_SECTION_L2  //SECTION(".wlan.critical.text")

// Wlan Section(Not Used)
#define WLAN_ROM_TEXT_SECTION
#define WLAN_ROM_DATA_SECTION
#define WLAN_RAM_MAP_SECTION
#define WLAN_ROM_BSS_SECTION

#ifndef CONFIG_BUILD_ROM

#define HAL_ROM_BSS_SECTION
#define HAL_ROM_BSS_SECTION_BANK
#define HAL_ROM_TEXT_SECTION
#define HAL_ROM_DATA_SECTION
#define SHELL_ROM_TEXT_SECTION
#define HAL_ROM_DATA_SECTION_BANK
#define SHELL_ROM_BSS_SECTION
#define LIBC_ROM_TEXT_SECTION
#define ROM_VECTOR_TABLE1_SECTION
#define RAM_VECTOR_TABLE2_SECTION
#define RAM_VECTOR_TABLE3_SECTION

#define HAL_ROM_TEXT_SECTION_SRAM       SRAM_ONLY_TEXT_SECTION
#define ROM_VECTOR_TABLE1_SECTION_SRAM       SRAM_ONLY_DATA_SECTION

#endif
#ifdef __cplusplus
}
#endif

#endif //_SECTION_CONFIG_H_
