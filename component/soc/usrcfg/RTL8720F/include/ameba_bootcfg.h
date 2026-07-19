/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTCFG_H_
#define _AMEBA_BOOTCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

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
* @brif  SocClk_Info select. Configured via menuconfig "SoC Clock Config".
* Boot_SocClk_Info_Idx is [0, sizeof(SocClk_Info)), Soc will set socclk by SocClk_Info[Boot_SocClk_Info_Idx]
*/
extern u8 Boot_SocClk_Info_Idx;


/**
* @brif  TRUSTZONE enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
extern const u8 Boot_Tzcfg_En;

#ifdef __cplusplus
}
#endif

#endif
