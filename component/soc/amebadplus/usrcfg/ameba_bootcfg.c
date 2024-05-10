/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/**
* @brif  TRUSTZONE enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
#ifdef CONFIG_TRUSTZONE
const u8 Boot_Tzcfg_En = TRUE;
#else
const u8 Boot_Tzcfg_En = FALSE;
#endif

/*
* @brif	RCC Configuration.
*	 by Users.
*/
const RCC_ConfDef RCC_Config[] = {
	/*Function,				Clock,							State*/
	/* AON */
	{APBPeriph_SDM,			APBPeriph_SDM_CLOCK,			ENABLE},
	{APBPeriph_ATIM,		APBPeriph_ATIM_CLOCK,			ENABLE},
	{APBPeriph_GPIO,		APBPeriph_GPIO_CLOCK,			ENABLE},
	{APBPeriph_IPC,			APBPeriph_IPC_CLOCK,			ENABLE},
	{APBPeriph_SHA,			APBPeriph_SHA_CLOCK,			ENABLE},
	{APBPeriph_AES,			APBPeriph_AES_CLOCK,			ENABLE},
	{APBPeriph_LX,			APBPeriph_LX_CLOCK,				ENABLE},
	{APBPeriph_LTIM0,		APBPeriph_LTIM0_CLOCK,			ENABLE},
	{APBPeriph_PTIM0,		APBPeriph_PTIM0_CLOCK,			ENABLE},
	{APBPeriph_PTIM1,		APBPeriph_PTIM1_CLOCK,			ENABLE},

	{0xFFFFFFFF,			NULL,							ENABLE},
};

u32 Cert_PKHash_OTP_ADDR = SEC_PKKEY_PK1_0;

// for km4, max 345MHz under 1.0v, max 260MHz under 0.9v
// for km0, max 115MHz under 1.0v, max 104MHz under 0.9v
// PLL can be 300MHz~688.128MHz
// KM4_CKD range is [1, 8], KM0_CKD range is [1, 16] or USEXTAL
const SocClk_Info_TypeDef SocClk_Info[] = {
	/* PLL_CLK,		Vol_Type,		KM4_CKD,	KM0_CKD,	PSRAMC_CKD*/
	{PLL_520M,		CORE_VOL_0P9,	CLKDIV(2),	CLKDIV(5),	CLKDIV(2)},
	{PLL_331M,		CORE_VOL_1P0,	CLKDIV(1),	CLKDIV(3),	CLKDIV(1)},
	{PLL_400M,		CORE_VOL_0P9,	CLKDIV(2),	CLKDIV(4),	CLKDIV(1)},
	{PLL_480M,		CORE_VOL_0P9,	CLKDIV(2),	CLKDIV(5),	CLKDIV(2)},  // 48M for usb
	{PLL_677P376M,	CORE_VOL_1P0,	CLKDIV(2),	CLKDIV(6),	CLKDIV(2)},
	{PLL_688P128M,	CORE_VOL_1P0,	CLKDIV(2),	CLKDIV(6),	CLKDIV(2)},
};

/**
* @brif  SocClk_Info select
* Boot_SocClk_Info_Idx is [0, sizeof(SocClk_Info)), Soc will set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/
#ifdef CONFIG_USB_DEVICE_EN
u8 Boot_SocClk_Info_Idx = 3; /* Make sure the PLL_CLK for USB is an integer multiple of 48MHz */
#else
u8 Boot_SocClk_Info_Idx = 0;
#endif

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Agg_En = FALSE;

