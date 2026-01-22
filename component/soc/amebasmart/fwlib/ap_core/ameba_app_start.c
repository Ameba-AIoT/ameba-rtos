/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */

#include "ameba_soc.h"
#include "FreeRTOS.h"

extern int main(void);

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif

/*
 * Starts all the other tasks, then starts the scheduler.
 */
void app_start(void)
{
	LOG_MASK(LEVEL_ERROR, 0xFFFFFFFF);

	rtos_mem_init();

#if defined (__GNUC__)
	extern void __libc_init_array(void);
	/* Add This for C++ support */
	__libc_init_array();
#endif

	/* Get flash_init_para info for AP */
	_memcpy((void *)&flash_init_para, (const void *)HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_FLASH_PARA_ADDR), sizeof(FLASH_InitTypeDef));

	main();
}
