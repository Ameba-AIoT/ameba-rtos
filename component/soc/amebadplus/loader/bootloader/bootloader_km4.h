/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
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

