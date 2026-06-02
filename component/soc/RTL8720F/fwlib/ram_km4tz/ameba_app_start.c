/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "ameba_v8m_backtrace.h"

static const char *TAG = "APP";

extern void newlib_locks_init(void);
extern int main(void);
extern void SOCPS_WakeFromPG_AP(void);

u32 app_mpu_nocache_check(u32 mem_addr)
{
	mpu_region_config mpu_cfg;


	mpu_cfg.region_base = (uint32_t)__ram_nocache_start__;
	mpu_cfg.region_size = __ram_nocache_end__ - __ram_nocache_start__;

	if ((mem_addr >= mpu_cfg.region_base) && (mem_addr < (mpu_cfg.region_base + mpu_cfg.region_size))) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*AP have 8 secure mpu entrys & 8 nonsecure mpu entrys*/
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	/* ROM Code inside CPU does not enter Cache, Set to RO for NULL ptr access error */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x20000;
	mpu_cfg.region_size = 0x18000;
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
	if (mpu_cfg.region_size >= 32) {
		mpu_region_cfg(mpu_entry, &mpu_cfg);
	}

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

	return 0;
}

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif

void app_testmode_status(void)
{
	/* OTPC and SIC share one master port, OTPC use the port defaultly and SIC can use then OTPC autoload done. */
	if (SYSCFG_TRP_TestMode()) {
		if (SYSCFG_TRP_OTPBYP()) {
			RTK_LOGI(TAG, "Bypass OTP autoload\r\n");
		} else {
			RTK_LOGI(TAG, "In Test mode: 0x%lx\r\n", SYSCFG_TRP_ICFG());
		}
	}
}


void app_init_debug_flag(void)
{
	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		/* Set log level for diaplaying*/
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_ERROR);
	}
}

void os_init(void)
{
#ifdef CONFIG_PSRAM_ALL_FOR_AP_HEAP
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
	extern bool os_heap_add(u8 * start_addr, size_t heap_size);
	if (ChipInfo_PsramExists()) {
		os_heap_add((uint8_t *)__km4tz_bd_psram_start__, (size_t)(__non_secure_psram_end__ - __km4tz_bd_psram_start__));
	}
#endif
#endif
	rtos_mem_init();
}

// The Main App entry point
void app_start(void)
{
	// if(MCM_SINGLE_DIE != ChipInfo_MemoryType()) {
	/* enable non-secure cache */
	Cache_Enable(ENABLE);
	// }

	/* Image2 Bss Initial */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));

	app_init_debug_flag();

#ifdef CONFIG_TRUSTZONE
	PutChar = (void (*)(char)) LOGUART_PutChar;
	SCB->VTOR = (u32)RomVectorTable;
	RomVectorTable[0] = (HAL_VECTOR_FUN)MSP_RAM_HP_NS;
#endif

	app_testmode_status();

	data_flash_highspeed_setup();

	SystemCoreClockUpdate();
	RTK_LOGI(TAG, "AP CPU CLK: %lu Hz \n", SystemCoreClock);

	/* Init heap region and configure FreeRTOS */
	os_init();


	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) {//Only Asic need OSC Calibration
		OSC2M_Calibration(30000);
	}

	SYSTIMER_Init();
	/* low power pin dont need pinmap init again after wake from dslp */
	pinmap_init(); /* 1.7ms */
#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	newlib_locks_init();

	mpu_init();
	app_mpu_nocache_init();

	PostFaultPatch_register(backtrace_post_patch_ameba);

	main(); /* project/xxxx/src/main.c */
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	SOCPS_WakeFromPG_AP,
	(u32) RomVectorTable
};
