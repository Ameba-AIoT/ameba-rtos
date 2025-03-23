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

void raw_pwm_demo(void)
{
	RTIM_TimeBaseInitTypeDef RTIM_InitStruct;
	TIM_CCInitTypeDef TIM_CCInitStruct;
	int pwm_chan = 0;
	int i;
	/* close swdclk/swdio*/
	Pinmux_Swdoff();
	/* Enable TIM_PWM function & clock */
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_TIM8, APBPeriph_TIM8_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_TIM_PWM, APBPeriph_TIM_PWM_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBADPLUS)
	RCC_PeriphClockCmd(APBPeriph_PWM0, APBPeriph_PWM0_CLOCK, ENABLE);
#endif

	RTIM_TimeBaseStructInit(&RTIM_InitStruct);
	RTIM_InitStruct.TIM_Idx = PWM_TIMER;
	RTIM_InitStruct.TIM_Prescaler = PWM_PRESCALER;
	RTIM_InitStruct.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIMx[PWM_TIMER], (&RTIM_InitStruct), TIMER8_IRQ, NULL, NULL);

	for (pwm_chan = 0; pwm_chan < PWM_CHANNEL_MAX; pwm_chan++) {
		RTIM_CCStructInit(&TIM_CCInitStruct);
		TIM_CCInitStruct.TIM_OCPulse = pwms[pwm_chan];
		RTIM_CCxInit(TIMx[PWM_TIMER], &TIM_CCInitStruct, pwm_chan);
		RTIM_CCxCmd(TIMx[PWM_TIMER], pwm_chan, TIM_CCx_Enable);
#if defined(CONFIG_AMEBASMART)
		Pinmux_Config(PWM_GPIOx_S0[pwm_chan], PINMUX_FUNCTION_PWM);
#else
		Pinmux_Config(PWM_GPIOx_S0[pwm_chan], (PINMUX_FUNCTION_PWM0 + pwm_chan));
#endif
	}

	RTIM_Cmd(TIMx[PWM_TIMER], ENABLE);

	while (1) {
		for (i = 0; i < PWM_CHANNEL_MAX; i++) {
			RTIM_CCRxSet(TIMx[PWM_TIMER], pwms[i], i);
			pwms[i] += steps[i];
			if (pwms[i] >= PWM_PERIOD) {
				steps[i] = -PWM_STEP;
				pwms[i] = PWM_PERIOD;
			}

			if (pwms[i] <= 0) {
				steps[i] = PWM_STEP;
				pwms[i] = 0;
			}
		}
		DelayMs(1);
	}
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_pwm(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "RAW_PWM_DEMO_TASK", (rtos_task_t)raw_pwm_demo, (void *)NULL, (2048), (1))) {
		printf("Create RAW_PWM_DEMO_TASK Err!!\n");
	}

	// rtos_sched_start();
	return 0;
}