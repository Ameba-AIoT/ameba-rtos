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

/* DSP is DISABLE by default */
#ifdef CONFIG_DSP_EN
u8 Boot_DSP_Enable = ENABLE;
#else
u8 Boot_DSP_Enable = DISABLE;
#endif

#ifdef CONFIG_DSP_WITHIN_IMG2_EN
u8 DSP_WITHIN_IMG2_Enable = ENABLE;
#else
u8 DSP_WITHIN_IMG2_Enable = DISABLE;
#endif

/*
* @brif	MMU Configuration.
*	There are 8 MMU entries totally. Entry 0 & Entry 1 are already used by OTA, Entry 2~7 can be used by Users.
*/
MMU_ConfDef Flash_MMU_Config[] = {
	/*VAddrStart,	VAddrEnd,			PAddrStart,		PAddrEnd*/
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 2
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 3
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 4
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 5
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 6
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 7
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},
};

/*
* @brif	RCC Configuration.
*	 by Users.
*/
RCC_ConfDef RCC_Config[] = {
	/*Function,				Clock,							State*/
	/* AON */
	{APBPeriph_SDM32K,		APBPeriph_SDM32K_CLOCK,			ENABLE},
	{APBPeriph_ATIM,		APBPeriph_ATIM_CLOCK,			ENABLE},
	{APBPeriph_GPIO,		APBPeriph_GPIO_CLOCK,			ENABLE},
	{APBPeriph_IPC,			APBPeriph_IPC_CLOCK,			ENABLE},
	{APBPeriph_SCE,			APBPeriph_CLOCK_NULL,			ENABLE},
	{APBPeriph_SHA,			APBPeriph_SHA_CLOCK,			ENABLE},
	{APBPeriph_AES,			APBPeriph_AES_CLOCK,			ENABLE},
	{APBPeriph_LX,			APBPeriph_LX_CLOCK,				ENABLE},
	{APBPeriph_TIM00,		APBPeriph_TIM00_CLOCK,			ENABLE},

	{0xFFFFFFFF,			NULL,							ENABLE},
};

/**
* @brif  boot log enable or disable.
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Log_En = TRUE;

u32 Cert_PKHash_OTP_ADDR = SEC_PKKEY_PK1_0;

// for kr4/km4, max 240MHz under 1.0v, max 200MHz under 0.9v
// for dsp, max 500MHz under 1.0v, max 400MHz under 0.9v
// CPUPLL(PLLM)/DSPPLL(PLLD) can be 330MHz~660MHz
// All CLKDIV range is [1, 16]
SocClk_Info_TypeDef SocClk_Info[] = {
	/* PLLM_CLK,	PLLD_CLK,	Vol_Type,		CPU_CKD,			PSRAMC_CKD*/
	{PLL_600M,		PLL_500M,	CORE_VOL_0P9,	CLKDIV(3) | ISPLLM,	CLKDIV(2) | ISPLLM}, /*0.9V, PSRAM-166M 8720E QFN48*/
	{PLL_600M,		PLL_500M,	CORE_VOL_1P0,	CLKDIV(3) | ISPLLM,	CLKDIV(2) | ISPLLM}, /*1.0V, PSRAM-166M 8720E QFN48*/
	{PLL_400M,		PLL_500M,	CORE_VOL_1P0,	CLKDIV(2) | ISPLLM,	CLKDIV(1) | ISPLLM}, /*1.0V, PSRAM-200M*/
	{PLL_480M,		PLL_500M,	CORE_VOL_1P0,	CLKDIV(2) | ISPLLM,	CLKDIV(1) | ISPLLD}, /*1.0V, PSRAM-250M 8726E QFN68*/
};

/**
* @brif  SocClk_Info select
* Boot_SocClk_Info_Idx valid value is [0, 3] and 0xFF
* when Boot_SocClk_Info_Idx is 0xFF, set socclk by chipinfo Automatically
* when Boot_SocClk_Info_Idx is [0, 3], set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/
u8 Boot_SocClk_Info_Idx = 0xFF;

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Agg_En = FALSE;

