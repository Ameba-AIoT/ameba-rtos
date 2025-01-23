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

#define PWM_1       PA_9
#define PWM_2       PA_10
#define PWM_3       PA_4
#define PWM_4       PA_5

#elif defined (CONFIG_AMEBALITE)

#define PWM_1       PA_28
#define PWM_2       PA_29
#define PWM_3       PA_30
#define PWM_4       PA_31

#elif defined (CONFIG_AMEBADPLUS)

#define PWM_1       PB_17
#define PWM_2       PB_18
#define PWM_3       PB_19
#define PWM_4       PB_30

#endif

#endif
