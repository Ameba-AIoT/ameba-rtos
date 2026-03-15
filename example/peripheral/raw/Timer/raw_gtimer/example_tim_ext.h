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

#define GPIO_LED_PIN1		_PB_10
#define GPIO_LED_PIN2		_PB_11

#elif defined (CONFIG_AMEBALITE)

#define GPIO_LED_PIN1		_PA_21
#define GPIO_LED_PIN2		_PA_22

#elif defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)

#define GPIO_LED_PIN1		_PB_17
#define GPIO_LED_PIN2		_PB_18

#endif
int example_raw_gtimer(void);
#endif
