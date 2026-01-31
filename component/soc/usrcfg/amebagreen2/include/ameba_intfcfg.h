/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_INTFCFG_H_
#define _AMEBA_INTFCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

extern SDIOHCFG_TypeDef sdioh_config;

extern const u8 SDIO_PAD[7][6];
extern u8 SDIO_Pin_Grp;
extern u8 SDH_Pin_Grp;
extern u8 ETHERNET_Pin_Grp;
extern const u8 ETHERNET_PAD[4][11];
#ifdef __cplusplus
}
#endif

#endif