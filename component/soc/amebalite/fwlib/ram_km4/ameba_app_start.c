/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *TAG = "APP";

extern int main(void);
extern u32 GlobalDebugEnable;
void NS_ENTRY BOOT_IMG3(void);
void app_init_psram(void);
extern void SOCPS_WakeFromPG_KM4(void);

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

/*KM4 have 4 secure mpu entrys & 8 nonsecure mpu entrys*/
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

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

	/* close rom_ns cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x1E000;
	mpu_cfg.region_size = 0x45000 - 0x1E000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	return 0;
}

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif

// The Main App entry point
void app_start(void)
{
	RTK_LOGI(TAG, "KM4 APP START \n");
	/* 1. Enable non-secure cache */
	Cache_Enable(ENABLE);

	/* 2. Configure heap region, and printf also needs to use heap. */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
	/* 3. irq table init and retarget interrupt vector */
	irq_table_init(MSP_RAM_HP_NS); /* NS Vector table init */

	/* Redirect hardfault for debug, and register function pointer to print task information when a crash occurs */
	Fault_Hanlder_Redirect(vTaskCrashCallback);

	RTK_LOGI(TAG, "VTOR: %lx, VTOR_NS:%lx\n", SCB->VTOR, SCB_NS->VTOR);

#ifdef CONFIG_AP_CORE_KM4
	extern bool os_heap_add(u8 * start_addr, size_t heap_size);
	if (ChipInfo_GetChipSram()) {
		os_heap_add((u8 *)__ap_sram_heap_start, (size_t) __ap_sram_heap_size);
	}
#endif
	rtos_mem_init();

	/* 4. lp platform has three master(KR4, OTPC, SIC) and two master port
	  * OTPC and KR4 share one master port, and SIC occupy another master port
	  * OTPC is selected by default and then KR4 is selected after OTPC autoload finished.
	  * KR4 boot is not conflict with SIC because they do not share with a common port.
	*/
	if (SYSCFG_TRP_TestMode()) {
		if (SYSCFG_TRP_OTPBYP()) {
			RTK_LOGI(TAG, "Bypass OTP autoload\r\n");
		} else {
			RTK_LOGI(TAG, "In Test mode: 0x%lx\r\n", SYSCFG_TRP_ICFG());
		}
	}

	/* 5. Confirm the CPU status */
	cmse_address_info_t cmse_address_info = cmse_TT((void *)app_start);
	RTK_LOGI(TAG, "IMG2 SECURE STATE: %d\n", cmse_address_info.flags.secure);

#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
	/* 6. Load secure image */
	BOOT_IMG3();
#endif

	/* 7.Update CPU clk */
	SystemCoreClockUpdate();

	/* 8. Calibration Clock*/
	XTAL_INIT();
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) {//Only Asic need OSC Calibration
		if (((BOOT_Reason()) & AON_BIT_RSTF_DSLP) == FALSE) {
			OSC131K_Calibration(30000); /* PPM=30000=3% *//* 7.5ms */
		}
		OSC4M_Init();
		OSC4M_Calibration(30000);
		XTAL_PDCK();
	}

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif
	/* 9. MPU Init*/
	mpu_init();

	app_mpu_nocache_init();

	/* Force SP align to 8bytes */
	__asm(
		"ldr r1, =#0xFFFFFF80\n"
		"mov r0, sp \n"
		"and r0, r0, r1\n"
		"mov sp, r0\n"
	);
	/* 10. Enter main */
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
#ifndef CONFIG_MP_INCLUDED
	SOCPS_WakeFromPG_KM4,
#else
	NULL,
#endif
	(u32)NewVectorTable
};

