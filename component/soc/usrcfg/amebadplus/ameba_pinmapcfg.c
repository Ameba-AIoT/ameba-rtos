/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

const PMAP_TypeDef pmap_func[] = {
//Pin Name		Func PU/PD					Slp PU/PD
	{_PA_0,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			//
	{_PA_1,			GPIO_PuPd_UP,			GPIO_PuPd_UP},				// flash cs pin
	{_PA_2,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			//
	{_PA_3,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			//
	{_PA_4,			GPIO_PuPd_KEEP,			GPIO_PuPd_UP},			 //
	{_PA_5,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PA_6,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_7,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_8,			GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_9,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_10,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_11,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_12,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_13,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_14,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_15,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_16,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_17,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_18,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 	// flash cs pin
	{_PA_19,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_20,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_21,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_22,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_23,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_24,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_25,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_26,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_27,		GPIO_PuPd_KEEP,			GPIO_PuPd_DOWN},			 //
	{_PA_28,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_29,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_30,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PA_31,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_0,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_1,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_2,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_3,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_4,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			 	// log_RX sleep need pull up
	{_PB_5,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			 	// log_TX sleep need pull up
	{_PB_6,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_7,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_8,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_9,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_10,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_11,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_12,		GPIO_PuPd_NOPULL,		GPIO_PuPd_NOPULL},			 //
	{_PB_13,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_14,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_15,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_16,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_17,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_18,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_19,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_20,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_21,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_22,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_23,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_24,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_25,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_26,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_27,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_28,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_29,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},			 //
	{_PB_30,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 	//
	{_PB_31,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 	//
	{_PNC,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //table end
};