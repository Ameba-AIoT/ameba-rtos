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

/* Certificate is suggested to use SEC_PKKEY_PK1_0 */
u32 Cert_PKHash_OTP_ADDR = SEC_PKKEY_PK1_0;

/**
 * @brief Clock Source configuration structure
 *
 * @note 1.CPU clk is divided by USB-PLL clk. At 0.9V, cpu clk is not higher than 240M, and at 1.0v it is not higher than 320M.
 * 		 2.USB_PLL fixed to 960M, SYS_PLL fixed to 400M.
 * 		 3.The clk of all peripherals can be set as close to the peak performance frequency as possible according to the different core voltages.
 * 		 4.RMII(GMAC) has a fixed input of 50M clk, while SPORT(audio) has a fixed input of 98.304M clk.
 * 		 5.The working clock range of CAN is 40M~120M. Peak performance is between 100-125M. Sleep and wake up using 4M OSC.
 */
SocClk_Info_TypeDef SocClk_Info[] = {
	/* USBPLL_CLK,		SYSPLL_CLK,		Vol_Type,		CPU_CKD*/
	/* 0. Low power consumption scenario. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
	/* 1. Use RMII or/and USB peripherals. (CPU clk: 320Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
};

/**
* @brif  SocClk_Info select. Configured via menuconfig "SoC Clock Config".
* Boot_SocClk_Info_Idx is [0, sizeof(SocClk_Info)), Soc will set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/
u8 Boot_SocClk_Info_Idx = CONFIG_CLKCFG_IDX;

