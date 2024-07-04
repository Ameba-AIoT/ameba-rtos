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
#define ADC_ID1		ADC_CH2
#define ADC_ID2		ADC_CH3
#define ADC_PIN1	ADC_CH2_PIN
#define ADC_PIN2	ADC_CH3_PIN
#define PINMUX_FUNCTION_ADC PINMUX_FUNCTION_CAPTOUCH

#elif defined (CONFIG_AMEBALITE)
#define ADC_ID1		ADC_CH0
#define ADC_ID2		ADC_CH1
#define ADC_PIN1	ADC_CH0_PIN
#define ADC_PIN2	ADC_CH1_PIN
#define PINMUX_FUNCTION_ADC PINMUX_FUNCTION_AUXIN

#endif

#endif /* ADC_EXT_H*/
