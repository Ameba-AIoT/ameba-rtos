/**
  ******************************************************************************
  * @file    bootloader_hp.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the definations of platform.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
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
void Boot_SDIO_ImgDownload(void);
#endif
