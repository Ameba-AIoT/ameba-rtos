/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTCFG_H_
#define _AMEBA_BOOTCFG_H_

extern MMU_ConfDef Flash_MMU_Config[];
extern RCC_ConfDef RCC_Config[];
extern u8 Force_OTA1_GPIO;
extern FuncPtr FwCheckCallback;
extern FuncPtr OTASelectHook;
extern u32 Cert_PKHash_OTP_ADDR;
extern u8 Boot_DSP_Enable;
extern u8 DSP_WITHIN_APP_IMG_Enable;
extern SocClk_Info_TypeDef SocClk_Info[];
extern u8 Boot_SocClk_Info_Idx;
extern u8 Boot_Agg_En;

/**
* @brif  TRUSTZONE enable or disable
* 	FALSE: disable
*	TRUE: enable
*/
extern const u8 Boot_Tzcfg_En;
#endif
