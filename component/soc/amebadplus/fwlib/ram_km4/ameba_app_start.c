/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *const TAG = "APP";

extern int main(void);
void NS_ENTRY BOOT_IMG3(void);
extern void newlib_locks_init(void);

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
//0x0000_0000	0x0007_FFFF	512K		KM4 ROM Top Address
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	/* ROM Code inside CPU does not enter Cache, Set to RO for NULL ptr access error */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00000000;
	mpu_cfg.region_size = 0x00080000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RO;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

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

void app_vdd1833_detect(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	u32 buf[16], i = 0;
	u32 temp = 0;
	u32 data = 0;

	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_AUTO_MODE;
	ADC_InitStruct.ADC_CvlistLen = 0;
	ADC_InitStruct.ADC_Cvlist[0] = ADC_CH9; // AVDD33
	ADC_Init(&ADC_InitStruct);

	ADC_Cmd(ENABLE);
	ADC_ReceiveBuf(buf, 16);
	ADC_Cmd(DISABLE);

	while (i < 16) {
		data += ADC_GET_DATA_GLOBAL(buf[i++]);
	}
	data >>= 4;

	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_RSVD_FOR_SW1);
	if (data > 3000) { // 3000: about 2.4V
		temp |= AON_BIT_WIFI_RF_1833_SEL;
	} else {
		temp &= ~AON_BIT_WIFI_RF_1833_SEL;
		RTK_LOGI(TAG, "IO Power 1.8V\n");
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_RSVD_FOR_SW1, temp);
}

// The Main App entry point
void app_start(void)
{
	RTK_LOGI(TAG, "KM4 APP START \n");
	/* enable non-secure cache */
	Cache_Enable(ENABLE);

	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));

	irq_table_init(MSP_RAM_HP_NS); /* NS Vector table init */
	/* Redirect hardfault for debug, and register function pointer to print task information when a crash occurs */
	Fault_Hanlder_Redirect(vTaskCrashCallback);

	RTK_LOGI(TAG, "VTOR: %lx, VTOR_NS:%lx\n", SCB->VTOR, SCB_NS->VTOR);

	/* configure FreeRTOS interrupt and heap region. After heap is configured, printf can be used*/
	rtos_mem_init();

	RTK_LOGI(TAG, "VTOR: %lx, VTOR_NS:%lx\n", SCB->VTOR, SCB_NS->VTOR);

	app_testmode_status();

	if (IS_TWO_FLASH(MEMORY_VENDOR_GET(ChipInfo_MemoryInfo()))) {
		data_flash_highspeed_setup();
	}

	cmse_address_info_t cmse_address_info = cmse_TT((void *)app_start);
	RTK_LOGI(TAG, "IMG2 SECURE STATE: %d\n", cmse_address_info.flags.secure);

#if defined (CONFIG_RDP_BASIC) && (CONFIG_RDP_BASIC == 1U)
	BOOT_IMG3();
#endif

	SystemCoreClockUpdate();

	XTAL_INIT();

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC_POSTSIM) {//Only Asic need OSC Calibration
		OSC4M_Init();
		OSC4M_Calibration(30000);
		if ((((BOOT_Reason()) & AON_BIT_RSTF_DSLP) == FALSE) && (RTCIO_IsEnabled() == FALSE)) {
			OSC131K_Calibration(30000); /* PPM=30000=3% *//* 7.5ms */
		}
	}


#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	newlib_locks_init();

	mpu_init();
	app_mpu_nocache_init();

	app_vdd1833_detect();

	/* Force SP align to 8bytes */
	__asm(
		"ldr r1, =#0xFFFFFFF8\n"
		"mov r0, sp \n"
		"and r0, r0, r1\n"
		"mov sp, r0\n"
	);

	main(); /* project/xxxx/src/main.c */
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
	NULL,//BOOT_RAM_WakeFromPG,
	(u32)NewVectorTable
};

