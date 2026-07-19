/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rom/ameba_dump_stack.h"
#include "rom/ameba_v8m_crashdump.h"
#include "ameba_v8m_backtrace.h"


static const char *const TAG = "APP";
extern void newlib_locks_init(void);
extern int main(void);
extern void SOCPS_WakeFromPG_NP(void);

//set all KM0 rom & ram no-cachable, just flash cachable
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	/* ROM Code inside CPU does not enter Cache, Set to RO for NULL ptr access error */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x20000;
	mpu_cfg.region_size = 0x1000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RO;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* set nocache region */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = (uint32_t)__ram_nocache_start__;
	mpu_cfg.region_size = __ram_nocache_end__ - __ram_nocache_start__;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* set global bss to nocache */
	DCache_CleanInvalidate((u32)__global_bss_start__, (u32)(__global_bss_end__ - __global_bss_start__));
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = (uint32_t)__global_bss_start__;
	mpu_cfg.region_size = __global_bss_end__ - __global_bss_start__;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	if (mpu_cfg.region_size >= 32) {
		mpu_region_cfg(mpu_entry, &mpu_cfg);
	}

	/* retention ram is no-cache region */
	/* IMG2 on flash keeps this SRAM region non-cacheable, unless all TRX code runs in SRAM */
#if defined(CONFIG_IMG2_FLASH) && !defined(CONFIG_WIFI_TRX_SRAM_ALL)
	/* set nocache region */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = (uint32_t)__sram_image2_start__;
	mpu_cfg.region_size = N_BYTE_ALIGMENT(((u32)__sram_image2_end__ - (u32)__sram_image2_start__), 32);
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	if (mpu_cfg.region_size >= 32) {
		mpu_region_cfg(mpu_entry, &mpu_cfg);
	}
#endif

	return 0;
}

void os_init(void)
{
	rtos_mem_init();
}

#ifdef CONFIG_SOLO

static void BOOT_ROM_InitDebugFlg(void)
{
	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying*/
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}
}

static void BOOT_CleanupSection(void)
{
	/* Important: Cleaning up the BSS section must be done before Heap init. */
	/* Clear ROM bss*/
	_memset((void *) __rom_bss_start__, 0, (__rom_bss_end__ - __rom_bss_start__));
	/* Clear application bss */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
}

void app_init_solo(void)
{
	u32 CpuClk;

	BOOT_CleanupSection();

	/* init necessary rom bss */
	PutChar = (void (*)(char)) LOGUART_PutChar;
	Crashmain_register(crash_Main);

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();

	SCB->VTOR = (u32)RomVectorTable;

	CpuClk = CPU_ClkGet();
	DelayClkUpdate(CpuClk);

	/* Set the crash dump stack depth for debugging */
	crash_SetExStackDepth(MIN_DUMP_DEPTH);
}
#endif

// The Main App entry point
void app_start(void)
{
#ifdef CONFIG_SOLO
	app_init_solo();
#endif
	SystemCoreClockUpdate();
	RTK_LOGI(TAG, "NP CPU CLK: %lu Hz \n", SystemCoreClock);

	/* configure FreeRTOS interrupt and heap region for printf(RTK_LOGx)*/
	os_init();

	newlib_locks_init();

	/* 6. Configure MPU */
	mpu_init();
	app_mpu_nocache_init();

	/* NP can not assign non-cache region which is less than 512M */
	PostFaultPatch_register(backtrace_post_patch_ameba);

	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	SOCPS_WakeFromPG_NP,
	(u32)RomVectorTable,
};
