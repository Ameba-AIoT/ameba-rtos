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


#define PWM_PERIOD	40000000/32000

static u32 timout_cnt = 0;

// PWM TIM8 generate 32KHz at channel 5
void tim8_gen_pwm_32k(void)
{
	int pwm_chan = TIM_Channel_5;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = 8;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIM8, &TIM_InitStruct_temp, TIMER8_IRQ, NULL, 0);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	RTIM_CCxInit(TIM8, &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIM8, pwm_chan, TIM_CCx_Enable);

	Pinmux_Config(PWM_32K_CH5_PIN, PINMUX_FUNCTION_PWM5);
	RTIM_Cmd(TIM8, ENABLE);
}

// TIM9 as capture at channel 0
u32 tim9_capture_ISR(void *data)
{
	UNUSED(data);
	timout_cnt ++;
	if (timout_cnt == 10000) {
		u32 value = RTIM_CCRxGet(TIM9, TIM_Channel_0);
		printf("Pulse width: %lu, %s\n", value, ((value + 1 == PWM_PERIOD / 2) ? "success" : "fail"));
		timout_cnt = 0;
	}

	RTIM_INTClear(TIM9);
	return 0;
}

void tim9_capture_pulse_width(void)
{
	RTIM_TimeBaseInitTypeDef  TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = 9;
	RTIM_TimeBaseInit(TIM9, &TIM_InitStruct_temp, TIMER9_IRQ, (IRQ_FUN)tim9_capture_ISR, 0);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_ICPulseMode = TIM_CCMode_PulseWidth;
	RTIM_CCxInit(TIM9, &TIM_CCInitStruct, TIM_Channel_0);
	RTIM_INTConfig(TIM9, TIM_IT_CC0, ENABLE);
	RTIM_CCxCmd(TIM9, TIM_Channel_0, TIM_CCx_Enable);
	RTIM_Cmd(TIM9, ENABLE);

	Pinmux_Config(TIM9_TRIG_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(TIM9_TRIG_PIN, GPIO_PuPd_UP);
}

void tim9_capture_pulse_width_32k(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */
#if defined (CONFIG_AMEBASMART)
	Pinmux_Swdoff();
	RCC_PeriphClockCmd(APBPeriph_TIM8, APBPeriph_TIM8_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIM9, APBPeriph_TIM9_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_TIM_PWM, APBPeriph_TIM_PWM_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIM_PULSE, APBPeriph_TIM_PULSE_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBADPLUS)
	RCC_PeriphClockCmd(APBPeriph_PWM0, APBPeriph_PWM0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_PWM1, APBPeriph_PWM1_CLOCK, ENABLE);
#endif

	/* generate 32760Hz pwm wave*/
	tim8_gen_pwm_32k();

	/* capture pwm pulse width*/
	tim9_capture_pulse_width();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, "TIM9 CAPTURE PULSE WIDTH DEMO", (rtos_task_t)tim9_capture_pulse_width_32k, NULL, 2048, (1)) != SUCCESS) {
		printf("Cannot create tim9 capture pulse width demo task\n\r");
	}

	rtos_sched_start();

	while (1);
}


