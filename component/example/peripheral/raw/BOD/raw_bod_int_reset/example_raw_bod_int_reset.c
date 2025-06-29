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

#define BOD_INTR_MODE	1

#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
#define BOD_FALL_THRE	0x0F
#define BOD_RISE_THRE	0x0A
#elif defined (CONFIG_AMEBAGREEN2)
#define BOD_FALL_THRE	0x07
#define BOD_RISE_THRE	0x04
#endif

static void bod_irq_handler(void)
{
	BOR_ClearINT();
	RTK_LOGI(NOTAG, "BOD interrupt\n");
}

void bod_intr_test(void)
{
	/* clean pending bod interrupt status */
	BOR_ClearINT();

	/* register BOD interrupt handler */
	InterruptRegister((IRQ_FUN)bod_irq_handler, BOR_IRQ, NULL, INT_PRI_HIGH);
	InterruptEn(BOR_IRQ, INT_PRI_HIGH);

	BOR_ThresholdSet(BOD_FALL_THRE, BOD_RISE_THRE);
	BOR_ModeSet(BOR_INTR);
	BOR_Enable(ENABLE);

	/* To avoid unwanted extra reset. */
	/* Default debounce delay: 100us(BOR_TDBC = 0x1) */
	/* It takes 100us for actual BOD output to sync to digital circuit. */
	DelayUs(100);
	RCC_PeriphClockCmd(APBPeriph_BOR, APBPeriph_CLOCK_NULL, ENABLE);

#if defined (CONFIG_AMEBASMART)
	RTK_LOGI(NOTAG, "Supply voltage under 2.67V triggers bod interrupt!!!\n");
#elif defined (CONFIG_AMEBALITE)
	RTK_LOGI(NOTAG, "Supply voltage under 2.54V triggers bod interrupt!!!\n");
#elif defined (CONFIG_AMEBADPLUS)
	RTK_LOGI(NOTAG, "Supply voltage under 2.21V triggers bod interrupt!!!\n");
#elif defined (CONFIG_AMEBAGREEN2)
	RTK_LOGI(NOTAG, "Supply voltage under 2.57V triggers bod interrupt!!!\n");
#endif

	rtos_task_delete(NULL);
}

void bod_reset_test(void)
{
	BOR_ThresholdSet(BOD_FALL_THRE, BOD_RISE_THRE);
	BOR_ModeSet(BOR_RESET);
	BOR_Enable(ENABLE);

	/* To avoid unwanted extra reset. */
	/* default debounce delay: 100us(BOR_TDBC = 0x1) */
	/* It takes 100us for actual BOD output to sync to digital circuit. */
	DelayUs(100);
	RCC_PeriphClockCmd(APBPeriph_BOR, APBPeriph_CLOCK_NULL, ENABLE);

#if defined (CONFIG_AMEBASMART)
	RTK_LOGI(NOTAG, "Supply voltage under 2.67V and recover it to above 2.87V causes bod reset!!!\n");
#elif defined (CONFIG_AMEBALITE)
	RTK_LOGI(NOTAG, "Supply voltage under 2.54V and recover it to above 2.83V causes bod reset!!!\n");
#elif defined (CONFIG_AMEBADPLUS)
	RTK_LOGI(NOTAG, "Supply voltage under 2.21V and recover it to above 2.50V causes bod reset!!!\n");
#elif defined (CONFIG_AMEBAGREEN2)
	RTK_LOGI(NOTAG, "Supply voltage under 2.57V and recover it to above 2.74V causes bod reset!!!\n");
#endif

	rtos_task_delete(NULL);
}

int example_raw_bod_int_reset(void)
{
#if BOD_INTR_MODE
	if (rtos_task_create(NULL, "BOD_INTR_TASK", (rtos_task_t)bod_intr_test, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create BOD_INTR_TASK\n");
	}
#else
	if (rtos_task_create(NULL, "BOD_RESET_TASK", (rtos_task_t)bod_reset_test, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create BOD_RESET_TASK\n");
	}
#endif

	//rtos_sched_start();

	return 0;
}
