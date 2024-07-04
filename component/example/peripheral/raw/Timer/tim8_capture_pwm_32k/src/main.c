/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "tim_ext.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>


#define TIM8_CAPTURE_CHNL		TIM_Channel_0
#define PWM_GEN_CHNL			TIM_Channel_5

#define PWM_PERIOD				40000000/32000

u32 timout_cnt = 0, freq = 0, freq_nums = 0;
u32 value1, value2;

u32 tim8_capture_ISR(void *data)
{
	UNUSED(data);
	timout_cnt ++;

	if (timout_cnt % 2) {
		value1 = RTIM_CCRxGet(TIM8, TIM8_CAPTURE_CHNL);
	} else {
		value2 = RTIM_CCRxGet(TIM8, TIM8_CAPTURE_CHNL);
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

	RTIM_INTClear(TIM8);
	return 0;
}

// PWM TIM8 generate 32kHz at channel 5
void tim8_gen_pwm_32k(void)
{
	int pwm_chan = PWM_GEN_CHNL;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = 8;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIM8, &TIM_InitStruct_temp, TIMER8_IRQ, tim8_capture_ISR, NULL);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	TIM_CCInitStruct.TIM_CCPolarity = TIM_CCPolarity_High;
	RTIM_CCxInit(TIM8, &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIM8, pwm_chan, TIM_CCx_Enable);

	Pinmux_Config(PWM_GEN_32KHz_CH5_PIN, PINMUX_FUNCTION_PWM5);

	RTIM_Cmd(TIM8, ENABLE);
}

// TIM8 as capture at channel 0
void tim8_capture_32k(void)
{
	u32 cap_chnl = TIM8_CAPTURE_CHNL;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(TIM8, &TIM_CCInitStruct, cap_chnl);

	RTIM_CCxCmd(TIM8, cap_chnl, TIM_CCx_Disable);
	RTIM_CCRxMode(TIM8, cap_chnl, TIM_CCMode_Inputcapture);
	RTIM_CCxCmd(TIM8, cap_chnl, TIM_CCx_Enable);

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
	UPS_SrcConfig(UPS_SRC_GPIO);
	UPS_DstConfig(UPS_DST_PWM_TRIG);
#endif

	Pinmux_Config(TIM8_TRIG_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(TIM8_TRIG_PIN, GPIO_PuPd_UP);

	RTIM_INTConfig(TIM8, TIM_IT_CCx[cap_chnl], ENABLE);
	RTIM_Cmd(TIM8, ENABLE);
}

void tim8_capture_pwm(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */

#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_TIM8, APBPeriph_TIM8_CLOCK, ENABLE);
	Pinmux_Swdoff(); // PA13 is SWDIO, so SWD func needs to be closed.
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_TIM_PWM, APBPeriph_TIM_PWM_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBADPLUS)
	RCC_PeriphClockCmd(APBPeriph_PWM0, APBPeriph_PWM0_CLOCK, ENABLE);
#endif

	/* generate 32kHz pwm wave*/
	tim8_gen_pwm_32k();

	/* capture pwm pulse width*/
	tim8_capture_32k();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, "TIM8 CAPTURE PWM DEMO", (rtos_task_t)tim8_capture_pwm, NULL, (2048), (1)) != SUCCESS) {
		printf("Cannot create tim8 capture pwm demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}


