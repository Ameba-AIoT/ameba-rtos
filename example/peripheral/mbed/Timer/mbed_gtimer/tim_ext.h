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

#define GPIO_LED_PIN1       PB_10
#define GPIO_LED_PIN2       PB_11

#elif defined (CONFIG_AMEBALITE)

#define GPIO_LED_PIN1       PA_21
#define GPIO_LED_PIN2       PA_22

#elif defined (CONFIG_AMEBADPLUS)

#define GPIO_LED_PIN1       PB_17
#define GPIO_LED_PIN2       PB_18

#elif defined (CONFIG_AMEBAGREEN2)

#define GPIO_LED_PIN1       PA_13
#define GPIO_LED_PIN2       PA_14
#endif

#endif
