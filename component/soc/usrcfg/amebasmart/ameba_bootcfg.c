/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

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
	{APBPeriph_IPC_LP,		APBPeriph_IPC_LP_CLOCK,			ENABLE},
	{APBPeriph_SCE,			APBPeriph_CLOCK_NULL,			ENABLE},
	{APBPeriph_DDRP,		APBPeriph_DDRP_CLOCK,			ENABLE},
	{APBPeriph_DDRC,		APBPeriph_DDRC_CLOCK,			ENABLE},
	{APBPeriph_IPC_HP,		APBPeriph_IPC_HP_CLOCK,			ENABLE},
	{APBPeriph_GDMA,		APBPeriph_GDMA_CLOCK,			ENABLE},
	{APBPeriph_IPSEC,		APBPeriph_IPSEC_CLOCK,			ENABLE},
	{APBPeriph_LX1,			APBPeriph_LX1_CLOCK,			ENABLE},
	{APBPeriph_TIM0,		APBPeriph_TIM0_CLOCK,			ENABLE},

	{0xFFFFFFFF,			NULL,							ENABLE},
};

u32 Cert_PKHash_OTP_ADDR = SEC_PKKEY_PK1_0;

#if defined (CONFIG_ARM_CORE_CM4)
/*
* @brif	CACHE and TCM share memory setting
*/
CACHETCM_TypeDef CACHETCM_Def[7] = {
//		TCM_Size		ICACHE_EN		DCACHE_EN			IWRR				DWRR		TCM_BASE		TCM_TOP
	{TCM_SIZE_0KB,		ENABLE,			ENABLE,		CACHE_WWR_4WAY,		CACHE_WWR_4WAY,			0,			0},
	{TCM_SIZE_16KB,		ENABLE, 		ENABLE, 	CACHE_WWR_3WAY,		CACHE_WWR_4WAY,		0x80000,		0x83fff},
	{TCM_SIZE_32KB,		ENABLE, 		ENABLE, 	CACHE_WWR_3WAY,		CACHE_WWR_2WAY,		0x80000,		0x87fff},
	{TCM_SIZE_48KB,		ENABLE, 		ENABLE, 	CACHE_WWR_2WAY,		CACHE_WWR_2WAY,		0x80000,		0x8bfff},
	{TCM_SIZE_64KB,		ENABLE, 		ENABLE, 	CACHE_WWR_1WAY,		CACHE_WWR_2WAY,		0x80000,		0x8ffff},
	{TCM_SIZE_80KB,		ENABLE, 		DISABLE, 	CACHE_WWR_1WAY,		CACHE_WWR_4WAY,		0x80000,		0x93fff},
	{TCM_SIZE_96KB,		DISABLE, 		DISABLE, 	CACHE_WWR_4WAY,		CACHE_WWR_4WAY,		0x80000,		0x97fff},
};

/**
* @brif  TCM size select.
*	If users need to set the TCM size share from cache.
*	TCM_SIZE_96KB: 96KB TCM, no cache
*	TCM_SIZE_80KB: 80KB TCM,  0KB D-Cache, 16KB I-Cache
*	TCM_SIZE_64KB: 64KB TCM, 16KB D-Cache, 16KB I-Cache
*	TCM_SIZE_48KB: 48KB TCM, 16KB D-Cache, 32KB I-Cache
*	TCM_SIZE_32KB: 32KB TCM, 16KB D-Cache, 48KB I-Cache
*	TCM_SIZE_16KB: 16KB TCM, 32KB D-Cache, 48KB I-Cache
*	TCM_SIZE_0KB :  0KB TCM, 32KB D-Cache, 64KB I-Cache
* @note: cache way restrict setting and TCM memory setting can only choose one
*/
u32 Boot_TCM_Size = TCM_SIZE_0KB;

/*
* @brif	CACHE way restrict setting
* @param  WRR: ways can be used for restrict range.
*		   This parameter can be one of the following values:
*			 CACHE_WWR_1WAY: 1 way for restrict range.
*			 CACHE_WWR_2WAY: 2 way for restrict range.
*			 CACHE_WWR_3WAY: 3 way for restrict range.
*			 CACHE_WWR_4WAY: 4 way for restrict range.
* @param WRR_BASE: way restrict base address, 64KB align.
* @param WRR_TOP: way restrict top address, 64KB align.
* @note: if MPU set the range non-cacheable, WRR setting is invalid
* @note: cache way restrict setting and TCM memory setting can only choose one
*/
CACHEWRR_TypeDef CACHEWRR_Def[2] = {
//	WRR_EN		WRR				WRR_BASE		WRR_TOP
	{DISABLE,	CACHE_WWR_4WAY,		0x00000000,		0xffff0000},	//for ICache Setting
	{DISABLE,	CACHE_WWR_4WAY,		0x00000000,		0xffff0000},	//for DCache Setting
};
#endif

u8 Boot_AP_Enbale = ENABLE;

u8 Boot_MemSwr_Only = DISABLE;

// for km4, max 333MHz under 1.0v, max 250MHz under 0.9v
// for AP, max 1200MHz under 1.0v, max 920MHz under 0.9v
// NP PLL can be 800MHz~1000MHz
SocClk_Info_TypeDef SocClk_Info[] = {
	/* NPPLL_CLK,	APPLL_Clk,			Vol_Type,	KM4_CPU_CKD,	AP_CPU_CKD*/
	{NPPLL_1000M,	APPLL_1200M,	VOL_10,			CLKDIV(3),		CLKDIV(1) | ISAPPLL},	//best porformance
	//{NPPLL_920M,	APPLL_NULL,		VOL_09,			CLKDIV(4),		CLKDIV(1) | ISNPPLL},

};

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Agg_En = FALSE;

