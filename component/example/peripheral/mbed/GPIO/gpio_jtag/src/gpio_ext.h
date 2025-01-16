/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GPIO_EXT_H
#define GPIO_EXT_H

#include "platform_autoconf.h"

#if defined (CONFIG_AMEBASMART)
#define GPIO_LED_PIN			_PA_14	// SWD_CLK
#define GPIO_JTAG_ENABLE_PIN	_PA_5

#elif defined (CONFIG_AMEBALITE)
#define GPIO_LED_PIN			_PB_1	// SWD_CLK
#define GPIO_JTAG_ENABLE_PIN	_PA_29

#elif defined (CONFIG_AMEBADPLUS)
#define GPIO_LED_PIN			_PA_30	// SWD_CLK
#define GPIO_JTAG_ENABLE_PIN	_PA_12

#elif defined (CONFIG_AMEBAD)
#define GPIO_LED_PIN			_PB_3	// SWD_CLK
#define GPIO_JTAG_ENABLE_PIN	_PA_13

#endif

#endif
