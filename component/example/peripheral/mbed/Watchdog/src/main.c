/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "diag.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "wdt_api.h"

#define printf	printf
#if defined (CONFIG_AMEBASMART)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2 | AON_BIT_RSTF_WDG3 | AON_BIT_RSTF_WDG4)
#elif defined (CONFIG_AMEBALITE)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2 | AON_BIT_RSTF_WDG3 | AON_BIT_RSTF_WDG4)
#elif defined (CONFIG_AMEBADPLUS)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG0 | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2)
#endif

void small_task(void)
{
	printf("\r\ndoing small task...\r\n");
	DelayMs(1000);
	printf("refresh watchdog\r\n\r\n");
	watchdog_refresh();
}

void big_task(void)
{
	int i = 0;
	printf("\r\ndoing big task...\r\n");
	for (i = 0; i < 10; i++) {
		printf("doing dummy task %d\r\n", i);
		DelayMs(1000);
	}
	printf("refresh watchdog\r\n\r\n");
	watchdog_refresh();
}

uint32_t my_watchdog_irq_handler(void *id)
{
	(void)id;
	printf("watchdog barks!!!\r\n");
	//watchdog_stop();
	while (1);
	return SUCCESS;
}

void watchdog_task(void)
{
	watchdog_init(5000); // setup 5s watchdog

	watchdog_irq_init(my_watchdog_irq_handler, 0);

	watchdog_start();

	small_task();
	big_task();

	while (1);
}

int main(void)
{
	if (BOOT_Reason() & WDG_RESET_SRC) {
		DelayMs(50);
		printf("reset by watchdog barks\r\n");
		printf("watchdog mbed example exec succeed!\n");
		rtos_sched_start();
		while (1);
	}

	if (SUCCESS != rtos_task_create(NULL, "watchdog_task", (rtos_task_t)watchdog_task, (void *)NULL, (512 * 6), (1))) {
		printf("Create watchdog_task Err!!\r\n");
	}

	rtos_sched_start();
	while (1);

	return SUCCESS;
}

