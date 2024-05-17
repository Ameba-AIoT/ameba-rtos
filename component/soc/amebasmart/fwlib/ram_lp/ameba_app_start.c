/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_system.h"
#include "FreeRTOS.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

#if defined ( __ICCARM__ )
#pragma section=".ram_image2.bss"
#pragma section="NOCACHE_DATA"

SECTION(".data") u8 *__bss_start__ = 0;
SECTION(".data") u8 *__bss_end__ = 0;
SECTION(".data") u8 *__ram_nocache_start__ = 0;
SECTION(".data") u8 *__ram_nocache_end__ = 0;
#endif

extern int main(void);
extern void SOCPS_WakeFromPG(void);
void app_section_init(void)
{
#if defined ( __ICCARM__ )
	__bss_start__               = (u8 *)__section_begin(".ram_image2.bss");
	__bss_end__                 = (u8 *)__section_end(".ram_image2.bss");
	__ram_nocache_start__       = (u8 *)__section_begin("NOCACHE_DATA");
	__ram_nocache_end__         = (u8 *)__section_end("NOCACHE_DATA");
#endif
}

//set all KM0 rom & ram no-cachable, just flash cachable
//KM0 have 4 mpu entrys
//0x0000_0000	0x0001_7FFF	96K		KM0 ITCM ROM
//0x0008_0000	0x0008_3FFF	16K		KM0 DTCM ROM
//0x2300_0000	0x2301_0000	64K		KM0 SRAM
//0x2400_0000	0x2400_01FF	0.5K	KM0 Retention SRAM
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	/* close rom cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00000000;
	mpu_cfg.region_size = 0x10000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
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

void app_start_autoicg(void)
{
	u32 temp = 0;

	/*Enable LP platform auto ICG*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL);
	temp |= LSYS_BIT_PLFM_AUTO_ICG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL, temp);

	/*Enable HP platform auto ICG*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL);
	temp |= HSYS_BIT_PLFM_AUTO_ICG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL, temp);
}

// The Main App entry point
void app_start(void)
{

	app_section_init();
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
	/* 1. Redirect hardfault for debug, and register function pointer to print task information when a crash occurs */
	Fault_Hanlder_Redirect(vTaskCrashCallback);

	/* 2. Init heap region for printf*/
	rtos_mem_init();

	/* 3. Update CPU clock */
	SystemCoreClockUpdate();

	/* 4. use for compensating PMC sleep time*/
	SYSTIMER_Init(); /* 0.2ms */

	//app_start_autoicg();

	/* 5. MPU init*/
	mpu_init();

	app_mpu_nocache_init();
	/* Force SP align to 8bytes */
	__asm(
		"ldr r1, =#0xFFFFFF80\n"
		"mov r0, sp \n"
		"and r0, r0, r1\n"
		"mov sp, r0\n"
	);
	/* 6. Enter Main. */
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
#ifndef CONFIG_MP_INCLUDED
	SOCPS_WakeFromPG,
#else
	NULL,
#endif
	NULL,
};
