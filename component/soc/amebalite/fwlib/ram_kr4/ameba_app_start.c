/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

extern int main(void);
extern void SOCPS_WakeFromPG(void);
extern void freertos_risc_v_trap_handler(void);
extern void newlib_locks_init(void);

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif
// The Main App entry point
void app_start(void)
{
	/* 0. BOOT_InitGP shall be init at the entrance of the function */
	BOOT_InitGP();

	/* 1. Configure Bss region . */
	_memset((void *) __sbss_start__, 0, (__sbss_end__ - __sbss_start__));
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));

#ifdef CONFIG_CORE_AS_AP
	extern bool os_heap_add(u8 * start_addr, size_t heap_size);
	if (ChipInfo_GetChipSram()) {
		os_heap_add((u8 *)__ap_sram_heap_start, (size_t) __ap_sram_heap_size);
	}

#ifdef CONFIG_PSRAM_ALL_FOR_AP_HEAP
	if (ChipInfo_PsramExists()) {
		os_heap_add((uint8_t *)__km4_bd_psram_start__, (size_t)(__non_secure_psram_end__ - __km4_bd_psram_start__));
	}
#endif
#endif
	rtos_mem_init();

	/* 2. Exception Handler Init, Mode 0 means All exceptions set pc to BASE.*/
	__set_MTVEC((u32)freertos_risc_v_trap_handler, 0);

	/* 3. Update Cpu clk */
	SystemCoreClockUpdate();

	/* 4. use for compensating PMC sleep time*/
	SYSTIMER_Init(); /* 0.2ms */

	/* 5. low power pin dont need pinmap init again after wake from dslp */
	pinmap_init(); /* 1.7ms */

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif
	newlib_locks_init();

	/* 6. KR4 can not assign non-cache region which is less than 512M */
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
	NULL,

};
