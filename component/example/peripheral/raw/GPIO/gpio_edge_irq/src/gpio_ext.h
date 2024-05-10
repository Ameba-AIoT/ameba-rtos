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
#define GPIO_SIGNAL_SOURCE		_PB_10
#define GPIO_IRQ_EDGE_PIN		_PA_10

#elif defined (CONFIG_AMEBALITE)
#define GPIO_SIGNAL_SOURCE		_PB_5
#define GPIO_IRQ_EDGE_PIN		_PA_29

#elif defined (CONFIG_AMEBADPLUS)
#define GPIO_SIGNAL_SOURCE		_PB_10
#define GPIO_IRQ_EDGE_PIN		_PA_12

#endif

#endif
