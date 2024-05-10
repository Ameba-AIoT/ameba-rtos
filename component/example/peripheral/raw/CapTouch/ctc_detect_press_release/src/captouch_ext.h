/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef CAPTOUCH_EXT_H
#define CAPTOUCH_EXT_H

#if defined (CONFIG_AMEBASMART)
#elif defined (CONFIG_AMEBALITE)
#define PINMUX_FUNCTION_CAPTOUCH	PINMUX_FUNCTION_AUXIN
#elif defined (CONFIG_AMEBADPLUS)
#define PINMUX_FUNCTION_CAPTOUCH	PINMUX_FUNCTION_CAP_TOUCH
#endif

#endif /* CAPTOUCH_EXT_H */
