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

#if defined(CONFIG_AMEBAGREEN2)
#define PWM_GEN_CHNL			TIM_Channel_3
#else
#define PWM_GEN_CHNL			TIM_Channel_5
#endif
#define PWM_PERIOD	40000000/32000

// PWM TIM4/8 generate 32KHz at channel 3/5
void tim_gen_pwm_32k(void)
{
	int pwm_chan = PWM_GEN_CHNL;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = PWM_TIMER;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIMx[PWM_TIMER], &TIM_InitStruct_temp, TIMx_irq[PWM_TIMER], NULL, 0);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	RTIM_CCxInit(TIMx[PWM_TIMER], &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIMx[PWM_TIMER], pwm_chan, TIM_CCx_Enable);
#if defined CONFIG_AMEBAGREEN2
	Pinmux_Config(PWM_32K_CH3_PIN, PINMUX_FUNCTION_TIM4_PWM3);
#else
	Pinmux_Config(PWM_32K_CH5_PIN, PINMUX_FUNCTION_PWM5);
#endif
	RTIM_Cmd(TIMx[PWM_TIMER], ENABLE);
}

// TIM as capture at channel 0
u32 tim_capture_ISR(void *data)
{
	UNUSED(data);
	u32 value = RTIM_CCRxGet(TIMx[CAPTURE_TIMER], TIM_Channel_0);
	printf("Pulse number: %lu, %s\n", value, (value == 3199 || value == 3200) ? "success" : "fail");
	RTIM_INTClear(TIMx[CAPTURE_TIMER]);
	return 0;
}

void tim_capture_pulse_num(void)
{
	RTIM_TimeBaseInitTypeDef  TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = CAPTURE_TIMER;
	TIM_InitStruct_temp.TIM_Prescaler = 199;
	TIM_InitStruct_temp.TIM_Period = 19999; //interrupt every 100ms
	RTIM_TimeBaseInit(TIMx[CAPTURE_TIMER], &TIM_InitStruct_temp, TIMx_irq[CAPTURE_TIMER], (IRQ_FUN)tim_capture_ISR, 0);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_ICPulseMode = TIM_CCMode_PulseNumber;
	RTIM_CCxInit(TIMx[CAPTURE_TIMER], &TIM_CCInitStruct, TIM_Channel_0);
	RTIM_INTConfig(TIMx[CAPTURE_TIMER], TIM_IT_CC0, ENABLE);
	RTIM_CCxCmd(TIMx[CAPTURE_TIMER], TIM_Channel_0, TIM_CCx_Enable);
	RTIM_Cmd(TIMx[CAPTURE_TIMER], ENABLE);
#if defined(CONFIG_AMEBAGREEN2)
	Pinmux_Config(TIM8_TRIG_PIN, PINMUX_FUNCTION_CAPT_TIM8_TRIG);
	PAD_PullCtrl(TIM8_TRIG_PIN, GPIO_PuPd_UP);
#else
	Pinmux_Config(TIM9_TRIG_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(TIM9_TRIG_PIN, GPIO_PuPd_UP);
#endif


}

void tim_capture_pulse_number_32k(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */
	RCC_PeriphClockCmd(APBPeriph_TIMx[PWM_TIMER], APBPeriph_TIMx_CLOCK[PWM_TIMER], ENABLE);
	RCC_PeriphClockCmd(APBPeriph_TIMx[CAPTURE_TIMER], APBPeriph_TIMx_CLOCK[CAPTURE_TIMER], ENABLE);
#if defined (CONFIG_AMEBASMART)
	Pinmux_Swdoff();
#endif


	/* generate 32kHz pwm wave*/
	tim_gen_pwm_32k();

	/* capture pwm pulse number*/
	tim_capture_pulse_num();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_tim_capture_pulse_number_32k(void)
{
	if (rtos_task_create(NULL, "TIM CAPTURE PULSE NUM DEMO", (rtos_task_t)tim_capture_pulse_number_32k, NULL, 2048, (1)) != RTK_SUCCESS) {
		printf("Cannot create tim capture pulse num demo task\n\r");
	}

	// rtos_sched_start();

	return 0;
}


