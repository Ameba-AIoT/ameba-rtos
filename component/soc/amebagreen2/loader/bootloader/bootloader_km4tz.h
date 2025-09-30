/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTLOADER_H_
#define _AMEBA_BOOTLOADER_H_

void BOOT_SOC_ClkChk(const SocClk_Info_TypeDef *pSocClk_Info);
void BOOT_SOC_ClkSet(void);

void BOOT_ResetMask_Config(void);
void BOOT_Enable_Config(void);
void BOOT_SRAM_Extend_Config(void);
void BOOT_CFG_CORE_Vol(void);
void BOOT_VerCheck(void);
void Boot_SDIO_Pinmux_init(void);
void Boot_Fullmac_LoadIMGAll(void);
void BOOT_FullSpeedCheck(u32 cpu_clk);
#endif
