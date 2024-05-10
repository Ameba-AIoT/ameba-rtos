/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef ADC_EXT_H
#define ADC_EXT_H

#if defined (CONFIG_AMEBASMART)
#define VBAT_CH		ADC_CH6
#define PINMUX_FUNCTION_ADC PINMUX_FUNCTION_CAPTOUCH

#elif defined (CONFIG_AMEBALITE)
#define VBAT_CH		ADC_CH6
#define PINMUX_FUNCTION_ADC PINMUX_FUNCTION_AUXIN

#elif defined (CONFIG_AMEBADPLUS)
#define VBAT_CH		ADC_CH7

#endif

#endif /* ADC_EXT_H*/
