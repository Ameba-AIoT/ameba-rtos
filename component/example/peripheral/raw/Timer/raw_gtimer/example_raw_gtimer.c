/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "example_tim_ext.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>


#define GTIMER_PERIOD_IDX		2 //basic TIM2
#define GTIMER_ONESHOT_IDX		3 //basic TIM3
#define GTIMER_PERIOD_US		1000000	//us
#define GTIMER_ONESHOT_US		500000	//us

volatile uint32_t time2_expired = 0;

u32 timer1_timeout_handler(void *id)
{
	RTIM_TimeBaseInitTypeDef *gtimer = (RTIM_TimeBaseInitTypeDef *) id;

	GPIO_WriteBit(GPIO_LED_PIN1, !GPIO_ReadDataBit(GPIO_LED_PIN1));
	RTIM_INTClear(TIMx[gtimer->TIM_Idx]);

	/* make sure all intr pending bits cleared ok, to avoid timeout is not enough in rom code */
	RTIM_INTClear(TIMx[gtimer->TIM_Idx]);
	return 0;
}

u32 timer2_timeout_handler(void *id)
{
	RTIM_TimeBaseInitTypeDef *gtimer = (RTIM_TimeBaseInitTypeDef *) id;

	time2_expired = 1;
	RTIM_INTClear(TIMx[gtimer->TIM_Idx]);
	RTIM_Cmd(TIMx[gtimer->TIM_Idx], DISABLE);
	return 0;
}

void gpio_ctrl_led_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct1;
	GPIO_InitTypeDef GPIO_InitStruct2;

	GPIO_InitStruct1.GPIO_Pin = GPIO_LED_PIN1;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct1);
	GPIO_WriteBit(GPIO_LED_PIN1, 1);

	GPIO_InitStruct2.GPIO_Pin = GPIO_LED_PIN2;
	GPIO_InitStruct2.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct2);
	GPIO_WriteBit(GPIO_LED_PIN2, 1);
}

void raw_gtimer_demo(void)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct1;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct2;

	/* If it doesn`t work, check GPIO and TIMx status in RCC_Config */
	/* Enable TIMx function & clock, take TIM2 and TIM3 as example */
#if defined (CONFIG_AMEBASMART)

	RCC_PeriphClockCmd(APBPeriph_TIM2, APBPeriph_TIM2_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIM3, APBPeriph_TIM3_CLOCK, ENABLE);

#elif defined (CONFIG_AMEBALITE)

	RCC_PeriphClockCmd(APBPeriph_TIM02, APBPeriph_TIM02_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIM03, APBPeriph_TIM03_CLOCK, ENABLE);

#elif defined (CONFIG_AMEBADPLUS)

	RCC_PeriphClockCmd(APBPeriph_LTIM2, APBPeriph_LTIM2_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LTIM3, APBPeriph_LTIM3_CLOCK, ENABLE);

#endif

	//Init LED control pin
	gpio_ctrl_led_config();

	//initial a periodical gimer
	RTIM_TimeBaseStructInit(&TIM_InitStruct1);
	TIM_InitStruct1.TIM_Idx = GTIMER_PERIOD_IDX;
	TIM_InitStruct1.TIM_Period = GTIMER_PERIOD_US / 1000000 * 32768 - 1;

	/* make sure all intr pending bits cleared ok in irq handler */
	RTIM_TimeBaseInit(TIMx[GTIMER_PERIOD_IDX], (&TIM_InitStruct1), TIMx_irq[GTIMER_PERIOD_IDX], (IRQ_FUN)timer1_timeout_handler, (u32)(&TIM_InitStruct1));
	RTIM_INTConfig(TIMx[GTIMER_PERIOD_IDX], TIM_IT_Update, ENABLE);
	RTIM_Cmd(TIMx[GTIMER_PERIOD_IDX], ENABLE);

	//initial a oneshort gimer and re-trigger it in while loop
	RTIM_TimeBaseStructInit(&TIM_InitStruct2);
	TIM_InitStruct2.TIM_Idx = GTIMER_ONESHOT_IDX;
	TIM_InitStruct2.TIM_Period = (u32)((float)GTIMER_ONESHOT_US / 1000000 * 32768) - 1;

	RTIM_TimeBaseInit(TIMx[GTIMER_ONESHOT_IDX], (&TIM_InitStruct2), TIMx_irq[GTIMER_ONESHOT_IDX], (IRQ_FUN)timer2_timeout_handler, (u32) &TIM_InitStruct2);
	time2_expired = 0;
	RTIM_INTConfig(TIMx[GTIMER_ONESHOT_IDX], TIM_IT_Update, ENABLE);
	RTIM_Cmd(TIMx[GTIMER_ONESHOT_IDX], ENABLE);

	while (1) {
		if (time2_expired) {
			GPIO_WriteBit(GPIO_LED_PIN2, !GPIO_ReadDataBit(GPIO_LED_PIN2));
			time2_expired = 0;
			RTIM_Cmd(TIMx[GTIMER_ONESHOT_IDX], ENABLE);
		}
	}
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_gtimer(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "RAW_GTIMER_DEMO_TASK", (rtos_task_t)raw_gtimer_demo, (void *)NULL, 2048, (1))) {
		printf("Create RAW_GTIMER_DEMO_TASK Err!!\n");
	}

	// rtos_sched_start();
	return 0;
}

