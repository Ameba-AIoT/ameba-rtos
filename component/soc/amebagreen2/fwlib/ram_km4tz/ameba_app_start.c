/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "os_wrapper_memory.h"

static const char *TAG = "APP";

extern int main(void);
extern void NS_ENTRY BOOT_IMG3(void);
extern void SOCPS_WakeFromPG_AP(void);

#if defined(CONFIG_PLATFORM_FREERTOS) && defined(CONFIG_TRUSTZONE)
SRAM_ONLY_DATA_SECTION
HAL_VECTOR_FUN RamVectorTable[95] ALIGNMTO(512) = {0};
#endif
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
	/* TCM Cache (0x000F0000, 0x00100000) is to be used as RAM in fullmac mode and it can't be read-only. */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0;
	mpu_cfg.region_size = 0x000F0000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RO;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* Currently, open share rom cache for better throughput and fix issue https://jira.realtek.com/browse/RSWLANDIOT-11327 */
	/* close share rom cache. Delay rom section should be cacheable for accurate
	   delay so it is not included. */
	// mpu_entry = mpu_entry_alloc();
	// mpu_cfg.region_base = 0x00100000;
	// mpu_cfg.region_size = 0x00147800 - 0x00100000;
	// mpu_cfg.xn = MPU_EXEC_ALLOW;
	// mpu_cfg.ap = MPU_UN_PRIV_RO;
	// mpu_cfg.sh = MPU_NON_SHAREABLE;
	// mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	// mpu_region_cfg(mpu_entry, &mpu_cfg);

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

	return 0;
}

#ifndef CONFIG_WIFI_HOST_CONTROL
#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif
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
#if !defined(CONFIG_WIFI_HOST_CONTROL) || defined(CONFIG_FULLMAC_NEW_VERSION) // When fullmac support XIP, need enable Cache and cannot share cache to TCM
	/* enable non-secure cache */
	Cache_Enable(ENABLE);
#endif

	/* Rom Bss NS Initial */
	_memset((void *) __rom_bss_start_ns__, 0, (__rom_bss_end_ns__ - __rom_bss_start_ns__));
	/* Image2 Bss Initial */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));

	RBSS_UDELAY_DIV = 5;
#if defined(CONFIG_PLATFORM_FREERTOS) && defined(CONFIG_TRUSTZONE)
	/*When using RAM OS and trustzone is enabled, the non secure image should use non_secure stack*/
	_memset(RamVectorTable, 0, sizeof(RamVectorTable));
	_memcpy(RamVectorTable, RomVectorTable, sizeof(RamVectorTable));
	RamVectorTable[0] = (HAL_VECTOR_FUN)MSP_RAM_HP_NS;
	SCB->VTOR = (u32)RamVectorTable;
#else
	/* When TZ not enabled, re-init pendsv/svcall/systick in the non-secure vector table for OS.*/
	SCB->VTOR = (u32)RomVectorTable;
#endif

#ifdef CONFIG_TRUSTZONE
	BOOT_IMG3();

	cmse_address_info_t cmse_address_info = cmse_TT((void *)app_start);
	RTK_LOGI(TAG, "IMG2 SECURE STATE: %d\n", cmse_address_info.flags.secure);
#endif

	app_testmode_status();

	data_flash_highspeed_setup();

	SystemCoreClockUpdate();
	RTK_LOGI(TAG, "AP CPU CLK: %lu Hz \n", SystemCoreClock);

	/* Init heap region and configure FreeRTOS */
	os_init();
	XTAL_INIT();

	if ((SYSCFG_RLVersion()) >= SYSCFG_CUT_VERSION_B) {
		if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC_POSTSIM) {//Only Asic need OSC Calibration
			OSC4M_Init();
			OSC4M_Calibration(30000);
		}

	}

#ifndef CONFIG_WIFI_HOST_CONTROL
#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif
#endif

	mpu_init();
	app_mpu_nocache_init();

	main(); /* project/xxxx/src/main.c */
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	SOCPS_WakeFromPG_AP,
	(u32) RomVectorTable
};

