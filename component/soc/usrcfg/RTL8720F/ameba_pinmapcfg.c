/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

const PMAP_TypeDef pmap_func[] = {
//Pin Name		Func PU/PD					Slp PU/PD
	{_PA_0,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_1,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_2,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_3,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_4,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_5,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_6,			GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_7,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_8,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_9,			GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_10,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_11,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_12,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_13,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_14,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},	     //
	{_PA_15,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_16,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_17,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_18,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_19,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //log_TX sleep need pull up
	{_PA_20,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //log_RX sleep need pull up
	{_PA_21,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_22,		GPIO_PuPd_UP,			GPIO_PuPd_UP},			 //
	{_PA_23,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_24,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_25,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_26,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_27,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_28,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_29,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_30,		GPIO_PuPd_DOWN,			GPIO_PuPd_DOWN},		 //
	{_PA_31,		GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PB_0,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PB_1,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PB_2,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PB_3,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},			 //
	{_PB_4,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},		 //
	{_PNC,			GPIO_PuPd_KEEP,			GPIO_PuPd_KEEP},		 //table end
};
