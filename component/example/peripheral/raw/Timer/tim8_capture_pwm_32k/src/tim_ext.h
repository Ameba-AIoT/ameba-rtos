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
#if defined (CONFIG_AMEBASMART)
#define TIM8_TRIG_PIN _PB_10 // or PB_7
#define PWM_GEN_32KHz_CH5_PIN _PA_13
#define PINMUX_FUNCTION_TIMER PINMUX_FUNCTION_TIMINPUT_HS
#define PINMUX_FUNCTION_PWM5 PINMUX_FUNCTION_PWM

#elif defined (CONFIG_AMEBALITE)
#define TIM8_TRIG_PIN			_PB_5 // or PB_0
#define PWM_GEN_32KHz_CH5_PIN	_PA_22
#define PINMUX_FUNCTION_TIMER PINMUX_FUNCTION_TIMINPUT_HS

#elif defined (CONFIG_AMEBADPLUS)
#define TIM8_TRIG_PIN			_PB_18
#define PWM_GEN_32KHz_CH5_PIN	_PB_20

#endif


#endif
