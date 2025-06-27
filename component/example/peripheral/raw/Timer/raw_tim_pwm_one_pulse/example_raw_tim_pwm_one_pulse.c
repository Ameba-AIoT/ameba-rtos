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

#if defined(CONFIG_AMEBAGREEN2)
#define PWM_GEN_CHNL			TIM_Channel_3
#else
#define PWM_GEN_CHNL			TIM_Channel_5
#endif
#define PWM_PERIOD	100

static GPIO_InitTypeDef GPIO_InitStruct;

void tim_gen_pwm_one_pulse(void)
{
	int pwm_chan = PWM_GEN_CHNL;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = PWM_TIMER;
	TIM_InitStruct_temp.TIM_Prescaler = 199;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIMx[PWM_TIMER], &TIM_InitStruct_temp, TIMx_irq[PWM_TIMER], NULL, 0);
	/*enable one pulse mode*/
	RTIM_SetOnePulseOutputMode(TIMx[PWM_TIMER], TIM_OPMode_Single, TIM_OPMode_ETP_negative);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	RTIM_CCxInit(TIMx[PWM_TIMER], &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIMx[PWM_TIMER], pwm_chan, TIM_CCx_Enable);

#if defined CONFIG_AMEBAGREEN2
	Pinmux_Config(PWM_GEN_CH3_PIN, PINMUX_FUNCTION_TIM4_PWM3);
#else
	Pinmux_Config(PWM_GEN_CH5_PIN, PINMUX_FUNCTION_PWM);
#endif
	RTIM_Cmd(TIMx[PWM_TIMER], ENABLE);

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
	UPS_SrcConfig(UPS_SRC_GPIO);
	UPS_DstConfig(UPS_DST_PWM_TRIG);
#elif defined(CONFIG_AMEBAGREEN2)
	UPS_SrcConfig(UPS_SRC_GPIO, (u8)(PWM_TIMER - 4));
	UPS_DstConfig(UPS_DST_PWM_TRIG, (u8)PWM_TIMER - 4);
#endif
#if defined(CONFIG_AMEBAGREEN2)
	Pinmux_Config(BUTTON_PIN, PINMUX_FUNCTION_PWM_TIM4_TRIG);
	PAD_PullCtrl(BUTTON_PIN, GPIO_PuPd_UP);
#else
	Pinmux_Config(BUTTON_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(BUTTON_PIN, GPIO_PuPd_UP);
#endif
}

void gpio_int_ISR(u32 data, u32 event)
{
	GPIO_InitTypeDef *gpio_irq_pin = (GPIO_InitTypeDef *)data;

	printf("%s pin:%lu ==> event=%lu\n", __FUNCTION__, gpio_irq_pin->GPIO_Pin, event);
}

void gpio_interrupt_enable(void)
{
	u32 GPIO_Pin = INTERRUPT_PIN;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
	GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
	GPIO_Init(&GPIO_InitStruct);

	InterruptRegister(GPIO_INTHandler, GPIOB_IRQ, (u32)GPIOB_BASE, 3);
	InterruptEn(GPIOB_IRQ, 3);
	GPIO_UserRegIrq(GPIO_Pin, (void *)gpio_int_ISR, &GPIO_InitStruct);

	GPIO_INTConfig(GPIO_Pin, ENABLE);
}

void tim_pwm_one_pulse(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */
	RCC_PeriphClockCmd(APBPeriph_TIMx[PWM_TIMER], APBPeriph_TIMx_CLOCK[PWM_TIMER], ENABLE);
#if defined (CONFIG_AMEBASMART)
	Pinmux_Swdoff();
#endif

	/* generate one pulse mode pwm wave*/
	tim_gen_pwm_one_pulse();

	/* enable gpio interrupt*/
	gpio_interrupt_enable();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_tim_pwm_one_pulse(void)
{
	if (rtos_task_create(NULL, "TIM PWM ONE PULSE DEMO", (rtos_task_t)tim_pwm_one_pulse, NULL, 2048, (1)) != RTK_SUCCESS) {
		printf("Cannot create tim pwm one pulse mode demo task\n\r");
	}

	// rtos_sched_start();
	return 0;
}


