/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

extern int main(void);
extern void SOCPS_WakeFromPG(void);
extern void newlib_locks_init(void);

//set all KM0 rom & ram no-cachable, just flash cachable
//KM0 have 4 mpu entrys
//0x0000_0000	0x0001_FFFF	128K		KM0 ROM Top Address
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	/* ROM Code inside CPU does not enter Cache, Set to RO for NULL ptr access error */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00000000;
	mpu_cfg.region_size = 0x00020000;
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

// The Main App entry point
void app_start(void)
{
	/* 1. Redirect hardfault for debug, and register function pointer to print task information when a crash occurs */
	Fault_Hanlder_Redirect(vTaskCrashCallback);

	newlib_locks_init();

	/* 2. Init heap region for printf*/
	rtos_mem_init();

	/* 3. Get Cpu clock */
	SystemCoreClockUpdate();

	/* 4. use for compensating PMC sleep time*/
	SYSTIMER_Init(); /* 0.2ms */

	/* 5. low power pin dont need pinmap init again after wake from dslp */
	pinmap_init(); /* 1.7ms */

	// RTK_LOGI(TAG, "KM0 VTOR:0x%lx \n", SCB->VTOR);
	/* 6. Configure MPU */
	mpu_init();

	app_mpu_nocache_init();
	/* Force SP align to 8bytes */
	__asm(
		"ldr r1, =#0xFFFFFFF8\n"
		"mov r0, sp \n"
		"and r0, r0, r1\n"
		"mov sp, r0\n"
	);
	/* 7. Enter Main. */
	main();
}

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
#ifndef CONFIG_MP_SHRINK
	SOCPS_WakeFromPG,
#else
	NULL,
#endif
	(u32)NewVectorTable,
};

