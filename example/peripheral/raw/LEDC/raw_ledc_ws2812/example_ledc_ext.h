/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef LEDC_EXT_H
#define LEDC_EXT_H

#include "basic_types.h"

#if defined (CONFIG_AMEBASMART)
#define LEDC_PIN	_PA_9
#elif defined (CONFIG_AMEBALITE)
#define LEDC_PIN	_PA_21
#elif defined (CONFIG_AMEBADPLUS)
#define LEDC_PIN	_PA_26
#endif

#define LED_T1H		800
#define LED_T1L		300
#define LED_T0H		300
#define LED_T0L		800
#define LED_RST		300000
#define F_INTERVAL_NS	1000000000

/* LEDC IP_CLK: 40MHz <--> 25ns */
#define NS2VAL(TIME)	((TIME)/25)

typedef struct {
	u32 *TxData;
	u32 TxLength;
	u32 Result;
} LEDC_OBJ;

typedef enum {
	RESULT_COMPLETE = 1,
	RESULT_ERR = 2,
	RESULT_RUNNING = 3,
} LEDC_RESULT;

extern int example_raw_ledc_ws2812(void);

#endif /* LEDC_EXT_H*/