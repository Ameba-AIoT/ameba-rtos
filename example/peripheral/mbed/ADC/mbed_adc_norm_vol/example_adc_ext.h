/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "PinNames.h"

#ifndef ADC_EXT_H
#define ADC_EXT_H

#if defined (CONFIG_AMEBASMART)
#define ADC_PIN1	AD_2
#define ADC_PIN2	AD_3
#elif defined (CONFIG_AMEBALITE)
#define ADC_PIN1	AD_0
#define ADC_PIN2	AD_1
#elif defined (CONFIG_AMEBADPLUS)
#define ADC_PIN1	AD_0
#define ADC_PIN2	AD_1
#elif defined (CONFIG_AMEBAGREEN2)
#define ADC_PIN1	AD_1
#define ADC_PIN2	AD_2
#endif

extern int example_mbed_adc_norm_vol(void);

#endif /* ADC_EXT_H*/
