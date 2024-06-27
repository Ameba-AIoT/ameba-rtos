/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WDG_WINDOW_TEST

#if defined (CONFIG_AMEBASMART)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2 | AON_BIT_RSTF_WDG3 | AON_BIT_RSTF_WDG4)
#if defined (ARM_CORE_CM0)
WDG_TypeDef *WDG_DEV = IWDG_DEV;
IRQn_Type WDG_IRQNUM = WDG0_IRQ;
#elif defined (ARM_CORE_CM4)
WDG_TypeDef *WDG_DEV = WDG2_DEV;
IRQn_Type WDG_IRQNUM = WDG2_IRQ;
#elif defined (ARM_CORE_CA32)
WDG_TypeDef *WDG_DEV = WDG4_DEV;
IRQn_Type WDG_IRQNUM = WDG4_IRQ;
#endif
#endif

#if defined (CONFIG_AMEBALITE)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2 | AON_BIT_RSTF_WDG3 | AON_BIT_RSTF_WDG4)
#if defined (ARM_CORE_CM4)
WDG_TypeDef *WDG_DEV = WDG2_DEV;
IRQn_Type WDG_IRQNUM = KM4_NS_WDG_INT_IRQ;
#elif defined (RSICV_CORE_KR4)
WDG_TypeDef *WDG_DEV = WDG3_DEV;
IRQn_Type WDG_IRQNUM = KR4_WDG_INT_IRQ;
#endif
#endif

#if defined (CONFIG_AMEBADPLUS)
#define WDG_RESET_SRC	(AON_BIT_RSTF_IWDG | AON_BIT_RSTF_WDG0 | AON_BIT_RSTF_WDG1 | AON_BIT_RSTF_WDG2)
#if defined (ARM_CORE_CM0)
WDG_TypeDef *WDG_DEV = IWDG_DEV;
IRQn_Type WDG_IRQNUM = IWDG_IRQ;
#elif defined (ARM_CORE_CM4)
WDG_TypeDef *WDG_DEV = KM4_NS_WDG_DEV;
IRQn_Type WDG_IRQNUM = KM4_NS_WDG_IRQ;
#endif
#endif


static rtos_timer_t xTimers_WDG;

static u32 WDG_IrqHandler(void *Data)
{
	UNUSED(Data);

	WDG_Refresh(WDG_DEV);
	WDG_ClearINT(WDG_DEV, WDG_BIT_EIC);
	WDG_ClearINT(WDG_DEV, WDG_BIT_EIC);//remember to clear twice for wdg run slower
	printf("WDG refreshed by watchdog interrupt\n");

	return 0;
}

static void vTimerCallback(rtos_timer_t pxTimer)
{
	const int32_t xMaxExpiryCountBeforeStopping = 5;
	static uint32_t expireCnt = 0;

	if (pxTimer == NULL) {
		printf("Timer callback NULL!\n");
	}

	// Increment the number of times that pxTimer has expired.
	expireCnt++;

	WDG_Refresh(WDG_DEV);
	printf("WDG refreshed by sw timer\n");

	// If the timer has expired 10 times then stop it from running.
	if (expireCnt == xMaxExpiryCountBeforeStopping) {
		// Do not use a block time if calling a timer API function from a
		// timer callback function, as doing so could cause a deadlock!
		rtos_timer_stop(pxTimer, 0);
		WDG_INTConfig(WDG_DEV, WDG_BIT_EIE, ENABLE);
		printf("stop sw timer, refresh watchdog by interrupt\n");
		printf("watchdog raw example exec succeed!\r\n\r\n");
	}
}

void small_task(void)
{
	// if watch dog reboot, then pass.
	if (BOOT_Reason() & WDG_RESET_SRC) {
		printf("\r\n reset by refresh watchdog inside Protected Window\r\n");
		return;
	}
	printf("\r\n doing small task...\r\n");
	DelayMs(1000);
	printf("refresh watchdog in 1s\r\n\r\n");
	WDG_Refresh(WDG_DEV);
}

void big_task(void)
{
	printf("\r\n doing big task...\r\n");
	DelayMs(10000);
	printf("big task done, watchdog is refreshed in big task\r\n\r\n");
}

void watchdog_task(void)
{
	WDG_InitTypeDef WDG_InitStruct;
	uint32_t Timeout = 5000;
	uint32_t window = 3000;
	uint32_t early_int_time = 500;
	uint32_t refresh_timer = 3500;
	uint8_t int_mode = 1;

	WDG_StructInit(&WDG_InitStruct);
#ifdef WDG_WINDOW_TEST
	WDG_InitStruct.Window = window;
#endif
	WDG_InitStruct.Timeout = Timeout;
	WDG_InitStruct.EICNT = early_int_time;
	WDG_InitStruct.EIMOD = int_mode & BIT0;

	WDG_Init(WDG_DEV, &WDG_InitStruct);
	WDG_Enable(WDG_DEV);

	/*WDG early interrupt configuration*/
	if (int_mode) {
		InterruptRegister((IRQ_FUN)WDG_IrqHandler, WDG_IRQNUM, (u32)NULL, INT_PRI_MIDDLE);
		InterruptEn(WDG_IRQNUM, INT_PRI_MIDDLE);
		WDG_INTConfig(WDG_DEV, WDG_BIT_EIE, ENABLE);
	}

	small_task();
	big_task();

	WDG_INTConfig(WDG_DEV, WDG_BIT_EIE, DISABLE);
	rtos_timer_create(&xTimers_WDG, "WDG_Timer", NULL, refresh_timer, TRUE, vTimerCallback);
	if (xTimers_WDG == NULL) {
		printf("sw timer create error \n");
	} else {
		if (rtos_timer_start(xTimers_WDG, 0) != SUCCESS) {
			printf("timer start failed !\n");
		}
	}
	rtos_task_delete(NULL);
}

int main(void)
{
	DelayMs(50);
	if (SUCCESS != rtos_task_create(NULL, "watchdog_task", (rtos_task_t)watchdog_task, (void *)NULL, (512 * 6), (1))) {
		printf("Create watchdog_task Err!!\r\n");
	}

	rtos_sched_start();
	while (1);
}
