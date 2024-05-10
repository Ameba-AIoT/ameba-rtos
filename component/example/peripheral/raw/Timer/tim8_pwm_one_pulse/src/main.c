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


#define PWM_PERIOD	100

static GPIO_InitTypeDef GPIO_InitStruct;

void tim8_gen_pwm_one_pulse(void)
{
	int pwm_chan = TIM_Channel_5;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = 8;
	TIM_InitStruct_temp.TIM_Prescaler = 199;
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD - 1;
	RTIM_TimeBaseInit(TIM8, &TIM_InitStruct_temp, TIMER8_IRQ, NULL, 0);
	/*enable one pulse mode*/
	RTIM_SetOnePulseOutputMode(TIM8, TIM_OPMode_Single, TIM_OPMode_ETP_negative);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	TIM_CCInitStruct.TIM_OCPulse = PWM_PERIOD / 2;
	RTIM_CCxInit(TIM8, &TIM_CCInitStruct, pwm_chan);
	RTIM_CCxCmd(TIM8, pwm_chan, TIM_CCx_Enable);

	Pinmux_Config(PWM_GEN_CH5_PIN, PINMUX_FUNCTION_PWM);
	RTIM_Cmd(TIM8, ENABLE);

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
	UPS_SrcConfig(UPS_SRC_GPIO);
	UPS_DstConfig(UPS_DST_PWM_TRIG);
#endif

	Pinmux_Config(BUTTON_PIN, PINMUX_FUNCTION_TIMER);
	PAD_PullCtrl(BUTTON_PIN, GPIO_PuPd_UP);
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

void tim8_pwm_one_pulse(void)
{
	/* Enable TIM_PWM & TIM_PULSE function & clock */
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_TIM8, APBPeriph_TIM8_CLOCK, ENABLE);
	Pinmux_Swdoff();
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_TIM_PWM, APBPeriph_TIM_PWM_CLOCK, ENABLE);
#elif defined (CONFIG_AMEBADPLUS)
	RCC_PeriphClockCmd(APBPeriph_PWM0, APBPeriph_PWM0_CLOCK, ENABLE);
#endif

	/* generate one pulse mode pwm wave*/
	tim8_gen_pwm_one_pulse();

	/* enable gpio interrupt*/
	gpio_interrupt_enable();

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, "TIM8 PWM ONE PULSE DEMO", (rtos_task_t)tim8_pwm_one_pulse, NULL, 2048, (1)) != SUCCESS) {
		printf("Cannot create tim8 pwm one pulse mode demo task\n\r");
	}

	rtos_sched_start();
}


