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

extern MMU_ConfDef Flash_MMU_Config[];
extern RCC_ConfDef RCC_Config[];
extern CACHETCM_TypeDef CACHETCM_Def[];
extern GRST_ConfDef GRST_Config[];
extern RSIP_OTFDef RSIP_OTF_Config[];
extern u8 Force_OTA1_GPIO;
extern u8 Boot_PSRAM_APM;
extern FuncPtr FwCheckCallback;
extern FuncPtr OTASelectHook;
extern u32 Cert_PKHash_OTP_ADDR;
extern u32 Boot_TCM_Size;
extern CACHEWRR_TypeDef CACHEWRR_Def[];
extern u8 Boot_AP_Enbale;
extern u8 Boot_MemSwr_Only;
extern SocClk_Info_TypeDef SocClk_Info[];
extern u8 Boot_Agg_En;

#ifdef __cplusplus
}
#endif

#endif
