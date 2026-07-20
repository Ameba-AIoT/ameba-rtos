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
 * @note 1.CPU clk is divided by USB-PLL clk. At 0.9V, cpu clk is not higher than 266.7M, and at 1.0v it is not higher than 333.33M.
 * 		 2.The floating point frequency option of the SYS PLL is used for the input of I2S
 * 		 3.The clk of all peripherals can be set as close to the peak performance frequency as possible according to the different core voltages.
 * 		 4.RMII(GMAC) has a fixed input of 50M clk, while SPORT(audio) has a fixed input of 98.304M clk.
 * 		 5.The working clock range of CAN is 40M~125M. Peak performance is between 100-125M. Sleep and wake up using 4M OSC.
 * 		 6.The eye diagram test for this IC failed at 0.9V. Therefore, all USB-enabled options are set to 1.0V by default.
 */
SocClk_Info_TypeDef SocClk_Info[] = {
	/* USBPLL_CLK,		SYSPLL_CLK,		Vol_Type,		CPU_CKD*/
	/* 0. Low power consumption scenario. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_NONE,		CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
	/* 1. Use RMII peripheral. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
	/* 2. Use Audio or/and USB peripherals. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_393P216M,	CORE_VOL_1P0,	CLKDIV(4) | IS_USB_PLL},
	/* 3. Use Audio or/and RMII/USB peripherals. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_361P2672M,	CORE_VOL_1P0,	CLKDIV(4) | IS_USB_PLL},
	/* 4. Use Audio or/and RMII peripherals. (CPU clk: 266.7Mhz) */
	{PLL_800M,			PLL_393P216M,	CORE_VOL_0P9,	CLKDIV(3) | IS_USB_PLL},
	/* 5. 1P0_RMII_USB: Use RMII or/and USB peripherals. (CPU clk: 320Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
	/* 6. 1P0_AUDIO_USB: Use Audio or/and USB peripherals. (CPU clk: 320Mhz) */
	{PLL_960M,			PLL_393P216M,	CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
	/* 7. 1P0_AUDIO_RMII: Use Audio or/and RMII peripherals. (CPU clk: 333.3Mhz) */
	{PLL_1000M,			PLL_393P216M,	CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
};

/**
* @brif  SocClk_Info select. Configured via menuconfig "SoC Clock Config".
* Boot_SocClk_Info_Idx is [0, sizeof(SocClk_Info)), Soc will set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/
u8 Boot_SocClk_Info_Idx = CONFIG_CLKCFG_IDX;
