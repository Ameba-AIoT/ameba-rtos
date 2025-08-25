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

#define CLKCFG_0P9_RMII_USB

/**
 * @brief Clock Source configuration structure
 *
 * @note 1.CPU clk is divided by USB-PLL clk. At 0.9V, cpu clk is not higher than 266.7M, and at 1.0v it is not higher than 333.33M.
 * 		 2.The floating point frequency option of the SYS PLL is used for the input of I2S
 * 		 3.The clk of all peripherals can be set as close to the peak performance frequency as possible according to the different core voltages.
 * 		 4.RMII(GMAC) has a fixed input of 50M clk, while SPORT(audio) has a fixed input of 98.304M clk.
 * 		 5.The working clock range of CAN is 40M~125M. Peak performance is between 100-125M. Sleep and wake up using 4M OSC.
 */
SocClk_Info_TypeDef SocClk_Info[1] = {
	/* USBPLL_CLK,		SYSPLL_CLK,		Vol_Type,		CPU_CKD*/
#if defined CLKCFG_0P9_NO_SYSPLL
	/* 1. Low power consumption scenario. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_NONE,		CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
#elif defined CLKCFG_0P9_RMII_USB
	/* 2. Use RMII or/and USB peripherals. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
#elif defined CLKCFG_0P9_AUDIO98P304M_USB
	/* 3. Use Audio or/and USB peripherals. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_393P216M,	CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
#elif defined CLKCFG_0P9_AUDIO45P1584M_USB
	/* 4. Use Audio or/and RMII peripherals. (CPU clk: 240Mhz) */
	{PLL_960M,			PLL_361P2672M,	CORE_VOL_0P9,	CLKDIV(4) | IS_USB_PLL},
#elif defined CLKCFG_0P9_AUDIO_RMII
	/* 5. Use Audio or/and RMII peripherals. (CPU clk: 266.7Mhz) */
	{PLL_800M,			PLL_393P216M,	CORE_VOL_0P9,	CLKDIV(3) | IS_USB_PLL},
#elif defined CLKCFG_1P0_RMII_USB
	/* 6. 1P0_RMII_USB: Use RMII or/and USB peripherals. (CPU clk: 320Mhz) */
	{PLL_960M,			PLL_400M,		CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
#elif defined CLKCFG_1P0_AUDIO_USB
	/* 7. 1P0_AUDIO_USB: Use Audio or/and USB peripherals. (CPU clk: 320Mhz) */
	{PLL_960M,			PLL_393P216M,	CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
#elif defined CLKCFG_1P0_AUDIO_RMII
	/* 8. 1P0_AUDIO_RMII: Use Audio or/and RMII peripherals. (CPU clk: 333.3Mhz) */
	{PLL_1000M,			PLL_393P216M,	CORE_VOL_1P0,	CLKDIV(3) | IS_USB_PLL},
#else
#error "Incorrect clock config"
#endif
};

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
u8 Boot_Agg_En = FALSE;
