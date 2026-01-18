/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

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
	/* Delay rom section should be cacheable for accurate delay so it is not included. */
	/* TCM Cache (0x000F0000, 0x00100000) is used in KM4_TZ */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0;
	mpu_cfg.region_size = 0x00147800 - 0;
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

	/* retention ram is no-cache region */

	return 0;
}

void os_init(void)
{
#ifdef CONFIG_PLATFORM_FREERTOS_ROM
	/* If using rom os, some variables should be initialized */
	extern void os_rom_init(void);
	os_rom_init();
#endif

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
	SystemCoreClockUpdate();
	RTK_LOGI(TAG, "NP CPU CLK: %lu Hz \n", SystemCoreClock);

	/* configure FreeRTOS interrupt and heap region for printf(RTK_LOGx)*/
	os_init();

	/* low power pin dont need pinmap init again after wake from dslp */
	pinmap_init(); /* 1.7ms */

	newlib_locks_init();

	/* 6. Configure MPU */
	mpu_init();
	app_mpu_nocache_init();

	/* NP can not assign non-cache region which is less than 512M */
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	SOCPS_WakeFromPG_NP,
	(u32)RomVectorTable,
};
