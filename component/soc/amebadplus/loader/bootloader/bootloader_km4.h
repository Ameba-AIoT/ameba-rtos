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
/*declaration of bootloader_km4_lib.c*/
void BOOT_TRNG_ParaSet(void);
void BOOT_ResetMask_Config(void);
void BOOT_Disable_PadSlewRate(void);
u32 BOOT_Share_Memory_Patch(void);
void BOOT_VerCheck(void);
void BOOT_OTPCheck(void);

/* declaration of boot_trustzone_km4.c*/
void BOOT_RAM_TZCfg(void);
#endif

