/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef TIM_EXT_H
#define TIM_EXT_H
#include "platform_autoconf.h"
#define PWM_TIMER		8
#define PWM_PRESCALER	39
#define PWM_PERIOD		20000
#define PWM_STEP		(PWM_PERIOD / 200)  //Brightness change speed

#if defined (CONFIG_AMEBASMART)
#define PWM_CHANNEL_MAX		6
int pwms[PWM_CHANNEL_MAX] = {0, PWM_PERIOD / 6, PWM_PERIOD / 3, PWM_PERIOD / 2, PWM_PERIOD / 3 * 2, PWM_PERIOD / 6 * 5};
int steps[PWM_CHANNEL_MAX] = {PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP};

#else
#define PWM_CHANNEL_MAX		8
int pwms[PWM_CHANNEL_MAX] = {0, PWM_PERIOD / 8, PWM_PERIOD / 4, PWM_PERIOD / 8 * 3, PWM_PERIOD / 2, PWM_PERIOD / 8 * 5, \
							 PWM_PERIOD / 4 * 3, PWM_PERIOD / 8 * 7
							};
int steps[PWM_CHANNEL_MAX] = {PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP};
#endif

u8 PWM_GPIOx_S0[PWM_CHANNEL_MAX] = {

#if defined (CONFIG_AMEBASMART)

	_PA_14,
	_PA_9,
	_PA_10,
	_PA_4,
	_PA_5,
	_PA_13,

#elif defined (CONFIG_AMEBALITE)

	_PA_28,
	_PA_29,
	_PA_30,
	_PA_31,
	_PA_26, //8726E, _PA_26; 8720E, _PA_15
	_PA_27, //8726E, _PA_27; 8720E, _PA_16
	_PA_21,
	_PA_22,

#elif defined (CONFIG_AMEBADPLUS)

	_PA_12,
	_PA_26,
	_PA_27,
	_PA_30,
	_PA_31,
	_PB_17,
	_PB_18,
	_PB_19,

#endif

};

#endif