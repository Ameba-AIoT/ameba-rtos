/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTCFG_H_
#define _AMEBA_BOOTCFG_H_
/*
* @brif	MMU Configuration.
*	There are 8 MMU entries totally. Entry 0 & Entry 1 are already used by OTA, Entry 2~7 can be used by Users.
*/
extern MMU_ConfDef Flash_MMU_Config[];

/*
* @brif	RCC Configuration.
*	 by Users.
*/

extern const RCC_ConfDef RCC_Config[];
/**
* @brif  GPIO force OTA1 as image2. 0xFF means force OTA1 trigger is disabled.
* 	BIT[7]: active level, 0 is low level active, 1 is high level active,
*	BIT[6:5]: port, 0 is PORT_A, 1 is PORT_B
*	BIT[4:0]: pin num 0~31
*/

extern u8 Force_OTA1_GPIO;


/**
* @brif  Firmware verify callback handler.
*	If users need to verify checksum/hash for image2, implement the function and assign the address
*	to this function pointer.
* @param  None
* @retval 1: Firmware verify successfully, 0: verify failed
*/

extern FuncPtr FwCheckCallback;

/**
* @brif  Firmware select hook.
*	If users need to implement own OTA select method, implement the function and assign the address
*	to this function pointer.
* @param  None
* @retval 0: both firmwares are invalid, select none, 1: boot from OTA1, 2: boot from OTA2
*/

extern FuncPtr OTASelectHook;



extern u32 Cert_PKHash_OTP_ADDR;


extern u8 Boot_From_APM_PSRAM ;

// for km4, max 330MHz under 1.0v, max 260MHz under 0.9v
// for km0, max 100MHz under all core power
// PLL can be 300MHz~600MHz
// KM4_CKD range is [1, 8], KM0_CKD range is [1, 16] or USEXTAL

extern const SocClk_Info_TypeDef SocClk_Info[];

/**
* @brif  SocClk_Info select
* Boot_SocClk_Info_Idx is [0, sizeof(SocClk_Info)), Soc will set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/

extern u8 Boot_SocClk_Info_Idx;

/**
 * @brif  In order to ensure the performance of RF, we limit the PLL setting values ​​for chips in different packages.
*/
extern u8 Valid_Boot_Idx_for_SiP_Psram[6];
extern u8 Valid_Boot_Idx_for_No_Psram[6];

/**
* @brif  Loguart AGG enable or disable
* 	FALSE: disable
*	TRUE: enable
*/

extern u8 Boot_Agg_En;

/**
* @brif  TRUSTZONE enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
extern const u8 Boot_Tzcfg_En;
#endif
