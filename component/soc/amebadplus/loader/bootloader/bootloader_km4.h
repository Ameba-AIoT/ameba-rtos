/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOTLOADER_H_
#define _AMEBA_BOOTLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*declaration of bootloader_km4_lib.c*/
void BOOT_TRNG_ParaSet(void);
void BOOT_ResetMask_Config(void);
void BOOT_Disable_PadSlewRate(void);
u32 BOOT_Share_Memory_Patch(void);
void BOOT_VerCheck(void);
void BOOT_OTPCheck(void);
bool BOOT_RRAM_InfoValid(void);
void BOOT_Enable_KM0(void);
void BOOT_ReasonSet(void);
void BOOT_PSRAM_Init(void);
void BOOT_SOC_ClkSet(void);
void BOOT_ComboSpic_PLL_Open(void);
void BOOT_Data_Flash_Init(void);
/* declaration of boot_trustzone_km4.c*/
void BOOT_RAM_TZCfg(void);

#ifdef __cplusplus
}
#endif

#endif
