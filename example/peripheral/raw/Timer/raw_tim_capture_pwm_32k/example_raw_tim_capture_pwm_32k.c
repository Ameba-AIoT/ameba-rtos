/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "example_tim_ext.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

#define CAPTURE_CHNL			TIM_Channel_0
#if defined(CONFIG_AMEBAGREEN2)
#define PWM_GEN_CHNL			TIM_Channel_3
#else
#define PWM_GEN_CHNL			TIM_Channel_5
#endif
#define PWM_PERIOD				40000000/32000

u32 timout_cnt = 0, freq = 0, freq_nums = 0;
u32 value1, value2;

u32 tim_capture_ISR(void *data)
{
	UNUSED(data);
	timout_cnt ++;

	if (timout_cnt % 2) {
		value1 = RTIM_CCRxGet(TIMx[PWM_TIMER], CAPTURE_CHNL);
	} else {
		value2 = RTIM_CCRxGet(TIMx[PWM_TIMER], CAPTURE_CHNL);
		timout_cnt = 0;
		if (value2 > value1) {
			freq += 40000000 / (value2 - value1);
			freq_nums ++;
		} else {
			freq += 40000000 / (PWM_PERIOD - value1 + value2);
			freq_nums ++;
		}
	}

	if (((freq_nums % 16000) == 0) & (freq_nums > 0)) {
		printf("frequency: %lu\n", freq / freq_nums);
		freq = freq_nums = 0;
	}

	RTIM_INTClear(TIMx[PWM_TIMER]);
	return 0;
}

// PWM generate 32kHz at channel 5/3
void tim_gen_pwm_32k(void)
{
	int pwm_chan = PWM_GEN_CHNL;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = PWM_TIMER;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIMx[PWM_TIMER], &TIM_InitStruct_temp, TIMx_irq[PWM_TIMER], tim_capture_ISR, NULL);
	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	TIM_CCInitStruct.TIM_CCPolarity = TIM_CCPolarity_High;
	RTIM_CCxInit(TIMx[PWM_TIMER], &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIMx[PWM_TIMER], pwm_chan, TIM_CCx_Enable);
#if defined CONFIG_AMEBAGREEN2
	Pinmux_Config(PWM_GEN_32KHz_CH3_PIN, PINMUX_FUNCTION_TIM4_PWM3);
#else
	Pinmux_Config(PWM_GEN_32KHz_CH5_PIN, PINMUX_FUNCTION_PWM5);
#endif
	RTIM_Cmd(TIMx[PWM_TIMER], ENABLE);
}

// TIM8/4 as capture at channel 0
void tim_capture_32k(void)
{
	u32 cap_chnl = CAPTURE_CHNL;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(TIMx[PWM_TIMER], &TIM_CCInitStruct, cap_chnl);

	RTIM_CCxCmd(TIMx[PWM_TIMER], cap_chnl, TIM_CCx_Disable);
	RTIM_CCRxMode(TIMx[PWM_TIMER], cap_chnl, TIM_CCMode_Inputcapture);
	RTIM_CCxCmd(TIMx[PWM_TIMER], cap_chnl, TIM_CCx_Enable);

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
	UPS_SrcConfig(UPS_SRC_GPIO);
	UPS_DstConfig(UPS_DST_PWM_TRIG);
#elif defined(CONFIG_AMEBAGREEN2)
	UPS_SrcConfig(UPS_SRC_GPIO, (u8)(PWM_TIMER - 4));
	UPS_DstConfig(UPS_DST_PWM_TRIG, (u8)PWM_TIMER - 4);
#endif

#if defined(CONFIG_AMEBAGREEN2)
	Pinmux_Config(TIM4_TRIG_PIN, PINMUX_FUNCTION_PWM_TIM4_TRIG);
	PAD_PullCtrl(TIM4_TRIG_PIN, GPIO_PuPd_UP);
#else
	Pinmux_Config(TIM8_TRIG_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(TIM8_TRIG_PIN, GPIO_PuPd_UP);
#endif
	RTIM_INTConfig(TIMx[PWM_TIMER], TIM_IT_CCx[cap_chnl], ENABLE);
	RTIM_Cmd(TIMx[PWM_TIMER], ENABLE);
}

void tim_capture_pwm(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */

	RCC_PeriphClockCmd(APBPeriph_TIMx[PWM_TIMER], APBPeriph_TIMx_CLOCK[PWM_TIMER], ENABLE);
#if defined (CONFIG_AMEBASMART)
	Pinmux_Swdoff(); // PA13 is SWDIO, so SWD func needs to be closed.
#endif

	/* generate 32kHz pwm wave*/
	tim_gen_pwm_32k();

	/* capture pwm pulse width*/
	tim_capture_32k();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_tim_capture_pwm_32k(void)
{
	if (rtos_task_create(NULL, "TIM CAPTURE PWM DEMO", (rtos_task_t)tim_capture_pwm, NULL, (2048), (1)) != RTK_SUCCESS) {
		printf("Cannot create tim capture pwm demo task\n\r");
	}

	// rtos_sched_start();
	return 0;
}


