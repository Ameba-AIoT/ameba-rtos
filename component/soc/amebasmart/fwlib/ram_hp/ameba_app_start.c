/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_system.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *const TAG = "APP";
#if defined(CONFIG_EXAMPLE_CM_BACKTRACE) && CONFIG_EXAMPLE_CM_BACKTRACE
#include "cm_backtrace/example_cm_backtrace.h"
#endif

extern void newlib_locks_init(void);
extern int main(void);
extern u32 GlobalDebugEnable;
void NS_ENTRY BOOT_IMG3(void);
void app_init_psram(void);

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
	mpu_cfg.region_size = 0x54000 - 0x1E000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* set 512Byte retention ram no-cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x23020000;
	mpu_cfg.region_size = 0x200;
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

#if 0
static void aontimer_dslp_handler(void)
{
	SOCPS_AONTimerClearINT();
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
}

static void rtc_dslp_handler(void)
{
	RTC_AlarmClear();
	RTC_DayIntClear();
	RTC_WakeupClear();
}

static void wakepin_dslp_handler(void)
{
	u32 pinidx;
	pinidx = SOCPS_WakePinCheck();
	SOCPS_WakePinClearINT(pinidx);
}

static void dslp_wake_handler(void)
{
	u32 BootReason;

	BootReason = SOCPS_AONWakeReason();
	RTK_LOGI(TAG, "DSLP WAKE REASON: %lx \n", BootReason);

	if (BootReason & AON_BIT_TIM_ISR_EVT) {
		//RTK_LOGI(TAG, "dslp from aontimer\n");
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		//SOCPS_AONTimerINT_EN_HP(ENABLE);
		InterruptRegister((IRQ_FUN)aontimer_dslp_handler, AON_TIM_IRQ, NULL, 3);
		InterruptEn(AON_TIM_IRQ, 3);
	}

	if (BootReason & AON_BIT_RTC_ISR_EVT) {
		InterruptRegister((IRQ_FUN)rtc_dslp_handler, RTC_IRQ, NULL, 3);
		InterruptEn(RTC_IRQ, 3);
	}

	if (BootReason & (AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT | AON_BIT_GPIO_PIN2_WAKDET_EVT | AON_BIT_GPIO_PIN3_WAKDET_EVT)) {
		InterruptRegister((IRQ_FUN)wakepin_dslp_handler, AON_WAKEPIN_IRQ, NULL, 3);
		InterruptEn(AON_WAKEPIN_IRQ, 3);
	}
}
#endif

// The Main App entry point
void app_start(void)
{
	RTK_LOGI(TAG, "KM4 APP_START \n");
	/* 1. enable non-secure cache */
	Cache_Enable(ENABLE);

	/* 2. Init heap region for printf */
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
	/* 3. Initialize Non-secure vector table and retarget partly exception handler function. */
	irq_table_init(MSP_RAM_HP_NS); /* NS Vector table init */
	/* Redirect hardfault for debug, and register function pointer to print task information when a crash occurs */
	Fault_Hanlder_Redirect(vTaskCrashCallback);

	RTK_LOGI(TAG, "VTOR: %lx, VTOR_NS:%lx\n", SCB->VTOR, SCB_NS->VTOR);

	rtos_mem_init();

	/* 4. low power pin dont need pinmap init again after wake from dslp*/
	pinmap_init();

	/* 5. Confirm CPU secure state*/
	cmse_address_info_t cmse_address_info = cmse_TT((void *)app_start);
	RTK_LOGI(TAG, "IMG2 SECURE STATE: %d\n", cmse_address_info.flags.secure);
	/* 6. Load secure image*/
#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
	BOOT_IMG3();
#endif

	/* 7.For reference only, users can modify the function if need */
	/* Attention, the handler is needed to clear NVIC pending int and ip int in dslp flow */
	if ((BOOT_Reason() & AON_BIT_RSTF_DSLP) != 0) {
		//dslp_wake_handler();
		//depends on user's request
	}
	/* 8. Update CPU clk. */
	SystemCoreClockUpdate();

	/* 9. Init XTAL*/
	XTAL_INIT();

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) {//Only Asic need OSC Calibration
		if (((BOOT_Reason()) & AON_BIT_RSTF_DSLP) == FALSE) {
			OSC131K_Calibration(30000); /* PPM=30000=3% *//* 7.5ms */
		}
		OSC4M_Calibration(30000);
		XTAL_PDCK();
	}

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif
	newlib_locks_init();
	/*10. MPU init*/
	mpu_init();
	app_mpu_nocache_init();

	/* Force SP align to 8bytes */
	__asm(
		"ldr r1, =#0xFFFFFFF8\n"
		"mov r0, sp \n"
		"and r0, r0, r1\n"
		"mov sp, r0\n"
	);
	/*11. Enter main*/
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	NULL,//BOOT_RAM_WakeFromPG,
	(u32)NewVectorTable
};

