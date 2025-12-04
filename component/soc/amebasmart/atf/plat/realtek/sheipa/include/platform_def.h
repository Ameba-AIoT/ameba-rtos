/*
 * Copyright (c) 2015-2019, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>
#include <common/tbbr/tbbr_img_def.h>
#include <lib/utils_def.h>
#include <plat/common/common_def.h>
#include "hal_platform.h"
#include "sysreg_lsys.h"
#include "platform_autoconf.h"

/*
 * Due to the 256KB SRAM constraint, the FIP, BL2, and BL33 are no longer copied from Flash.​
 * BL2 and BL33 will execute in place (XIP) from Flash, while BL32 will still be relocated to SRAM for execution.​
 * The SRAM layout is partitioned based on the actual code size of each corresponding bootloader.​
 *
 * FIP Layout: 0x20100000 – 0x2014FFFF (320KB)
 * FIP_ALIGN=64KB
 *
 * | Image              | Range                  | Actual Size  |
 * |--------------------|------------------------|--------------|
 * | TOC                | 0x20100000-0x201000FF  | 0x1A0        |
 * | bl2.bin            | 0x20110000-0x2011A2A4  | 0xA2A5       |
 * | bl32.bin (sp_min)  | 0x20120000-0x20124048  | 0x4049       |
 * | bl33.bin (FreeRTOS)| 0x20130000-0x2014FFFF  | 0x145E0      |
 *
 *
 * SRAM Layout: 0x40000000 – 0x40040000 (256KB)
 *
 * | Usage          | Size (KB) | Range                  |
 * |----------------|-----------|------------------------|
 * | SHARED_SRAM    | 4         | 0x40000000 - 0x40000FFF|
 * | BL1 (data)     | 44        | 0x40001000 - 0x4000BFFF|
 * | BL2 (data)     | 44        | 0x4000C000 - 0x40016FFF|
 * | BL32 (text+data)| 72       | 0x40017000 - 0x40028FFF|
 * | BL33 (data)    | 92        | 0x40029000 - 0x4003FFFF|
 */

/* Special value used to verify platform parameters from BL2 to BL3-1 */
#define SHEIPA_BL31_PLAT_PARAM_VAL	0x0f1e2d3c4b5a6978ULL

#define PLATFORM_STACK_SIZE 0x1000

#define PLATFORM_MAX_CPUS_PER_CLUSTER	2
#define PLATFORM_CLUSTER_COUNT		1
#define PLATFORM_CLUSTER0_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CLUSTER1_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER_COUNT * PLATFORM_MAX_CPUS_PER_CLUSTER)

#define SHEIPA_PRIMARY_CPU		0
#define PLAT_NUM_PWR_DOMAINS		(1 + PLATFORM_CLUSTER_COUNT + \
					PLATFORM_CORE_COUNT)
#define PLAT_MAX_PWR_LVL		MPIDR_AFFLVL2

#define PLAT_MAX_RET_STATE		1
#define PLAT_MAX_OFF_STATE		2

/* Local power state for power domains in Run state. */
#define PLAT_LOCAL_STATE_RUN		0
/* Local power state for retention. Valid only for CPU power domains */
#define PLAT_LOCAL_STATE_RET		1
/*
 * Local power state for OFF/power-down. Valid for CPU and cluster power
 * domains.
 */
#define PLAT_LOCAL_STATE_OFF		2

/*
 * Macros used to parse state information from State-ID if it is using the
 * recommended encoding for State-ID.
 */
#define PLAT_LOCAL_PSTATE_WIDTH		4
#define PLAT_LOCAL_PSTATE_MASK		((1 << PLAT_LOCAL_PSTATE_WIDTH) - 1)

/*
 * Some data must be aligned on the biggest cache line size in the platform.
 * This is known only to the platform as it might have a combination of
 * integrated and external caches.
 */
#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

/*
 * Partition memory into secure ROM, non-secure DRAM, secure "SRAM",
 * and secure DRAM.
 */

#define KM0_IPC_RAM				0x2301FD00

#define SEC_ROM_BASE			0x0
#define SEC_ROM_SIZE			0x00000400

#ifdef CONFIG_CP_TEST_CA32
#define DRAM_RANGE_BASE			UL(0x20000000)
#define DRAM_RANGE_SIZE			UL(0x20000000)
#else
#define DRAM_RANGE_BASE			UL(0x60000000)
#define DRAM_RANGE_SIZE			UL(0x20000000)
#endif

#ifdef CONFIG_CP_TEST_CA32
#define SEC_DRAM_BASE			UL(0x20020000)
#define SEC_DRAM_SIZE			(0x20040000 - SEC_DRAM_BASE)
#elif defined(CONFIG_IMG2_FLASH)
#define SEC_DRAM_BASE			UL(0x70060000)
#define SEC_DRAM_SIZE			UL(0x00028000)
#else
#define SEC_DRAM_BASE 			UL(0x70180000)
#define SEC_DRAM_SIZE 			UL(0x00180000)
#endif

#ifdef CONFIG_CP_TEST_CA32
#define NS_DRAM0_BASE			0x20000000
#define NS_DRAM0_SIZE			(0x20020000 - NS_DRAM0_BASE)
#else
#define NS_DRAM0_BASE			((SEC_DRAM_BASE + SEC_DRAM_SIZE) & ~TZ_IDAU_SEC_OFFSET)
#define NS_DRAM0_SIZE			(PSRAM_END - NS_DRAM0_BASE)
#endif
/*
 * ARM-TF lives in SRAM, partition it here
 */
#ifdef CONFIG_IMG2_FLASH
#define SHARED_RAM_BASE			BL2_LIMIT
#define SHARED_RAM_SIZE			0x00001000
#else
#define SHARED_RAM_BASE			BL2_LIMIT
#define SHARED_RAM_SIZE			0x00010000
#endif

#define PLAT_SHEIPA_TRUSTED_MAILBOX_BASE	SHARED_RAM_BASE
#define PLAT_SHEIPA_TRUSTED_MAILBOX_SIZE	(8 + PLAT_SHEIPA_HOLD_SIZE)
#define PLAT_SHEIPA_HOLD_BASE		(PLAT_SHEIPA_TRUSTED_MAILBOX_BASE + 8)
#define PLAT_SHEIPA_HOLD_SIZE		(PLATFORM_CORE_COUNT * \
					 PLAT_SHEIPA_HOLD_ENTRY_SIZE)
#define PLAT_SHEIPA_HOLD_ENTRY_SIZE	8
#define PLAT_SHEIPA_HOLD_STATE_WAIT	0
#define PLAT_SHEIPA_HOLD_STATE_GO		1

#define PLAT_SHEIPA_WARM_BOOT_BASE	(PLAT_SHEIPA_TRUSTED_MAILBOX_BASE + 0x100)

/*
 * BL1 specific defines.
 *
 * BL1 RW data is relocated from ROM to RAM at runtime so we need 2 sets of
 * addresses.
 * Put BL1 RW at the top of the Secure SRAM. BL1_RW_BASE is calculated using
 * the current BL1 RW debug size plus a little space for growth.
 */
#ifdef CONFIG_IMG2_FLASH
#define BL1_RO_SIZE			(0x4000)
#define BL1_RW_SIZE			(0x8000)
#else
#define BL1_RO_SIZE			(0x20000)
#define BL1_RW_SIZE			(0x10000)
#endif

#ifdef CONFIG_CP_TEST_CA32
#define BL1_RO_BASE			0x0A000000
#define BL1_RO_LIMIT		(BL1_RO_BASE + BL1_RO_SIZE)
#define BL1_RW_BASE			(SEC_DRAM_BASE)
#define BL1_RW_LIMIT		(BL1_RW_BASE + BL1_RW_SIZE)
#else
#define BL1_RO_BASE			SEC_DRAM_BASE
#define BL1_RO_LIMIT		(BL1_RO_BASE + BL1_RO_SIZE)
#define BL1_RW_BASE			(BL1_RO_LIMIT)
#define BL1_RW_LIMIT		(BL1_RW_BASE + BL1_RW_SIZE)
#endif
/*
 * BL2 specific defines.
 *
 * Put BL2 just below BL3-1. BL2_BASE is calculated using the current BL2 debug
 * size plus a little space for growth.
 */

#ifdef CONFIG_CP_TEST_CA32
#define BL2_RO_BASE			(SHEIPA_FIP_BASE + 0x10000)
#define BL2_RO_SIZE			(0x10000) //64KB
#define BL2_RO_LIMIT		(BL2_RO_BASE + BL2_RO_SIZE)

#define BL2_BASE			(BL1_RW_LIMIT)
#define BL2_LIMIT			(BL2_BASE + 0x8000)
#elif defined(CONFIG_IMG2_FLASH)
#define BL2_BASE			(BL1_RW_LIMIT)
#define BL2_LIMIT			(BL2_BASE + 0xC000)
#else
#define BL2_BASE 			(BL1_RW_LIMIT)
#define BL2_LIMIT			(BL2_BASE + 0x40000)
#endif

#define BL2_RW_BASE			(BL2_BASE)
#define BL2_RW_LIMIT		(BL2_LIMIT)
#define BL2_RW_SIZE			(BL2_RW_LIMIT - BL2_RW_BASE)

/*
 * BL3-1 specific defines.
 *
 * Put BL3-1 at the top of the Trusted SRAM. BL31_BASE is calculated using the
 * current BL3-1 debug size plus a little space for growth.
 */
#define BL31_BASE
#define BL31_LIMIT

/*
 * BL3-2 specific defines.
 *
 * BL3-2 can execute from Secure SRAM, or Secure DRAM.
 */
#define BL32_DRAM_BASE			(SHARED_RAM_BASE + SHARED_RAM_SIZE)
#define BL32_DRAM_LIMIT			(SEC_DRAM_BASE + SEC_DRAM_SIZE)

#define SEC_DRAM_ID				1

#define BL32_BASE				BL32_DRAM_BASE
#define BL32_LIMIT				BL32_DRAM_LIMIT
#define BL32_SIZE				(BL32_LIMIT - BL32_BASE)

#define BL32_MEM_BASE			BL32_DRAM_BASE
#define BL32_MEM_SIZE			(BL32_LIMIT - BL32_BASE)

#define NS_IMAGE_OFFSET			NS_DRAM0_BASE
#define NS_IMAGE_MAX_SIZE		NS_DRAM0_SIZE

#define PLAT_PHY_ADDR_SPACE_SIZE	(1ull << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ull << 32)
#define MAX_MMAP_REGIONS		16
#define MAX_XLAT_TABLES			6 /* align with 2M will save xlat table, actually used is 5 */
#define MAX_IO_DEVICES			3
#define MAX_IO_HANDLES			4

/* Load pageable part of OP-TEE 2MB above BL32 base */
#define SHEIPA_OPTEE_PAGEABLE_LOAD_BASE	(BL32_BASE + 0x00200000)
#define SHEIPA_OPTEE_PAGEABLE_LOAD_SIZE	0x00100000

#define SHEIPA_OPTEE_SHMEM_START		0x602e0000
#define SHEIPA_OPTEE_SHMEM_SIZE			0x00020000

/*******************************************************************************
 * TSP  specific defines.
 ******************************************************************************/
#define TSP_SEC_MEM_BASE		BL32_BASE
#define TSP_SEC_MEM_SIZE		(BL32_LIMIT - BL32_BASE + 1)

/* ID of the secure physical generic timer interrupt used by the TSP */
#define ARM_IRQ_SEC_PHY_TIMER		29
#define TSP_IRQ_SEC_PHY_TIMER		ARM_IRQ_SEC_PHY_TIMER
/*
 * UART related constants
 */
#define UART0_BASE			UARTLOG_REG_BASE
#define UART1_BASE			UARTLOG_REG_BASE
#define UART0_CLK_IN_HZ			25000000
#define UART1_CLK_IN_HZ			25000000

#define PLAT_SHEIPA_BOOT_UART_BASE	UART0_BASE
#define PLAT_SHEIPA_BOOT_UART_CLK_IN_HZ	UART0_CLK_IN_HZ   	// not used

#define PLAT_SHEIPA_CRASH_UART_BASE		UART1_BASE
#define PLAT_SHEIPA_CRASH_UART_CLK_IN_HZ	UART1_CLK_IN_HZ

#define PLAT_SHEIPA_CONSOLE_BAUDRATE	1500000

#define SHEIPA_FLASH_BASE		SPI_FLASH_BASE
#define SHEIPA_FLASH_SIZE		0x08000000

#ifdef CONFIG_CP_TEST_CA32
#define SHEIPA_FIP_BASE		(0x0C000000)
#define SHEIPA_FIP_SIZE		(0x01000000)
#else
#define SHEIPA_FIP_BASE		(NS_DRAM0_BASE + TZ_IDAU_SEC_OFFSET)
#define SHEIPA_FIP_SIZE		NS_DRAM0_SIZE
#endif
#define PLAT_SHEIPA_FIP_BASE		SHEIPA_FIP_BASE
#define PLAT_SHEIPA_FIP_MAX_SIZE	SHEIPA_FIP_SIZE

#define DEVICE0_BASE			0x40000000
#define DEVICE0_SIZE			0x20000000

/* TODO: Non-volatile counters for Trusted board boot,
 * Because fast model did't have hw non-volatile counters, so
 * we did't implement this feature.
 */

/*
 * GIC related constants
 */
#if ARM_GIC_ARCH == 3
#define GICD_BASE			0x02100000
#define GICR_BASE			0x02140000
#else
#define GICD_BASE			0xA0101000
#define GICC_BASE			0xA0102000
#endif

#define DEVICE1_BASE			0x80000000
#define DEVICE1_SIZE			0x40000000 /* maximum address < 0xC0000000, save translation table size */

/*
 * DT related constants
 */
#if (BOOT_FLASH)
#define PLAT_SHEIPA_DT_BASE		(SEC_ROM_BASE + 0x40000)
#define PLAT_SHEIPA_DT_MAX_SIZE		0x000c0000
#else
#define PLAT_SHEIPA_DT_BASE			0x81f00000
#define PLAT_SHEIPA_DT_MAX_SIZE		0x000c0000
#endif

/*
 * System counter
 */
#define SYS_COUNTER_FREQ_IN_TICKS	(0x1800000)

#define MPC1_BASE_S          KM4_MPC1_BASE_S
/*
 * Boot flag related constants
 */

#define BOOT_REASON_SW_REG			(SYSTEM_CTRL_BASE_LP + REG_LSYS_BOOT_REASON_SW)
#define BIT_AP_WAKE_FROM_PG_FLAG	(0x00000001UL << 16) // LSYS_BIT_AP_WAKE_FROM_PG
#define BIT_AP_RUNNING				(0x00000001UL << 17) // LSYS_BIT_AP_RUNNING

#endif /* PLATFORM_DEF_H */
