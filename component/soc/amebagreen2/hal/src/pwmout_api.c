/** mbed Microcontroller Library
 ******************************************************************************
 * @file     pwmout_api.c
 * @author
 * @version V1.0.0
 * @date     2016-08-01
 * @brief    This file provides mbed API for PWM.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "device.h"
#include "objects.h"
#include "pwmout_api.h"

#define PWM_CHANNEL_MAX     4
#define PWM_TIMER_NUM       4

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_PWM
 *  @brief      MBED_PWM driver modules.
 *  @{
 */

/** @defgroup MBED_PWM_Exported_Constants MBED_PWM Exported Constants
 * @{
 */

u8 timer_start[PWM_TIMER_NUM] = {0};
u8 ch_start[PWM_TIMER_NUM][PWM_CHANNEL_MAX] = {0};
u8 prescaler = 0;

static const u32 PinMap_PWM[PWM_TIMER_NUM][PWM_CHANNEL_MAX] = {
	{
		PINMUX_FUNCTION_TIM4_PWM0,
		PINMUX_FUNCTION_TIM4_PWM1,
		PINMUX_FUNCTION_TIM4_PWM2,
		PINMUX_FUNCTION_TIM4_PWM3
	},
	{
		PINMUX_FUNCTION_TIM5_PWM0,
		PINMUX_FUNCTION_TIM5_PWM1,
		PINMUX_FUNCTION_TIM5_PWM2,
		PINMUX_FUNCTION_TIM5_PWM3
	},
	{
		PINMUX_FUNCTION_TIM6_PWM0,
		PINMUX_FUNCTION_TIM6_PWM1,
		PINMUX_FUNCTION_TIM6_PWM2,
		PINMUX_FUNCTION_TIM6_PWM3
	},
	{
		PINMUX_FUNCTION_TIM7_PWM0,
		PINMUX_FUNCTION_TIM7_PWM1,
		PINMUX_FUNCTION_TIM7_PWM2,
		PINMUX_FUNCTION_TIM7_PWM3
	},
};

/**
  * @}
  */

/** @defgroup MBED_PWM_Exported_Functions MBED_PWM Exported Functions
 * @{
 */

/**
  * @brief  Initialize the PWMTIM device.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
static void pwmout_timer_init(pwmout_t *obj)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Period = obj->period * 40 / (prescaler + 1) - 1;
	TIM_InitStruct.TIM_Idx = obj->pwmtimer_idx;

	RCC_PeriphClockCmd(APBPeriph_TIMx[obj->pwmtimer_idx], APBPeriph_TIMx_CLOCK[obj->pwmtimer_idx], ENABLE);

	RTIM_TimeBaseInit(TIMx[obj->pwmtimer_idx], &TIM_InitStruct, TIMx_irq[obj->pwmtimer_idx], NULL, (u32)&TIM_InitStruct);
	RTIM_PrescalerConfig(TIMx[obj->pwmtimer_idx], prescaler, TIM_PSCReloadMode_Update);
	RTIM_Cmd(TIMx[obj->pwmtimer_idx], ENABLE);

	timer_start[obj->pwmtimer_idx - 4] = 1;
}

/**
  * @brief  Initialize PWM channel output according to the specified pin.
  * @param  obj: PWM object defined in application software.
  * @param  pin: Pinname of corresponding PWM channel to be set.
  * @retval none
  * @note
  *             - default period: 1638us
  *             - default pulse width: 102us
  *             - default duty cycle: 6.227%
  */
void pwmout_init(pwmout_t *obj, PinName pin)
{
	u32 pwm_chan;
	u32 pwmtimer_idx;

	assert_param(obj->pwm_idx < PWM_CHANNEL_MAX);

	pwmtimer_idx = obj->pwmtimer_idx;
	pwm_chan = obj->pwm_idx;
	if ((obj->period == 0)) {
		obj->period = 0x10000 * (prescaler + 1) / 40;
		obj->pulse = 0x1000 * (prescaler + 1) / 40;
	}

	if (!timer_start[obj->pwmtimer_idx - 4]) {
		pwmout_timer_init(obj);
	}

	RTIM_CCxCmd(TIMx[pwmtimer_idx], pwm_chan, TIM_CCx_Enable);

	Pinmux_Config(pin, PinMap_PWM[pwmtimer_idx - 4][pwm_chan]);
	ch_start[pwmtimer_idx - 4][pwm_chan] = 1;
}

/**
  * @brief  Deinitialize the PWM device of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @retval none
  * @note  If all channels are released, PWMTIMER will also be disabled.
  */
void pwmout_free(pwmout_t *obj)
{
	/* disable pwm channel */
	uint8_t pwm_chan = obj->pwm_idx;
	uint8_t pwmtimer_idx = obj->pwmtimer_idx;

	if (ch_start[pwmtimer_idx - 4][pwm_chan]) {
		ch_start[pwmtimer_idx - 4][pwm_chan] = 0;
		RTIM_CCxCmd(TIMx[pwmtimer_idx], pwm_chan, TIM_CCx_Disable);

		/* stop pwmtimer if no pwm channels starts */
		for (pwm_chan = 0; pwm_chan < PWM_CHANNEL_MAX; pwm_chan++) {
			if (ch_start[pwmtimer_idx - 4][pwm_chan]) {
				return;
			}
		}

		RTIM_Cmd(TIMx[obj->pwmtimer_idx], DISABLE);
		timer_start[obj->pwmtimer_idx - 4] = 0;
	}

	return;
}

/**
  * @brief  Set the duty cycle of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @param  percent: The duty cycle value to be set.
  * @retval none
  */
void pwmout_write(pwmout_t *obj, float percent) //write duty-cycle
{
	u32 ccrx;

	if (percent < (float)0.0) {
		percent = 0.0;
	} else if (percent > (float)1.0) {
		percent = 1.0;
	}

	obj->pulse = (percent * obj->period);

	ccrx = (u32)(obj->pulse  * 40 / (prescaler + 1)) & 0x0000ffff;
	RCC_PeriphClockCmd(APBPeriph_TIMx[obj->pwmtimer_idx], APBPeriph_TIMx_CLOCK[obj->pwmtimer_idx], ENABLE);
	RTIM_CCRxSet(TIMx[obj->pwmtimer_idx], ccrx, obj->pwm_idx);
}

/**
  * @brief  Get the duty cycle of the specified channel.
  * @param  obj: PWM object defined in application software.
  * @return Duty cycle of the specified channel.
  * @retval 0~1.0
  */
float pwmout_read(pwmout_t *obj) //read duty-cycle
{
	float value = 0;
	if (obj->period > 0) {
		value = (float)obj->pulse / (float)obj->period;
	}
	return ((value > 1.0) ? (1.0) : (value));
}

/**
  * @brief  Set the period of the specified channel in seconds.
  * @param  obj: PWM object defined in application software.
  * @param  seconds: The period value to be set in seconds(s).
  * @retval none
  */
void pwmout_period(pwmout_t *obj, float seconds)
{
	pwmout_period_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the period of the specified channel in millseconds.
  * @param  obj: PWM object defined in application software.
  * @param  ms: The period value to be set in millseconds(ms).
  * @retval none
  */
void pwmout_period_ms(pwmout_t *obj, int ms)
{
	pwmout_period_us(obj, (int)(ms * 1000));
}

/**
  * @brief  Set the period of the specified channel in microseconds.
  * @param  obj: PWM object defined in application software.
  * @param  us: The period value to be set in microseconds(us).
  * @retval none
  */
void pwmout_period_us(pwmout_t *obj, int us)
{
	u32 arr;
	float dc = pwmout_read(obj);
	u32 tmp = us * 40 / (prescaler + 1);

	RCC_PeriphClockCmd(APBPeriph_TIMx[obj->pwmtimer_idx], APBPeriph_TIMx_CLOCK[obj->pwmtimer_idx], ENABLE);

	if (tmp > 0x10000) {
		prescaler = us * 40 / 0x10000;
	}

	obj->period = us;
	arr = us * 40 / (prescaler + 1) - 1;

	RTIM_ChangePeriod(TIMx[obj->pwmtimer_idx], arr);
	pwmout_write(obj, dc);
}

/**
  * @brief  Set the pulse width of the specified channel in seconds.
  * @param  obj: PWM object defined in application software.
  * @param  seconds: The pulse width value to be set in seconds(s).
  * @retval none
  */
void pwmout_pulsewidth(pwmout_t *obj, float seconds)
{
	pwmout_pulsewidth_us(obj, (int)(seconds * 1000000.0f));
}

/**
  * @brief  Set the pulse width of the specified channel in milliseconds.
  * @param  obj: PWM object defined in application software.
  * @param  ms: The pulse width value to be set in milliseconds(ms).
  * @retval none
  */
void pwmout_pulsewidth_ms(pwmout_t *obj, int ms)
{
	pwmout_pulsewidth_us(obj, ms * 1000);
}

/**
  * @brief  Set the pulse width of the specified channel in microseconds.
  * @param  obj: PWM object defined in application software.
  * @param  us: The pulse width value to be set in microseconds(us).
  * @retval none
  */
void pwmout_pulsewidth_us(pwmout_t *obj, int us)
{
	u32 ccrx;

	obj->pulse = (float)us;
	ccrx = (u32)(obj->pulse  * 40 / (prescaler + 1)) & 0x0000ffff;
	RCC_PeriphClockCmd(APBPeriph_TIMx[obj->pwmtimer_idx],  APBPeriph_TIMx_CLOCK[obj->pwmtimer_idx], ENABLE);
	RTIM_CCRxSet(TIMx[obj->pwmtimer_idx], ccrx, obj->pwm_idx);
}

/**
  * @brief  Enable the specified channel to output PWM.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
void pwmout_start(pwmout_t *obj)
{
	RTIM_CCxCmd(TIMx[obj->pwmtimer_idx], obj->pwm_idx, TIM_CCx_Enable);
}

/**
  * @brief  Disable the specified channel to output PWM.
  * @param  obj: PWM object defined in application software.
  * @retval none
  */
void pwmout_stop(pwmout_t *obj)
{
	RTIM_CCxCmd(TIMx[obj->pwmtimer_idx], obj->pwm_idx, TIM_CCx_Disable);
}

/**
  * @brief  Set the polarity of the specified PWM channel.
  * @param  obj: PWM object defined in application software.
  * @param  polarity: This parameter can be one of the following values:
  *   @arg  0: Output is LOW when timer count < setvalue.
  *   @arg  1: Output is HIGH when timer count < setvalue.(default)
  * @attention Configure polarity after setting duty cycle or pulse width.
  * @retval none
  */
void pwmout_set_polarity(pwmout_t *obj, int polarity)
{
	if (0 == polarity) {
		RTIM_CCxPolarityConfig(TIMx[obj->pwmtimer_idx], TIM_CCPolarity_Low, obj->pwm_idx);
	} else {
		RTIM_CCxPolarityConfig(TIMx[obj->pwmtimer_idx], TIM_CCPolarity_High, obj->pwm_idx);
	}
}

/** @} */
/** @} */
/** @} */


