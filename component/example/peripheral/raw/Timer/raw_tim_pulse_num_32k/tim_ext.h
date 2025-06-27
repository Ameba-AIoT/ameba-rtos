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
#if defined (CONFIG_AMEBAGREEN2)
#define PWM_TIMER		4
#define CAPTURE_TIMER   8
#else
#define PWM_TIMER		8
#define CAPTURE_TIMER   9
#endif
#if defined (CONFIG_AMEBASMART)
#define TIM9_TRIG_PIN _PB_11
#define PWM_32K_CH5_PIN _PA_13
#define PINMUX_FUNCTION_TIMER PINMUX_FUNCTION_TIMINPUT_HS
#define PINMUX_FUNCTION_PWM5 PINMUX_FUNCTION_PWM

#elif defined (CONFIG_AMEBALITE)
#define TIM9_TRIG_PIN		_PA_21
#define PWM_32K_CH5_PIN		_PA_22
#define PINMUX_FUNCTION_TIMER PINMUX_FUNCTION_TIMINPUT_HS

#elif defined (CONFIG_AMEBADPLUS)
#define TIM9_TRIG_PIN		_PB_17
#define PWM_32K_CH5_PIN		_PB_19

#elif defined (CONFIG_AMEBAGREEN2)
#define TIM8_TRIG_PIN		_PA_3
#define PWM_32K_CH3_PIN	    _PB_16
#endif


#endif
