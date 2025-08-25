/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTCFG_H_
#define _AMEBA_BOOTCFG_H_

/**
* @brif  Firmware select hook.
*	If users need to implement own OTA select method, implement the function and assign the address
*	to this function pointer.
* @param  None
* @retval 0: both firmwares are invalid, select none, 1: boot from OTA1, 2: boot from OTA2
*/
extern FuncPtr OTASelectHook;

/* Certificate is suggested to use SEC_PKKEY_PK1_0 */
extern u32 Cert_PKHash_OTP_ADDR;


// PLL is fixed to 960M, Vol_Type is fixed to 0.9V
// CPU_CKD range is [3, 5], PSRAMC_CKD range is [3, 4]
extern SocClk_Info_TypeDef SocClk_Info[];

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
